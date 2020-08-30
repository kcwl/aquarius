#pragma once
#include <cstddef>
#include <future>
#include "context.hpp"
#include "detail/factory.hpp"
#include <aquarius/qualify.hpp>
#include "test.hpp"

namespace aquarius
{
	namespace aqnet
	{
		class async_control
		{
		public:
			virtual ~async_control()
			{
			}

		public:
			template<std::size_t N>
			void collect(std::array<std::byte, N>&& buff, std::size_t bytes_transferred)
			{
				std::for_each_n(buff.begin(), bytes_transferred,
					[this](auto iter)
					{
						buffer_.push_back(iter);
					});
			}

			void complete(iostream&& stream, std::size_t bytes_transferred, int error)
			{
				//if(stream.size() != bytes_transferred)
				//	return;

				this->bytes_transferred_ = bytes_transferred;

				this->error_ = error;


				auto protocol_id = std::forward<iostream>(stream).get<uint32_t>();

				//处理message
				auto context = detail::context_register<basic_context>::instance().attach(std::move(protocol_id));

				if(!context.has_value())
					return;

				auto context_ptr = context.value()();

				context_ptr->process(std::forward<iostream>(stream));
			}

		private:
			std::size_t bytes_transferred_;

			int error_;

			std::vector<std::byte> buffer_;
		};
	}
}