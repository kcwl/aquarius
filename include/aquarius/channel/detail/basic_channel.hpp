#pragma once
#include <map>
#include <memory>
#include <string>

namespace aquarius
{
	namespace channel
	{
		namespace detail
		{
			template <typename Subscriber>
			class basic_channel
			{
			public:
				using topic = typename Subscriber::topic;

				using subscriber = Subscriber;

			public:
				explicit basic_channel(topic tpc)
					: topic_(tpc)
				{}

			public:
				void subscribe(std::shared_ptr<subscriber> sub_ptr)
				{
					auto id = sub_ptr->id();

					auto iter = subscirbers_.find(id);

					if (iter != subscirbers_.end())
						return;

					subscirbers_.insert({ id, sub_ptr });
				}

				void unsubscribe(std::size_t id)
				{
					subscirbers_.erase(id);
				}

				template <typename... Args>
				void publish(Args&&... args)
				{
					for (auto& s : subscirbers_)
					{
						if (!s)
							continue;

						s->call(std::forward<Args>(args)...);
					}
				}

				std::string name() const
				{
					return topic_;
				}

			private:
				topic topic_;

				std::map<std::size_t, std::shared_ptr<subscriber>> subscirbers_;
			};
		} // namespace detail

	} // namespace channel
} // namespace aquarius