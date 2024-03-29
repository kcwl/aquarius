#pragma once
#include <aquarius/channel/group.hpp>
#include <aquarius/channel/impl/role.hpp>

namespace aquarius
{
	namespace channel
	{
		template<typename _Ty>
		class subscriber : public impl::role
		{
		public:
			void subscribe(const std::string& topic)
			{
				default_group::instance().subscribe(topic, deri_this()->shared_from_this());
			}

		private:
			_Ty* deri_this()
			{
				return static_cast<_Ty*>(this);
			}
		};
	} // namespace channel
} // namespace aquarius