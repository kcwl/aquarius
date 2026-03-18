#include "field.h"

namespace aquarius
{
	namespace lazytool
	{
		field::field(struct_type s)
			: type_(s)
			, name_()
		{}

		void field::set_name(const std::string& name)
		{
			name_ = name;
		}

		std::string field::name() const
		{
			return name_;
		}

		std::string& field::name()
		{
			return name_;
		}

		struct_type field::type() const
		{
			return type_;
		}
	} // namespace lazytool
} // namespace aquarius