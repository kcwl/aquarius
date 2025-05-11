#pragma once
#include <aquarius/logger.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>

namespace aquarius
{
	class ini_tree
	{
	public:
		explicit ini_tree(const std::string& file_name, int mode = std::ios::in)
			: pt_()
			, file_name_(file_name)
		{
			if (mode == std::ios::in)
			{
				boost::property_tree::ini_parser::read_ini(file_name_, pt_);
			}
		}

	public:
		template <typename _Ty>
		_Ty get(const std::string& name)
		{
			_Ty value{};
			try
			{
				value = pt_.get<_Ty>(name);
			}
			catch (std::exception& ec)
			{
				XLOG_ERROR() << "[" << file_name_ << "] " << ec.what();
			}

			return value;
		}

		template <typename _Ty>
		void put(const std::string& name, _Ty value)
		{
			pt_.put<_Ty>(name, value);
		}

		template <typename _Func>
		void for_each(_Func&& f)
		{
			for (const auto& sec : pt_)
			{
				std::forward<_Func>(f)(sec);
			}
		}

		void save()
		{
			boost::property_tree::ini_parser::write_ini(file_name_, pt_);
		}

	private:
		boost::property_tree::ptree pt_;
		std::string file_name_;
	};
} // namespace aquarius