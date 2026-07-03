#pragma once
#include <aquarius/detail/flex_buffer.hpp>
#include <boost/describe.hpp>

namespace aquarius
{
	class serialization
	{
	public:
		virtual void to_binary(flex_buffer&) = 0;
		virtual void from_binary(flex_buffer&) = 0;
		virtual void to_json(flex_buffer&) = 0;
		virtual void from_json(flex_buffer&) = 0;
		virtual void to_kv(flex_buffer&) = 0;
		virtual void from_kv(flex_buffer&) = 0;
	};

	enum class serialize_way
	{
		binary,
		json,
		kv
	};

} // namespace aquarius

#define AQUARIUS_SERIALIZE_STRUCT(S,Members) BOOST_DESCRIBE_STRUCT(S, (), Members)
#define AQUARIUS_SERIALIZE_ENUM(S,Members) BOOST_DESCRIBE_ENUM(S, (), Members)