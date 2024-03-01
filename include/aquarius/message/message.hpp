#pragma once
#include <aquarius/elastic.hpp>
#include <aquarius/error_code.hpp>
#include <aquarius/system/defines.hpp>
#include <aquarius/system/field.hpp>
#include <aquarius/system/visitor.hpp>
#include <cstddef>
#include <aquarius/system/uuid.hpp>
#include <aquarius/system/type_traits.hpp>

namespace aquarius
{
	class basic_context;
}

namespace aquarius
{
	struct null_body
	{};

	class basic_message : public system::visitable
	{
	public:
		basic_message() = default;
		virtual ~basic_message() = default;

	public:
		DEFINE_VISITABLE()

		virtual std::size_t uuid()
		{
			return {};
		}

		virtual void set_uuid(std::size_t)
		{
			return;
		}
	};

	template <typename _Header, typename _Body, uint32_t N>
	class message : public basic_message, public fields<_Header>
	{
	public:
		using header_type = _Header;
		using body_type =  _Body;

		constexpr static uint32_t Number = N;

	public:
		message()
			: body_()
			, uid_(uuid::invoke())
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
			, uid_(other.uid_)
		{
			other.header_ptr_ = nullptr;

			if constexpr (system::swap_t<body_type>)
			{
				body_type{}.swap(other.body_);
			}
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
			if (!elastic::from_binary(uid_, stream))
				return ec = system_errc::invalid_stream;

			if (!elastic::from_binary(*header_ptr_, stream))
				return ec = system_errc::invalid_stream;

			elastic::from_binary(body_, stream);

			ec = error_code{};

			return ec;
		}

		error_code to_binary(flex_buffer_t& stream, error_code& ec)
		{
			if (!elastic::to_binary(Number, stream))
				return ec = system_errc::invalid_stream;

			flex_buffer_t uid_buffer{};

			elastic::to_binary(uid_, uid_buffer);

			flex_buffer_t body_buffer{};

			elastic::to_binary(body_, body_buffer);

			header_ptr_->size = body_buffer.size();

			flex_buffer_t header_buffer{};

			elastic::to_binary(*header_ptr_, header_buffer);

			std::size_t total_size = uid_buffer.size() + header_buffer.size() + header_ptr_->size;

			elastic::to_binary(total_size, stream);

			stream.append(std::move(uid_buffer));

			stream.append(std::move(header_buffer));

			stream.append(std::move(body_buffer));

			ec = error_code{};

			return ec;
		}

		virtual void set_uuid(std::size_t uid) override
		{
			uid_ = uid;
		}

		virtual std::size_t uuid() override
		{
			return uid_;
		}

	private:
		void swap(message& other)
		{
			std::swap(uid_, other.uid_);

			auto tmp_ptr = this->header_ptr_;
			this->header_ptr_ = other.header_ptr_;
			other.header_ptr_ = tmp_ptr;

			if constexpr (system::swap_t<body_type>)
			{
				other.body().swap(this->body_);
			}
		}

	private:
		header_type* header_ptr_;

		body_type body_;

		std::size_t uid_;
	};
} // namespace aquarius