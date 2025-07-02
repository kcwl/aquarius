#pragma once
#include <aquarius/context/detail/context_base.hpp>
#include <aquarius/context/handler_router.hpp>

namespace aquarius
{
	namespace context
	{
		template <typename Protocol>
		class basic_context : public detail::context_base
		{
		public:
			using buffer_type = std::vector<char>;

			using session = typename Protocol::session;

		public:
			template <typename... Args>
			basic_context(const std::string& name, std::chrono::steady_clock::duration timeout, std::size_t id,
						  std::shared_ptr<session> session, Args&&... args)
				: detail::context_base(name, timeout)
			{
				handler_router<Protocol>::get_mutable_instance().invoke(id, session, std::forward_like<Args>(args)...);
			}

		public:
			virtual void visit(buffer_type buff, std::shared_ptr<session> session)
			{
				return;
			}
		};

		template <typename Protocol>
		class stream_context : public basic_context<Protocol>
		{
		public:
			using base_type = basic_context<Protocol>;

			using typename base_type::session;

		public:
			template <typename... Args>
			stream_context(std::size_t id, std::shared_ptr<session> session, Args&&... args)
				: base_type("stream context", 10s, id, session, std::forward_like<Args>(args)...)
			{}
		};

		template<typename Protocol>
		struct stream
		{
			template <typename... Args>
			constexpr stream_context<Protocol> operator()(std::size_t id,
														  std::shared_ptr<typename Protocol::session> session,
														  Args&&... args)
			{
				return stream_context<Protocol>(id, session, std::forward_like<Args>(args)...);
			}
		};

		template <typename Protocol>
		class transfer_context : public basic_context<Protocol>
		{
		public:
			using base_type = basic_context<Protocol>;

			using typename base_type::session;

		public:
			template <typename... Args>
			transfer_context(std::size_t id, std::shared_ptr<session> session, Args&&... args)
				: base_type("transfer context", 10s, id, session, std::forward_like<Args>(args)...)
			{}
		};

		template<typename Protocol>
		struct transfer
		{
			constexpr static auto __transfer_proto = 1;

			template <typename... Args>
			constexpr transfer_context<Protocol> operator()(std::shared_ptr<typename Protocol::session> session,
															Args&&... args)
			{
				return transfer_context<Protocol>(__transfer_proto, session, std::forward_like<Args>(args)...);
			}
		};

		template<typename Protocol>
		inline constexpr auto transfer = _transfer<Protocol>();

		constexpr static auto __transfer_proto = 1;

		struct rpc_transfer
		{
			constexpr static auto id = __transfer_proto;
			using request = std::vector<char>;
			using response = std::vector<char>;
		};
	} // namespace context
} // namespace aquarius