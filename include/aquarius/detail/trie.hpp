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

			std::size_t next;

			std::size_t end;

			std::vector<std::shared_ptr<tri_node>> children;
		};

		template<typename Func>
		class trie
		{
			using node_func = Func;

			using node = tri_node<char, node_func>;

		public:
			trie()
				: root(new node('/',nullptr))
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
				auto cur_node = root;

				for (auto c : word)
				{
					auto it = std::find_if(cur_node->children.begin(), cur_node->children.end(),
										   [&](auto node) { return node->key == c; });

					if (it == cur_node->children.end())
					{
						return;
					}
					else
					{
						cur_node = *it;
					}
				}

				if (--cur_node->end == 0 && --cur_node->next == 0)
				{
					std::shared_ptr<node>().swap(cur_node);
				}
				else
				{
					cur_node->func = nullptr;
				}
			}

		private:
			std::shared_ptr<node> root;
		};
	}
}