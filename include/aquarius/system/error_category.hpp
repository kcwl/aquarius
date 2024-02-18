#pragma once
#include <system_error>
#include <aquarius/system/error_code/system_error_message.hpp>

namespace aquarius
{
	namespace system
	{
		inline const std::error_category& system_category() noexcept;

		class system_error_category : public std::error_category
		{
		public:
			system_error_category() noexcept
				: std::error_category(std::error_category::_System_addr)
			{}

		public:
			const char* name() const noexcept override
			{
				return "aquarius system";
			}

			std::string message(int err_code) const override
			{
				const system_error_message msg(err_code);

				if (msg.message_.empty())
				{
					static constexpr char unknown_error[] = "unknown error";
					constexpr std::size_t length = sizeof(unknown_error) - 1;

					return std::string{ unknown_error ,length };
				}

				return msg.message_;
			}

			std::error_condition default_error_condition(int err_value) const noexcept override
			{
				return std::error_condition{ err_value, system_category() };
			}
		};

		template <typename _Category>
		const _Category& immortalize_memcpy_image() noexcept
		{
			static _Category _static;

			return _static;
		}

		inline const std::error_category& system_category() noexcept
		{
			return immortalize_memcpy_image<system_error_category>();
		}
	}
} // namespace aquarius