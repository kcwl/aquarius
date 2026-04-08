#pragma once
#include <aquarius/asio.hpp>
#include <aquarius/error_code.hpp>
#include <aquarius/resource/global_resource.hpp>
#include <boost/mysql.hpp>
#include <boost/pfr.hpp>
#include <chrono>

using namespace std::chrono_literals;

namespace aquarius
{
	template <typename Executor>
	class mysql_io_service
	{
		using executor_type = Executor;

	public:
		explicit mysql_io_service(const executor_type& executor)
			: executor_(executor)
		{}

		explicit mysql_io_service(asio::io_context& context)
			: executor_(context.get_executor())
		{}

		~mysql_io_service() = default;

	public:
		bool init()
		{
			return true;
		}

		bool enabled()
		{
			return true;
		}

		void async_run()
		{
			auto& mysql = global_resource::get_mutable_instance().mysql();
			boost::mysql::pool_params params{};
			params.server_address.emplace_host_and_port(mysql.host, static_cast<uint16_t>(mysql.port));
			params.username = mysql.user;
			params.password = mysql.password;
			params.database = mysql.db;
			params.ssl = boost::mysql::ssl_mode::disable;

			pool_ptr_ = std::make_shared<boost::mysql::connection_pool>(executor_, std::move(params));

			pool_ptr_->async_run(asio::detached);
		}

		template <typename T, typename Task>
		T visit(std::shared_ptr<Task> task)
		{
			return (*task)(this);
		}

		template <typename T, typename Task>
		auto async_visit(std::shared_ptr<Task> task) -> asio::awaitable<T>
		{
			co_return co_await (*task)(this);
		}

		void stop()
		{
			return;
		}

		auto async_stop() -> asio::awaitable<void>
		{
			co_return;
		}

		void wait(std::chrono::milliseconds ms)
		{
			return;
		}

		auto async_wait(std::chrono::milliseconds ms) -> asio::awaitable<void>
		{
			co_return;
		}

		template <typename T>
		auto async_query(std::string_view sql, error_code& ec) -> asio::awaitable<std::vector<T>>
		{
			auto conn_ptr = co_await pool_ptr_->async_get_connection(
				asio::cancel_after(1s, asio::redirect_error(asio::use_awaitable, ec)));

			std::vector<T> results{};

			if (ec)
			{
				co_return results;
			}

			boost::mysql::results result{};

			co_await conn_ptr->async_execute(sql, result,
											 asio::cancel_after(1s, asio::redirect_error(asio::use_awaitable, ec)));
			
			if (!ec)
			{
				for (const auto r : result.rows())
				{
					make_result<T>(results, r);
				}
			}

			co_return results;
		}

		auto async_execute(std::string_view sql, error_code& ec) -> asio::awaitable<std::size_t>
		{
			auto conn_ptr = co_await pool_ptr_->async_get_connection(
				asio::cancel_after(1s, asio::redirect_error(asio::use_awaitable, ec)));

			std::size_t affected{};

			if (!ec)
			{
				boost::mysql::results result{};

				co_await conn_ptr->async_execute(sql, result);

				affected = result.affected_rows();
			}

			co_return affected;
		}

		bool enable_transaction() const
		{
			return enable_transaction_;
		}

		auto begin() -> asio::awaitable<error_code>
		{
			return transation("BEGIN");
		}

		auto commit() -> asio::awaitable<error_code>
		{
			return transation("COMMIT");
		}

		auto rollback() -> asio::awaitable<error_code>
		{
			return transation("ROLLBACK");
		}

	private:
		auto transation(const std::string& option) -> asio::awaitable<error_code>
		{
			boost::mysql::results empty_result{};

			error_code ec{};

			boost::mysql::pooled_connection conn_ptr = co_await pool_ptr_->async_get_connection(
				asio::cancel_after(1s, asio::redirect_error(asio::use_awaitable, ec)));

			if (ec)
			{
				co_return ec;
			}

			co_await conn_ptr->async_execute(option, empty_result, asio::redirect_error(asio::use_awaitable, ec));

			co_return ec;
		}

		template <typename T, typename Row>
		void make_result(std::vector<T>& results, const Row& row)
		{
			auto to_struct = [&, this]<std::size_t... I>(std::index_sequence<I...>)
			{ return T{ cast<boost::pfr::tuple_element_t<I, T>>(row[I])... }; };

			constexpr static auto size = boost::pfr::tuple_size_v<T>;

			results.push_back(to_struct(std::make_index_sequence<size>{}));
		}

		template <typename T, typename Row>
		requires(std::is_integral_v<T>)
		void make_result(std::vector<T>& results, const Row& row)
		{
			std::stringstream ss{};

			ss << row[0];

			T result{};

			ss >> result;

			results.push_back(result);
		}

		template <typename T, typename Field>
		auto cast(const Field& field)
		{
			std::stringstream ss{};
			ss << field;

			std::remove_cvref_t<T> value{};

			ss >> value;

			return value;
		}

	private:
		executor_type executor_;

		std::shared_ptr<boost::mysql::connection_pool> pool_ptr_;

		bool enable_transaction_;
	};
} // namespace aquarius