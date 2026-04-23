#include "subscriber.h"

namespace aquarius
{
	namespace serviced
	{
		subscriber::subscriber(std::size_t id)
			: id_(id)
		{}

		std::size_t subscriber::id() const
		{
			return id_;
		}

		void subscriber::id(std::size_t v)
		{
			id_ = v;
		}
	} // namespace serviced
} // namespace aquarius