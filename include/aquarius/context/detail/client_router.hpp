#pragma once
#include <aquarius/basic_router.hpp>
#include <aquarius/flex_buffer.hpp>

namespace aquarius
{
	namespace context
	{
		namespace detail
		{
			class client_router : public single_router<client_router, void, flex_buffer>
			{
			public:
				client_router() = default;

			public:
				template <typename Response, typename Func>
				void regist(std::size_t id, Func&& func)
				{
					this->map_invokes_[id] = [&](flex_buffer buffer)
					{
						Response resp{};
						resp.from_binary(buffer);

						func(std::move(resp));
					};
				}
			};
		} // namespace detail
	} // namespace context
} // namespace aquarius