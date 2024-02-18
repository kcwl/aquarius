#pragma once
#include <aquarius/defines.hpp>
#include <aquarius/detail/field.hpp>
#include <aquarius/detail/visitor.hpp>
#include <aquarius/elastic.hpp>
#include <cstddef>
#include <aquarius/error_code.hpp>

namespace aquarius
{
	class basic_context;
}

namespace aquarius
{
	struct null_body
	{};

	class basic_message : public detail::visitable
	{
	public:
		basic_message() = default;
		virtual ~basic_message() = default;

	public:
		DEFINE_VISITABLE()
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
			: header_ptr_(other.header_ptr_)
			, body_(other.body_)
		{
			other.header_ptr_ = nullptr;

			body_type{}.swap(other.body_);
		}

		message& operator=(message&& other)
		{
			message(std::move(other)).swap(*this);

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

		error_code from_binary(flex_buffer_t& stream, error_code& ec)
		{
			if (!elastic::from_binary(*header_ptr_, stream))
				return ec = system::system_errc::invalid_stream;

			elastic::from_binary(body_, stream);

			ec = error_code{};

			return ec;
		}

		error_code to_binary(flex_buffer_t& stream, error_code& ec)
		{
			if(!elastic::to_binary(Number, stream))
				return ec = system::system_errc::invalid_stream;

			flex_buffer_t body_buffer{};

			elastic::to_binary(body_, body_buffer);

			header_ptr_->size = body_buffer.size();

			flex_buffer_t header_buffer{};

			elastic::to_binary(*header_ptr_, header_buffer);

			std::size_t total_size = header_buffer.size() + header_ptr_->size;

			elastic::to_binary(total_size, stream);

			stream.append(std::move(header_buffer));

			stream.append(std::move(body_buffer));

			ec = error_code{};

			return ec;
		}

	private:
		void swap(message& other)
		{
			auto tmp_ptr = this->header_ptr_;
			this->header_ptr_ = other.header_ptr_;
			other.header_ptr_ = tmp_ptr;

			other.body().swap(this->body_);
		}

	private:
		header_type* header_ptr_;

		body_type body_;
	};
} // namespace aquarius