#pragma once
#include <aquarius/asio.hpp>
#include <memory>
#include <string_view>

namespace aquarius
{
	namespace serviced
	{
		template <typename T>
		struct subscribe_task
		{
			using return_type = void;

			subscribe_task(std::string_view t, std::shared_ptr<T> role)
				: topic(t)
				, role(role)
			{}

			template <typename Core>
			void operator()(std::shared_ptr<Core> core)
			{
				core->subscribe(topic, role);
			}

			std::string_view topic;

			std::shared_ptr<T> role;
		};

		template <typename T>
		struct publish_task
		{
			using return_type = void;

			publish_task(std::string_view t, T task)
				: topic(t)
				, task(task)
			{}

			template <typename Core>
			void operator()(std::shared_ptr<Core> core)
			{
				core->publish(topic, task);
			}

			std::string_view topic;

			T task;
		};
	} // namespace serviced
} // namespace aquarius