#pragma once
#include <aquarius/impl/flex_buffer.hpp>
#include <cstdint>
#include <aquarius/impl/defines.hpp>

namespace aquarius
{
	namespace impl
	{
		struct tcp_header
		{
			uint32_t magic_;
			int32_t size_;

			void clone(const tcp_header& header)
			{
				magic_ = header.magic_;
				size_ = header.size_;
			}
		};

		struct tcp_request_header : tcp_header
		{
			uint32_t uid_;

		private:
			friend class elastic::access;

			template <typename _Archive>
			void serialize(_Archive& ar)
			{
				ar& elastic::serialize::base_object<tcp_header>(*this);
				ar& uid_;
			}

		public:
			read_handle_result parse_bytes(flex_buffer_t& stream)
			{
				iarchive ia(stream);
				ia >> *this;

				return read_handle_result::ok;
			}

			read_handle_result to_bytes(flex_buffer_t& stream)
			{
				oarchive oa(stream);
				oa << *this;

				return read_handle_result::ok;
			}
		};

		struct tcp_response_header : tcp_header
		{
			int32_t result_;

		private:
			friend class elastic::access;

			template <typename _Archive>
			void serialize(_Archive& ar)
			{
				ar& elastic::serialize::base_object<tcp_header>(*this);
				ar& result_;
			}

		public:
			read_handle_result parse_bytes(flex_buffer_t& stream)
			{
				iarchive ia(stream);
				ia >> *this;

				return read_handle_result::ok;
			}

			read_handle_result to_bytes(flex_buffer_t& stream)
			{
				oarchive oa(stream);
				oa << *this;

				return read_handle_result::ok;
			}
		};
	} // namespace impl
} // namespace aquarius