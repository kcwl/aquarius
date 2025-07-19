#pragma once
#include <aquarius/config.hpp>
#include <boost/program_options.hpp>
#include <iostream>
#include <string>

namespace aquarius
{
	class cmd_options
	{
	public:
		cmd_options(const std::string& desc)
			: desc_str_(desc)
			, desc_(desc)
		{

		}

	public:
		void load_options(int argc, char* argv[])
		{
			try
			{
				boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc_), vm_);

				boost::program_options::notify(vm_);
			}
			catch (const std::exception& e)
			{
				std::cout << e.what() << std::endl;
			}
		}


		void load_options(const std::vector<std::string>& argvs)
		{
			int argc = static_cast<int>(argvs.size());

			char** argv = new char*[argc];

			int i = 0;
			for (auto a : argvs)
			{
				argv[i++] = a.data();
			}

			try
			{
				boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc_), vm_);

				boost::program_options::notify(vm_);
			}
			catch (const std::exception& e)
			{
				std::cout << e.what() << std::endl;
			}
		}

		template<typename T>
		void add_option(const std::string& option, const std::string& desc)
		{
			desc_.add_options()(option.c_str(), boost::program_options::value<T>(), desc.c_str());
		}

		template<typename T>
		T option(const std::string& op)
		{
			if (!vm_.count(op))
				return {};

			return vm_[op].as<T>();
		}

		std::string desc() const
		{
			return desc_str_;
		}

		auto options() const
		{
			return desc_.options();
		}

	private:
		std::string desc_str_;

		boost::program_options::options_description desc_;

		boost::program_options::variables_map vm_;
	};
} // namespace aquarius