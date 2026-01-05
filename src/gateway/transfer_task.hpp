#pragma once
#include <aquarius/serialize/flex_buffer.hpp>

namespace aquarius
{
	namespace gateway
	{
		template <typename Func>
		struct transfer_task
		{
			using return_type = void;

			transfer_task(Func&& f, flex_buffer& buffer)
				: f_(f)
				, buffer_(buffer)
			{}

			template <typename Core>
			void operator()(std::shared_ptr<Core> core_ptr)
			{
				core_ptr->template async_call(buffer_, f_);
			}

			flex_buffer& buffer_;
			Func f_;
		};

	} // namespace gateway
} // namespace aquarius