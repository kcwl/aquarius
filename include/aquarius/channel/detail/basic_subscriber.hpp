#pragma once
#include <functional>
#include <map>
#include <memory>

namespace aquarius
{
	namespace channel
	{
		namespace detail
		{
			template <typename Topic>
			class basic_subscriber : public std::enable_shared_from_this<basic_subscriber<Topic>>
			{
			public:
				explicit basic_subscriber(std::size_t id)
					: id_(id)
				{
				}

			public:
				template <typename Subscribor, typename Func>
				void call(Topic tpc, Func&& f)
				{
					create_channel<Topic, Subscribor>().subscribe(tpc, this->shared_from_this());

					topics_[tpc] = std::forward<Func>(f);
				}

				std::size_t id()
				{
					return id_;
				}

				template <typename... Args>
				void invoke(Topic tpc, Args&&... args)
				{
					auto iter = topics_.find(tpc);
					if (iter == topics_.end())
						return;

					iter->second(std::forward<Args>(args)...);
				}

			private:
				std::size_t id_;

				std::map<Topic, std::function<void()>> topics_;
			};
		}
	} // namespace channel
} // namespace aquarius