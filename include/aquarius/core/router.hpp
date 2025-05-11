#pragma once
#include <mutex>
#include <unordered_map>
#include <boost/noncopyable.hpp>


namespace aquarius
{
    template <typename Key, typename Value>
    class router_base : public boost::noncopyable
    {
    public:
        router_base() = default;

        virtual ~router_base() = default;

    protected:
        std::unordered_map<Key, Value> map_invokes_;
    };

    template <typename Result, typename... Args>
    class basic_router : public router_base<std::size_t, std::function<Result(Args...)>>
    {
    public:
        basic_router() = default;

    public:
        Result invoke(std::size_t key, Args... args)
        {
            auto iter = this->map_invokes_.find(key);

            if (iter == this->map_invokes_.end())
                return Result();

            return iter->second(args...);
        }
    };
} // namespace aquarius
