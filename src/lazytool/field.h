#pragma once
#include <array>
#include <memory>
#include <string>
#include <vector>

namespace aquarius
{
	namespace lazytool
	{
		enum class struct_type
		{
			structure,
			message,
			request,
			response,
			enumture,
		};

		class field_base
		{
		public:
			field_base(struct_type s, const std::string& name = {});

			virtual ~field_base() = default;

		public:
			void set_name(const std::string& name);

			std::string name() const;

			std::string& name();

			struct_type type() const;

			std::vector<std::pair<std::string, std::string>> fields() const;

			bool add_field(const std::string& type, const std::string& value);

		protected:
			struct_type type_;

			std::string name_;

			std::vector<std::pair<std::string, std::string>> fields_;
		};

		class message_field : public field_base
		{
		public:
			constexpr static auto message_part = 2;

			constexpr static auto tag_request = 0;
			constexpr static auto tag_response = 1;

			constexpr static auto request_subfix = "_req_body";
			constexpr static auto response_subfix = "_resp_body";

		public:
			message_field(struct_type s, const std::string& name = {});

			~message_field() = default;

		public:
			void set_router(const std::string& r);

			std::string router() const;

			template <std::size_t I>
			std::shared_ptr<field_base> get()
			{
				static_assert(I < 2, "message only has two parts that request and response");

				return messages_[I];
			}

			void set_protocol_name(const std::string& name);

		private:
			std::array<std::shared_ptr<field_base>, message_part> messages_;

			std::string router_;
		};
	} // namespace lazytool
} // namespace aquarius