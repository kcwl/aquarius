#pragma once
#include <aquarius/tcp/broadcast_context.hpp>
#include <aquarius/tcp/stream_context.hpp>
#include <aquarius/tcp/handler.hpp>

#define AQUARIUS_STREAM_CONTEXT()                                                                                      \
	[[maybe_unused]] static aquarius::tcp::auto_context_register<aquarius::tcp::stream_context>                        \
		__auto_context_register_stream;

#define AQUARIUS_BROADCAST_CONTEXT()                                                                                   \
	[[maybe_unused]] static aquarius::tcp::auto_context_register<aquarius::tcp::broadcast_context>                     \
		__auto_context_register_broadcast;

#define AQUARIUS_TRANSFER_CONTEXT()                                                                                    \
	class transfer_hander;                                                                                             \
	[[maybe_unused]] static aquarius::tcp::auto_context_register<aquarius::tcp::broadcast_context<transfer_handler>>   \
		__auto_context_register_broadcast;                                                                             \
	AQUARIUS_DEFAULT_HANDLER(transfer_hander, aquarius::flex_buffer)
