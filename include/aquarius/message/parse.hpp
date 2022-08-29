#pragma once
#include "../stream.hpp"

namespace aquarius
{
	namespace message
	{
		template<typename _Ty, typename _Stream>
		class parse;

		template<typename _Ty>
		class parse<_Ty, ftstream>
		{
			using value_type = typename _Ty::value_type;

		public:
			using stream_type = ftstream;

		public:
			bool parse_bytes(value_type* ptr, ftstream& archive)
			{
				return true;
			}

			bool to_bytes(value_type* ptr, ftstream& archive)
			{
				return true;
			}
		};
	}

	template<typename _Ty>
	using ftstream_parse = message::parse<_Ty, ftstream>;
}