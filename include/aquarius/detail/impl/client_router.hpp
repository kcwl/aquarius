#pragma once
#include <aquarius/basic_router.hpp>

namespace aquarius
{
	namespace detail
	{
		namespace impl
		{
			class client_router : public single_router<client_router, void, std::vector<char>>
			{
			public:
				client_router() = default;

			public:
				template <typename Response, typename Func>
				void regist(std::size_t id, Func&& func)
				{
					this->map_invokes_[id] = [f = std::move(func)](std::vector<char> buffer)
					{
						Response req{};
						req.unpack(buffer);

						f(std::move(req));
					};
				}
			};
		} // namespace impl
	} // namespace detail
} // namespace aquarius