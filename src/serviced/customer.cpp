#include "customer.h"

namespace aquarius
{
	namespace serviced
	{
		customer::customer(std::size_t id)
			: id_(id)
			, name_()
			, host_()
			, port_()
			, healthy_()
			, group_()
			, weight_()
			, version_()
		{}

		std::size_t customer::id() const
		{
			return id_;
		}

		std::string customer::name() const
		{
			return name_;
		}

		std::string& customer::name()
		{
			return name_;
		}

		void customer::name(const std::string& name)
		{
			name_ = name;
		}

		std::string customer::host() const
		{
			return host_;
		}

		std::string& customer::host()
		{
			return host_;
		}

		void customer::host(const std::string& host)
		{
			host_ = host;
		}

		int32_t customer::port() const
		{
			return port_;
		}

		int32_t& customer::port()
		{
			return port_;
		}

		void customer::port(int32_t port)
		{
			port_ = port;
		}

		bool customer::healthy() const
		{
			return healthy_;
		}

		bool& customer::healthy()
		{
			return healthy_;
		}

		void customer::healthy(bool v)
		{
			healthy_ = v;
		}

		std::string customer::group() const
		{
			return group_;
		}

		std::string& customer::group()
		{
			return group_;
		}

		void customer::group(const std::string& group)
		{
			group_ = group;
		}

		std::string customer::topic() const
		{
			return topic_;
		}

		std::string& customer::topic()
		{
			return topic_;
		}

		void customer::topic(const std::string& topic)
		{
			topic_ = topic;
		}

		int32_t customer::weight() const
		{
			return weight_;
		}

		int32_t& customer::weight()
		{
			return weight_;
		}

		void customer::weight(int32_t w)
		{
			weight_ = w;
		}

		std::string customer::version() const
		{
			return version_;
		}

		std::string& customer::version()
		{
			return version_;
		}

		void customer::version(const std::string& version)
		{
			version_ = version;
		}
	} // namespace serviced
} // namespace aquarius