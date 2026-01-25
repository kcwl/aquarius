#include "gate_error_code.h"

namespace aquarius
{
	namespace gateway
	{
		std::string_view from_error_string(gate_op e)
		{
			static const std::map<gate_op, std::string_view> desc = { { gate_op::success, "success" },
																	  { gate_op::auth_failed, "auth_failed" } };

			auto iter = desc.find(e);

			if (iter == desc.end())
				return "unknown error!";

			return iter->second;
		}

		const char* gate_op_error_category::name() const noexcept
		{
			return "gate_op error category";
		}

		std::string gate_op_error_category::message(int err_code) const
		{
			return std::string(from_error_string(static_cast<gate_op>(err_code)).data());
		}

		static gate_op_error_category& get_gate_op_error_category()
		{
			static gate_op_error_category category{};

			return category;
		}

		error_code make_error_code(gate_op ec)
		{
			return error_code(static_cast<int>(ec), get_gate_op_error_category());
		}
	} // namespace gateway
} // namespace aquarius