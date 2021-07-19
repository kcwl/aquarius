#pragma once
#include <cstddef>
#include <future>
#include "detail/router.hpp"

namespace aquarius
{
	class async_control
	{
	public:
		virtual ~async_control()
		{
		}

	public:
		template<typename T>
		void complete(detail::streambuf& stream,std::shared_ptr<connect> conn_ptr)
		{
			T proto_id{};

			std::memcpy(&proto_id, reinterpret_cast<void*>(stream.data()), sizeof(T));

			//处理message
			detail::router::instance().route_invoke("msg_" + std::to_string(proto_id), stream,conn_ptr);
		}
	};
}