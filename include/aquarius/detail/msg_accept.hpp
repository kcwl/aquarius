#pragma once
#include <memory>

namespace aquarius
{
	template <typename Request, typename HandleMethod, typename Session>
	void msg_accept(std::shared_ptr<Request> request, std::shared_ptr<HandleMethod> method_ptr, std::shared_ptr<Session> session_ptr)
	{
		return request->accept(method_ptr, session_ptr);
	}
} // namespace aquarius