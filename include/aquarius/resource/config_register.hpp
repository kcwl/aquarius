#pragma once
#include <aquarius/resource/global_resource.hpp>

namespace aquarius
{
	template <typename T>
	class config_register
	{
	public:
		config_register()
			: value_()
		{}

	private:
		T value_;
	};
} // namespace aquarius