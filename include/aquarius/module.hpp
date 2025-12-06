#pragma once
#include <aquarius/asio.hpp>
#include <aquarius/error_code.hpp>
#include <aquarius/ip/router.hpp>

namespace aquarius
{
	class module_base
	{
	public:
		module_base() = default;

	public:
		virtual auto visit() -> awaitable<error_code>
		{
			co_return co_await handle();
		}

	protected:
		virtual auto handle() -> awaitable<error_code> = 0;
	};

	class module_router : public aquarius::singleton<module_router>
	{
		using T = std::function<void()>;

	public:
		void regist(const T& func)
		{
			routers_.push_back(func);
		}

		void run()
		{
			for (auto& f : routers_)
			{
				f();
			}
		}

	private:
		std::vector<T> routers_;
	};

	template <typename Module>
	struct auto_module_register
	{
		auto_module_register(std::string_view topic)
		{
			auto func = []
			{
				auto module_ptr = std::make_shared<Module>();

				module_ptr->visit();
			};

			router<proto_tag::module_, int>::get_mutable_instance().regist(topic, func);
		}
	};
} // namespace aquarius

#define AQUARIUS_MODULE(__method)                                                                                      \
	class __method;                                                                                                    \
	[[maybe_unused]] static aquarius::auto_module_register<__method> __auto_register_##__method(##__method);           \
	class __method : public module_base                                                                                \
	{                                                                                                                  \
	public:                                                                                                            \
		virtual auto handle() -> awaitable<error_code> override;                                                       \
	};                                                                                                                 \
	auto __method::handle() -> awaitable<error_code>\
