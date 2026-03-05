#pragma once
#include <aquarius/basic_handler.hpp>
#include <aquarius/module/schedule.hpp>
#include <aquarius/virgo/http_null_protocol.hpp>
#include <vector>

namespace aquarius
{
	template <typename Func>
	class channel_impl
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
		channel_impl()
			: root_ptr_(new node())
		{}

		virtual ~channel_impl() = default;

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

	class channel_module : public _module<channel_module>
	{
		using base = _module<channel_module>;

	public:
		using subscribe_t = basic_handler<virgo::http_null_request>;

		using subscribe_func_t = std::function<std::shared_ptr<subscribe_t>()>;

	public:
		channel_module(io_context& io, const std::string& name)
			: base(io, name)
		{}

	public:
		auto publish(std::string_view topic)
		{
			std::shared_lock lk(mutex_);

			auto subscribe = impl_.find(std::string(topic));

			return !subscribe ? std::make_shared<subscribe_t>("null handler") : subscribe();
		}

		void subscribe(std::string_view topic, const subscribe_func_t& func)
		{
			std::unique_lock lk(mutex_);

			auto subscribe = impl_.find(std::string(topic));
			if (subscribe)
			{
				return;
			}

			impl_.add(std::string(topic), func);
		}

	private:
		std::shared_mutex mutex_;

		channel_impl<subscribe_func_t> impl_;
	};

	inline auto mpc_publish(std::string_view topic) -> awaitable<std::shared_ptr<channel_module::subscribe_t>>
	{
		co_return co_await mpc::call<std::shared_ptr<channel_module::subscribe_t>, channel_module>(
			[&](channel_module* ptr) -> awaitable<std::shared_ptr<typename channel_module::subscribe_t>>
			{ co_return ptr->publish(topic); });
	}

	inline void mpc_subscribe(std::string_view topic, const channel_module::subscribe_func_t& func)
	{
		return mpc::call_sync<void, channel_module>([&](channel_module* ptr) { return ptr->subscribe(topic, func); });
	}

} // namespace aquarius