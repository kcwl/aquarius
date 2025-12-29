#pragma once
#include "basic_channel.hpp"
#include "payload.hpp"

namespace aquarius
{
	namespace serviced
	{
		struct round_robin_less
		{
			template <typename T>
			bool operator()(const std::shared_ptr<T>& lhs, const std::shared_ptr<T>& rhs) const
			{
				if (!lhs || !rhs)
					return false;

				return lhs->id() < rhs->id();
			}
		};

		template <typename Session>
		using channel = basic_channel<Session, round_robin, round_robin_less>;

	} // namespace serviced
} // namespace aquarius