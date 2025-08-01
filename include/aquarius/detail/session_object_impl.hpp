#pragma once
#include <aquarius/any_io_executor.hpp>
#include <aquarius/ssl/ssl_context_factory.hpp>
#include <boost/asio/detail/io_object_impl.hpp>

namespace aquarius
{
	namespace detail
	{
		template <typename Service, typename Executor = any_io_executor>
		class session_object_impl : public boost::asio::detail::io_object_impl<Service, Executor>
		{
		public:
			using base_type = boost::asio::detail::io_object_impl<Service, Executor>;

			using socket_type = typename Service::socket;

		public:
			session_object_impl(socket_type socket)
				: base_type(0, socket.get_executor())
			{
				this->get_service().move_copy(this->get_implementation(), std::move(socket));
			}
		};
	} // namespace detail
} // namespace aquarius