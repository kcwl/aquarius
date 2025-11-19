#pragma once
#include <memory>
#include <string_view>
#include <vector>
#include <aquarius/singleton.hpp>


namespace aquarius
{
    template <typename Key, typename Func>
    requires (std::convertible_to<Key, std::string_view>)
    class basic_router : public singleton<basic_router<Key, Func>>
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

    public:
        basic_router()
            : root_ptr_(new node())
        {
        }

        virtual ~basic_router() = default;

    public:
        template <typename... Args>
        bool invoke(Key key, Args&&... args)
        {
            auto func = find(key);

            return func == nullptr ? false : func(std::forward<Args>(args)...);
        }

        void regist(Key key, function_type&& func)
        {
            auto f = find(key);

            if (f)
                return;

            add(key, std::forward<function_type>(func));
        }

    private:
        void add(Key key, function_type&& func)
        {
            auto cur_node = root_ptr_;

            for (auto c : key)
            {
                auto it = std::find_if(cur_node->children.begin(), cur_node->children.end(),
                                       [&] (auto node) { return node->key == c; });

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

        function_type find(Key key)
        {
            auto cur_node = root_ptr_;

            auto iter = key.begin();

            while (iter != key.end())
            {
                auto it = std::find_if(cur_node->children.begin(), cur_node->children.end(),
                                       [&] (auto node) { return node->key == *iter; });

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

        void remove(std::string_view word)
        {
            auto slow_ptr = root_ptr_;
            auto fast_ptr = root_ptr_;

            for (auto c : word)
            {
                auto it = std::find_if(fast_ptr->children.begin(), fast_ptr->children.end(),
                                       [&] (auto node) { return node->key == c; });

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
                                                    [=] (auto node) { return node->key == fast_ptr->key; }),
                                     slow_ptr->children.end());

            std::shared_ptr<node>().swap(fast_ptr);
        }

    protected:
        std::shared_ptr<node> root_ptr_;
    };
} // namespace aquarius