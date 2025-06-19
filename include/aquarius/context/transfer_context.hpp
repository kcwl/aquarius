#pragma once
#include <aquarius/detail/context_base.hpp>

namespace aquarius
{
	namespace context
	{
		template <typename Hander>
		class transfer_context : public context_base
		{
		public:
			transfer_context()
				: context_base("transfer context", 10s)
			{}

		public:
			template <typename Session>
			auto invoke(std::vector<char>, std::shared_ptr<Session>)
			{
				return;
			}
		};
	} // namespace context
} // namespace aquarius