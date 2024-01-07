#pragma once
#include <aquarius/session/session_manager.hpp>
#include <aquarius/event_callback.hpp>

namespace aquarius
{
	class context : public detail::visitor<xmessage, int>, public event_callback
	{
	public:
		context(const std::string& name, std::chrono::milliseconds timeout)
			: name_(name)
			, timeout_(timeout)
		{}

		virtual ~context() = default;

		context(const context&) = delete;

		context(context&&) = default;

		context& operator=(const context&) = delete;

	public:
		virtual int visit(std::shared_ptr<xmessage>, std::shared_ptr<xsession>)
		{
			XLOG(warning) << name_ << "visit a unknown message!";

			return 0;
		}
		
		virtual int visit(flex_buffer_t& buffer)
		{
			return 0;
		}

	protected:
		std::string name_;

		std::chrono::milliseconds timeout_;

		std::shared_ptr<xsession> session_ptr_;
	};
} // namespace aquarius