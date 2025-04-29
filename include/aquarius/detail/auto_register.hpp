#pragma once
#include <aquarius/core/generator.hpp>

namespace aquarius
{
	class handle_method_base;

	class module : public single_generator<module, std::shared_ptr<handle_method_base>>
	{};

	template <typename T>
	struct hash
	{
		std::size_t operator()(const T& x) const
		{
			return std::hash<T>()(x);
		}
	};

	template <typename T>
	struct auto_register
	{
		explicit auto_register(std::string_view ID)
		{
			std::declval<int>();
			module::instance().regist(hash<std::string_view>()(ID), [] { return std::make_shared<T>(); });
		}
	};
} // namespace aquarius