#pragma once
#include <aquarius/basic_context.hpp>

namespace aquarius
{
	namespace tcp
	{
		template <typename Handler>
		class transfer_context : public basic_context<context_kind::transfer>
		{
			using base_type = basic_context<context_kind::transfer>;

		public:
			using base_type::mode;

		public:
			transfer_context()
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
	} // namespace tcp
} // namespace aquarius