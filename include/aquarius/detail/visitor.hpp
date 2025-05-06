#pragma once
#include <memory>

namespace aquarius
{
	class session_base;

	class visitor_base
	{};

	template <typename T>
	class visitor : public visitor_base
	{
	public:
		virtual int visit(T* request, std::shared_ptr<session_base>) = 0;
	};
} // namespace aquarius