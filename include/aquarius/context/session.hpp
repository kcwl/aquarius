#pragma once
#include <any>
#include <aquarius/context/impl/session.hpp>
#include <aquarius/core/core.hpp>
#include <deque>
#include <memory>
#include <string>

namespace aquarius
{
	template <typename _Connector>
	class session : public basic_session
	{
		friend class basic_context;

	public:
		explicit session(std::shared_ptr<_Connector> conn_ptr)
			: conn_ptr_(conn_ptr)
			, mutex_()
			, ctxs_()
		{}

		virtual ~session() = default;

	public:
		virtual std::size_t uuid() override
		{
			if (conn_ptr_.expired())
				return false;

			auto ptr = conn_ptr_.lock();

			return ptr->uuid();
		}

		virtual bool async_write(const std::size_t proto, flex_buffer_t&& buffer) override
		{
			if (conn_ptr_.expired())
				return false;

			auto ptr = conn_ptr_.lock();

			auto pack_size = buffer.size();

			if (pack_size > pack_limit)
			{
				return async_write_large(ptr, std::forward<flex_buffer_t>(buffer), proto, pack_size);
			}

			flex_buffer_t stream{};

			elastic::to_binary(pack_flag::normal, stream);

			stream.save(buffer.wdata(), buffer.size());

			ptr->async_write(std::move(stream));

			return true;
		}

		virtual void attach(std::size_t proto, std::shared_ptr<basic_context> context_ptr) override
		{
			std::lock_guard lk(mutex_);

			ctxs_.insert({ proto, context_ptr });
		}

		virtual void detach(std::size_t proto) override
		{
			std::lock_guard lk(mutex_);

			ctxs_.erase(proto);
		}

		virtual void attach_buffer(const std::size_t proto, flex_buffer_t& buffer) override
		{
			std::lock_guard lk(large_mutex_);

			auto& pack = large_packs_[proto];

			const auto buffer_size = buffer.size();

			pack->save(buffer.wdata(), buffer_size);

			buffer.consume(buffer_size);
		}

		virtual flex_buffer_t complete(const std::size_t proto, flex_buffer_t& buffer) override
		{
			attach_buffer(proto, buffer);

			std::lock_guard lk(large_mutex_);

			auto pack = *large_packs_[proto];

			large_packs_.erase(proto);

			return pack;
		}

	private:
		bool async_write_large(std::shared_ptr<_Connector> ptr, flex_buffer_t&& buffer, const std::size_t proto,
							   std::size_t pack_size)
		{
			while (pack_size > 0)
			{
				std::size_t temp_size{};

				pack_size > pack_limit ? temp_size = pack_limit : temp_size = pack_size;

				flex_buffer_t stream{};

				elastic::to_binary(pack_flag::middle, stream);

				elastic::to_binary(proto, stream);

				temp_size -= stream.size();

				stream.save(buffer.wdata(), temp_size);

				buffer.consume(temp_size);

				pack_size -= temp_size;

				ptr->async_write(std::move(stream));
			}

			return true;
		}

	private:
		std::weak_ptr<_Connector> conn_ptr_;

		std::mutex mutex_;

		std::unordered_multimap<std::size_t, std::weak_ptr<basic_context>> ctxs_;

		std::mutex large_mutex_;

		std::unordered_map<std::size_t, std::shared_ptr<flex_buffer_t>> large_packs_;
	};
} // namespace aquarius