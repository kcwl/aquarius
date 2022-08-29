#pragma once
#include "ssl_context.hpp"

namespace aquarius
{
	template<typename _Socket>
	struct socket_traits
	{
		template<typename _Ty>
		static auto& socket(std::shared_ptr<_Ty> shared_this)
		{
			return shared_this->socket_;
		}
	};

#if ENABLE_SSL
	template<typename _Socket>
	struct socket_traits<boost::asio::ssl::stream<_Socket>>
	{
		template<typename _Ty>
		static auto& socket(std::shared_ptr<_Ty> shared_this)
		{
			return shared_this->socket_.lowest_layer();
		}
	};
#endif
}