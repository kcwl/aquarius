#pragma once
#include <algorithm>
#include <aquarius/io_service_pool.hpp>
#include <boost/mysql.hpp>
#include <deque>
#include <format>
#include <memory>
#include <mutex>

namespace aquarius
{
	template <typename _Service>
	class service_pool
	{
		using service_ptr = std::unique_ptr<_Service>;

		static constexpr std::size_t connect_number = 2 * 3;

	public:
		template <typename... _Args>
		explicit service_pool(io_service_pool& pool, _Args&&... args)
			: pool_(pool)
		{
			make_service_pool(pool_, std::forward<_Args>(args)...);
		}

		~service_pool() = default;

		void stop()
		{
			std::lock_guard lk(free_mutex_);

			while (!free_queue_.empty())
			{
				auto& front = free_queue_.front();

				if (front)
					front->close();

				free_queue_.pop_front();
			}
		}

		bool execute(const std::string& sql)
		{
			auto conn_ptr = get_service();

			if (conn_ptr == nullptr)
				conn_ptr = std::make_shared<_Service>(pool_.get_io_service(), endpoint_, params_);

			boost::mysql::error_code ec;

			if (!conn_ptr->execute(sql))
			{
				XLOG(error) << "sql: " << sql << " execute failed! " << ec.what();
			}

			this->recycle_service(std::move(conn_ptr));
		}

		template <typename _Func>
		auto async_execute(const std::string& sql, _Func&& f)
		{
			auto conn_ptr = get_service();

			if (conn_ptr == nullptr)
				conn_ptr = std::make_unique<_Service>(pool_.get_io_service(), endpoint_, params_);

			conn_ptr->async_excute(sql,
								   [&, ptr = std::move(conn_ptr), func = std::move(f)](bool value) mutable
								   {
									   func(value);

									   this->recycle_service(std::move(ptr));
								   });
		}

		template <typename _Ty>
		std::vector<_Ty> query(const std::string& sql)
		{
			auto conn_ptr = get_service();

			if (conn_ptr == nullptr)
				conn_ptr = std::make_unique<_Service>(pool_.get_io_service(), endpoint_, params_);

			boost::mysql::error_code ec;

			std::vector<_Ty> result{};
			if (!conn_ptr->query(sql, result, ec))
			{
				XLOG(error) << "sql: " << sql << " query failed! " << ec.what();
			}

			this->template recycle_service(std::move(conn_ptr));

			return result;
		}

		template <typename _Ty, typename _Fmt, typename... _Args>
		auto async_pquery(_Fmt&& f, _Args&&... args)
		{
			return async_query<_Ty>(std::format(std::forward<_Fmt>(f), std::forward<_Args>(args)...));
		}

		template <typename _Ty, typename _Func>
		auto async_query(const std::string& sql, _Func&& f)
		{
			auto conn_ptr = get_service();

			if (conn_ptr == nullptr)
				conn_ptr = std::make_unique<_Service>(pool_.get_io_service(), endpoint_, params_);

			return conn_ptr->template async_query(sql,
												  [&, ptr = std::move(conn_ptr), func = std::move(f)](_Ty&& value)
												  {
													  func(std::move(value));

													  this->recycle_service(std::move(ptr));
												  });
		}

	private:
		template <typename... _Args>
		void make_service_pool(io_service_pool& pool, _Args&&... args)
		{
			make_param(std::forward<_Args>(args)...);

			for (std::size_t i = 0; i < connect_number; i++)
			{
				free_queue_.push_back(std::make_unique<_Service>(pool.get_io_service(), endpoint_, params_));
			}
		}

		service_ptr get_service()
		{
			std::lock_guard lk(free_mutex_);

			if (free_queue_.empty())
				return nullptr;

			auto& front = free_queue_.front();

			auto conn_ptr = std::move(front);

			free_queue_.pop_front();

			return conn_ptr;
		}

		void recycle_service(service_ptr&& conn_ptr)
		{
			std::lock_guard lk(free_mutex_);

			free_queue_.push_back(std::move(conn_ptr));
		}

		template <typename _Host, typename _Passwd, typename... _Args>
		void make_param(_Host&& host, _Passwd&& psw, _Args&&... args)
		{
			boost::asio::ip::tcp::resolver resolve(pool_.get_io_service());

			endpoint_ = resolve.resolve(host, psw);

			params_.reset(new boost::mysql::handshake_params(std::forward<_Args>(args)...));
		}

	private:
		io_service_pool& pool_;

		std::deque<service_ptr> free_queue_;

		std::mutex free_mutex_;

		boost::asio::ip::tcp::resolver::results_type endpoint_;

		std::shared_ptr<boost::mysql::handshake_params> params_;
	};
} // namespace aquarius