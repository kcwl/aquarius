#pragma once
#include <aquarius/detail/visitor.hpp>
#include <aquarius/core/logger.hpp>

namespace aquarius
{
    class context_base : public visitor<int>
    {
    public:
        explicit context_base(const std::string& name, const std::chrono::milliseconds& timeout)
            : ctx_name_(name)
            , timeout_(timeout)
        {
        }

    public:
        virtual int visit(int*, std::shared_ptr<session_base>) override
        {
             XLOG_WARNING() << ctx_name_ << " maybe visit an unknown message!";

            return 0;
        }

    protected:
        std::string ctx_name_;

        std::chrono::milliseconds timeout_;

        std::shared_ptr<session_base> session_ptr_;
    };
} // namespace aquarius