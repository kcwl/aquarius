#pragma once
#include <aquarius/io_service_pool.hpp>
#include <aquarius/logger.hpp>
#include <boost/mysql.hpp>
#include <string>
#include <vector>
#include <aquarius/mysql/algorithm.hpp>

namespace aquarius
{
	class mysql_connect final
	{
	public:
		template <typename... _Args>
		explicit mysql_connect(boost::asio::io_service& ios, _Args&&... args)
			: io_service_(ios)
			, ssl_ctx_(boost::asio::ssl::context::tls_client)
			, mysql_ptr_(new boost::mysql::tcp_ssl_connection(io_service_, ssl_ctx_))
		{
			make_param(std::forward<_Args>(args)...);
		}

		~mysql_connect() = default;

	public:
		void run()
		{
			mysql_ptr_->async_connect(*endpoint_.begin(), *params_,
									  [this](const boost::mysql::error_code& ec)
									  {
										  if (!ec)
										  {
											  XLOG(error) << "mysql connect error!";
											  return;
										  }

										  set_charset();
									  });
		}

		void close()
		{
			mysql_ptr_->close();
		}

		void set_charset(const std::string& charset = "utf8mb4")
		{
			mutex_.lock();

			boost::mysql::results result;

			mysql_ptr_->async_execute("SET NAMES " + charset, result,
									  [this, charset](const boost::mysql::error_code& ec)
									  {
										  if (ec)
										  {
											  XLOG(error) << "set charset failed! charset=" << charset;
										  }
										  else
										  {
											  XLOG(info) << "set charset " << charset << "successful";
										  }

										  mutex_.unlock();
									  });
		}

		bool execute(const std::string& sql, boost::mysql::error_code& ec)
		{
			std::lock_guard lk(mutex_);

			boost::mysql::results result{};
			boost::mysql::diagnostics diag{};

			mysql_ptr_->execute(sql, result, ec, diag);

			return result.has_value();
		}

		template <typename _Func>
		auto async_excute(std::string_view sql, bool& result)
		{
			result = false;

			mutex_.lock();

			boost::mysql::results result{};
			return mysql_ptr_->async_execute(sql, result,
											 [&](const boost::mysql::error_code& ec)
											 {
												 if (ec)
												 {
													 XLOG(error) << "failed at excute sql:" << sql;
													 return;
												 }

												 result = true;

												 mutex_.unlock();
											 });
		}

		template <typename T>
		T query(const std::string& sql, boost::mysql::error_code& ec)
		{
			std::lock_guard	lk(mutex_);

			T results{};

			boost::mysql::results result{};
			boost::mysql::diagnostics diag{};

			mysql_ptr_->query(sql, result, ec, diag);

			if (!result.has_value())
				return results;

			if constexpr (detail::is_container_v<T>)
			{
				for (auto& column : result)
				{
					results.push_back(to_struct<T>(column));
				}
			}
			else
			{
				static_assert(std::is_trivial_v<T> && std::is_standard_layout_v<T>, "T error!");

				auto& column = result.back();
				results = to_struct<T>(column);
			}

			return results;
		}

		template <typename _Ty, typename _Func>
		auto async_query(const std::string& sql, _Func&& f)
		{
			mutex_.lock();

			boost::mysql::results result{};

			return mysql_ptr_->async_query(sql, result,
										   [&](const boost::mysql::error_code& ec)
										   {
											   if (ec)
											   {
												   XLOG(error) << "failed at excute sql:" << sql;
												   return;
											   }

											   f(make_result<_Ty>(result));

											   mutex_.unlock();
										   });
		}

		bool try_lock()
		{
			return mutex_.try_lock();
		}

	private:
		template <typename _Host, typename _Passwd, typename... _Args>
		void make_param(_Host&& host, _Passwd&& psw, _Args&&... args)
		{
			boost::asio::ip::tcp::resolver resolve(io_service_);

			endpoint_ = resolve.resolve(host, psw);

			params_.reset(new boost::mysql::handshake_params(std::forward<_Args>(args)...));
		}

		template <typename _Ty>
		_Ty make_result(const boost::mysql::results& result)
		{
			_Ty results{};

			if (!result.has_value())
				return results;

			if constexpr (detail::is_container_v<_Ty>)
			{
				for (auto& column : result)
				{
					results.push_back(to_struct<_Ty>(column));
				}
			}
			else
			{
				static_assert(std::is_trivial_v<_Ty> && std::is_standard_layout_v<_Ty>, "T error!");

				auto& column = result.back();
				results = to_struct<_Ty>(column);
			}

			return results;
		}

	private:
		boost::asio::io_service& io_service_;

		boost::asio::ip::tcp::resolver::results_type endpoint_;

		std::unique_ptr<boost::mysql::handshake_params> params_;

		boost::asio::ssl::context ssl_ctx_;

		std::unique_ptr<boost::mysql::tcp_ssl_connection> mysql_ptr_;

		std::mutex mutex_;
	};
} // namespace aquarius