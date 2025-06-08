#pragma once
#include <aquarius/tcp/broadcast_context.hpp>
#include <aquarius/tcp/handler.hpp>
#include <aquarius/tcp/stream_context.hpp>
#include <aquarius/tcp/transfer_context.hpp>

#define AQUARIUS_STREAM_CONTEXT()                                                                                      \
	[[maybe_unused]] static aquarius::tcp::auto_context_register<aquarius::tcp::stream_context>                        \
		__auto_context_register_stream;

#define AQUARIUS_BROADCAST_CONTEXT()                                                                                   \
	[[maybe_unused]] static aquarius::tcp::auto_context_register<aquarius::tcp::broadcast_context>                     \
		__auto_context_register_broadcast;

#define AQUARIUS_TRANSFER_CONTEXT()                                                                                    \
	class transfer_handler;                                                                                             \
	[[maybe_unused]] static aquarius::tcp::auto_context_register<aquarius::tcp::transfer_context<transfer_handler>>    \
		__auto_context_register_transfer;                                                                             \
	AQUARIUS_TRANSFER_HANDLER(transfer_handler, aquarius::flex_buffer)
