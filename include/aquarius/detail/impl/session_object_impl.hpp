#pragma once
#include <aquarius/any_io_executor.hpp>
#include <aquarius/detail/impl/io_object_impl.hpp>
#include <aquarius/ssl/ssl_context_factory.hpp>

namespace aquarius
{
	namespace detail
	{
		namespace impl
		{
			template <typename Service, typename Executor = any_io_executor>
			class session_object_impl : public io_object_impl<Service, Executor>
			{
			public:
				using base_type = io_object_impl<Service, Executor>;

				using socket_type = typename Service::socket;

			public:
				session_object_impl(socket_type socket)
					: base_type(0, socket.get_executor())
				{
					this->get_service().move_copy(this->get_implementation(), std::move(socket));
				}
			};
		} // namespace impl
	} // namespace detail
} // namespace aquarius