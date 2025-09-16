#pragma once
#include <aquarius/virgo/serialize/message_serialize_base.hpp>

namespace aquarius
{
	namespace virgo
	{
		template <typename HeaderParse, typename BodyParse = HeaderParse>
		class basic_message_serialize : public message_serialize_base
		{
		public:
			basic_message_serialize() = default;
			virtual ~basic_message_serialize() = default;

		public:
			template <typename T>
			void parse_header(T&& value, detail::flex_buffer<char>& buffer)
			{
				return header_parse_.to_datas(std::forward<T>(value), buffer);
			}

			template <typename T>
			T parse_header(detail::flex_buffer<char>& buffer)
			{
				return header_parse_.from_datas<T>(buffer);
			}

			template <typename T>
			void parse_body(T&& value, detail::flex_buffer<char>& buffer)
			{
				return body_parse_.to_datas(std::forward<T>(value), buffer);
			}

			template <typename T>
			T parse_body(detail::flex_buffer<char>& buffer)
			{
				return body_parse_.from_datas<T>(buffer);
			}

		private:
			HeaderParse header_parse_;

			BodyParse body_parse_;
		};
	} // namespace virgo
} // namespace aquarius