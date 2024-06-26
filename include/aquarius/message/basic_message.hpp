#pragma once
#include <aquarius/context/visitable.hpp>
#include <aquarius/core/elastic.hpp>

namespace aquarius
{
	class basic_context;
} // namespace aquarius

namespace aquarius
{
	class basic_message : public visitable<flex_buffer_t, basic_context>
	{
		using length_t = uint16_t;

	public:
		basic_message()
			: length_()
			, single_(true)
		{}

		virtual ~basic_message() = default;

	public:
		basic_message(const basic_message&) = default;

		basic_message(basic_message&& other)
			: length_(other.length_)
			, single_(other.single_)
		{
			other.length_ = 0;
			other.single_ = false;
		}

		basic_message& operator=(const basic_message&) = default;
		basic_message& operator=(basic_message&&) = default;

	public:
		AQUARIUS_VISITABLE_BASE()

	public:
		void add_length(std::size_t length)
		{
			length_ += static_cast<length_t>(length);
		}

		length_t length() const
		{
			return length_;
		}

		void reset()
		{
			length_ = 0;
		}

		void swap(basic_message& other)
		{
			std::swap(this->length_, other.length_);
			std::swap(this->single_, other.single_);
		}

	public:
		virtual void set_uuid(uint32_t /*uuid*/)
		{
			return;
		}

		virtual uint32_t uuid() const
		{
			return 0;
		}

	protected:
		virtual error_code from_binary(flex_buffer_t& stream, error_code& ec)
		{
			constexpr auto size_uint16 = sizeof(uint16_t);

			if (stream.size() < size_uint16)
				return ec = errc::incomplete;

			std::memcpy((char*)&length_, stream.wdata(), size_uint16);

			stream.consume(size_uint16);

			if (length_ > stream.size())
			{
				stream.consume(-(static_cast<int>(size_uint16)));

				return ec = errc::incomplete;
			}

			if(single_)
				stream.consume(length_);

			return ec = errc::ok;
		}

		virtual error_code to_binary(flex_buffer_t& stream, error_code& ec)
		{
			constexpr auto size_uint16 = sizeof(uint16_t);

			std::memcpy(stream.rdata(), &this->length_, size_uint16);

			stream.commit(size_uint16);

			return ec = errc::ok;
		}

		void set_single(bool single)
		{
			single_ = single;
		}

	protected:
		length_t length_;

		bool single_;
	};

} // namespace aquarius