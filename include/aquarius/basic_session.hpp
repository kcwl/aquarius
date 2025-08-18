#pragma once
#include <aquarius/any_io_executor.hpp>
#include <aquarius/awaitable.hpp>
#include <aquarius/concepts/concepts.hpp>
#include <aquarius/detached.hpp>
#include <aquarius/detail/config.hpp>
#include <aquarius/detail/session_object_impl.hpp>
#include <aquarius/detail/session_service.hpp>
#include <aquarius/error_code.hpp>
#include <aquarius/flex_buffer.hpp>
#include <span>

namespace aquarius
{
	template <bool Server, template <bool> typename Protocol>
	class basic_session : public std::enable_shared_from_this<basic_session<Server, Protocol>>
	{
		friend class Protocol<Server>;

	public:
		constexpr static auto is_server = Server;

		using socket = typename detail::session_service::socket;

		using impl_type = detail::session_object_impl<detail::session_service>;

		using endpoint = typename detail::session_service::endpoint;

		using acceptor = typename detail::session_service::acceptor;

		using resolver = typename detail::session_service::resolver;

		using header = typename Protocol<is_server>::header;

	public:
		explicit basic_session(socket sock)
			: impl_(std::move(sock))
		{}

		virtual ~basic_session() = default;

	public:
		auto get_executor()
		{
			return impl_.get_executor();
		}

		std::size_t id() const
		{
			return id_;
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

			if (ec)
			{
				XLOG_ERROR() << "async write is failed! maybe " << ec.message();
			}
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

	public:
		static endpoint make_v4_endpoint(uint16_t port)
		{
			return endpoint(typename detail::session_service::protocol::v4(), port);
		}

		static endpoint make_v6_endpoint(uint16_t port)
		{
			return endpoint(typename detail::session_service::protocol::v6(), port);
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

		std::size_t id_;

		Protocol<Server> proto_;
	};
} // namespace aquarius