#pragma once
#include <aquarius/detail/flex_buffer.hpp>
#include <aquarius/error_code.hpp>
#include <aquarius/singleton.hpp>
#include <expected>
#include <shared_mutex>
#include <vector>

namespace aquarius
{
	template <typename Func>
	class handler_channel_impl
	{
		struct node
		{
			char key;

			Func func;

			int32_t next;

			int32_t end;

			std::vector<std::shared_ptr<node>> children;
		};

	public:
		using function_type = Func;

		using key_t = std::string;

	public:
		handler_channel_impl()
			: root_ptr_(new node())
		{}

		virtual ~handler_channel_impl() = default;

	public:
		void add(key_t key, const function_type& func)
		{
			auto cur_node = root_ptr_;

			for (auto c : key)
			{
				auto it = std::find_if(cur_node->children.begin(), cur_node->children.end(),
									   [&](auto node) { return node->key == c; });

				if (it == cur_node->children.end())
				{
					auto n = std::make_shared<node>(c);
					cur_node->children.push_back(n);
					cur_node = n;
				}
				else
				{
					cur_node = *it;
				}

				cur_node->next++;
			}

			cur_node->func = func;
			cur_node->end++;
		}

		function_type find(key_t key)
		{
			auto cur_node = root_ptr_;

			auto iter = key.begin();

			while (iter != key.end())
			{
				auto it = std::find_if(cur_node->children.begin(), cur_node->children.end(),
									   [&](auto node) { return node->key == *iter; });

				if (it == cur_node->children.end())
				{
					return nullptr;
				}
				else
				{
					cur_node = *it;
				}

				iter++;
			}

			return cur_node->func;
		}

	protected:
		std::shared_ptr<node> root_ptr_;
	};

	class handler_channel : public singleton<handler_channel>
	{
	public:
		using subscribe_func_t = std::function<awaitable<std::expected<flex_buffer, error_code>>(flex_buffer&, int)>;

	public:
		auto publish(const std::string& topic, flex_buffer& buffer, int version)
			-> awaitable<std::expected<flex_buffer, error_code>>
		{
			std::shared_lock lk(mutex_);

			auto subscribe = impl_.find(topic);

			if (!subscribe)
			{
				co_return std::unexpected(error_code{});
			}

			co_return co_await subscribe(buffer, version);
		}

		void subscribe(const std::string& topic, const subscribe_func_t& func)
		{
			std::unique_lock lk(mutex_);

			auto subscribe = impl_.find(topic);
			if (subscribe)
			{
				return;
			}

			impl_.add(topic, func);
		}

	private:
		std::shared_mutex mutex_;

		handler_channel_impl<subscribe_func_t> impl_;
	};

	inline auto mpc_publish(const std::string& topic, flex_buffer& buffer, int version = 0)
		-> awaitable<std::expected<flex_buffer, error_code>>
	{
		co_return co_await handler_channel::get_mutable_instance().publish(topic, buffer, version);
	}

	inline void mpc_subscribe(const std::string& topic, const handler_channel::subscribe_func_t& func)
	{
		return handler_channel::get_mutable_instance().subscribe(topic, func);
	}
} // namespace aquarius