#pragma once
#include <aquarius/context/handler.hpp>
#include <aquarius/context/stream_context.hpp>
#include <aquarius/context/tcp_broadcast.hpp>
#include <aquarius/context/transfer_context.hpp>


#define AQUARIUS_STREAM_CONTEXT()                                                                                      \
	[[maybe_unused]] static aquarius::context::auto_context_register<                                                  \
		aquarius::ip::tcp::server_session, aquarius::context::stream_context> __auto_context_register_stream;

#define AQUARIUS_BROADCAST_CONTEXT()                                                                                   \
	[[maybe_unused]] static aquarius::context::auto_context_register<                                                  \
		aquarius::ip::tcp::server_session, aquarius::context::broadcast_context> __auto_context_register_broadcast;

#define AQUARIUS_TRANSFER_CONTEXT()                                                                                    \
	class transfer_handler;                                                                                            \
	[[maybe_unused]] static aquarius::context::auto_context_register<                                                  \
		aquarius::ip::tcp::server_session, aquarius::context::transfer_context<transfer_handler>>                      \
		__auto_context_register_transfer;                                                                              \
	AQUARIUS_TRANSFER_HANDLER(transfer_handler, aquarius::flex_buffer)
