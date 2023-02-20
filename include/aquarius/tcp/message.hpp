#pragma once
#include <aquarius/tcp/body_of.hpp>
#include <aquarius/core/flex_buffer.hpp>
#include <aquarius/context/visitor.hpp>
#include <aquarius/core/defines.hpp>
#include <cstddef>

namespace aquarius
{
	namespace tcp
	{
		class xmessage : public ctx::visitable<int>
		{
		public:
			DEFINE_VISITABLE(int)

		public:
			virtual uint32_t unique_key() { return 0; };

			virtual core::read_handle_result visit(core::flex_buffer_t&, core::visit_mode) { return core::read_handle_result::error; }
		};

		template <typename _Header, typename _Body, uint32_t N>
		class message : public xmessage, public fields<_Header>
		{
		public:
			using header_type = _Header;
			using body_type = std::conditional_t<std::same_as<_Body, void>, void*, _Body>;

			constexpr static uint32_t Number = N;

			DEFINE_VISITABLE(int)

		public:
			message() { this->alloc(header_ptr_); }

			virtual ~message() { this->dealloc(header_ptr_); }

		public:
			header_type& header() noexcept { return *header_ptr_; }

			const header_type& header() const noexcept { return *header_ptr_; }

			body_type& body() noexcept { return *body_.get(); }

			const body_type& body() const noexcept { return *body_.get(); }

			virtual uint32_t unique_key() override { return Number; }

			virtual core::read_handle_result visit(core::flex_buffer_t& stream, core::visit_mode mode) override
			{
				if (mode == core::visit_mode::input)
				{
					return parse_message(stream);
				}

				return to_message(stream);
			}

		private:
			core::read_handle_result parse_message(core::flex_buffer_t& stream)
			{
				auto res = header_ptr_->parse_bytes(stream);

				if (res != core::read_handle_result::ok)
					return res;

				res = body_.parse_bytes(stream);

				if (res != core::read_handle_result::ok)
				{
					return res;
				}

				return core::read_handle_result::ok;
			}

			core::read_handle_result to_message(core::flex_buffer_t& stream)
			{
				core::oarchive oa(stream);
				oa << Number;

				auto res = header_ptr_->to_bytes(stream);

				if (res != core::read_handle_result::ok)
				{
					return res;
				}

				res = body_.to_bytes(stream);

				if (res != core::read_handle_result::ok)
				{
					return res;
				}

				return core::read_handle_result::ok;
			}

		private:
			header_type* header_ptr_;

			body_of<body_type> body_;
		};
	} // namespace impl
} // namespace aquarius