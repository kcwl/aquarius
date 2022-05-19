#pragma once
#include "header_fields.hpp"

namespace aquarius
{
	namespace msg
	{
		namespace detail
		{
			template<bool Request, typename Fields>
			class header;

			template<typename Fields>
			class header<true, Fields> : public Fields
			{
			public:
				using value_type = typename Fields;

			public:
				using is_request = std::true_type;

			public:
				header() = default;

				header(const header&) = default;

				header(header&&) = default;

				header& operator=(const header&) = default;

				~header() = default;
			};


			template<typename Fields>
			class header<false, Fields> : public Fields
			{
			public:
				using value_type = typename Fields;

			public:
				using is_request = std::false_type;

				header() = default;

				header(const header&) = default;

				header(header&&) = default;

				~header() = default;
			};
		}
	}
}