#pragma once
#include <cstdint>

namespace aquarius
{
	namespace msg
	{
		namespace detail
		{
			struct tcp_header
			{
				uint32_t proto_id_;
				int32_t size_;
				uint32_t sequence_;

				tcp_header& operator=(const tcp_header& other)
				{
					this->proto_id_ = other.proto_id_;
					this->size_ = other.size_;
					this->sequence_ = other.sequence_;
					
					return *this;
				}
			};

			struct request_header :public tcp_header
			{
				uint32_t u_id_;

				request_header& operator=(const request_header& other)
				{
					tcp_header::operator=(other);

					this->u_id_ = other.u_id_;

					return *this;
				}
			};

			struct response_header : public tcp_header
			{
				int32_t result_;

				response_header& operator=(const response_header& other)
				{
					tcp_header::operator=(other);

					this->result_ = other.result_;

					return *this;
				}
			};
		}
	}
}