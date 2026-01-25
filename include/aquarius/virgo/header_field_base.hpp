#pragma once
#include <cstdint>

namespace aquarius
{
	class header_field_base
	{
	public:
		header_field_base()
			: seq_number_()
		{

		}

		virtual ~header_field_base() = default;

	public:
		header_field_base(const header_field_base& other)
			: seq_number_(other.seq_number_)
		{}

		header_field_base(header_field_base&& other) noexcept
			: seq_number_(std::exchange(other.seq_number_, 0))
		{}

		header_field_base& operator=(const header_field_base& other)
		{
			if (std::addressof(other) != this)
			{
				seq_number_ = other.seq_number_;
			}

			return *this;
		}

		header_field_base& operator=(header_field_base&& other) noexcept
		{
			if (std::addressof(other) != this)
			{
				seq_number_ = std::exchange(other.seq_number_, 0);
			}

			return *this;
		}

	public:
		uint32_t seq_number() const
		{
			return seq_number_;
		}

		void seq_number(uint32_t seq)
		{
			seq_number_ = seq;
		}

	protected:
		uint32_t seq_number_;
	};
} // namespace aquarius