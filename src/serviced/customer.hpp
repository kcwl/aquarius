#pragma once
#include "basic_customer.hpp"

namespace aquarius
{
	namespace serviced
	{
		template <typename T>
		class customer : public basic_customer<T>, std::enable_shared_from_this<customer<T>>
		{
			friend class service_center;

			using func_t = typename basic_customer<T>::func_t;

		public:
			customer(std::size_t id)
				: id_(id)
				, task_total_()
				, name_()
				, host_()
				, port_()
				, healthy_()
				, group_()
				, weight_()
				, version_()
			{

			}

			virtual ~customer() = default;

		public:
			std::size_t id() const;

			auto feedback(flex_buffer& buffer, error_code& ec, std::size_t id) -> asio::awaitable<flex_buffer>;

			void complete();

			std::string name() const
			{
				return name_;
			}

			std::string& name()
			{
				return name_;
			}

			void name(const std::string& name)
			{
				name_ = name;
			}

			std::string host() const
			{
				return host_;
			}

			std::string& host()
			{
				return host_;
			}

			void host(const std::string& host)
			{
				host_ = host;
			}

			int32_t port() const
			{
				return port_;
			}

			int32_t& port()
			{
				return port_;
			}

			void port(int32_t port)
			{
				port_ = port;
			}

			bool healthy() const
			{
				return healthy_;
			}

			bool& healthy()
			{
				return healthy_;
			}

			void healthy(bool v)
			{
				healthy_ = v;
			}

			virtual std::string group() const override
			{
				return group_;
			}

			std::string& group()
			{
				return group_;
			}

			void group(const std::string& group)
			{
				group_ = group;
			}

			int32_t weight() const
			{
				return weight_;
			}

			int32_t& weight()
			{
				return weight_;
			}

			void weight(int32_t w)
			{
				weight_ = w;
			}

			std::string version() const
			{
				return version_;
			}

			std::string& version()
			{
				return version_;
			}

			void version(const std::string& version)
			{
				version_ = version;
			}

			virtual func_t subscribe() override
			{
				auto self = this->shared_from_this();
				auto f = [this, self](flex_buffer& buffer,
									  int32_t method) -> asio::awaitable<std::expected<flex_buffer, error_code>>
				{ co_return co_await this->async_send(buffer, method); };

				return f;
			}

			virtual auto heart() -> asio::awaitable<void> override
			{
				co_return;
			}

		private:
			std::size_t id_;

			std::atomic_size_t task_total_;

			std::string name_;

			std::string host_;

			int32_t port_;

			bool healthy_;

			std::string group_;

			int32_t weight_;

			std::string version_;
		};
	} // namespace serviced
} // namespace aquarius