#pragma once
#include <aquarius/impl/body_of.hpp>
#include <aquarius/impl/context.hpp>
#include <aquarius/impl/flex_buffer.hpp>
#include <aquarius/impl/header_of.hpp>
#include <aquarius/impl/visitor.hpp>
#include <cstddef>

namespace aquarius
{
	namespace impl
	{
		struct empty_body
		{
			std::string SerializeAsString()
			{
				return {};
			}

			template <typename _Ty>
			bool ParseFromArray([[maybe_unused]] _Ty* data, [[maybe_unused]] std::size_t bytes)
			{
				return true;
			}

			int ByteSize()
			{
				return 0;
			}
		};

		class xmessage : public visitable<int>
		{
		public:
			using streambuf_t = flex_buffer_t;

			DEFINE_VISITOR()

		public:
			virtual uint32_t unique_key()
			{
				return 0;
			};

			virtual bool parse_message([[maybe_unused]] streambuf_t& archive)
			{
				return false;
			}

			virtual bool to_message([[maybe_unused]] streambuf_t& archive)
			{
				return false;
			}
		};

		template <typename _Header, typename _Body, uint32_t N>
		class message : public xmessage, private header_of<_Header, xmessage::streambuf_t>
		{
		public:
			using header_type = _Header;
			using body_type = _Body;

			using streambuf_t = typename xmessage::streambuf_t;

			using base_type = header_of<_Header, streambuf_t>;

			constexpr static uint32_t Number = N;

			DEFINE_VISITOR()

		public:
			header_type& header() noexcept
			{
				return *this->get();
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
			body_of<body_type, streambuf_t> body_;
		};
	} // namespace impl
} // namespace aquarius