#pragma once

namespace aquarius
{
	template <typename Request, typename Func, typename ReturnT = int>
	class visitor
	{
	public:
		virtual ~visitor() {}
		virtual ReturnT visit(std::shared_ptr<Request> visited,Func&& f) = 0;
	};
}