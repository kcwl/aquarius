#pragma once
#include <aquarius/defines.hpp>
#include <aquarius/detail/field.hpp>
#include <aquarius/detail/visitor.hpp>
#include <aquarius/elastic/flex_buffer.hpp>
#include <cstddef>

namespace aquarius
{
	struct null_body
	{};

	class xmessage : public detail::visitable<read_handle_result>
	{
	public:
		DEFINE_VISITABLE_NULL(read_handle_result)

	public:
		virtual uint32_t unique_key()
		{
			return 0;
		};

		virtual int32_t size()
		{
			return 0;
		}
	};

	template <typename _Header, typename _Body, uint32_t N>
	class message : public xmessage, public fields<_Header>
	{
	public:
		using header_type = _Header;
		using body_type = std::conditional_t<std::same_as<_Body, void>, void*, _Body>;

		constexpr static uint32_t Number = N;

	public:
		message()
		{
			this->alloc(header_ptr_);
		}

		virtual ~message()
		{
			this->dealloc(header_ptr_);
		}

		message(const message& other)
		{
			header() = other.header();

			body().Copy(other.body());
		}

		message(message&& other)
		{
			header() = std::move(other.header());

			body().Move(other.body());
		}

		message& operator=(message&& other)
		{
			if (this == std::addressof(other))
				return *this;

			*this = std::move(other);

			return *this;
		}

	public:
		header_type& header() noexcept
		{
			return *header_ptr_;
		}

		const header_type& header() const noexcept
		{
			return *header_ptr_;
		}

		body_type& body() noexcept
		{
			return body_;
		}

		const body_type& body() const noexcept
		{
			return body_;
		}

		virtual uint32_t unique_key() override
		{
			return Number;
		}

		virtual int32_t size()
		{
			return static_cast<int32_t>(bytes_);
		}

		read_handle_result from_binary(flex_buffer_t& stream)
		{
			if (!aquarius::from_binary(*header_ptr_, stream))
				return read_handle_result::header_error;

			if (!body_.from_binary(stream))
				return read_handle_result::body_error;

			return read_handle_result::ok;
		}

		read_handle_result to_binary(flex_buffer_t& stream)
		{
			if (!aquarius::to_binary(*header_ptr_, stream))
				return read_handle_result::header_error;

			if (!body_.to_binary(stream))
				return read_handle_result::body_error;

			bytes_ += stream.size();

			return read_handle_result::ok;
		}

	private:
		header_type* header_ptr_;

		body_type body_;

		std::size_t bytes_;
	};
} // namespace aquarius