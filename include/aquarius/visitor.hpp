#pragma once

namespace aquarius
{
	class context;

	template <typename Request, typename ReturnT = int>
	class visitor
	{
	public:
		virtual ~visitor() {}
		virtual ReturnT visit(std::shared_ptr<Request> req_ptr) = 0;
	};

	template<typename R = int>
	class visiable
	{
	public:
		virtual ~visiable() = default;

		virtual R accept(std::shared_ptr<context> ctx_ptr) = 0;

	protected:
		template<typename BasicMessage, typename T>
		R accept_impl(std::shared_ptr<T> msg_ptr, std::shared_ptr<context> ctx_ptr)
		{
			using visitor_type = visitor<T, int> ;
			using base_type =  visitor<BasicMessage, int>;


			if (auto ptr = std::dynamic_pointer_cast<visitor_type>(ctx_ptr))
			{
				return ptr->visit(msg_ptr);
			}
			else if (auto base_ptr = std::dynamic_pointer_cast<base_type>(ctx_ptr))
			{
				return base_ptr->visit(msg_ptr);
			}

			return R{};
		}
	};
}