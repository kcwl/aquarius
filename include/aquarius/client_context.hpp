#pragma once
#include <aquarius/basic_context.hpp>

namespace aquarius
{
	template <typename Response>
	class client_context : public basic_context<Response>
	{
		using base_type = basic_context<Response>;

	public:
		client_context(const std::string& name, const std::chrono::milliseconds& timeout = 100ms)
			: base_type(name, timeout)
		{}
	};

	struct client_invoke
	{
		template<typename Session>
		static void invoke(std::size_t context_id, flex_buffer buffer, std::shared_ptr<Session>)
		{
			client_invokes::get_mutable_instance().invoke(context_id, buffer);
		}

		template<typename Response, typename  Func>
		static void regist(std::size_t context_id, Func&& func)
		{
			client_invokes::get_mutable_instance().template regist<Response>(context_id, std::forward<Func>(func));
		}
	};
} // namespace aquarius