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
		cmd_options()
			: desc_("server usage")

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

		auto get_transfer() const
		{
			return transfer_;
		}

		std::size_t pool_size() const
		{
			return pool_size_;
		}

		int listen_port() const
		{
			return listen_port_;
		}

		std::string server_name() const
		{
			return name_;
		}

	private:
		boost::program_options::options_description desc_;

		boost::program_options::variables_map vm_;

		std::pair<std::string, uint16_t> transfer_;

		std::size_t pool_size_;

		int listen_port_;

		std::string name_;
	};
} // namespace aquarius