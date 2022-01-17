#pragma once
#include "filed.hpp"


namespace aquarius
{
	namespace tcp
	{
		template<typename Body, typename Fields>
		class basic_body : public Fields
		{
		public:
			using is_request_body = std::true_type;

		public:
			basic_body()
				: Fields()
			{
				
			}
		};

		template<typename T>
		using body = basic_body<T, body_fields<T>>;
	}
}