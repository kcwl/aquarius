#pragma once
#include <aquarius/core/singleton.hpp>
#include <aquarius/router/impl/router.hpp>
#include <functional>

namespace aquarius
{
	namespace impl
	{
		template <typename _R, typename... _Args>
		class generator : public router<std::size_t, std::function<_R(_Args...)>>
		{
		public:
			generator() = default;

		public:
			_R invoke(std::size_t key, _Args&... args)
			{
				std::lock_guard lk(this->mutex_);

				auto iter = this->map_invokes_.find(key);

				if (iter == this->map_invokes_.end())
					return _R();

				return iter->second(args...);
			}
		};

		template <typename _Single, typename _R, typename... _Args>
		class single_generator : public generator<_R, _Args...>, public singleton<_Single>
		{};
	} // namespace impl
} // namespace aquarius