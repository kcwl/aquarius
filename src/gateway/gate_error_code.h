#pragma once
#include <aquarius/error_code.hpp>
#include <map>
#include <string_view>

namespace aquarius
{
	namespace gateway
	{
		enum class gate_op
		{
			success,
			auth_failed,
		};

		std::string_view from_error_string(gate_op e);

		class gate_op_error_category : public boost::system::error_category
		{
		public:
			constexpr gate_op_error_category() = default;

			[[nodiscard]] const char* name() const noexcept override;

			[[nodiscard]] std::string message(int err_code) const override;
		};

		static gate_op_error_category& get_serialize_error_category();

		error_code make_error_code(gate_op ec);
	} // namespace gateway
} // namespace aquarius

namespace std
{
	template <>
	struct is_error_code_enum<aquarius::gateway::gate_op> : std::true_type
	{};
} // namespace std