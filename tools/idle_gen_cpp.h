#pragma once
#include "code_generator.h"
#include "common.hpp"
#include "part.h"

#include <array>
#include <fstream>
#include <string_view>

using namespace std::string_view_literals;

namespace elastic
{
	namespace compiler
	{
		namespace cpp
		{
			class generate_cpp : public code_generator
			{
				constexpr static std::string_view tab = "\t"sv;
				constexpr static std::string_view enter = "\n"sv;
				constexpr static std::string_view double_enter = "\n\n"sv;

			public:
				generate_cpp() = default;

				virtual ~generate_cpp() = default;

			public:
				virtual bool generate(const std::string& input_file, const std::string& output_dir)
				{
					read_file_stream_.open(input_file, std::ios::binary | std::ios::in);
					write_h_stream_.open(input_file + ".h", std::ios::binary | std::ios::out);

					if (!read_file())
						return false;

					write_h();

					// write_cpp();

					return true;
				}

			private:
				bool read_file()
				{
					if (!read_file_stream_.is_open())
						return false;

					std::array<char, 1024> lines{};

					while (read_file_stream_.get(&lines[0], 1024, ' '))
					{
						std::string str_temp(lines.data());

						common::trip(str_temp, '\r', '\n', '\t');

						switch (common::enum_type(str_temp))
						{
						case keyword_type::include_t:
							{
								auto str = read_to_spilt(read_file_stream_, ';');

								hf_.list_.push_back(common::trip(str, ' '));
							}
							break;
						case keyword_type::namespace_t:
							{
								auto str = read_to_spilt(read_file_stream_, ';');

								namespace_file_ = common::trip(str, ' ');
							}
							break;
						case keyword_type::enumclass_t:
						case keyword_type::message_t:
							{
								struct_impl impl{};

								impl.type_ = common::get_type_name(str_temp);

								impl.kt_ = common::enum_type(str_temp);

								read_struct_head(impl);

								read_struct_body(impl);

								read_file_stream_.get();

								sf_.list_.push_back(impl);
							}
							break;
						default:
							break;
						}
					}

					return true;
				}

				bool write_h()
				{
					if (!write_h_stream_.is_open())
						return false;

					write_header();

					write_namespace_begin(write_h_stream_);

					write_struct();

					write_namespace_end(write_h_stream_);

					return true;
				}

				std::string read_to_spilt(std::ifstream& ifs, const char sp)
				{
					std::array<char, 1024> lines;

					ifs.get(&lines[0], 1024, sp);

					ifs.get();

					return std::string(lines.data());
				}

				void read_struct_head(struct_impl& impl)
				{
					auto str = read_to_spilt(read_file_stream_, '{');

					common::trip(str, '\r', '\n', '\t');

					auto pos = str.find_first_of('=');

					if (pos == std::string::npos)
					{
						impl.name_ = common::trip(str, ' ');
					}
					else
					{
						auto str_name = str.substr(0, pos);

						impl.name_ = common::trip(str_name, '\r', '\n', '\t', ' ');

						auto str_type = str.substr(pos + 1, str.size() - pos - 1);

						impl.default_type_ = common::trip(str_type, '\r', '\n', '\t', ' ');
					}
				}

				void read_struct_body(struct_impl& impl)
				{
					auto str = read_to_spilt(read_file_stream_, '}');

					common::trip(str, '\r', '\n', '\t');

					int count = 0;

					while (!str.empty())
					{
						auto pos = str.find_first_of(';');

						if (pos == std::string::npos)
							continue;

						impl.member_list_.push_back({});
						auto& member = impl.member_list_.back();

						pos = str.find_first_of(']]');
						if (pos != std::string::npos)
						{
							member.property_ = str.substr(2, pos - 2);
							str = str.substr(pos + 2);
						}

						pos = str.find_first_of(':');
						if (pos == std::string::npos)
							break;

						member.type_ = str.substr(0, pos);

						common::trip(member.type_, ' ');

						str = str.substr(pos + 1);

						pos = str.find_first_of('=');

						if (pos == std::string::npos)
							break;

						member.name_ = str.substr(0, pos);

						common::trip(member.name_, ' ');

						str = str.substr(pos + 1);

						pos = str.find_first_of(';');

						member.flag_ = std::atoi(str.substr(0, pos).c_str());

						if (member.property_ == "require")
						{
							impl.require_list_.push_back(member.flag_);
						}

						str = str.substr(pos + 1);
					}
				}

				void write_header()
				{
					for (auto iter : hf_.list_)
					{
						write_h_stream_ << "#include <" + iter + ">" << enter;
					}

					write_h_stream_ << "#include <elastic.hpp>" << enter;

					write_h_stream_ << enter;
				}

				void write_namespace_begin(std::ofstream& fs)
				{
					if (namespace_file_.empty())
						return;

					fs << "namespace " << namespace_file_ << enter << "{" << enter;
					layout_++;
				}

				void write_namespace_end(std::ofstream& fs)
				{
					if (namespace_file_.empty())
						return;

					fs << "}" << enter;
				}

				void write_struct()
				{
					for (auto& iter : sf_.list_)
					{
						write_h_stream_ << advance_tab() << iter.type_ << " " << iter.name_;

						switch (iter.kt_)
						{
						case keyword_type::enumclass_t:
							{
								if (!iter.default_type_.empty())
								{
									write_h_stream_ << advance_tab() << " : " << iter.default_type_;
								}
							}
							break;
						case keyword_type::message_t:
							{
								// write_h_stream_ << " : public parse";
							}
							break;
						default:
							break;
						}

						write_h_stream_ << enter;

						write_h_stream_ << advance_tab() << "{" << enter;

						if (iter.kt_ == keyword_type::message_t)
						{
							layout_++;
							for (auto& impl : iter.member_list_)
							{
								write_h_stream_ << advance_tab();
								if (impl.property_ == "optional")
								{
									write_h_stream_ << "elastic::optional<" + impl.type_ + "> ";
								}
								else if (impl.property_ == "require")
								{
									write_h_stream_ << "elastic::require<" + impl.type_ + "> ";
								}
								else if (impl.property_ == "repeated")
								{
									write_h_stream_ << "elastic::vector<" + impl.type_ + "> ";
								}
								else
								{
									write_h_stream_ << impl.type_ + " ";
								}

								write_h_stream_ << impl.name_ << ";" << double_enter;
							}
						}

						write_meta_member(iter);

						//----
						write_serialize();

						write_parse();

						layout_--;

						write_h_stream_ << advance_tab() << "};" << double_enter;
					}
				}

				void write_meta_member(struct_impl& iter)
				{
					write_h_stream_ << advance_tab() << "static auto member()" << enter;
					write_h_stream_ << advance_tab() << "{" << enter;

					layout_++;
					write_h_stream_ << advance_tab() << "struct member_impl" << enter;
					write_h_stream_ << advance_tab() << "{" << enter;
					layout_++;
					write_h_stream_ << advance_tab() << "static constexpr auto member_list() { return std::make_tupe(";
					std::size_t count = 1;
					for (auto& impl : iter.member_list_)
					{
						write_h_stream_ << "std::pair{ " << impl.flag_ << R"(, ")" << impl.name_ << R"(" })";
						if (count != iter.member_list_.size())
							write_h_stream_ << ", ";
					}

					write_h_stream_ << ")};" << enter;
					write_h_stream_ << advance_tab() << "static constexpr auto index() { return " << iter.type_ << "; }"
									<< enter;

					write_h_stream_ << advance_tab() << "static constexpr auto require() { std::array<int,"
									<< iter.require_list_.size()
									<< "> { ";

					count = 1;
					for (auto& req : iter.require_list_)
					{
						write_h_stream_ << req;

						if (count == iter.require_list_.size())
							continue;

						write_h_stream_ << ",";
					}

					write_h_stream_ << "};}" << enter;
					
					layout_--;
					write_h_stream_ << advance_tab() << "};" << double_enter;

					write_h_stream_ << advance_tab() << "return member_impl{};" << enter;
					layout_--;
					write_h_stream_ << advance_tab() << "}" << double_enter;
				}

				void write_serialize()
				{
					write_h_stream_ << advance_tab()
									<< "auto serialize_to_array() -> std::pair<const char*,std::size_t>"
									<< enter;

					write_h_stream_ << advance_tab() << "{" << enter;
					layout_++;
					write_h_stream_ << advance_tab() << "elastic::iostream ios{};" << enter;
					write_h_stream_ << advance_tab() << "ios << *this;" << double_enter;
					write_h_stream_ << advance_tab() << "return { (char*)ios.data(), ios.size() };" << enter;
					layout_--;
					write_h_stream_ << advance_tab() << "}" << double_enter;

					write_h_stream_ << advance_tab() << "std::string serialize_as_string()" << enter;
					write_h_stream_ << advance_tab() << "{" << enter;
					layout_++;
					write_h_stream_ << advance_tab() << "auto [buf,size] = serialize_to_array();" << double_enter;
					write_h_stream_ << advance_tab() << "return std::string(buf, size);" << enter;
					layout_--;
					write_h_stream_ << advance_tab() << "}" << double_enter;
				}

				void write_parse()
				{
					write_h_stream_ << advance_tab() << "bool parse_from_array(const void* buf, int size)" << enter;
					write_h_stream_ << advance_tab() << "{" << enter;
					layout_++;
					write_h_stream_ << advance_tab()
									<< "elastic::iostream ios(std::span<uint8_t>((uint8_t*)buf, (uint8_t*)buf + size));"
									<< double_enter;
					write_h_stream_ << advance_tab() << "ios >> *this;" << double_enter;
					write_h_stream_ << advance_tab() << "return true;" << double_enter;
					layout_--;
					write_h_stream_ << advance_tab() << "}" << double_enter;

					write_h_stream_ << advance_tab() << "bool parse_from_string(const std::string& buf)" << enter;
					write_h_stream_ << advance_tab() << "{" << enter;
					layout_++;
					write_h_stream_ << advance_tab() << "elastic::iostream ios{ std::span<const char>{buf} };" << enter
									<< enter;
					write_h_stream_ << advance_tab() << "ios >> *this;" << double_enter;
					write_h_stream_ << advance_tab() << "return true;" << enter;
					layout_--;
					write_h_stream_ << advance_tab() << "}" << enter;
				}

				std::string advance_tab()
				{
					std::string result{};
					for (std::size_t i = 0; i < layout_; ++i)
					{
						result += tab;
					}

					return result;
				}

			private:
				std::ifstream read_file_stream_;

				std::ofstream write_h_stream_;

				header_file hf_;

				std::string namespace_file_;

				struct_file sf_;

				int layout_;
			};
		} // namespace cpp
	}	  // namespace compiler
} // namespace elastic
