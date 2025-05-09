#pragma once
#include <aquarius/core/uuid.hpp>
#include <aquarius/detail/auto_register.hpp>
#include <aquarius/detail/flex_buffer.hpp>
#include <aquarius/detail/package_base.hpp>
#include <aquarius/detail/session_base.hpp>
#include <aquarius/detail/session_object_impl.hpp>

namespace aquarius
{
	template <typename IO>
	class basic_session : public std::enable_shared_from_this<basic_session<IO>>,
						  public package_base,
						  public session_base
	{
		using executor_type = typename IO::executor_type;

		using socket_type = typename IO::socket_type;

		using ssl_socket_type = typename IO::ssl_socket_type;

		using this_type = basic_session<IO>;

		constexpr static std::size_t pack_limit = 4096;

	public:
		explicit basic_session(socket_type socket)
			: impl_(std::move(socket))
			, read_buffer_()
			, uuid_(invoke_uuid<uint32_t>())
		{}

		template <execution_context_convertible ExecutionContext>
		explicit basic_session(ExecutionContext& context)
			: impl_(context)
			, read_buffer_(pack_limit)
			, uuid_(invoke_uuid<uint32_t>())
		{}

		virtual ~basic_session() = default;

	public:
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

		auto start() -> boost::asio::awaitable<void>
		{
			// invoke_session_helper::push(this->shared_from_this());

			co_await impl_.get_service().start(impl_.get_implementation());

			co_await read_messages();
		}

		virtual void send_packet(flex_buffer_t&&)
		{
			return;
		}

		void write(std::size_t proto, flex_buffer_t& buffer)
		{
			auto buffer_size = buffer.size();

			buffer_size > 4096 - 1 ? write_package(proto, buffer) : write_normal(proto, buffer);
		}

		void read(flex_buffer_t& buffer)
		{
			auto flag = from_binary<uint8_t>(buffer);

			auto req_id = from_binary<uint32_t>(buffer);

			auto seq = flag >> 2;

			switch (static_cast<mvcc>(flag & 0x4))
			{
			case mvcc::header:
				{
					read_header(req_id, seq, buffer);
				}
				break;
			case mvcc::middle:
				{
					read_package(req_id, seq, buffer);
				}
				break;
			case mvcc::complete:
				{
					read_complete(req_id, seq, buffer);
				}
				break;
			case mvcc::normal:
				{
					read_normal(req_id, buffer);
				}
				break;
			default:
				break;
			}
		}

		void shutdown()
		{
			return impl_.get_service().shutdown();
		}

		std::size_t uuid() const
		{
			return uuid_;
		}

	private:
		auto read_messages() -> boost::asio::awaitable<void>
		{
			auto self(this->shared_from_this());

			boost::system::error_code ec;

			std::size_t bytes_transferred = co_await impl_.get_service().async_read_some(
				impl_.get_implementation(), boost::asio::buffer(read_buffer_.rdata(), read_buffer_.active()), ec);

			if (ec)
			{
				if (ec != asio::error::eof)
				{
					XLOG_ERROR() << "on read some occur error - " << ec.message();
				}

				co_return impl_.get_service().shutdown();
			}

			read(read_buffer_);

			read_buffer_.commit(static_cast<int>(bytes_transferred));

			co_await read_messages();
		}

		void write_package(std::size_t proto, flex_buffer_t& buffer)
		{
			auto buffer_size = buffer.size();

			uint8_t total = static_cast<uint8_t>(buffer_size / (4096 - 1));

			uint8_t flag = (total << 2) | static_cast<uint8_t>(mvcc::header);

			int counter = 0;

			while (buffer_size != 0)
			{
				auto cur_size = std::min<std::size_t>(buffer_size, 4095);

				if (cur_size == buffer_size)
				{
					flag = (counter << 2) | static_cast<uint8_t>(mvcc::complete);
				}
				else
				{
					flag = (counter << 2) | static_cast<uint8_t>(mvcc::middle);
				}

				boost::asio::streambuf write_buffer{};
				write_buffer.sputn((char*)&flag, 1);
				write_buffer.sputn((char*)buffer.wdata() + counter * 4095, cur_size);
				buffer_size -= cur_size;
			}
		}

		void write_normal(std::size_t proto, flex_buffer_t& buffer)
		{
			uint8_t flag = static_cast<uint8_t>(mvcc::normal);
			boost::asio::streambuf write_buffer{};
			write_buffer.sputn((char*)&flag, 1);
			write_buffer.sputn((char*)&proto, sizeof(uint32_t));
			write_buffer.sputn((char*)buffer.wdata(), buffer.size());

			// send
			boost::asio::co_spawn(impl_.get_executor(), impl_.get_service().async_write_some(buffer),
								  boost::asio::detached);
		}

		template <typename Iterator>
		void partical_insert_sort(std::list<sequence::block>& buffer, Iterator begin, Iterator end, std::size_t pred)
		{
			for (auto iter = begin; iter != end;)
			{
				if (iter->seq != pred)
					continue;

				buffer.insert(begin, std::move(*iter));
				buffer.erase(iter);
			}
		}

		void read_header(std::size_t proto, std::size_t total, flex_buffer_t& buffer)
		{
			auto& pack = buffers_[proto];
			pack.total = total;
			pack.buffers.push_back(sequence::block{ 0, std::move(buffer) });
		}

		bool read_package(std::size_t proto, std::size_t seq, flex_buffer_t& buffer)
		{
			auto& pack = buffers_[proto];

			pack.buffers.push_back({ seq, buffer });

			if (pack.has_complete) [[unlikely]]
			{
				dispatch(proto, pack);
			}

			return true;
		}

		void read_complete(std::size_t proto, std::size_t seq, flex_buffer_t& buffer)
		{
			auto& pack = buffers_[proto];
			pack.buffers.push_back({ seq, buffer });

			!dispatch(proto, pack) ? pack.has_complete = true : 0;
		}

		void read_normal(std::size_t proto, flex_buffer_t& buffer)
		{
			boost::asio::post(impl_.get_executor(),
							  [&, buf = std::move(buffer), this]
							  {
								  sequence pack{};
								  pack.buffers.push_back({ 1, buffer });

								  dispatch(proto, pack);
							  });
		}

		bool dispatch(std::size_t proto, sequence& pack)
		{
			if (!pack.check_complete())
				return false;

			int counter = 0;

			flex_buffer_t complete_buffer;

			for (auto iter = pack.buffers.begin(); iter != pack.buffers.end();)
			{
				if (iter->seq == counter++)
				{
					complete_buffer.save(iter->buffer.rdata(), iter->buffer.size());

					iter++;
				}
				else
				{
					partical_insert_sort(pack.buffers, iter, pack.buffers.end(), counter);
				}
			}

			auto self = this->shared_from_this();

			invoke_context(proto, complete_buffer, self);

			buffers_.erase(proto);

			return true;
		}

	private:
		session_object_impl<IO, executor_type, IO::ssl_version> impl_;

		flex_buffer_t read_buffer_;

		uint32_t uuid_;
	};
} // namespace aquarius