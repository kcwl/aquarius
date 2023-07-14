#pragma once
#include <aquarius/detail/invoke.hpp>
#include <aquarius/detail/type_traits.hpp>
#include <aquarius/flex_buffer.hpp>
#include <aquarius/response.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <map>
#include <type_traits>

namespace aquarius
{
	template <typename _Connector>
	class client : public std::enable_shared_from_this<client<_Connector>>
	{
	public:
		explicit client()
			: io_service_()
			, endpoint_()
		{}

		client(const boost::asio::ip::tcp::resolver::results_type& endpoints)
			: client()
			, endpoint_(endpoints)
		{
			do_connect(endpoint_);
		}

		template <class... _Args, class = std::enable_if_t<(sizeof...(_Args) > 1)>>
		client(_Args&&... args)
			: client()
		{
			if constexpr (sizeof...(args) < 2)
				std::throw_with_nested(std::overflow_error("Usage: client <host> <port>"));

			auto endpoint_list = std::make_tuple(std::forward<_Args>(args)...);

			auto host = std::get<0>(endpoint_list);
			auto port = std::get<1>(endpoint_list);

			if constexpr (!detail::is_string_v<decltype(host)> || !detail::is_string_v<decltype(port)>)
				throw std::overflow_error("Usage: client <host> <port> : type - string");

			async_connect(std::get<0>(endpoint_list), std::get<1>(endpoint_list));
		}

	public:
		void async_connect(const std::string& ip, const std::string& port)
		{
			boost::asio::ip::tcp::resolver resolver(io_service_);

			endpoint_ = resolver.resolve(ip, port);

			do_connect(endpoint_);
		}

		bool connect(const std::string& ip, int32_t port)
		{
			boost::asio::ip::tcp::resolver resolver(io_service_);

			endpoint_ = resolver.resolve(ip, std::to_string(port));

			conn_ptr_ = std::make_shared<_Connector>(io_service_);

			boost::system::error_code ec;

			boost::asio::connect(conn_ptr_->socket(), endpoint_, ec);

			if (ec)
				return false;

			return true;
		}

		void run()
		{
			io_service_.run();
		}

		void stop()
		{
			io_service_.stop();

			conn_ptr_->shut_down();
		}

		template<typename _Request>
		void async_write(_Request&& req)
		{
			flex_buffer_t fs{};
			req.visit(fs, visit_mode::output);

			conn_ptr_->async_write(std::move(fs));
		}

		template<typename _Request>
		void write(_Request&& req)
		{
			flex_buffer_t fs{};
			req.visit(fs, visit_mode::output);

			conn_ptr_->write(std::move(fs));
		}

		void async_read()
		{
			conn_ptr_->establish_async_read();
		}

		std::size_t read()
		{
			return conn_ptr_->read();
		}

		template<typename _Func>
		std::size_t read_if(_Func&& f)
		{
			return conn_ptr_->read(std::forward<_Func>(f));
		}

		std::string remote_address()
		{
			return conn_ptr_->remote_address();
		}

		int remote_port()
		{
			return conn_ptr_->remote_port();
		}

		template<connect_event E, typename _Func>
		void regist_callback(_Func&& f)
		{
			if constexpr (E == connect_event::start)
			{
				start_func_ = std::forward<_Func>(f);
			}
			else if constexpr (E == connect_event::close)
			{
				close_func_ = std::forward<_Func>(f);
			}
		}

		void re_connect()
		{
			conn_ptr_->shut_down();

			do_connect(endpoint_);
		}

	private:
		void do_connect(boost::asio::ip::tcp::resolver::results_type endpoints)
		{
			conn_ptr_ = std::make_shared<_Connector>(io_service_);

			boost::asio::async_connect(conn_ptr_->socket(), endpoints,
									   [this](boost::system::error_code ec, boost::asio::ip::tcp::endpoint)
									   {
										   if (ec)
											   return;

										   conn_ptr_->regist_callback<connect_event::start>(start_func_);

										   conn_ptr_->regist_callback<connect_event::close>(close_func_);

										   conn_ptr_->start();
									   });
		}

	private:
		boost::asio::io_service io_service_;

		std::shared_ptr<_Connector> conn_ptr_;

		std::function<void(std::shared_ptr<basic_session>)> start_func_;

		std::function<void(std::shared_ptr<basic_session>)> close_func_;

		boost::asio::ip::tcp::resolver::results_type endpoint_;
	};
} // namespace aquarius