#pragma once
#include <aquarius/message/field.hpp>
#include <aquarius/message/impl/header.hpp>
#include <aquarius/message/impl/protocol.hpp>
#include <aquarius/core/uuid.hpp>

namespace aquarius
{
	template <typename _Header, std::size_t N>
	class tcp_header : public impl::basic_header, public fields<_Header>
	{
		using basic_header_type = impl::basic_header;

	public:
		using header_type = _Header;

		using field_base_type = fields<header_type>;

		constexpr static std::size_t Number = N;

	public:
		tcp_header()
			: uuid_(uuid::invoke())
		{
			this->alloc(header_ptr_);
		}

		virtual ~tcp_header()
		{
			this->dealloc(header_ptr_);
		}

	public:
		tcp_header(tcp_header&& other)
			: uuid_(other.uuid_)
			, header_ptr_(other.header_ptr_)
		{
			other.uuid_ = 0;
			other.header_ptr_ = nullptr;
		}

		tcp_header& operator=(const tcp_header&) = default;

	public:
		header_type* header() noexcept
		{
			return header_ptr_;
		}

		void set_uuid(std::size_t uuid)
		{
			uuid_ = uuid;
		}

		std::size_t uuid() const
		{
			return uuid_;
		}

		error_code complete(flex_buffer_t& stream, error_code& ec)
		{
			stream.normalize();

			flex_buffer_t buffer{};
			buffer.swap(stream);

			if (!elastic::to_binary(Number, stream))
				return ec = system_errc::invalid_stream;

			if (basic_header_type::to_binary(stream, ec) != system_errc::ok)
				return ec;

			stream.append(std::move(buffer));

			return {};
		}

		void swap(tcp_header& other)
		{
			basic_header_type::swap(other);

			std::swap(uuid_, other.uuid_);

			auto tmp_ptr = this->header_ptr_;

			this->header_ptr_ = other.header_ptr_;

			other.header_ptr_ = tmp_ptr;
		}

	protected:
		error_code from_binary(flex_buffer_t& stream, error_code& ec)
		{
			if (basic_header_type::from_binary(stream, ec) != system_errc::ok)
				return ec;

			if(!elastic::from_binary(uuid_, stream))
				return ec = system_errc::invalid_stream;

			if (!elastic::from_binary(*header_ptr_, stream))
				return ec = system_errc::invalid_stream;

			return ec = error_code{};;
		}

		error_code to_binary(flex_buffer_t& stream, error_code& ec)
		{
			std::size_t current = stream.size();

			elastic::to_binary(uuid_, stream);

			if (!elastic::to_binary(*header_ptr_, stream))
				return ec = system_errc::invalid_stream;

			this->add_length(stream.size() - current);

			return ec = {};
		}

	private:
		std::size_t uuid_;

		header_type* header_ptr_;
	};

	class http_header : public impl::basic_header
	{};
} // namespace aquarius