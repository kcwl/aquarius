#pragma once
#include <aquarius/any_io_executor.hpp>
#include <aquarius/session/io_object_impl.hpp>
#include <aquarius/session/ssl_context_factory.hpp>

namespace aquarius
{
	namespace session
	{
		template <typename Service, typename Executor = any_io_executor>
		class session_object_impl : public io_object_impl<Service, Executor>
		{
		public:
			using base_type = io_object_impl<Service, Executor>;

			using socket_type = typename Service::socket_type;

		public:
			session_object_impl(socket_type socket)
				: base_type(0, socket.get_executor())
			{
				this->get_service().move_copy(this->get_implementation(), std::move(socket));
			}
		};
	} // namespace detail
} // namespace aquarius