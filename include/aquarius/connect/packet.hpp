#pragma once
#include <aquarius/connect/mode.hpp>
#include <aquarius/context.hpp>
#include <aquarius/context/generator.hpp>
#include <aquarius/context/invoke.hpp>
#include <aquarius/core/elastic.hpp>
#include <aquarius/core/error_code.hpp>
#include <aquarius/core/logger.hpp>
#include <aquarius/message/generator.hpp>
#include <aquarius/message/message.hpp>

namespace aquarius
{
	template <typename _Connector>
	class packet
	{
		using conn_t = _Connector;

	public:
		explicit packet(conn_t* conn_ptr)
			: connect_ptr_(conn_ptr)
		{}

	public:
		error_code process(flex_buffer_t& buffer)
		{
			aquarius::error_code ec{};

			auto pos = buffer.pubseekoff(0, std::ios::cur, std::ios::out);

			pack_flag flag{};

			if (!elastic::from_binary(flag, buffer))
			{
				XLOG_WARNING() << "parse proto error, maybe message is not haved flag.";

				return ec = errc::incomplete;
			}

			uint32_t proto{};
			if (!elastic::from_binary(proto, buffer))
			{
				XLOG_WARNING() << "parse proto error, maybe message is not complete.";

				return ec = errc::unknown;
			}

			switch (flag)
			{
			case pack_flag::normal:
				{
					ec = normalize(buffer, proto);
				}
				break;
			case pack_flag::middle:
				{
					attach(proto, buffer);
				}
				break;
			case pack_flag::end:
				{
					auto buf = complete(proto, buffer);

					ec = normalize(buf, proto);
				}
				break;
			default:
				break;
			}

			if (ec.value() != static_cast<int>(errc::pending) &&
				ec.value() != static_cast<int>(errc::ok))
				buffer.pubseekpos(pos, std::ios::out);

			return ec;
		}

		bool async_write(flex_buffer_t&& buffer)
		{
			auto pack_size = buffer.size();

			if (pack_size > pack_limit)
			{
				return async_write_large(std::forward<flex_buffer_t>(buffer), pack_size);
			}

			flex_buffer_t stream{};

			elastic::to_binary(pack_flag::normal, stream);

			stream.save(buffer.wdata(), buffer.size());

			connect_ptr_->async_write(std::move(stream));

			return true;
		}

	private:
		error_code normalize(flex_buffer_t& buffer, const std::size_t proto)
		{
			auto request_ptr = invoke_message_helper::invoke(proto);

			if (!request_ptr)
			{
				request_ptr = std::make_shared<basic_message>();
			}

			auto context_ptr = invoke_context_helper::invoke(proto);

			if (!context_ptr)
				context_ptr = std::make_shared<basic_context>();

			attach(request_ptr->uuid(), context_ptr);

			context_ptr->on_accept();

			auto result = request_ptr->accept(buffer, context_ptr, connect_ptr_);

			if (result.value() == static_cast<int>(errc::ok))
				detach(request_ptr->uuid());

			return buffer.size() != 0 ? process(buffer) : result;
		}

		void attach(std::size_t uid, std::shared_ptr<basic_context> context_ptr)
		{
			ctxs_.insert({ uid, context_ptr });
		}

		void detach(std::size_t uid)
		{
			ctxs_.erase(uid);
		}

		void attach(const std::size_t uid, flex_buffer_t& buffer)
		{
			auto& pack = large_packs_[uid];

			if (!pack)
				pack = std::make_shared<flex_buffer_t>();

			const auto buffer_size = buffer.size();

			pack->save(buffer.wdata(), buffer_size);

			buffer.consume(buffer_size);
		}

		flex_buffer_t complete(const std::size_t uid, flex_buffer_t& buffer)
		{
			attach(uid, buffer);

			auto pack = *large_packs_[uid];

			large_packs_.erase(uid);

			return pack;
		}

		bool async_write_large(flex_buffer_t&& buffer, std::size_t pack_size)
		{
			uint32_t proto{};

			elastic::from_binary(proto, buffer);

			while (pack_size > 0)
			{
				flex_buffer_t stream{};

				std::size_t temp_size{};

				pack_flag flag = pack_flag::middle;

				if (pack_size > pack_limit)
				{
					temp_size = pack_limit;
				}
				else
				{
					temp_size = pack_size;

					flag = pack_flag::end;
				}

				elastic::to_binary(flag, stream);

				elastic::to_binary(proto, stream);

				if (flag != pack_flag::end)
				{
					temp_size -= stream.size();
				}

				stream.save(buffer.wdata(), temp_size);

				buffer.consume(temp_size);

				pack_size -= temp_size;

				connect_ptr_->async_write(std::move(stream));
			}

			return true;
		}

	private:
		std::unordered_map<std::size_t, std::shared_ptr<flex_buffer_t>> large_packs_;

		std::unordered_map<std::size_t, std::shared_ptr<basic_context>> ctxs_;

		conn_t* connect_ptr_;
	};
} // namespace aquarius