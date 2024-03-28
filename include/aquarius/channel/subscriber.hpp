#pragma once
#include <aquarius/channel/group.hpp>
#include <aquarius/channel/impl/role.hpp>

namespace aquarius
{
	namespace channel
	{
		template <typename _Ty>
		class subscriber : public impl::role, public std::enable_shared_from_this<subscriber<_Ty>>
		{
		public:
			void subscribe(const std::string& topic)
			{
				default_group::instance().subscribe(topic, shared_from_this());
			}

			template <typename _Func, typename... _Args>
			void apply(_Func&& f, _Args&&... args)
			{
				deri_this()->process(std::forward<_Func>(f), std::forward<_Args>(args)...);
			}

		private:
			void deri_this()
			{
				return static_cast<_Ty*>(this);
			}
		};
	} // namespace channel
} // namespace aquarius