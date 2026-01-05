#pragma once
#include <aquarius/module/module.hpp>
#include <string_view>

namespace aquarius
{
	namespace gateway
	{
        class auth_op
        {
        public:
            auth_op() = default;
            ~auth_op() = default;

        public:
            bool auth(const std::string_view& username, const std::string_view& password);
        };

        class auth_module : public _module<auth_op>
        {
        public:
            auth_module() = default;

        private:
            std::shared_ptr<auth_op> op_ptr_;
        };
	}
}