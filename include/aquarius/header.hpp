#pragma once
#include <aquarius/detail/defines.hpp>
#include <aquarius/flex_buffer.hpp>
#include <cstdint>

namespace aquarius
{
	struct tcp_header
	{
		uint32_t magic_;
		int32_t size_;
		uint32_t uid_;
		uint32_t append_;

		void set_size(std::size_t sz)
		{
			size_ = static_cast<uint32_t>(sz);
		}

		void clone(const tcp_header& header)
		{
			uid_ = header.uid_;
			append_ = header.append_;
		}
	};

	struct tcp_request_header : tcp_header
	{
		uint32_t reserve_;

	private:
		friend class access;

		template <typename _Archive>
		void serialize(_Archive& ar)
		{
			ar& boost::serialization::base_object<tcp_header>(*this);
			ar& reserve_;
		}

	public:
		detail::read_handle_result parse_bytes(flex_buffer_t& stream)
		{
			boost::archive::binary_iarchive ia(stream);
			ia >> *this;

			return detail::read_handle_result::ok;
		}

		detail::read_handle_result to_bytes(flex_buffer_t& stream)
		{
			boost::archive::binary_oarchive oa(stream);
			oa << *this;

			return detail::read_handle_result::ok;
		}
	};

	struct tcp_response_header : tcp_header
	{
		int32_t result_;

	private:
		friend class access;

		template <typename _Archive>
		void serialize(_Archive& ar)
		{
			ar& boost::serialization::base_object<tcp_header>(*this);
			ar& result_;
		}

	public:
		detail::read_handle_result parse_bytes(flex_buffer_t& stream)
		{
			boost::archive::binary_iarchive ia(stream);
			ia >> *this;

			return detail::read_handle_result::ok;
		}

		detail::read_handle_result to_bytes(flex_buffer_t& stream)
		{
			boost::archive::binary_oarchive oa(stream);
			oa << *this;

			return detail::read_handle_result::ok;
		}
	};
} // namespace aquarius