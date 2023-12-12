#pragma once
#include <aquarius/io_service_pool.hpp>
#include <aquarius/logger.hpp>
#include <aquarius/mysql/algorithm.hpp>
#include <boost/mysql.hpp>
#include <string>
#include <vector>

namespace aquarius
{
	class mysql_connect final
	{
	public:
		template <typename _Endpoint, typename _Param>
		explicit mysql_connect(boost::asio::io_service& ios, _Endpoint&& host, _Param&& param)
			: io_service_(ios)
			, ssl_ctx_(boost::asio::ssl::context::tls_client)
			, mysql_ptr_(new boost::mysql::tcp_ssl_connection(io_service_, ssl_ctx_))
			, endpoint_(host)
			, params_(param)
		{
			run();
		}

		~mysql_connect() = default;

	public:
		void close()
		{
			mysql_ptr_->async_quit(
				[&](const boost::mysql::error_code& ec)
				{
					if (ec)
					{
						XLOG(error) << "mysql async quit error! " << ec.what();
						return;
					}

					XLOG(info) << "mysql connect async quit successful!";
				});
		}

		void set_charset(const std::string& charset = "utf8mb4")
		{
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
									  });
		}

		bool execute(const std::string& sql, boost::mysql::error_code& ec)
		{
			boost::mysql::results result{};
			boost::mysql::diagnostics diag{};

			mysql_ptr_->execute(sql, result, ec, diag);

			return result.has_value();
		}

		template <typename _Func>
		auto async_excute(std::string_view sql, _Func&& f)
		{
			boost::mysql::results result{};
			return mysql_ptr_->async_execute(sql, result,
											 [&](const boost::mysql::error_code& ec)
											 {
												 if (ec)
												 {
													 XLOG(error) << "failed at excute sql:" << sql;
													 f(false);
													 return;
												 }

												 f(true);
											 });
		}

		template <typename _Ty>
		bool query(const std::string& sql, std::vector<_Ty>& t, boost::mysql::error_code& ec)
		{
			boost::mysql::results result{};
			boost::mysql::diagnostics diag{};

			mysql_ptr_->query(sql, result, ec, diag);

			if (!result.has_value())
				return false;

			for (auto& column : result)
			{
				t.push_back(to_struct<_Ty>(column));
			}

			return true;
		}

		template <typename _Ty, typename _Func>
		auto async_query(const std::string& sql, _Func&& f)
		{
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
										   });
		}

	private:
		void run()
		{
			mysql_ptr_->async_connect(*endpoint_.begin(), *params_,
									  [this](const boost::mysql::error_code& ec)
									  {
										  if (ec)
										  {
											  XLOG(error) << "mysql connect error! " << ec.what();
											  return;
										  }

										  XLOG(info) << "msyql async connect success!";

										  // set_charset();
									  });
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

		boost::asio::ssl::context ssl_ctx_;

		std::unique_ptr<boost::mysql::tcp_ssl_connection> mysql_ptr_;

		boost::asio::ip::tcp::resolver::results_type endpoint_;

		std::shared_ptr<boost::mysql::handshake_params> params_;
	};
} // namespace aquarius