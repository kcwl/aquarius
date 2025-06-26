#pragma once
#include <aquarius/awaitable.hpp>
#include <boost/mysql.hpp>
#include <string_view>
#include <aquarius/singleton.hpp>

#ifdef AQUARIUS_ENABLE_SSL
#include <aquarius/ssl/ssl.hpp>
#endif

namespace aquarius
{
	class mysql_pool : public singleton<mysql_pool>
	{
	public:
		mysql_pool()
			: conn_params_()
			, number_(0)
		{
			
		}

	public:
		template<typename IoPool>
		auto async_run(IoPool& pool, std::size_t number, std::string_view host, std::string_view user, std::string_view password) -> awaitable<void>
		{
			number_ = number;

			conn_params_.server_address.emplace_host_and_port(std::string(host));
			conn_params_.username = std::string(user);
			conn_params_.password = std::string(password);

			create_ssl_context();

			for (auto i = 0; i < number_; ++i)
			{
				connectors_.emplace_back(std::make_shared<boost::mysql::connection_pool>(pool.get_io_service(), std::move(conn_params_)));

				connectors_.back()->async_run(boost::asio::detached);
			}
		}

		auto async_execute(std::string_view sql) -> awaitable<bool>
		{
			auto conn_ptr = get_someone();

			if (!conn_ptr)
			{
				co_await conn_ptr->async_run(boost::asio::use_awaitable);
			}

			boost::mysql::pooled_connection conn =
				co_await conn_ptr->async_get_connection(boost::asio::cancel_after(1s));

			boost::mysql::results result;

			boost::system::error_code ec;

			co_await conn->async_execute(sql, result, boost::asio::redirect_error(boost::asio::use_awaitable, ec));

			co_return !ec;
		}

		template <typename T>
		auto async_query(std::string_view sql) -> awaitable<std::vector<T>>
		{
			auto conn_ptr = get_someone();

			if (!conn_ptr)
			{
				co_await conn_ptr->async_run(boost::asio::use_awaitable);
			}

			boost::mysql::pooled_connection conn =
				co_await conn_ptr->async_get_connection(boost::asio::cancel_after(1s));

			boost::mysql::results result;

			boost::system::error_code ec;

			co_await conn->async_execute(sql, result, boost::asio::redirect_error(boost::asio::use_awaitable, ec));

			if (ec)
			{
				co_return {};
			}

			co_return make_result<T>(result);
		}

	private:
		void create_ssl_context()
		{
#ifdef AQUARIUS_ENABLE_SSL
			conn_params_.ssl_ctx.value().set_verify_mode(boost::asio::ssl::context_base::tls_client);

			conn_params_.ssl_ctx.value().set_verify_mode(boost::asio::ssl::verify_peer);

			// conn_params_.ssl_ctx.value().add_certificate_authority(boost::asio::buffer(CA_PEM));

			conn_params_.ssl_ctx.value().set_verify_callback(boost::asio::ssl::host_name_verification("mysql"));
#endif
		}

	private:
		auto get_someone() -> std::shared_ptr<boost::mysql::connection_pool>
		{
			if (next_io_ == number_)
				next_io_ = 0;

			return connectors_[next_io_++];
		}

		template <typename T>
		std::vector<T> make_result(const boost::mysql::results& result)
		{
			std::vector<T> results{};

			if (!result.has_value())
				return results;

			auto to_struct = []<std::size_t... I>(const boost::mysql::row& row, std::index_sequence<I...>)
			{ return T{ cast<decltype(boost::pfr::get<I>(std::declval<T>()))>(row[I])... }; };

			for (auto column : result.rows())
			{
				results.push_back(to_struct(column, std::make_index_sequence<boost::pfr::tuple_size_v<T>>{}));
			}

			return results;
		}

	private:
		boost::mysql::pool_params conn_params_;

		std::size_t number_;

		std::vector<std::shared_ptr<boost::mysql::connection_pool>> connectors_;

		std::size_t next_io_;
	};
} // namespace aquarius