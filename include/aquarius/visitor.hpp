#pragma once

namespace aquarius
{
	template <typename Request, typename ReturnT = int>
	class visitor
	{
	public:
		virtual ~visitor() {}
		virtual ReturnT visit(std::shared_ptr<Request> visited) = 0;
	};
}