#pragma once
#include <aquarius/core/error_code.hpp>

namespace aquarius
{
	class basic_session;
}

namespace aquarius
{
	namespace impl
	{
		template <typename _Request>
		class basic_visitor
		{
		public:
			basic_visitor() = default;
			virtual ~basic_visitor() = default;

		public:
			virtual error_code visit(_Request visited, std::shared_ptr<basic_session> session_ptr, error_code& ec) = 0;
		};

		template <typename _Request>
		class shared_visitor : public virtual basic_visitor<std::shared_ptr<_Request>>
		{
		public:
			shared_visitor() = default;
			virtual ~shared_visitor() = default;
		};

		template <typename _Request>
		class bare_visitor : public virtual basic_visitor<_Request*>
		{
		public:
			bare_visitor() = default;

			virtual ~bare_visitor() = default;
		};
	} // namespace impl
} // namespace aquarius

#define DEFINE_VISITOR(_Request, _Return)                                                                              \
	virtual _Return visit([[maybe_unused]] std::shared_ptr<_Request>,                                                  \
						  [[maybe_unused]] std::shared_ptr<aquarius::basic_session>)                                        \
	{                                                                                                                  \
		return _Return{};                                                                                              \
	}