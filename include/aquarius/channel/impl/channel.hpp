#pragma once
#include <memory>
#include <string>
#include <vector>

namespace aquarius
{
	namespace channel
	{
		namespace impl
		{
			template <typename _Subscribe>
			class basic_channel
			{
				using subscribe_t = _Subscribe;

			public:
				basic_channel() = default;
				~basic_channel() = default;

			public:
				std::string topic() const
				{
					return topic_;
				}

				void subscribe(std::shared_ptr<subscribe_t> subscribe_role)
				{
					subscirbers_.push_back(subscribe_role);
				}

				template<typename _Func, typename... _Args>
				void call(_Func&& f, _Args&&... args)
				{
					for (auto& subs : subscirbers_)
					{
						subs->apply(std::forward<_Func>(f), std::forward<_Args>(args)...);
					}
				}

			private:
				std::string topic_;

				std::vector<std::shared_ptr<subscribe_t>> subscirbers_;
			};
		} // namespace impl
	}	  // namespace channel
} // namespace aquarius