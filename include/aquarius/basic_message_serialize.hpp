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
		virtual void serialize(flex_buffer<char>& buffer) = 0;

		virtual void deserialize(flex_buffer<char>& buffer) = 0;

	public:
		template <typename T, typename U>
		void parse_to(T&& value, flex_buffer<U>& buffer)
		{
			return parse_.to_datas(std::forward<T>(value), buffer);
		}

		template <typename T, typename U>
		T parse_from(flex_buffer<U>& buffer)
		{
			return parse_.template from_datas<T>(buffer);
		}

	private:
		Parse parse_;
	};
} // namespace aquarius