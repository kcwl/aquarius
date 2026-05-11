#pragma once
#include <aquarius/basic_context.hpp>
#include <aquarius/detail/flex_buffer.hpp>
#include <aquarius/error_code.hpp>
#include <aquarius/singleton.hpp>
#include <expected>
#include <shared_mutex>
#include <vector>

namespace aquarius
{
	template <typename Key, typename Func>
	class node
	{
		struct node_impl
		{
			char key;

			Func func;

			int32_t next;

			int32_t end;

			std::vector<std::shared_ptr<node_impl>> children;
		};

	public:
		using function_type = Func;

		using key_t = Key;

	public:
		node()
			: root_ptr_(std::make_shared<node_impl>())
		{}

		virtual ~node() = default;

	public:
		bool add(key_t key, const function_type& func)
		{
			auto cur_node = root_ptr_;

			for (auto c : key)
			{
				auto it = std::find_if(cur_node->children.begin(), cur_node->children.end(),
									   [&](auto node) { return node->key == c; });

				if (it == cur_node->children.end())
				{
					auto n = std::make_shared<node_impl>(c);
					cur_node->children.push_back(n);
					cur_node = n;
				}
				else
				{
					return false;
				}

				cur_node->next++;
			}

			cur_node->func = func;
			cur_node->end++;

			return true;
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
		std::shared_ptr<node_impl> root_ptr_;
	};

	class context_reg : public singleton<context_reg>
	{
	public:
		using node_t = std::shared_ptr<context_base>;

	public:
		std::shared_ptr<context_base> get(const std::string& topic)
		{
			std::shared_lock lk(mutex_);

			return impl_.find(topic);
		}

		bool put(const std::string& topic, const node_t& n)
		{
			std::unique_lock lk(mutex_);

			return impl_.add(topic, n);
		}

	private:
		std::shared_mutex mutex_;

		node<std::string, node_t> impl_;
	};

	inline std::shared_ptr<context_base> mpc_get_context(const std::string& topic)
	{
		return context_reg::get_mutable_instance().get(topic);
	}

	inline bool mpc_put_context(const std::string& topic, const context_reg::node_t& n)
	{
		return context_reg::get_mutable_instance().put(topic, n);
	}
} // namespace aquarius