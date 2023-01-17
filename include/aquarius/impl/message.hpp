#pragma once
#include <aquarius/impl/body_of.hpp>
#include <aquarius/impl/flex_buffer.hpp>
#include <aquarius/impl/visitor.hpp>
#include <cstddef>

namespace aquarius
{
	namespace impl
	{
		class xmessage : public visitable<int>, public visitor<flex_buffer_t, int>
		{
		public:
			DEFINE_VISITABLE(int)

			DEFINE_VISITOR(flex_buffer_t, int)

		public:
			virtual uint32_t unique_key()
			{
				return 0;
			};
		};

		template <typename _Header, typename _Body, uint32_t N>
		class message : public xmessage, public fields<_Header>
		{
		public:
			using header_type = _Header;
			using body_type = _Body;

			constexpr static uint32_t Number = N;

			DEFINE_VISITABLE(int)

		public:
			message()
			{
				this->alloc(header_ptr_);
			}

			virtual ~message()
			{
				this->dealloc(header_ptr_);
			}

		public:
			header_type& header() noexcept
			{
				return *header_ptr_;
			}

			const header_type& header() const noexcept
			{
				return *header_ptr_;
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
				if (!header_ptr_->parse_bytes(stream))
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

				if (!header_ptr_->to_bytes(stream))
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
			header_type* header_ptr_;

			body_of<body_type> body_;
		};
	} // namespace impl
} // namespace aquarius