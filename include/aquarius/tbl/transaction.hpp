#pragma once

namespace aquarius
{
	namespace sql
	{
		template <typename T>
		class trans_guard
		{
		public:
			trans_guard(T* service)
				: service_(service)
			{
				service_->begin();
			}

			~trans_guard()
			{
				service_->end();
			}

		private:
			T* service_;
		};

		template <typename T>
		trans_guard(T*) -> trans_guard<T>;

	} // namespace sql
} // namespace aquarius