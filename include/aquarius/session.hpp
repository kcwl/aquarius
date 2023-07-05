#pragma once
#include <string>
#include "flex_buffer.hpp"

namespace aquarius
{
	class basic_session
	{
	public:
		virtual std::string remote_address() = 0;

		virtual std::size_t remote_address_u() = 0;

		virtual int32_t remote_port() = 0;

		virtual std::size_t uid() = 0;

		virtual void async_write(flex_buffer_t&& buffer) = 0;
	};

	template<typename _Connector>
	class session : public basic_session
	{
	public:
		explicit session()
			: session(nullptr)
		{

		}

		session(std::shared_ptr<_Connector> conn_ptr)
			: conn_ptr_(conn_ptr) {}

	public:
		virtual std::string remote_address() override
		{
			if (!conn_ptr_)
				return std::string{};

			return conn_ptr_->remote_address();
		}

		virtual std::size_t remote_address_u() override
		{
			if (!conn_ptr_)
				return std::size_t{};

			return conn_ptr_->remote_address_u();
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

			return conn_ptr_->uuid();
		}

		virtual void async_write(flex_buffer_t&& buffer) override
		{
			return conn_ptr_->async_write(std::forward<flex_buffer_t>(buffer));
		}

	private:
		std::shared_ptr<_Connector> conn_ptr_;
	};
}