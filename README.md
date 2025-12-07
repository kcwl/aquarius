# aquarius
c++ modern frame work with asio

## build status
[![codecov](https://codecov.io/gh/kcwl/aquarius/graph/badge.svg?token=4ACYCIPH8B)](https://codecov.io/gh/kcwl/aquarius)
[![ubuntu(gcc)](https://github.com/kcwl/aquarius/actions/workflows/cmake-ubuntu-gcc.yml/badge.svg)](https://github.com/kcwl/aquarius/actions/workflows/cmake-ubuntu-gcc.yml)
[![visual stadio](https://github.com/kcwl/aquarius/actions/workflows/cmake-windows-vs.yml/badge.svg)](https://github.com/kcwl/aquarius/actions/workflows/cmake-windows-vs.yml)


# How To Install
do not need to install, just include the header file

# How To Compile Tools
use cmake to compile cmakelists.txt where on root directory of project

# How To Use Server?
```
#include <aquarius.hpp>

// tcp server
aquarius::tcp_server srv(8100,10, "test tcp server");

// http server
aquarius::http_server srv(8100,10, "test tcp server");

srv.run();
```

# How To Use Client?
```
#include <aquarius.hpp>

// tcp client
aquarius::tcp_client cli("127.0.0.1", 8100);

// http client
aquarius::http_client cli("127.0.0.1", 8100);

cli.connect();
std::string msg = "hello";
cli.send(msg);
auto res = cli.recv();
```

# How To Generate Protocol?
define a virgo file for protocol and use lazytool to generate, like this:
```
lazytool --input test.virgo --cpp_out .
```

# How To Define Protocol Handler?
```
#include <aquarius.hpp>

AQUAIURS_HANDLER(request, response, ctx_test)

{
    // To do something
}
```

# How To Use SQL?

```
#include <aquarius.hpp>

struct personal
{
	int32_t age;
	bool sex;_
};

//start sql pool
co_await aquarius::sql_pool().run(pool.size(), db_param);

auto res = co_await aquarius::sql_pool().async_execute(
	aquarius::make_execute_task(sql_insert(p)()));

// res will be number `1` that insert effective rows

auto result = co_await aquarius::sql_pool().async_execute(
	aquarius::make_query_task<personal>(sql_select(personal)()));

// result will be a vector of personal

```

# How to Use Lua for Handler?
Coming Soon

# How to Extern Custom Protocal and Handler?
Coming Soon