#pragma once
#include <aquarius/serialize/message_serialize_base.hpp>

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
			template <typename T, typename U>
			void parse_header_to(T&& value, detail::flex_buffer<U>& buffer)
			{
				return header_parse_.to_datas(std::forward<T>(value), buffer);
			}

			template <typename T, typename U>
			T parse_header_from(detail::flex_buffer<U>& buffer)
			{
				return header_parse_.from_datas<T>(buffer);
			}

			template <typename T, typename U>
			void parse_body_to(T&& value, detail::flex_buffer<U>& buffer)
			{
				return body_parse_.to_datas(std::forward<T>(value), buffer);
			}

			template <typename T, typename U>
			T parse_body_from(detail::flex_buffer<U>& buffer)
			{
				return body_parse_.from_datas<T>(buffer);
			}

		private:
			HeaderParse header_parse_;

			BodyParse body_parse_;
		};
	} // namespace virgo
} // namespace aquarius