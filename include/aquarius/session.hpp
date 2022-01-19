#pragma once
#include <memory>
#include "router.hpp"
#include "tcp/message.hpp"
#include "detail/stream.hpp"

namespace aquarius
{
	class session : public std::enable_shared_from_this<session>
	{
	public:
		session() = default;

		virtual ~session() = default;

		void run(streambuf& buf)
		{
			if (buf.size() == 0)
				return;

			return parse_package(buf);
		}

		template<typename Func>
		void attach(Func&& send_f)
		{
			send_f_ = send_f;
		}

		template<typename T>
		void async_send(T t)
		{
			buf_ << t;

			return send_f_(std::move(buf_));
		}

		void on_close()
		{
			return;
		}

		void on_connect()
		{
			return;
		}

	private:
		void parse_package(streambuf& buf)
		{
			uint32_t proto_id = get_id(buf);

			if (proto_id == static_cast<uint32_t>(-1))
				return;

			auto ctx_ptr = invoke_helper<true, std::shared_ptr<context>, std::shared_ptr<session>>::invoke("ctx_" + std::to_string(proto_id), shared_from_this());

			invoke_helper<false, void, std::shared_ptr<context>, streambuf&>::invoke("msg_" + std::to_string(proto_id), std::move(ctx_ptr), std::ref(buf));

			return boost::asio::post(std::bind(&session::parse_package, shared_from_this(), std::ref(buf)));
		}

		uint32_t get_id(streambuf& stream)
		{
			uint32_t proto_id{};

			uint32_t length{};

			std::size_t stream_length = stream.size();

			stream >> proto_id >> length;

			stream.reset(sizeof(proto_id) + sizeof(length));

			if (length != stream_length)
			{
				return static_cast<uint32_t>(-1);
			}

			return proto_id;
		}

	private:
		std::function<void(streambuf&&)> send_f_;

		streambuf buf_;
	};
}