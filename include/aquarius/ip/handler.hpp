#pragma once
#include <aquarius/detail/flex_buffer.hpp>

namespace aquarius
{
	template <typename Response>
	class basic_handler
	{
	public:
		using handle_message_t = Response;

		using session_callback = std::function<asio::awaitable<error_code>(flex_buffer&, const std::string&)>;

	public:
		basic_handler(const std::string& name)
			: name_(name)
		{}

	public:
		Response& response()
		{
			return response_;
		}

		std::string name() const
		{
			return name_;
		}

		virtual error_code visit(flex_buffer& buffer)
		{
			return response().consume(buffer);
		}

		void attach_session(const session_callback& cb)
		{
			cb_ = cb;
		}

		session_callback session()
		{
			return cb_;
		}

		void generate_members(std::vector<std::string>& member_names, std::vector<std::string>& member_types)
		{
			member_names = response_.get_member_names();

			member_types = response_.get_member_types();
		}

		virtual auto handle() -> asio::awaitable<error_code> = 0;

	private:
		std::string name_;

		Response response_;

		session_callback cb_;
	};

	template <typename Request, typename Response>
	class handler : public basic_handler<Response>
	{
	public:
		using base = basic_handler<Response>;

		using handle_message_t = Request;

	public:
		handler(const std::string& name)
			: base(name)
			, request_ptr_(std::make_shared<Request>())
		{}

	public:
		virtual error_code visit(flex_buffer& buffer)
		{
			return request()->consume(buffer);
		}

		std::shared_ptr<Request> request() const
		{
			return request_ptr_;
		}

	private:
		std::shared_ptr<Request> request_ptr_;

		std::string name_;
	};
} // namespace aquarius