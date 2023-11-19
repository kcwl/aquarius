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

	class xmessage : public detail::visitable<read_handle_result>
	{
	public:
		DEFINE_VISITABLE_NULL(read_handle_result)

	public:
		virtual uint32_t unique_key()
		{
			return 0;
		};

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

		virtual int32_t size()
		{
			return static_cast<int32_t>(bytes_);
		}

		read_handle_result from_binary(flex_buffer_t& stream)
		{
			// auto sz = stream.size();

			// auto res = header_ptr_->parse_bytes(stream);
			// auto distance = elastic::from_binary(*header_ptr_, stream);

			// if (distance == 0)
			//	return read_handle_result::waiting_for_query;

			// bytes_ += sz - stream.size();

			// if (res != read_handle_result::ok)
			//	return res;
			// if (header_ptr_->size_ > stream.size())
			//{
			//
			//	stream.consume(-distance);

			return read_handle_result::ok;
			//}

			if constexpr (!std::is_same_v<body_type, null_body>)
			{
				// if (!body_.ParseFromArray(stream.rdata(), header_ptr_->size_))
				//{
				//	res = read_handle_result::error;
				// }

				// if (res != read_handle_result::ok)
				//{
				//	return res;
				// }

				// auto bytes = elastic::from_binary(body_, stream);

				// if (bytes == 0)
				//	return read_handle_result::waiting_for_query;

				// bytes_ += body_.ByteSizeLong();
				// stream.consume(header_ptr_->size_);
				// return read_handle_result::ok;
			}

			return read_handle_result::ok;
		}

		read_handle_result to_binary(flex_buffer_t& stream)
		{
			// elastic::to_binary(Number, stream);

			////auto res = header_ptr_->to_bytes(stream);
			///*auto res = */elastic::to_binary(*header_ptr_, stream);

			////if (res != read_handle_result::ok)
			////{
			////	return res;
			////}

			// if constexpr (!std::is_same_v<body_type, null_body>)
			//{
			//	// auto buf = body_.SerializeAsString();

			//	// header_ptr_->set_size(buf.size());

			//	// auto res = header_ptr_->to_bytes(stream);

			//	// if (res != read_handle_result::ok)
			//	//{
			//	//	return res;
			//	// }

			//	// if (!buf.empty())
			//	//	oa.save_binary(buf.data(), buf.size());

			//	elastic::to_binary(body_, stream);
			//}

			return read_handle_result::ok;
		}

	private:
		header_type* header_ptr_;

		body_type body_;

		std::size_t bytes_;
	};
} // namespace aquarius