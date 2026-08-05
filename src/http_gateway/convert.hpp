#pragma once
#include <aquarius/detail/flex_buffer.hpp>
#include <aquarius/ip/http.hpp>
#include <aquarius/ip/tcp.hpp>
#include <aquarius/virgo/http_header.hpp>
#include <aquarius/virgo/tcp_header.hpp>

namespace aquarius
{
	namespace gateway
	{
        struct convert
        {
            static error_code from_tcp_to_http(flex_buffer& input, flex_buffer& output)
            {
                int32_t ec = binary_parse{}.from_datas<int32_t>(input);

                tcp_header t{};
                t.deserialize(input);

                http_header h;
                h.content_type("application/json");
                h.content_length(input.size());
                set_options_header(h, true, create_http());
                h.serialize(output);

                output.sputn((char*)input.data().data(), input.size());

                return static_cast<http_status>(ec);
            }

            static error_code from_http_to_tcp(http_method method, flex_buffer& input, flex_buffer& output)
            {
                http_header h{};
                h.deserialize(input);

                tcp_header t{};
                t.way() = static_cast<int16_t>(method == http_method::get ? serialize_way::kv : serialize_way::json);
                t.serialize(output);

                output.sputn((char*)input.data().data(), input.size());

                return error_code{};
            }
        };
	} // namespace gateway
} // namespace aquarius