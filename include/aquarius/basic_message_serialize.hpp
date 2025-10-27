#pragma once
#include <aquarius/serialize/flex_buffer.hpp>

namespace aquarius
{
	template <typename Parse>
	class basic_message_serialize
	{
	public:
		basic_message_serialize() = default;
		virtual ~basic_message_serialize() = default;

	public:
		virtual void serialize(flex_buffer& buffer) = 0;

		virtual void deserialize(flex_buffer& buffer) = 0;

	public:
		template <typename T>
		void parse_to(T&& value, flex_buffer& buffer)
		{
			return parse_.to_datas(std::forward<T>(value), buffer);
		}

		template <typename T>
		T parse_from(flex_buffer& buffer)
		{
			return parse_.template from_datas<T>(buffer);
		}

	private:
		Parse parse_;
	};
} // namespace aquarius