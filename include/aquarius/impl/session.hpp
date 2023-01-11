#pragma once
#include <aquarius/detail/type_traits.hpp>
#include <aquarius/impl/connect.hpp>
#include <aquarius/impl/router.hpp>

namespace aquarius
{
	namespace impl
	{
		class session : public std::enable_shared_from_this<session>
		{
		public:
			session(std::shared_ptr<connector> conn_ptr)
				: conn_ptr_(conn_ptr)
			{}

		public:
			template <typename _Message>
			bool write(_Message&& resp, int timeout)
			{
				flex_buffer_t fs;

				std::forward<_Message>(resp).to_message(fs);

				conn_ptr_->write(std::move(fs), timeout);

				return true;
			}

			bool read()
			{
				flex_buffer_t& read_buffer = conn_ptr_->get_read_buffer();

				if (read_buffer.size() < sizeof(uint32_t))
					return false;

				uint32_t id{};

				elastic::binary_iarchive ia(read_buffer);
				ia >> id;

				auto self = shared_from_this();

				auto res = invoke_helper::invoke(id, self, read_buffer);

				if (!res)
				{
					return false;
				}

				return true;
			}

			void close()
			{
				return conn_ptr_->close();
			}

		private:
			std::shared_ptr<connector> conn_ptr_;
		};
	} // namespace impl
} // namespace aquarius