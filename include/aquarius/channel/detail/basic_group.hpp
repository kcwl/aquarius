#pragma once
#include <map>
#include <memory>
#include <regex>

namespace aquarius
{
	namespace channel
	{
		namespace detail
		{
			template <typename Channel>
			class basic_group
			{
			public:
				using topic = typename Channel::topic;

				using subscriber = typename Channel::subscriber;

			public:
				void subsribe(topic tpc, std::shared_ptr<subscriber> sub_ptr)
				{
					auto& channel = channels_[tpc];

					if (!channel)
					{
						channel = std::make_shared<Channel>(tpc);
					}

					channel->subscribe(sub_ptr);
				}

				void unsubscribe(topic tpc, std::size_t sub_id)
				{
					auto iter = channels_.find(tpc);

					if (iter == channels_.end())
					{
						return;
					}

					iter->second->unsubscribe(sub_id);
				}

				template <typename... Args>
				void publish(topic tpc, Args&&... args)
				{
					auto iter = channels_.find(tpc);

					if (iter == channels_.end())
					{
						return;
					}

					iter->second->invoke(tpc, std::forward_like<Args>(args)...);
				}

				void subscribe_parttern(std::string_view pattern, std::shared_ptr<subscriber> sub_ptr)
				{
					std::regex re(pattern.data());

					for (auto& ch : channels_)
					{
						auto name = ch->name();

						if (std::regex_match(name, re))
						{
							ch->subscribe(sub_ptr);
						}
					}
				}

				void unsubscribe_parttern(std::string_view pattern, std::shared_ptr<subscriber> sub_ptr)
				{
					std::regex re(pattern.data());

					for (auto& ch : channels_)
					{
						auto name = ch->name();

						if (std::regex_match(name, re))
						{
							ch->unsubscribe(sub_ptr);
						}
					}
				}

				template <typename... Args>
				void publish_parttern(std::string_view pattern, Args&&... args)
				{
					std::regex re(pattern.data());

					for (auto& ch : channels_)
					{
						auto name = ch->name();

						if (std::regex_match(name, re))
						{
							ch->invoke(name, std::forward_like<Args>(args)...);
						}
					}
				}

			private:
				std::map<std::size_t, std::shared_ptr<Channel>> channels_;
			};
		} // namespace detail
	} // namespace channel
} // namespace aquarius