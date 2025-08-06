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

			std::vector<tri_node*> children;
		};

		template<typename Func>
		class trie
		{
			using node_func = Func;

			using node = tri_node<char, node_func>;

		public:
			trie()
				: root(new node())
			{}

			~trie()
			{
				if (root)
				{
					delete root;
					root = nullptr;
				}
			}

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
						cur_node->children.push_back(new node{ c, nullptr });
					}
					else
					{
						cur_node = *it;
					}

					cur_node->next++;
				}

				cur_node->func = func;
				cur_node->next--;
				cur_node->end++;
			}

			node_func find(std::string_view ley)
			{
				auto cur_node = root;

				for (auto c : ley)
				{
					auto it = std::find_if(cur_node->children.begin(), cur_node->children.end(),
										   [&](auto node) { return node->key == c; });

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

			void remove(const std::string& word)
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
					delete cur_node;
					cur_node = nullptr;
				}
				else
				{
					cur_node->func = nullptr;
				}
			}

		private:
			node* root;
		};
	}
}