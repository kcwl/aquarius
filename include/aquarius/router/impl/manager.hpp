#pragma once
#include <aquarius/router/impl/router.hpp>
#include <aquarius/router/impl/singleton.hpp>

namespace aquarius
{
	namespace impl
	{
		template <typename _Ty>
		class manager : public impl::router<std::size_t, _Ty>
		{
		public:
			using value_type = _Ty;

		public:
			manager() = default;

		public:
			value_type invoke(std::size_t key)
			{
				std::lock_guard lk(this->mutex_);

				auto iter = this->map_invokes_.find(key);

				if (iter == this->map_invokes_.end())
					return {};

				return iter->second;
			}

			void erase(std::size_t key) noexcept
			{
				std::lock_guard lk(this->mutex_);

				this->map_invokes_.erase(key);
			}
		};

		template <typename _Single, typename _Ty>
		class single_manager : public manager<_Ty>, public impl::singleton<_Single>
		{};
	} // namespace impl
} // namespace aquarius