#pragma once
#include <cstddef>
#include <future>
#include "common.hpp"
#include "visitor.hpp"

namespace aquarius
{
	class router;

	class schedule
		: public std::enable_shared_from_this<schedule>
		
	{
	public:
		schedule() = default;
		virtual ~schedule() = default;

	public:
		template<typename Func>
		void parse_package(streambuf& buf, Func&& f)
		{
			protocol_type proto_id = make_proto_id(buf);

			if (proto_id == static_cast<protocol_type>(-1))
				return;

			//触发ctx
			auto ctx_ptr = router::instance().route_func("ctx_" + std::to_string(proto_id));

			//处理message
			router::instance().route_invoke("msg_" + std::to_string(proto_id), shared_from_this(), buf, ctx_ptr,std::forward<Func>(f));

			return buf.clear();
		}

		template<typename Request, typename Context, typename Func>
		int accept(std::shared_ptr<Request> msg_ptr, std::shared_ptr<Context> ctx_ptr, Func&& f)
		{
			using request_type = visitor<Request, Func, int>;

			auto request_ptr = std::dynamic_pointer_cast<request_type>(ctx_ptr);

			if (request_ptr == nullptr)
				return ctx_ptr->visit(msg_ptr, std::forward<Func>(f));

			return request_ptr->visit(msg_ptr, std::forward<Func>(f));
		}

	private:
		protocol_type make_proto_id(streambuf& stream)
		{
			protocol_type proto_id{};

			uint32_t length{};

			std::size_t stream_length = stream.size();

			stream >> proto_id >> length;

			if (length > stream_length)
				return static_cast<protocol_type>(-1);

			return proto_id;
		}

		
	};
}