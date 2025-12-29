#pragma once
#include <aquarius/serialize/flex_buffer.hpp>
#include <atomic>
#include <memory>

namespace aquarius
{
	namespace serviced
	{

		template <typename Session>
		class butler
		{
		public:
			butler(std::shared_ptr<Session> session_ptr)
				: session_ptr_(session_ptr)
			{}

			virtual ~butler() = default;

		public:
			std::size_t id() const
			{
				auto ptr = session_ptr_.lock();
				if (!ptr)
					return static_cast<std::size_t>(-1);

				return ptr->uuid();
			}

			auto dispatch(const flex_buffer& buffer) -> awaitable<void>
			{
				auto ptr = session_ptr_.lock();

				if (!ptr)
					co_return;

				co_await ptr->send_response(buffer);
			}

		private:
			std::weak_ptr<Session> session_ptr_;
		};

		template <typename Session>
		class channel_role : public std::enable_shared_from_this<channel_role<Session>>, public butler<Session>
		{
		public:
			channel_role(std::shared_ptr<Session> session_ptr)
				: butler<Session>(session_ptr)
				, task_total_(0)
				, weight_(0)
			{}

			virtual ~channel_role() = default;

		public:
			template <typename Task>
			void feedback(Task&& task)
			{
				this->dispatch(std::forward<Task>(task));

				complete();
			}

			void complete()
			{
				task_total_--;
			}

			int32_t weight() const
			{
				return weight_;
			}

			void weight(int32_t w)
			{
				weight_ = w;
			}

		private:
			std::atomic_size_t task_total_;

			int32_t weight_;
		};
	} // namespace serviced
} // namespace aquarius