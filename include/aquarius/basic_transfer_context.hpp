#pragma once
#include <aquarius/basic_context.hpp>

namespace aquarius
{
	template <typename Executor, typename Handler>
	class basic_transfer_context : public basic_context<Executor, context_kind::transfer>
	{
		using base_type = basic_context<Executor, context_kind::transfer>;

	public:
		basic_transfer_context()
			: base_type("transfer context")
		{}

	public:
		template <typename... Args>
		void visit(flex_buffer buffer, Args&&...)
		{
			handler_.visit(buffer);
		}

	private:
		Handler handler_;
	};
} // namespace aquarius