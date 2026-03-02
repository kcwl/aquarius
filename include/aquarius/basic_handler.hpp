#pragma once
#include <aquarius/asio.hpp>
#include <aquarius/basic_protocol.hpp>
#include <aquarius/error_code.hpp>
#include <memory>

namespace aquarius
{
	template <typename Session>
	class basic_handler
	{
		using request_t = null_protocol;

	public:
		basic_handler(const std::string& name)
			: name_(name)
		{}

	public:
		virtual auto visit(std::shared_ptr<Session> sessoin_ptr) -> awaitable<error_code>
		{
			XLOG_ERROR() << "invalid protocol";

			co_return boost::asio::error::eof;
		}

		std::shared_ptr<request_t> request() const
		{
			return request_ptr_;
		}

		std::string name() const
		{
			return name_;
		}

	private:
		std::shared_ptr<request_t> request_ptr_;

		std::string name_;
	};
} // namespace aquarius