#pragma once

namespace aquarius
{
	namespace tcp
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

		template<typename Fields>
		using reqeust_header = header<true, Fields>;
		
		template<typename Fields>
		using response_header = header<false, Fields>;
	}
}