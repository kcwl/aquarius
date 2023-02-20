#pragma once
#include <aquarius/core/defines.hpp>
#include <aquarius/core/flex_buffer.hpp>
#include <cstdint>

namespace aquarius
{
	namespace tcp
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
			core::read_handle_result parse_bytes(core::flex_buffer_t& stream)
			{
				core::iarchive ia(stream);
				ia >> *this;

				return core::read_handle_result::ok;
			}

			core::read_handle_result to_bytes(core::flex_buffer_t& stream)
			{
				core::oarchive oa(stream);
				oa << *this;

				return core::read_handle_result::ok;
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
			core::read_handle_result parse_bytes(core::flex_buffer_t& stream)
			{
				core::iarchive ia(stream);
				ia >> *this;

				return core::read_handle_result::ok;
			}

			core::read_handle_result to_bytes(core::flex_buffer_t& stream)
			{
				core::oarchive oa(stream);
				oa << *this;

				return core::read_handle_result::ok;
			}
		};
	} // namespace impl
} // namespace aquarius