#pragma once
#include <fstream>

namespace aquarius
{
	namespace lazytool
	{
		class data_field;

		namespace cpp
		{
			class json_tag
			{
				enum json_type
				{
					integer,
					string,
					array,
					object
				};

			public:
				json_tag() = default;
				virtual ~json_tag() = default;

			public:
				void generate_to_tag_define(std::ofstream& ofs, std::shared_ptr<data_field> parser);

				void generate_from_tag_define(std::ofstream& ofs, std::shared_ptr<data_field> parser);

				void generate_to_tag(std::ofstream& ofs, std::shared_ptr<data_field> parser);

				void generate_from_tag(std::ofstream& ofs, std::shared_ptr<data_field> parser);

			private:
				bool generate_to_int(std::ofstream& ofs, const std::string& type, const std::string& value);
				bool generate_to_string(std::ofstream& ofs, const std::string& type, const std::string& value);
				bool generate_to_array(std::ofstream& ofs, const std::string& type, const std::string& value);
				bool generate_to_object(std::ofstream& ofs, const std::string& type, const std::string& value);

				bool generate_from_int(std::ofstream& ofs, const std::string& type, const std::string& value);
				bool generate_from_string(std::ofstream& ofs, const std::string& type, const std::string& value);
				bool generate_from_array(std::ofstream& ofs, const std::string& type, const std::string& value);
				bool generate_from_object(std::ofstream& ofs, const std::string& type, const std::string& value);

				json_type check_type(const std::string& type);
			};
		}
	} // namespace lazytool
} // namespace aquarius