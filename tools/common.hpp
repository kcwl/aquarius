#pragma once
#include <string>
#include "part.h"

namespace elastic
{
	namespace compiler
	{
		namespace common
		{
			template <typename T, std::size_t... I>
			bool spilt(const char elem, T&& t, std::index_sequence<I...>)
			{
				return ((elem != std::get<I>(std::forward<T>(t))) && ...);
			}

			template <typename... Args>
			bool is_spilt(const char elem, Args&&... args)
			{
				return common::template spilt(elem, std::make_tuple(std::forward<Args>(args)...),
											  std::make_index_sequence<sizeof...(args)>{});
			}

			template <typename... Args>
			std::string trip(std::string& str, Args&&... args)
			{
				for (auto iter = str.begin(); iter != str.end();)
				{
					if (common::template is_spilt(*iter, std::forward<Args>(args)...))
						iter++;
					else
						iter = str.erase(iter);
				}

				return str;
			}

			std::string get_type_name(const std::string& name)
			{
				if (name == "enum")
					return "enum class";
				else if (name == "message")
					return "struct";

				return {};
			}

			keyword_type enum_type(const std::string& ty)
			{
				keyword_type tp;
				if (ty.compare("include") == 0)
					tp = keyword_type::include_t;
				else if (ty.compare("namespace") == 0)
					tp = keyword_type::namespace_t;
				else if (ty.compare("message") == 0)
					tp = keyword_type::message_t;
				else if (ty.compare("enum") == 0)
					tp = keyword_type::enumclass_t;
				else
					tp = keyword_type::none_t;

				return tp;
			}

			bool is_container(const std::string& type)
			{
				if (type.compare("string") == 0)
					return true;

				return false;
			}
		} // namespace common
	}
}