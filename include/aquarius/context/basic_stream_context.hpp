#pragma once
#include <aquarius/context/detail/handler_router.hpp>
#include <aquarius/flex_buffer.hpp>

namespace aquarius
{
	namespace context
	{
        template<typename Session>
        class basic_stream_context : public basic_context<Session>
        {
            using base_type = basic_context<Session>;

        public:
            basic_stream_context(std::chrono::steady_clock::duration timeout = 30ms)
                : base_type("stream context", timeout)
            {
            }

        public:
            virtual void visit(std::vector<char>& buff, std::shared_ptr<Session> session) override
            {
                post(session->get_executor(), [session, buff = std::move(buff), this] mutable
                    {
                        this->invoke(session, [session, buff = std::move(buff)]
                            {
                                uint32_t proto{};
                                buff.load((uint8_t*)&proto, sizeof(proto));

                                detail::handler_router<Session>::get_mutable_instance().invoke(proto, std::move(buff), session);
                            });

                    });
            }
        };

	} // namespace context
} // namespace aquarius