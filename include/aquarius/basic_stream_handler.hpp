#pragma once
#include <aquarius/basic_handler.hpp>

namespace aquarius
{

    template<typename T, typename R, typename E>
    class basic_stream_handler : public basic_handler<T, E>
    {
        using base_type = basic_handler<T, E>;

    public:
        basic_stream_handler(const std::string& name)
            : base_type(name)
        {

        }

    public:
        auto visit(T message) -> boost::asio::awaitable<R>
        {
            auto result = co_await base_type::visit(message);

            make_response(result);

            co_return response_;
        }

        R& response()
        {
            return response_;
        }

    private:
        void make_response(E result)
        {
            response_.header()->set_result(static_cast<int64_t>(result));
        }

    protected:
        R response_;
    };

}