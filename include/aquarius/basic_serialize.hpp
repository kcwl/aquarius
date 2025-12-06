#pragma once
#include <aquarius/serialize/flex_buffer.hpp>

namespace aquarius
{
	template <typename Parse>
	class basic_serialize
	{
	public:
		basic_serialize()
			: parse_()
		{}
		virtual ~basic_serialize() = default;

		basic_serialize(const basic_serialize& other)
			: parse_(other.parse_)
		{}

		basic_serialize& operator=(const basic_serialize& other)
		{
			if (this != std::addressof(other))
			{
				parse_ = other.parse_;
			}

			return *this;
		}

		basic_serialize(basic_serialize&& other) noexcept
			: parse_(std::move(other.parse_))
		{}

		basic_serialize& operator=(basic_serialize&& other) noexcept
		{
			if (this != std::addressof(other))
			{
				parse_ = std::exchange(other.parse_, {});
			}

			return *this;
		}

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

	protected:
		Parse parse_;
	};
} // namespace aquarius