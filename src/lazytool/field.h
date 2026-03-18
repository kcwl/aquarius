#pragma once
#include <string>

namespace aquarius
{
	namespace lazytool
	{
		enum class struct_type
		{
			none,
			structure,
			message,
			enumture,
			model
		};

		class field
		{
		public:
			field(struct_type s);
			virtual ~field() = default;

		public:
			void set_name(const std::string& name);

			std::string name() const;

			std::string& name();

			struct_type type() const;

		protected:
			struct_type type_;

			std::string name_;
		};
	} // namespace lazytool
} // namespace aquarius