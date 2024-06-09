#pragma once
#include <aquarius/core/error_code.hpp>

namespace aquarius
{
	class basic_connect;
}

namespace aquarius
{
	template <typename _Ty>
	class basic_visitor
	{
	public:
		basic_visitor() = default;
		virtual ~basic_visitor() = default;

	public:
		virtual error_code visit(_Ty visited, basic_connect*) = 0;
	};

	template <typename _Ty>
	class shared_visitor : public virtual basic_visitor<std::shared_ptr<_Ty>>
	{
	public:
		shared_visitor() = default;
		virtual ~shared_visitor() = default;
	};

	template <typename _Ty>
	class bare_visitor : public virtual basic_visitor<_Ty*>
	{
	public:
		bare_visitor() = default;

		virtual ~bare_visitor() = default;
	};
} // namespace aquarius