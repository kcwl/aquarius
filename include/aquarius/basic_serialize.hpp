#pragma once
#include <aquarius/serialize/flex_buffer.hpp>

namespace aquarius
{
	template <typename Parse>
	class basic_serialize
	{
	public:
		basic_serialize() = default;
		virtual ~basic_serialize() = default;

	public:
		virtual void serialize(flex_buffer& buffer) = 0;

		virtual void deserialize(flex_buffer& buffer) = 0;

	public:
		template <typename T, typename... Args>
		void parse_to(T&& value, flex_buffer& buffer, Args&&... args)
		{
			return parse_.to_datas(std::forward<T>(value), buffer, std::forward<Args>(args)...);
		}

		template <typename T, typename... Args>
		T parse_from(flex_buffer& buffer, Args&&... args)
		{
			return parse_.template from_datas<T>(buffer, std::forward<Args>(args)...);
		}

	private:
		Parse parse_;
	};
} // namespace aquarius