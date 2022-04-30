#pragma once

namespace aquarius
{
	namespace msg
	{
		template<bool Request,typename Fields>
		class header;

		template<typename Fields>
		class header<true, Fields> : public Fields
		{
		public:
			using is_request = std::true_type;

		public:
			header()
				: Fields()
			{

			}

			header(Fields::value_type value)
				: Fields(value)
			{

			}

			header(const header&) = default;

			header(header&&) = default;

			header& operator=(const header&) = default;

			~header() = default;
		};


		template<typename Fields>
		class header<false, Fields> : public Fields
		{
		public:
			using is_request = std::false_type;

			header()
				: Fields()
			{

			}

			header(const header&) = default;

			header(header&&) = default;

			~header() = default;
		};
	}
}