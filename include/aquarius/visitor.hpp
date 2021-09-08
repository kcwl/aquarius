#pragma once

namespace aquarius
{
	template <typename FinalT, typename ReturnT = int>
	class visitor
	{
	public:
		typedef ReturnT ReturnType;
		typedef FinalT FinalType;

		virtual ~visitor() {}
		virtual ReturnType visit(std::shared_ptr<FinalType> visited) = 0;
	};
}