#pragma once
#include <aquarius/awaitable.hpp>
#include <aquarius/concepts/concepts.hpp>
#include <aquarius/detached.hpp>
#include <aquarius/detail/config.hpp>
#include <aquarius/detail/session_object_impl.hpp>
#include <aquarius/detail/uuid_generator.hpp>
#include <aquarius/error_code.hpp>
#include <aquarius/flex_buffer.hpp>
#include <span>

#ifdef AQUARIUS_ENABLE_SSL
#include <aquarius/detail/basic_ssl_session_service.hpp>
#else
#include <aquarius/detail/basic_session_service.hpp>
#endif

namespace aquarius
{

	template <bool Server, template <bool> typename Protocol>
	class basic_session : public std::enable_shared_from_this<basic_session<Server, Protocol>>
	{
		friend class Protocol<Server>;

	public:
		using protocol = Protocol<Server>;

		constexpr static auto is_server = Server;

		using socket = typename protocol::socket;

		using endpoint = typename protocol::endpoint;

		using acceptor = typename protocol::acceptor;

		using resolver = typename protocol::resolver;

		using header = typename protocol::header;

#ifdef AQUARIUS_ENABLE_SSL
		using impl_type = detail::session_object_impl<detail::basic_ssl_session_service<Server, protocol>,
													  typename socket::executor_type>;
#else
		using impl_type =
			detail::session_object_impl<detail::basic_session_service<protocol>, typename socket::executor_type>;
#endif

	public:
		explicit basic_session(socket sock)
			: impl_(std::move(sock))
			, uuid_(detail::uuid_generator()())
		{}

		virtual ~basic_session() = default;

	public:
		auto get_executor()
		{
			return impl_.get_executor();
		}

		std::size_t uuid() const
		{
			return uuid_;
		}

		std::string remote_address() const
		{
			return impl_.get_service().remote_address(impl_.get_implementation());
		}

		uint32_t remote_address_u() const
		{
			return impl_.get_service().remote_address_u(impl_.get_implementation());
		}

		uint16_t remote_port() const
		{
			return impl_.get_service().remote_port(impl_.get_implementation());
		}

		template <typename BufferSequence>
		auto async_read(BufferSequence& buff) -> awaitable<error_code>
		{
			error_code ec;

			co_await service().async_read(implementation(), buff, ec);

			co_return ec;
		}

		template <typename BufferSequence>
		auto async_send(BufferSequence buffer) -> awaitable<void>
		{
			error_code ec{};
			co_await service().async_write_some(implementation(), std::move(buffer), ec);
		}

		void shutdown()
		{
			service().shutdown();
		}

		void close()
		{
			service().close(implementation());
		}

		auto accept() -> awaitable<error_code>
		{
			return proto_.accept(this->shared_from_this());
		}

	protected:
		auto& service()
		{
			return impl_.get_service();
		}

		auto& implementation()
		{
			return impl_.get_implementation();
		}

	private:
		impl_type impl_;

		std::size_t uuid_;

		Protocol<Server> proto_;
	};
} // namespace aquarius