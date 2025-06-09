#pragma once
#include <aquarius/ssl/ssl_context_factory.hpp>
#include <aquarius/detail/io_object_impl.hpp>

namespace aquarius
{
	namespace detail
	{
		template <typename Service, typename Executor>
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