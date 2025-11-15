#pragma once
#include <aquarius/basic_serialize.hpp>
#include <aquarius/serialize/html.hpp>

namespace aquarius
{

	class http_html_serialize : public basic_serialize<html_parse>
	{
	public:
		http_html_serialize() = default;
		virtual ~http_html_serialize() = default;

	public:
		template <typename... Args>
		void parse_to(flex_buffer& buffer, Args&&... args)
		{
			return this->parse_.to_datas(stream_, buffer, std::forward<Args>(args)...);
		}

		template <typename... Args>
		void parse_from(flex_buffer & buffer, Args&&... args)
		{
			stream_ = this->parse_.from_datas(buffer, std::forward<Args>(args)...);
		}

	public:
		std::string stream_;
	};
} // namespace aquarius