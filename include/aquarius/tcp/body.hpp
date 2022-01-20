#pragma once
#include <array>
#include "../detail/stream.hpp"

namespace aquarius
{
	namespace tcp
	{
		struct body_helpter
		{
			template<typename T>
			static inline void serialize(T* body, streambuf& buf)
			{
				buf >> *body;
			}

			template<typename T>
			static inline streambuf& deserialize(T* body)
			{
				static streambuf buf;

				buf.clear();

				buf << *body;

				return buf;
			}
		};
	}
}

#define SERIALIZATION \
void serialize(aquarius::streambuf& buf)\
{ \
	return aquarius::tcp::body_helpter::serialize(this, buf);\
}\
aquarius::streambuf& deserialize()\
{\
	return aquarius::tcp::body_helpter::deserialize(this);\
}