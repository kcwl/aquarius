#pragma once
#include <aquarius/asio.hpp>
#include <aquarius/error_code.hpp>
#include <aquarius/serialize/flex_buffer.hpp>
#include <memory>
#include <optional>

namespace aquarius
{
	template <typename Request>
	class basic_handler
	{
	public:
		using request_t = Request;

	public:
		basic_handler(const std::string& name)
			: request_ptr_(new Request)
			, name_(name)
		{}

	public:
		virtual auto visit(error_code& ec) -> awaitable<flex_buffer>
		{
			XLOG_ERROR() << "invalid protocol";

			ec = boost::asio::error::eof;

			co_return flex_buffer{};
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