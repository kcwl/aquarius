#include "srvd_bridge.h"
#include "basic_transfer_context.hpp"
#include "convert.hpp"
#include "gate_error_code.h"
#include <serviced/proto/shake.virgo.h>
#include <serviced/srvd_client.hpp>

namespace aquarius
{
	namespace gateway
	{

        srvd_bridge::srvd_bridge()
            : group_("gateway")
            , srvd_host_()
            , srvd_port_()
            , host_()
            , port_()
            , weight_()
            , version_()
            , local_server_name_()
        {}

        bool srvd_bridge::init()
        {
            srvd_host_ = srv_config::get_mutable_instance().srvd_host;
            srvd_port_ = srv_config::get_mutable_instance().srvd_port;

            host_ = srv_config::get_mutable_instance().host;
            port_ = srv_config::get_mutable_instance().port;

            weight_ = srv_config::get_mutable_instance().weight;

            version_ = srv_config::get_mutable_instance().version;

            local_server_name_ = srv_config::get_mutable_instance().local_server_name;

            return true;
        }

        auto srvd_bridge::run() -> asio::awaitable<bool>
        {
            XLOG_INFO() << "registing service....";

            auto request = std::make_shared<shake_request>();

            request->body().host() = host_;
            request->body().port() = port_;
            request->body().name() = local_server_name_;
            request->body().group() = group_;
            request->body().weight() = weight_;
            request->body().version() = version_;

            auto resp =
                co_await mpc_async_call<&serviced::srvd_client::async_call<shake_response, shake_request>>(request);

            XLOG_INFO() << "registed result:" << resp.result();

            co_return co_await set_topics(resp.body().topics());
        }

        auto srvd_bridge::set_topics(const std::vector<std::string>& topics) -> asio::awaitable<bool>
        {
            auto ctx_func = [this]<typename Func>(flex_buffer & buffer, const std::string & router,
                Func && f) -> asio::awaitable<error_code>
            {
                auto tf = [func = std::move(f)](flex_buffer& buf, const std::string&) -> asio::awaitable<error_code>
                    {
                        flex_buffer out{};

                        auto ec = convert::from_tcp_to_http(buf, out);
                        co_return co_await func(out, ec);
                    };

                co_return co_await mpc_async_call<
                    &serviced::srvd_client::async_call_buffer<decltype(tf), flex_buffer&>>(std::ref(buffer), router,
                        tf);
            };

            std::shared_ptr<context_base> ctx = std::make_shared<basic_transfer_context<http, http_method>>(ctx_func);

            for (auto& topic : topics)
            {
                XLOG_INFO() << "Regist Context: " << topic;
                mpc_put_context(topic, ctx, true);
            }

            co_return true;
        }

	} // namespace gateway
} // namespace aquarius