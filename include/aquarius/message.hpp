#pragma once
#include <aquarius/defines.hpp>
#include <aquarius/detail/field.hpp>
#include <aquarius/detail/visitor.hpp>
#include <aquarius/flex_buffer.hpp>
#include <cstddef>

namespace aquarius
{
	struct null_body
	{};

	class xmessage : public detail::visitable<int>
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

		virtual int32_t size()
		{
			return 0;
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

		virtual read_handle_result visit(flex_buffer_t& stream, visit_mode mode) override
		{
			if (mode == visit_mode::input)
			{
				return parse_message(stream);
			}

			return to_message(stream);
		}

		virtual int32_t size()
		{
			return bytes_;
		}

	private:
		read_handle_result parse_message(flex_buffer_t& stream)
		{
			auto sz = stream.size();

			auto res = header_ptr_->parse_bytes(stream);

			bytes_ += sz - stream.size();

			if (res != read_handle_result::ok)
				return res;

			if constexpr (!std::is_same_v<body_type, null_body>)
			{
				if (!body_.ParseFromArray(stream.rdata(), header_ptr_->size_))
				{
					res = read_handle_result::error;
				}

				if (res != read_handle_result::ok)
				{
					return res;
				}

				bytes_ += body_.ByteSizeLong();
				stream.consume(header_ptr_->size_);
			}

			return read_handle_result::ok;
		}

		read_handle_result to_message(flex_buffer_t& stream)
		{
			boost::archive::binary_oarchive oa(stream);
			oa << Number;

			if constexpr (!std::is_same_v<body_type, null_body>)
			{
				auto buf = body_.SerializeAsString();

				header_ptr_->set_size(buf.size());

				auto res = header_ptr_->to_bytes(stream);

				if (res != read_handle_result::ok)
				{
					return res;
				}

				if (!buf.empty())
					oa.save_binary(buf.data(), buf.size());
			}

			return read_handle_result::ok;
		}

	private:
		header_type* header_ptr_;

		body_type body_;

		std::size_t bytes_;
	};
} // namespace aquarius