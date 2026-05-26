#pragma once
#include <string>

namespace aquarius
{
	namespace serviced
	{
		class customer
		{
		public:
			customer(std::size_t id = 0);

			virtual ~customer() = default;

		public:
			std::size_t id() const;

			std::string name() const;

			std::string& name();

			void name(const std::string& name);

			std::string host() const;

			std::string& host();

			void host(const std::string& host);

			int32_t port() const;

			int32_t& port();

			void port(int32_t port);

			bool healthy() const;

			bool& healthy();

			void healthy(bool v);

			std::string group() const;

			std::string& group();

			void group(const std::string& group);

			std::string topic() const;

			std::string& topic();

			void topic(const std::string& topic);

			int32_t weight() const;

			int32_t& weight();

			void weight(int32_t w);

			std::string version() const;

			std::string& version();

			void version(const std::string& version);

		private:
			std::size_t id_;

			std::string name_;

			std::string host_;

			int32_t port_;

			bool healthy_;

			std::string group_;

			std::string topic_;

			int32_t weight_;

			std::string version_;
		};
	} // namespace serviced
} // namespace aquarius