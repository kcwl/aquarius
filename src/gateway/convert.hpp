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
		template <typename From, typename To>
		struct convert;

		template <>
		struct convert<tcp, http>
		{
			static error_code apply(flex_buffer& input, flex_buffer& output)
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
		};

		template <>
		struct convert<http, tcp>
		{
			static error_code apply(flex_buffer& input, flex_buffer& output)
			{
				http_header h{};
				h.deserialize(input);

				tcp_header t{};
				t.serialize(output);

				output.sputn((char*)input.data().data(), input.size());


				return error_code{};
			}
		};
	} // namespace gateway
} // namespace aquarius