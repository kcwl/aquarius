#pragma once
#include <aquarius/impl/body_of.hpp>
#include <aquarius/impl/flex_buffer.hpp>
#include <aquarius/impl/visitor.hpp>
#include <cstddef>

namespace aquarius
{
	namespace impl
	{
		class xmessage : public visitable<int>
		{
		public:
			DEFINE_VISITABLE(int)

		public:
			virtual uint32_t unique_key()
			{
				return 0;
			};

			virtual read_handle_result visit(flex_buffer_t&, visit_mode)
			{
				return read_handle_result::error;
			}
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

			virtual read_handle_result visit(flex_buffer_t& stream, visit_mode mode) override
			{
				if (mode == visit_mode::input)
				{
					return parse_message(stream);
				}

				return to_message(stream);
			}

		private:
			read_handle_result parse_message(flex_buffer_t& stream)
			{
				auto res = header_ptr_->parse_bytes(stream);

				if (res != read_handle_result::ok)
					return res;

				res = body_.parse_bytes(stream);

				if (res != read_handle_result::ok)
				{
					return res;
				}

				return read_handle_result::ok;
			}

			read_handle_result to_message(flex_buffer_t& stream)
			{
				elastic::binary_oarchive oa(stream);
				oa << Number;

				auto res = header_ptr_->to_bytes(stream);

				if (res != read_handle_result::ok)
				{
					return res;
				}

				res = body_.to_bytes(stream);

				if (res != read_handle_result::ok)
				{
					return res;
				}

				return read_handle_result::ok;
			}

		private:
			header_type* header_ptr_;

			body_of<body_type> body_;
		};
	} // namespace impl
} // namespace aquarius