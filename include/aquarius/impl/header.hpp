#pragma once
#include <cstdint>
#include <elastic.hpp>

namespace aquarius
{
	namespace impl
	{
		struct tcp_header
		{
			uint32_t magic_;
			int32_t size_;
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
		};
	} // namespace impl
} // namespace aquarius