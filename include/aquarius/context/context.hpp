#pragma once
#include <aquarius/context/visitor.hpp>
#include <memory>

namespace aquarius
{
	namespace ctx
	{
		class context : public ctx::visitor<tcp::xmessage, int>
		{
		public:
			context(const std::string& name)
				: name_(name)
			{
				on_connected();
			}

			context(const context&) = delete;

			context(context&&) = default;

			virtual ~context()
			{
				on_closed();
			}

			context& operator=(const context&) = delete;

			DEFINE_VISITOR(tcp::xmessage, int)

		public:
			virtual int on_connected()
			{
				return 0;
			}

			virtual int on_closed()
			{
				return 0;
			}

			virtual int on_timeout([[maybe_unused]] std::shared_ptr<srv::basic_connector> session_ptr)
			{
				return 0;
			}

		protected:
			virtual void on_error([[maybe_unused]] int result){};

		protected:
			std::shared_ptr<srv::basic_connector> conn_ptr_;

			std::string name_;
		};

		template <typename _Request, typename _Response>
		class context_impl : public context, public ctx::visitor<_Request, int>
		{
		public:
			context_impl(const std::string& name)
				: context(name)
				, request_ptr_(nullptr)
			{}

		public:
			virtual int on_connected() override
			{
				return 0;
			}

			virtual int on_closed() override
			{
				return 0;
			}

			virtual int on_timeout(std::shared_ptr<srv::basic_connector> conn_ptr) override
			{
				return 0;
			}

			virtual int visit(_Request* req, std::shared_ptr<srv::basic_connector> conn_ptr)
			{
				request_ptr_ = req;

				conn_ptr_ = conn_ptr;

				return handle();
			}

		protected:
			virtual int handle() = 0;

			virtual void on_error([[maybe_unused]] int result) override
			{}

			bool send_response(int result)
			{
				response_.header().clone(request_ptr_->header());

				response_.header().result_ = result;

				core::flex_buffer_t fs;

				response_.visit(fs, core::visit_mode::output);

				conn_ptr_->write(std::move(fs));

				return true;
			}

		protected:
			_Request* request_ptr_;

			_Response response_;
		};
	} // namespace impl
} // namespace aquarius