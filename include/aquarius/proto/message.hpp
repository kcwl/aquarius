#pragma once
#include <cstddef>
#include "body_of.hpp"
#include "header_of.hpp"
#include <aquarius/proto/parse.hpp>

namespace aquarius
{
	namespace proto
	{
		struct empty_body
		{
			std::string SerializeAsString() { return {}; }

			template<typename _Ty>
			bool ParseFromArray([[maybe_unused]] _Ty* data, [[maybe_unused]] std::size_t bytes) { return true; }

			int ByteSize() { return 0; }
		};

		template<typename _Parse, typename _Header, typename _Body, uint32_t N>
		class basic_message : private header_of<_Header, _Parse>
		{
		public:
			using header_type = _Header;
			using body_type = _Body;

			using stream_type = typename _Parse::stream_type;

			constexpr static uint32_t Number = N;

		public:
			header_type* header() noexcept
			{
				return this->get();
			}

			const header_type* header() const noexcept
			{
				return this->get();
			}

			body_type& body() noexcept
			{
				return *body_.get();
			}

			const body_type& body() const noexcept
			{
				return *body_.get();
			}

			bool parse_message(stream_type& stream)
			{
				if (!this->parse_bytes(stream))
				{
					return false;
				}

				if (!body_.parse_bytes(stream))
				{
					return false;
				}

				return true;
			}

			bool to_message(stream_type& stream)
			{
				elastic::binary_oarchive oa(stream);
				oa << Number;

				if (!this->to_bytes(stream))
				{
					return false;
				}

				if (!body_.to_bytes(stream))
				{
					return false;
				}

				return true;
			}

		private:
			body_of<body_type, _Parse> body_;
		};
	}

	template<typename _Header, typename _Body, uint32_t N>
	using message = proto::basic_message<proto::parse<flex_buffer_t>, _Header, _Body, N>;
}