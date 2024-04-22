#pragma once
#include <aquarius/core/elastic.hpp>

namespace aquarius
{
	class basic_context;
}

namespace aquarius
{
	class basic_session
	{
	public:
		basic_session() = default;
		virtual ~basic_session() = default;

	public:
		virtual std::size_t uuid() = 0;

		virtual bool async_write(const std::size_t proto, flex_buffer_t&& buffer) = 0;

		virtual void attach(std::size_t proto, std::shared_ptr<basic_context> context_ptr) = 0;

		virtual void detach(std::size_t proto) = 0;

		virtual void attach_buffer(const std::size_t proto, flex_buffer_t& buffer) = 0;

		virtual flex_buffer_t complete(const std::size_t proto, flex_buffer_t& buffer) = 0;
	};
} // namespace aquarius