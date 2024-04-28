#pragma once
#include <aquarius/core/uuid.hpp>
#include <aquarius/message/field.hpp>
#include <aquarius/message/impl/header.hpp>
#include <aquarius/message/impl/protocol.hpp>

namespace aquarius
{
	template <typename _Header, std::size_t N>
	class tcp_header : virtual public impl::basic_header, public fields<_Header>
	{
	public:
		using header_type = _Header;

		using field_base_type = fields<header_type>;

		using base_type = impl::basic_header;

		using typename base_type::virtual_base_type;

		constexpr static std::size_t Number = N;

	public:
		tcp_header()
			: uuid_(invoke_uuid<uint32_t>())
			, header_ptr_(nullptr)
			, pos_(0)
		{
			this->alloc(header_ptr_);
		}

		virtual ~tcp_header()
		{
			this->dealloc(header_ptr_);
		}

	public:
		tcp_header(tcp_header&& other) noexcept
			: base_type(std::move(other))
			, uuid_(other.uuid_)
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

		void set_uuid(uint32_t uuid)
		{
			uuid_ = uuid;
		}

		uint32_t uuid() const
		{
			return uuid_;
		}

		bool complete(flex_buffer_t& stream)
		{
			const auto cur_pos = stream.pubseekoff(0, std::ios::cur, std::ios::in);

			stream.pubseekpos(pos_, std::ios::in);

			std::memcpy(stream.rdata(), &this->length_, 2);

			stream.pubseekpos(cur_pos, std::ios::in);

			pos_ = 0;

			return true;
		}

		void swap(tcp_header& other)
		{
			virtual_base_type::swap(other);

			std::swap(uuid_, other.uuid_);

			auto tmp_ptr = this->header_ptr_;

			this->header_ptr_ = other.header_ptr_;

			other.header_ptr_ = tmp_ptr;
		}

	protected:
		bool from_binary(flex_buffer_t& stream)
		{
			if (!virtual_base_type::from_binary(stream))
				return false;

			elastic::from_binary(uuid_, stream);

			elastic::from_binary(*header_ptr_, stream);

			return true;
		}

		bool to_binary(flex_buffer_t& stream)
		{
			elastic::to_binary(Number, stream);

			pos_ = static_cast<int32_t>(stream.pubseekoff(0, std::ios::cur, std::ios::in));

			stream.commit(2);

			std::size_t current = stream.size();

			elastic::to_binary(uuid_, stream);

			elastic::to_binary(*header_ptr_, stream);

			this->add_length(stream.size() - current);

			return true;
		}

	private:
		uint32_t uuid_;

		header_type* header_ptr_;

		int32_t pos_;
	};

	class http_header : public impl::basic_header
	{};
} // namespace aquarius