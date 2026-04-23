#pragma once
#include <string>

namespace aquarius
{
	namespace serviced
	{
		class subscriber
		{
		public:
			subscriber(std::size_t id);

		public:
			std::size_t id() const;

			std::size_t& id();

			void id(std::size_t v);

		private:
			std::size_t id_;
		};
	} // namespace serviced
} // namespace aquarius