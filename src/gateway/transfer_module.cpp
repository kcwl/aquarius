#include "transfer_module.h"
#include <error.hpp>

namespace aquarius
{
	namespace gateway
	{
		transfer_module::transfer_module(io_context& io, const std::string& name)
			: _module<transfer_module, transfer_config>(io, name)
		{}

		//auto transfer_module::run() -> awaitable<void>
		//{
		//	//transfer_ptr_ = std::make_shared<tcp_client>(ios);

		//	//auto result = co_await transfer_ptr_->async_connect(this->configs().ip_addr, this->configs().port);

		//	//if (!result)
		//	//{
		//	//	XLOG_ERROR() << "transfer connect error!";
		//	//}
		//}

		auto transfer_module::async_sendback(flex_buffer& buffer, error_code& ec) -> awaitable<flex_buffer>
		{
			//auto req = std::make_shared<transfer_tcp_request>(std::move(*hf));

			//std::copy((char*)buffer.data().data(), (char*)buffer.data().data() + buffer.data().size(),
			//		  std::back_inserter(req->body().feedbuf));

			//auto resp = co_await transfer_ptr_->async_send<transfer_tcp_response>(req);

			//ec = static_cast<serviced::errc>(resp.result());

			flex_buffer resp_buffer{};
			//resp_buffer.sputn(resp.body().feedbuf.data(), resp.body().feedbuf.size());

			co_return std::move(resp_buffer);
		}
	} // namespace gateway
} // namespace aquarius

AQUARIUS_MODULE_NAMESPACE(aquarius::gateway, transfer_module)