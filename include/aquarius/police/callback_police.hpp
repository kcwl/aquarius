#pragma once
#include "../router.hpp"


namespace aquarius
{
	using callback_router = detail::router<void>;

	class callback_police
	{
	public:
		enum class police_type
		{
			conn,
			disconn
		};

		template<typename F>
		void regist_callback(police_type pt, F&& f)
		{
			router_.regist("pt_" + std::to_string(static_cast<int>(pt)), std::forward<F>(f));
		}

		template<police_type pt>
		auto invoke_callback()
		{
			router_.invoke("pt_" + std::to_string(static_cast<int>(pt)));
		}

	private:
		callback_router router_;
	};
}