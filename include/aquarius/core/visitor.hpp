#pragma once

namespace aquarius
{
	template <typename _Ty, typename _Transfer>
	class basic_visitor
	{
	public:
		basic_visitor() = default;
		virtual ~basic_visitor() = default;

	public:
		virtual bool visit(_Ty visited, std::shared_ptr<_Transfer>) = 0;
	};

	template <typename _Ty, typename _Transfer>
	class shared_visitor : public virtual basic_visitor<std::shared_ptr<_Ty>, _Transfer>
	{
	public:
		shared_visitor() = default;
		virtual ~shared_visitor() = default;
	};

	template <typename _Ty, typename _Transfer>
	class bare_visitor : public virtual basic_visitor<_Ty*, _Transfer>
	{
	public:
		bare_visitor() = default;

		virtual ~bare_visitor() = default;
	};
} // namespace aquarius