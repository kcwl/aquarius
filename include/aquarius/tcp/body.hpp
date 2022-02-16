#pragma once
#include <array>
#include "../stream.hpp"

namespace aquarius
{
	namespace tcp
	{
		struct body_helpter
		{
			template<typename T>
			static inline void serialize(T* body, ftstream& buf)
			{
				buf >> *body;
			}

			template<typename T>
			static inline ftstream& deserialize(T* body)
			{
				static ftstream buf;

				buf.clear();

				buf << *body;

				return buf;
			}
		};
	}
}

#define SERIALIZATION \
void serialize(aquarius::ftstream& buf)\
{ \
	return aquarius::tcp::body_helpter::serialize(this, buf);\
}\
aquarius::ftstream& deserialize()\
{\
	return aquarius::tcp::body_helpter::deserialize(this);\
}