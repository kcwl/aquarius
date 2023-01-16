#pragma once
#include <aquarius/detail/type_traits.hpp>
#include <aquarius/impl/connect.hpp>
#include <aquarius/impl/invoke.hpp>

namespace aquarius
{
	namespace impl
	{
		class context;

		class session : public std::enable_shared_from_this<session>
		{
		public:
			session(std::shared_ptr<connector> conn_ptr)
				: conn_ptr_(conn_ptr)
			{}

		public:
			template <typename _Message>
			bool write(_Message&& resp, std::chrono::steady_clock::duration timeout)
			{
				flex_buffer_t fs;

				std::forward<_Message>(resp).visit(&fs, visit_mode::output);

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

				auto ctx = use_context(id);

				if (!ctx)
				{
					auto self = shared_from_this();

					ctx = context_invoke_helper::invoke(id);

					if (!ctx)
					{
						ctx = std::make_shared<context>("", 0s);
					}

					attach_context(id, ctx);
				}

				auto req_ptr = message_invoke_helpter::invoke(id);

				if (!req_ptr)
				{
					return false;
				}

				if (!req_ptr->visit(&read_buffer, visit_mode::input))
					return false;

				if (!req_ptr->accept(ctx))
					return false;

				return true;
			}

			std::shared_ptr<context> use_context(uint32_t id)
			{
				auto iter = ctxs_.find(id);

				if (iter == ctxs_.end())
					return nullptr;

				return iter->second;
			}

			void attach_context(uint32_t id, std::shared_ptr<context> ctx_ptr)
			{
				auto iter = ctxs_.find(id);
				if (iter != ctxs_.end())
					return;

				ctxs_.insert({ id, ctx_ptr });
			}

			void close()
			{
				return conn_ptr_->close();
			}

		private:
			std::shared_ptr<connector> conn_ptr_;

			std::map<uint32_t, std::shared_ptr<context>> ctxs_;
		};
	} // namespace impl
} // namespace aquarius