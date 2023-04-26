#pragma once
#include <aquarius/context/visitor.hpp>
#include <aquarius/core/defines.hpp>
#include <aquarius/core/flex_buffer.hpp>
#include <aquarius/tcp/field.hpp>
#include <cstddef>

namespace aquarius
{
	namespace tcp
	{
		struct null_body
		{};

		class xmessage : public ctx::visitable<int>
		{
		public:
			DEFINE_VISITABLE(int)

		public:
			virtual uint32_t unique_key()
			{
				return 0;
			};

			virtual core::read_handle_result visit(core::flex_buffer_t&, core::visit_mode)
			{
				return core::read_handle_result::error;
			}
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
			message()
			{
				this->alloc(header_ptr_);
			}

			virtual ~message()
			{
				this->dealloc(header_ptr_);
			}

			message(const message& other)
			{
				header() = other.header();

				body().Copy(other.body());
			}

			message(message&& other)
			{
				header() = std::move(other.header());

				body().Move(other.body());
			}

			message& operator=(message&& other)
			{
				if (this == std::addressof(other))
					return *this;

				*this = std::move(other);

				return *this;
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
				return body_;
			}

			const body_type& body() const noexcept
			{
				return body_;
			}

			virtual uint32_t unique_key() override
			{
				return Number;
			}

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

				if constexpr (!std::is_same_v<body_type, null_body>)
				{
					if (!body_.ParseFromArray(stream.rdata(), header_ptr_->size_))
					{
						res = core::read_handle_result::error;
					}

					if (res != core::read_handle_result::ok)
					{
						return res;
					}
				}

				return core::read_handle_result::ok;
			}

			core::read_handle_result to_message(core::flex_buffer_t& stream)
			{
				core::oarchive oa(stream);
				oa << Number;

				if constexpr (!std::is_same_v<body_type, null_body>)
				{
					auto buf = body_.SerializeAsString();

					header_ptr_->set_size(buf.size());

					auto res = header_ptr_->to_bytes(stream);

					if (res != core::read_handle_result::ok)
					{
						return res;
					}

					if (!buf.empty())
						oa.save_binary(buf.data(), buf.size());
				}

				return core::read_handle_result::ok;
			}

		private:
			header_type* header_ptr_;

			body_type body_;
		};
	} // namespace tcp
} // namespace aquarius