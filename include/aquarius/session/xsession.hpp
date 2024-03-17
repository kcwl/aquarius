#pragma once
#include <aquarius/elastic.hpp>
#include <aquarius/system/event.hpp>

namespace aquarius
{
	class basic_context;
}

namespace aquarius
{
	class xsession : public system::event_call
	{
	public:
		xsession() = default;
		virtual ~xsession() = default;

	public:
		virtual std::size_t uuid() = 0;

		virtual bool async_write(flex_buffer_t&& buffer) = 0;

		virtual void attach(std::size_t proto, std::shared_ptr<basic_context> context_ptr) = 0;

		virtual void detach(std::size_t proto) = 0;

		virtual std::shared_ptr<basic_context> get(std::size_t id) = 0;
	};
} // namespace aquarius