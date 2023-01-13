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
		class xmessage : public visitable<int>
		{
		public:
			DEFINE_VISITOR()

		public:
			virtual uint32_t unique_key()
			{
				return 0;
			};
		};

		template <typename _Header, typename _Body, uint32_t N>
		class message : public xmessage, private header_of<_Header>, public visitor<flex_buffer_t, int>
		{
		public:
			using header_type = _Header;
			using body_type = _Body;

			using base_type = header_of<_Header>;

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

			virtual int visit(flex_buffer_t* stream, visit_mode mode) override
			{
				if (mode == visit_mode::input)
				{
					return parse_message(*stream);
				}

				return to_message(*stream);
			}

		private:
			bool parse_message(flex_buffer_t& stream)
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

			bool to_message(flex_buffer_t& stream)
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
			body_of<body_type> body_;
		};
	} // namespace impl
} // namespace aquarius