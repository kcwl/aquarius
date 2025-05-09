#pragma once
#include <aquarius/core/generator.hpp>
#include <aquarius/detail/io_service_pool.hpp>

namespace aquarius
{
	template <typename T>
	struct hash
	{
		std::size_t operator()(const T& x) const
		{
			return std::hash<T>()(x);
		}
	};

	template<typename Request, typename Context>
	struct auto_register
	{
		explicit auto_register(std::size_t proto)
		{
			context_router::get_const_instance().regist<Request, Context>(proto);
		}
	};

	template<typename Session>
	inline void invoke_context(std::size_t proto, flex_buffer_t& buffer, std::shared_ptr<Session> session)
	{
		static io_service_pool router_pool(10);

		boost::asio::post(router_pool.get_io_service(),
			[&]
			{
				context_router::get_mutable_instance().invoke(proto, router_pool.get_io_service(), std::ref(buffer), session);
			});
	}

} // namespace aquarius