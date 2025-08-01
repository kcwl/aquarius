#pragma once
#include <aquarius/awaitable.hpp>
#include <aquarius/detached.hpp>
#include <aquarius/detail/config.hpp>
#include <aquarius/detail/redirect_error.hpp>
#include <aquarius/error_code.hpp>
#include <aquarius/use_awaitable.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/mysql.hpp>
#include <boost/pfr.hpp>

using namespace std::chrono_literals;

namespace aquarius
{
	class io_context : public boost::asio::io_context
	{
	public:
		using executor_type = boost::asio::io_context::executor_type;

	public:
		io_context() = default;

	public:
		auto sql_execute(std::string_view sql) -> awaitable<bool>
		{
			if (!connector_pool_ptr_)
			{
				co_await connector_pool_ptr_->async_run(use_awaitable);
			}

			boost::mysql::pooled_connection conn =
				co_await connector_pool_ptr_->async_get_connection(boost::asio::cancel_after(1s));

			boost::mysql::results result;

			error_code ec;

			co_await conn->async_execute(sql, result, redirect_error(boost::asio::use_awaitable, ec));

			co_return !ec;
		}

		template <typename T>
		auto sql_query(std::string_view sql) -> awaitable<std::vector<T>>
		{
			if (!connector_pool_ptr_)
			{
				co_await connector_pool_ptr_->async_run(use_awaitable);
			}

			boost::mysql::pooled_connection conn =
				co_await connector_pool_ptr_->async_get_connection(boost::asio::cancel_after(1s));

			boost::mysql::results result;

			error_code ec;

			co_await conn->async_execute(sql, result, redirect_error(use_awaitable, ec));

			if (ec)
			{
				co_return {};
			}

			co_return make_result<T>(result);
		}

		void sql_set_param(std::string_view host, std::string_view user, std::string_view password)
		{
			conn_params_.server_address.emplace_host_and_port(std::string(host));
			conn_params_.username = std::string(user);
			conn_params_.password = std::string(password);

			create_ssl_context();

			connector_pool_ptr_ = std::make_shared<boost::mysql::connection_pool>(*this, std::move(conn_params_));

			connector_pool_ptr_->async_run(detached);
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
		template <typename T>
		auto cast(const boost::mysql::field_view& field)
		{
			std::stringstream ss{};
			ss << field;

			using type = std::remove_cvref_t<T>;

			type result{};

			ss >> result;

			return result;
		}

	private:
		boost::mysql::pool_params conn_params_;

		std::shared_ptr<boost::mysql::connection_pool> connector_pool_ptr_;
	};
} // namespace aquarius