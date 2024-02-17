#pragma once
#include <system_error>

namespace aquarius
{
	namespace system
	{
		const std::error_category& system_category();

		class system_error_ccategory : public std::error_category
		{
		public:
			system_error_ccategory() noexcept
				: std::error_category(std::error_category::_System_addr)
			{}

		public:
			const char* name() const noexcept override
			{
				return "aquaruis system";
			}

			std::string message(int err_code) const override
			{
				const std::_System_error_message msg(static_cast<unsigned long>(err_code));

				if (msg._Length == 0)
				{
					static constexpr char unknown_error[] = "unknown error";
					constexpr std::size_t length = sizeof(unknown_error) - 1;

					return std::string{ unknown_error ,length };
				}

				return std::string{ msg._Str,msg._Length };
			}

			std::error_condition default_error_condition(int err_value) const noexcept override
			{
				return std::error_condition{ err_value, system_category() };
			}
		};

		template <typename _Category>
		const _Category& immortalize_memcpy_image() noexcept
		{
			static constexpr _Category _static;

			return _static;
		}

		inline const std::error_category& system_category() noexcept
		{
			return immortalize_memcpy_image<system_error_ccategory>();
		}
	}
} // namespace aquarius