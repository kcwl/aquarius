#pragma once
#include <aquarius/core/uuid.hpp>
#include <aquarius/message/impl/header.hpp>
#include <aquarius/message/impl/protocol.hpp>

namespace aquarius
{
	template <typename _Header, std::size_t N>
	class tcp_header : virtual public impl::basic_header
	{
	public:
		using header_type = _Header;

		using base_type = impl::basic_header;

		using typename base_type::virtual_base_type;

		constexpr static std::size_t Number = N;

	public:
		tcp_header()
			: uuid_(invoke_uuid<uint32_t>())
			, header_()
			, pos_(0)
		{
			
		}

		virtual ~tcp_header() = default;

	public:
		tcp_header(tcp_header&& other) noexcept
			: base_type(std::move(other))
			, uuid_(other.uuid_)
			, header_(std::move(other.header_))
		{
			other.uuid_ = 0;
		}

		tcp_header& operator=(const tcp_header&) = default;

	public:
		header_type* header() noexcept
		{
			return &header_;
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

			std::swap(header_, other.header_);
		}

	protected:
		bool from_binary(flex_buffer_t& stream)
		{
			if (!virtual_base_type::from_binary(stream))
				return false;

			elastic::from_binary(uuid_, stream);

			elastic::from_binary(header_, stream);

			return true;
		}

		bool to_binary(flex_buffer_t& stream)
		{
			elastic::to_binary(Number, stream);

			pos_ = static_cast<int32_t>(stream.pubseekoff(0, std::ios::cur, std::ios::in));

			stream.commit(2);

			std::size_t current = stream.size();

			elastic::to_binary(uuid_, stream);

			elastic::to_binary(header_, stream);

			this->add_length(stream.size() - current);

			return true;
		}

	private:
		uint32_t uuid_;

		header_type header_;

		int32_t pos_;
	};

	class http_header : public impl::basic_header
	{};
} // namespace aquarius