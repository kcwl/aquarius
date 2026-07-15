#pragma once
#include <aquarius.hpp>

namespace aquarius
{
	namespace login
	{
		AQUARIUS_MODULE(srv_regist)
		{
		public:
			srv_regist();

		public:
			virtual bool init() override;
			virtual auto run() -> asio::awaitable<bool> override;

		private:
			std::string group_;

			std::string host_;

			int32_t port_;
		};
	} // namespace login
} // namespace aquarius