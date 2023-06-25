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
			{}

			basic_context(const basic_context&) = delete;

			basic_context(basic_context&&) = default;

			virtual ~basic_context() = default;

			basic_context& operator=(const basic_context&) = delete;

			DEFINE_VISITOR(xmessage, int)

		protected:
			std::shared_ptr<transfer> conn_ptr_;

			std::string name_;
		};
	} // namespace detail
} // namespace aquarius