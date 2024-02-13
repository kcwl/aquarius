#pragma once
#include <aquarius/defines.hpp>
#include <aquarius/detail/field.hpp>
#include <aquarius/detail/visitor.hpp>
#include <aquarius/elastic.hpp>
#include <cstddef>

namespace aquarius
{
	class basic_context;
}

namespace aquarius
{
	struct null_body
	{};

	class basic_message : public detail::visitable<read_handle_result>
	{
	public:
		DEFINE_VISITABLE(read_handle_result)
	};

	template <typename _Header, typename _Body, uint32_t N>
	class message : public basic_message, public fields<_Header>
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

		message(message&& other)
			: header_ptr_(std::move(other.header_ptr_))
			, body_(std::move(other.body_))
		{
			other.header_ptr_ = nullptr;

			body_type{}.swap(other.body_);
		}

		message& operator=(message&& other)
		{
			this->header_ptr_ = other.header_ptr_;

			this->body_ = std::move(other.body_);

			other.header_ptr_ = nullptr;

			body_type().swap(other.body_);

			return *this;
		}

	private:
		message(const message& other) = delete;
		message& operator=(const message&) = delete;

	public:
		header_type* header() noexcept
		{
			return header_ptr_;
		}

		body_type& body() noexcept
		{
			return body_;
		}

		read_handle_result from_binary(flex_buffer_t& stream)
		{
			if (!elastic::from_binary(*header_ptr_, stream))
				return read_handle_result::header_error;

			if (!elastic::from_binary(body_, stream))
				return read_handle_result::body_error;

			return read_handle_result::ok;
		}

		read_handle_result to_binary(flex_buffer_t& stream)
		{
			if (!elastic::to_binary(Number, stream))
				return read_handle_result::unknown_error;

			flex_buffer_t body_buffer{};

			if (!elastic::to_binary(body_, body_buffer))
				return read_handle_result::body_error;

			header_ptr_->size = body_buffer.size();

			flex_buffer_t header_buffer{};

			if (!elastic::to_binary(*header_ptr_, header_buffer))
				return read_handle_result::header_error;

			std::size_t total_size = header_buffer.size() + header_ptr_->size;

			if (!elastic::to_binary(total_size, stream))
				return read_handle_result::unknown_error;

			stream.append(std::move(header_buffer));

			stream.append(std::move(body_buffer));

			return read_handle_result::ok;
		}

	private:
		header_type* header_ptr_;

		body_type body_;
	};
} // namespace aquarius