#pragma once
#include <cstddef>
#include "body_of.hpp"
#include "header_of.hpp"
#include <aquarius/core/visitor.hpp>

namespace aquarius
{
	namespace proto
	{
		struct empty_body
		{
			std::string SerializeAsString() { return {}; }

			template<typename _Ty>
			bool ParseFromArray([[maybe_unused]] _Ty* data, [[maybe_unused]] std::size_t bytes) { return true; }

			int ByteSize() { return 0; }
		};

		template<typename _Streambuf = flex_buffer_t>
		class basic_message : public aquarius::core::visitable<int>
		{
		public:
			using streambuf_t = _Streambuf;

		public:
			virtual uint32_t unique_key() = 0;

			virtual bool parse_message(streambuf_t& archive) = 0;

			virtual bool to_message(streambuf_t& archive) = 0;
		};

		using xmessage = basic_message<>;

		template<typename _Header, typename _Body, uint32_t N>
		class message
			: public xmessage
			, private header_of<_Header, basic_message<>::streambuf_t>
		{
		public:
			using header_type = _Header;
			using body_type = _Body;

			using streambuf_t = typename basic_message<>::streambuf_t;

			using base_type = header_of<_Header, basic_message<>::streambuf_t>;

			constexpr static uint32_t Number = N;

		public:
			header_type* header() noexcept
			{
				return this->get();
			}

			const header_type* header() const noexcept
			{
				return this->get();
			}

			body_type& body() noexcept
			{
				return *body_.get();
			}

			const body_type& body() const noexcept
			{
				return *body_.get();
			}

			virtual uint32_t unique_key() override
			{
				return Number;
			}

			virtual int accept(aquarius::context* v) override
			{
				return accept_impl(this, v);
			}

			virtual bool parse_message(streambuf_t& stream) override
			{
				if (!this->parse_bytes(stream))
				{
					return false;
				}

				if (!body_.parse_bytes(stream))
				{
					return false;
				}

				return true;
			}

			virtual bool to_message(streambuf_t& stream) override
			{
				elastic::binary_oarchive oa(stream);
				oa << Number;

				if (!this->to_bytes(stream))
				{
					return false;
				}

				if (!body_.to_bytes(stream))
				{
					return false;
				}

				return true;
			}

		private:
			body_of<body_type, basic_message<>::streambuf_t> body_;
		};
	}
}