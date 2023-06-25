#pragma once
#include <aquarius/detail/visitor.hpp>
#include <memory>
#include <string>

namespace aquarius
{
	namespace detail
	{
		class basic_context : public detail::visitor<xmessage, int>
		{
		public:
			basic_context(const std::string& name)
				: name_(name)
			{
				on_connected();
			}

			basic_context(const basic_context&) = delete;

			basic_context(basic_context&&) = default;

			virtual ~basic_context()
			{
				on_closed();
			}

			basic_context& operator=(const basic_context&) = delete;

			DEFINE_VISITOR(xmessage, int)

		public:
			virtual int on_connected()
			{
				return 0;
			}

			virtual int on_closed()
			{
				return 0;
			}

			virtual int on_timeout([[maybe_unused]] std::shared_ptr<transfer> session_ptr)
			{
				return 0;
			}

		protected:
			virtual void on_error([[maybe_unused]] int result){};

		protected:
			std::shared_ptr<transfer> conn_ptr_;

			std::string name_;
		};
	} // namespace detail
} // namespace aquarius