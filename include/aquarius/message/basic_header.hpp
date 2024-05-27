#pragma once
#include <aquarius/message/basic_message.hpp>
#include <aquarius/core/uuid.hpp>

namespace aquarius
{
	template <std::size_t N>
	class basic_header : public basic_message
	{
	public:
		using header_type = basic_header<N>;

		using base_type = basic_message;

		constexpr static std::size_t Number = N;

	public:
		basic_header()
			: uuid_(invoke_uuid<uint32_t>())
			, pos_(0)
		{
		}
		virtual ~basic_header() = default;

	public:
		basic_header(const basic_header&) = default;

		basic_header(basic_header&& other)
			: uuid_(other.uuid_)
			, pos_(other.pos_)
		{
			other.uuid_ = 0;
		}

		basic_header& operator=(const basic_header&) = default;

		basic_header& operator=(basic_header&&) = default;

	public:
		virtual void set_uuid(uint32_t uuid) override
		{
			uuid_ = uuid;
		}

		virtual uint32_t uuid() const override
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

		void swap(basic_header& other)
		{
			base_type::swap(other);

			std::swap(session_id_, other.session_id_);

			std::swap(uuid_, other.uuid_);

			std::swap(pos_, other.pos_);
		}

	protected:
		virtual error_code from_binary(flex_buffer_t& stream, error_code& ec) override
		{
			ec = base_type::from_binary(stream, ec);

			if (!ec)
				return ec;

			elastic::from_binary(uuid_, stream);

			return ec = error_code_result::ok;
		}

		virtual error_code to_binary(flex_buffer_t& stream, error_code& ec) override
		{
			elastic::to_binary(Number, stream);

			pos_ = static_cast<int32_t>(stream.pubseekoff(0, std::ios::cur, std::ios::in));

			stream.commit(2);

			std::size_t current = stream.size();

			elastic::to_binary(uuid_, stream);

			this->add_length(stream.size() - current);

			return ec = error_code_result::ok;
		}

	public:
		uint32_t session_id_;

	private:
		uint32_t uuid_;

		int32_t pos_;
	};
}