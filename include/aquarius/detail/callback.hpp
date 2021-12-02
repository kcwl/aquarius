#pragma once
#include <functional>


namespace aquarius
{
	namespace detail
	{
		template<typename _T>
		class callback
		{
		public:
			using connect_callback = std::function<void(std::shared_ptr<_T>)>;

			using disconnect_callback = std::function<void(std::shared_ptr<_T>)>;

		public:
			callback() = default;
			virtual ~callback() = default;

		public:
			connect_callback conn_cb_;

			disconnect_callback disconn_cb_;
		};
	}
}