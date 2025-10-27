#pragma once
#include <vector>
#include <functional>
#include <string>

namespace aquarius
{
	namespace detail
	{
		template <typename T, typename Func>
		struct tri_node
		{
			T key;

			Func func;

			int32_t next;

			int32_t end;

			std::vector<std::shared_ptr<tri_node>> children;
		};

		template <typename Func>
		class trie
		{
			using node_func = Func;

			using node = tri_node<char, node_func>;

		public:
			trie()
				: root(new node('/', nullptr))
			{}

			~trie() = default;

		public:
			void add(std::string_view key, const node_func& func)
			{
				auto cur_node = root;

				for (auto c : key)
				{
					auto it = std::find_if(cur_node->children.begin(), cur_node->children.end(),
										   [&](auto node) { return node->key == c; });

					if (it == cur_node->children.end())
					{
						auto n = std::make_shared<node>(c, nullptr);
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

			node_func find(std::string_view key)
			{
				auto cur_node = root;

				auto iter = key.begin();

				while (iter != key.end())
				{
					auto it = std::find_if(cur_node->children.begin(), cur_node->children.end(),
										   [&](auto node) { return node->key == *iter++; });

					if (it == cur_node->children.end())
					{
						return nullptr;
					}
					else
					{
						cur_node = *it;
					}
				}

				return cur_node->func;
			}

			void remove(std::string_view word)
			{
				auto slow_ptr = root;
				auto fast_ptr = root;

				for (auto c : word)
				{
					auto it = std::find_if(fast_ptr->children.begin(), fast_ptr->children.end(),
										   [&](auto node) { return node->key == c; });

					if (it == fast_ptr->children.end())
					{
						return;
					}
					else
					{
						slow_ptr = fast_ptr;
						fast_ptr = *it;
					}

					if (--fast_ptr->end <= 0 && --fast_ptr->next <= 0)
						break;
				}

				slow_ptr->children.erase(std::remove_if(slow_ptr->children.begin(), slow_ptr->children.end(),
													 [=](auto node) { return node->key == fast_ptr->key; }),
										 slow_ptr->children.end());

				std::shared_ptr<node>().swap(fast_ptr);
			}

		private:
			std::shared_ptr<node> root;
		};
	} // namespace detail
} // namespace aquarius