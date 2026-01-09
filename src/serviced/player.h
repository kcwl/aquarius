#pragma once
#include <aquarius/asio.hpp>
#include <aquarius/serialize/flex_buffer.hpp>
#include <atomic>
#include <memory>

namespace aquarius
{
	namespace serviced
	{
		class player : public std::enable_shared_from_this<player>
		{
		public:
			player(std::size_t id);

			virtual ~player() = default;

		public:
			std::size_t id() const;

			auto feedback(flex_buffer& buffer) -> awaitable<void>;

			void complete();

			int32_t weight() const;

			void weight(int32_t w);

		private:
			std::size_t id_;

			std::atomic_size_t task_total_;

			int32_t weight_;
		};
	} // namespace serviced
} // namespace aquarius