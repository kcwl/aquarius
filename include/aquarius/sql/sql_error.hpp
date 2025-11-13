#pragma once
#include <errmsg.h>
#include <aquarius/error_code.hpp>
#include <type_traits>

namespace aquarius
{

#define AQUARIUS_DB_DEF(value) value

    enum class db_error
    {
        success = 0,
        host_error = AQUARIUS_DB_DEF(CR_CONN_HOST_ERROR),
        connect = AQUARIUS_DB_DEF(CR_CONNECTION_ERROR),
        ipsock = AQUARIUS_DB_DEF(CR_IPSOCK_ERROR),
        out_of_memory = AQUARIUS_DB_DEF(CR_OUT_OF_MEMORY),
        socket_create = AQUARIUS_DB_DEF(CR_SOCKET_CREATE_ERROR),
        unknown_host = AQUARIUS_DB_DEF(CR_UNKNOWN_HOST),
        version = AQUARIUS_DB_DEF(CR_VERSION_ERROR),
        named_pipe = AQUARIUS_DB_DEF(CR_NAMEDPIPEOPEN_ERROR),
        named_pipe_wait = AQUARIUS_DB_DEF(CR_NAMEDPIPEWAIT_ERROR),
        named_pipe_set_state = AQUARIUS_DB_DEF(CR_NAMEDPIPESETSTATE_ERROR),
        server_lost = AQUARIUS_DB_DEF(CR_SERVER_LOST),
        already_connect = AQUARIUS_DB_DEF(CR_ALREADY_CONNECTED),

        out_of_sync = AQUARIUS_DB_DEF(CR_COMMANDS_OUT_OF_SYNC),
        server_gone = AQUARIUS_DB_DEF(CR_SERVER_GONE_ERROR),
        unknown_error = AQUARIUS_DB_DEF(CR_UNKNOWN_ERROR),

        field_not_empty = 9001,
        field_empty,
        field_number,
        bad_alloc,
    };

    inline std::string_view from_db_error_string(db_error e)
    {
        static std::map<db_error, std::string_view> desc = {
            {db_error::host_error, "Failed to connect to the MySQL server."},
            {db_error::connect, "Failed to connect to the local MySQL server."},
            {db_error::ipsock, "Failed to create an IP socket."},
            {db_error::out_of_memory, "out of memory"},
            {db_error::socket_create, "Failed to create a Unix socket."},
            {db_error::unknown_host, "Failed to find the IP address for the host name."},
            {db_error::version, "A protocol mismatch resulted from attempting to connect to a server with a client library that uses a different protocol version."},
            {db_error::named_pipe, "Failed to create a named pipe on Windows."},
            {db_error::named_pipe_wait, "Failed to wait for a named pipe on Windows."},
            {db_error::named_pipe_set_state, "Failed to get a pipe handler on Windows."},
            {db_error::server_lost, "server lost"},
            {db_error::already_connect, "The MYSQL connection handler is already connected."},
            {db_error::out_of_sync, "Commands were executed in an improper order."},
            {db_error::server_gone, "The MySQL server has gone away."},
            {db_error::unknown_error, "Unknown error"},

            {db_error::success, "success"},
            {db_error::field_not_empty, "field not empty"},
            {db_error::field_empty, "field empty"},
            {db_error::field_number, "field number is not fitted"},
            {db_error::bad_alloc, "bad alloc"},
        };

        return desc[e];
    }

    class db_error_category : public boost::system::error_category
    {
    public:
        const char* name() const noexcept override
        {
            return "db";
        }

        std::string message(int ev) const override
        {
            return  std::string(from_db_error_string(static_cast<db_error>(ev)));
        }
    };

    inline const db_error_category& get_db_error_category()
    {
        static db_error_category category{};

        return category;
    }

    inline error_code make_error_code(db_error ec)
    {
        return error_code(static_cast<int>(ec), get_db_error_category());
    }
}

namespace std
{
    template<>
    struct is_error_code_enum<aquarius::db_error> : std::true_type {};
}
