#pragma once
#include "../3rd/elastic/include/elastic.hpp"

namespace aquarius
{
	using flex_buffer_t = elastic::flex_buffer_t;

	using access = elastic::access;
} // namespace aquarius

#define AQUARIUS_ACCESS(...) ELASTIC_ACCESS(__VA_ARGS__)

#define AQUARIUS_ACCESS_IF(base, ...) ELASTIC_ACCESS_IF(base, __VA_ARGS__)

#define AQUARIUS_ACCESS_NULL()                                                                                         \
private:                                                                                                               \
	friend class elastic::access;                                                                                      \
	template <typename _Archive>                                                                                       \
	void serialize(_Archive&)                                                                                          \
	{}