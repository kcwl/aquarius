#pragma once

namespace aquarius
{
	class basic_message;

	template <typename Request, typename Func, typename ReturnT = int>
	class visitor
	{
	public:
		virtual ~visitor() {}
		virtual ReturnT visit(std::shared_ptr<Request> visited, Func&& f) = 0;
	};


	template<typename Context, typename Func>
	class visiable
	{
	public:
		virtual int accept(std::shared_ptr<Context> ctx_ptr, Func&& f) = 0;

	protected:
		template<typename Request, typename Context, typename Func>
		int accept_impl(std::shared_ptr<Request> msg_ptr, std::shared_ptr<Context> ctx_ptr, Func&& f)
		{
			using request_type = visitor<Request, Func, int>;

			auto request_ptr = std::dynamic_pointer_cast<request_type>(ctx_ptr);

			if (request_ptr == nullptr)
				return 0;

			return request_ptr->visit(msg_ptr, std::forward<Func>(f));
		}
	};
}