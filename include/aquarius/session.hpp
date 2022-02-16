#pragma once
#include <memory>
#include "router.hpp"
#include "tcp/message.hpp"
#include "stream.hpp"

namespace aquarius
{
	class session : public std::enable_shared_from_this<session>
	{
	public:
		session() = default;

		virtual ~session() = default;

		void run(ftstream& buf)
		{
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
		void parse_package(ftstream& buf)
		{
			auto hv = parse_header<tcp::header_fields::value_t>(buf);

			if (hv == decltype(hv)())
				return;

			auto str_proto_id = std::to_string(hv.proto_);

			auto ctx_ptr = invoke_helper<true, std::shared_ptr<context>, std::shared_ptr<session>>::invoke("ctx_" + str_proto_id, shared_from_this());

			invoke_helper<false, void, std::shared_ptr<context>, tcp::header_value, ftstream&>::invoke("msg_" + str_proto_id, std::move(ctx_ptr), std::move(hv), std::ref(buf));
		}

		template<typename T>
		T parse_header(ftstream& buf)
		{
			if (buf.size() < sizeof(T))
				return T();

			T value;

			buf >> value;

			return value;
		}

	private:
		std::function<void(ftstream&&)> send_f_;

		ftstream buf_;
	};
}