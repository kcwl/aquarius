#pragma once
#include <memory>


namespace aquarius
{
	namespace detail
	{
		template<class T>
		class message_body
		{
		public:
			message_body()
				: val_ptr_(new T{})
			{
			}
		public:
			std::size_t parse_bytes(iostream&& stream, int length)
			{
				return val_ptr_->parse_bytes(std::forward<iostream>(stream), length);
			}

			void* to_bytes()
			{
				return val_ptr_->to_bytes();
			}

			auto body() -> std::shared_ptr<T>
			{
				return val_ptr_;
			}

		private:
			std::shared_ptr<T> val_ptr_;
		};
	}
}