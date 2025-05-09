#pragma once
#include <aquarius/core/logger.hpp>

namespace aquarius
{
    class session_base;

    class context_base
    {
    public:
        explicit context_base(const std::string& name, const std::chrono::milliseconds& timeout)
            : ctx_name_(name)
            , timeout_(timeout)
        {
        }

    protected:
        std::string ctx_name_;

        std::chrono::milliseconds timeout_;

        std::shared_ptr<session_base> session_ptr_;
    };
} // namespace aquarius