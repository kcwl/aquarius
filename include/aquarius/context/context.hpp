#pragma once
#include <aquarius/context/auto_context.hpp>
#include <aquarius/context/handler.hpp>

#define AQUARIUS_STREAM_CONTEXT(session)                                                                               \
	[[maybe_unused]] static aquarius::context::auto_context_register<session, aquarius::context::basic_stream_context> \
		__auto_context_register_stream;

#define AQUARIUS_BROADCAST_CONTEXT(session)                                                                            \
	[[maybe_unused]] static aquarius::context::auto_context_register<                                                  \
		session, aquarius::context::basic_broadcast_context> __auto_context_register_broadcast;

#define AQUARIUS_TRANSFER_CONTEXT(session)                                                                             \
	class transfer_handler;                                                                                            \
	[[maybe_unused]] static aquarius::context::auto_context_register<                                                  \
		session, aquarius::context::basic_transfer_context<transfer_handler>> __auto_context_register_transfer;        \
	AQUARIUS_TRANSFER_HANDLER(transfer_handler, std::vector<char>)
