#pragma once
#include <cstddef>
#include <future>
#include "context.hpp"
#include "detail/router.hpp"

namespace aquarius
{
	namespace net
	{
		class async_control
		{
		public:
			virtual ~async_control()
			{
			}

		public:
			void complete(const detail::streambuf& stream,detail::streambuf& os_buf)
			{
				auto protocol_id = stream.get_first<uint32_t>();

				//处理message
				detail::router::get().route(std::to_string(protocol_id), stream,os_buf);
			}
		};
	}
}