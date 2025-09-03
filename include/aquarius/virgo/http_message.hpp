#pragma once
#include <map>
#include <virgo/detail/basic_message.hpp>
#include <virgo/detail/json.hpp>
#include <virgo/http/detail/basic_header.hpp>
#include <virgo/http/detail/header_fileds.hpp>

namespace virgo
{
	namespace http
	{
		namespace detail
		{
			template <bool Request, typename Body, typename Fields = header_fields,
					  typename Allocator = std::allocator<Body>>
			class basic_message : public virgo::detail::basic_message<std::add_pointer_t<Body>, Allocator>,
								  public basic_header<Request>
			{
			public:
				using header_t = basic_header<Request>;

				using filed = Fields;

			public:
				basic_message() = default;

				basic_message(header_t h)
					: header_t(h)
				{}

			public:
				filed& header()
				{
					return fields_;
				}

				const filed& header() const
				{
					return fields_;
				}

				header_t& base_header()
				{
					return *this;
				}

				const header_t& base_header() const
				{
					return *this;
				}

			public:
				std::ostream& operator<<(std::ostream& os) const
				{
					os << header() << this->body();

					return os;
				}

				template <typename BufferSequence>
				void commit(BufferSequence& buffer)
				{
					auto str = to_json(this->body());

					std::copy(str.begin(), str.end(), std::back_inserter(buffer));
				}

				void consume(std::span<char> buffer)
				{
					auto next_span = fields_.consume(buffer);

					if (next_span.size() < sizeof("\r\n"))
						return;

					auto body_span = next_span.subspan(2);

					auto body_length = body_span.size() > *header().length() ? *header().length() : body_span.size();

					this->body() = from_json<Body>(std::string(body_span.data(), body_length));
				}

			private:
				filed fields_{};
			};

			template <bool Request, typename Body, typename Allocator>
			inline std::ostream& operator<<(std::ostream& os, const basic_message<Request, Body, Allocator>& other)
			{
				other << os;

				return os;
			}
		} // namespace detail
	} // namespace http
} // namespace virgo