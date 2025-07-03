#pragma once
#include <aquarius/channel.hpp>

namespace serviced
{
	class channel_role
	{
		using subscriber = aquarius::channel::subscriber;

		using channel = aquarius::channel::default_channel;

		using topic = typename channel::topic;

		using publisher = aquarius::channel::publisher;

	public:
		explicit channel_role(std::size_t id)
			: id_(id)
			, sub_ptr_(std::make_shared<subscriber>(id_))
		{}

	public:
		std::size_t id() const
		{
			return id_;
		}

		void subscribe(topic tpc)
		{
			sub_ptr_->template call<aquarius::channel::default_channel>(tpc);
		}

		template <typename... Args>
		void publish(topic tpc, Args&&... args)
		{
			pub_.call<channel>(tpc, std::forward<Args>(args)...);
		}

	private:
		std::size_t id_;

		std::shared_ptr<subscriber> sub_ptr_;

		publisher pub_;
	};
} // namespace serviced