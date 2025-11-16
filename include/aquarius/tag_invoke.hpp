#pragma once

namespace aquarius
{
	struct http_param_tag;
	struct http_param;
}

namespace aquarius
{
	template <typename T>
	struct value_to
	{};


	void tag_invoke(value_to<http_param_tag>, http_param&);
} // namespace aquarius