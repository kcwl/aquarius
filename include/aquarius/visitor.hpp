#pragma once

namespace aquarius
{
	class connect;

	template <typename FinalT, typename ReturnT = int>
	class visitor
	{
	public:
		typedef ReturnT ReturnType;
		typedef FinalT FinalType;

		virtual ~visitor() {}
		virtual ReturnType visit(std::shared_ptr<FinalType> visited,std::shared_ptr<connect> conn_ptr) = 0;
	};
}