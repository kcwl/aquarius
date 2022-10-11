#pragma once
#include "../io.hpp"

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
				constexpr auto sz = sizeof(value_type);

				if (archive.size() < sz)
					return false;

				*ptr = archive.get<value_type>();

				return true;
			}

			bool to_bytes(value_type* ptr, ftstream& archive)
			{
				archive.put(ptr, sizeof(value_type));

				return true;
			}
		};
	}

	template<typename _Ty>
	using ftstream_parse = message::parse<_Ty, ftstream>;
}