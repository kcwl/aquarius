#pragma once
#include <string>

namespace aquarius
{
	class basic_session
	{
	public:
		virtual std::string remote_ip() = 0;

		virtual std::size_t remote_ip_u() = 0;

		virtual int32_t remote_port() = 0;

		virtual std::size_t uid() = 0;

		virtual void async_write(flex_buffer_t&& buffer) = 0;
	};

	template<typename _Connector>
	class session
	{
	public:
		explicit session()
			: session(nullptr)
		{

		}

		session(std::shared_ptr<_Connector> conn_ptr)
			: conn_ptr_(conn_ptr) {}

	public:
		virtual std::string remote_ip() override
		{
			if (!conn_ptr_)
				return std::string{};

			return conn_ptr_->remote_ip();
		}

		virtual std::size_t remote_ip_u() override
		{
			if (!conn_ptr_)
				return std::size_t{};

			return conn_ptr_->remote_ip_u();
		}

		virtual int32_t remote_port() override
		{
			if (!conn_ptr_)
				return int32_t{};

			return conn_ptr_->remote_port();
		}

		virtual std::size_t uid() override
		{
			if (!conn_ptr_)
				return std::size_t{};

			return conn_ptr_->uid();
		}

		virtual void async_write(flex_buffer_t&& buffer) override
		{
			return conn_ptr_->async_write(std::forward<flex_buffer_t>(buffer));
		}

	private:
		std::shared_ptr<_Connector> conn_ptr_;
	};
}