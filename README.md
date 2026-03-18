# aquarius
[![codecov](https://codecov.io/gh/kcwl/aquarius/graph/badge.svg?token=4ACYCIPH8B)](https://codecov.io/gh/kcwl/aquarius)
[![ubuntu(gcc)](https://github.com/kcwl/aquarius/actions/workflows/cmake-ubuntu-gcc.yml/badge.svg)](https://github.com/kcwl/aquarius/actions/workflows/cmake-ubuntu-gcc.yml)
[![visual stadio](https://github.com/kcwl/aquarius/actions/workflows/cmake-windows-vs.yml/badge.svg)](https://github.com/kcwl/aquarius/actions/workflows/cmake-windows-vs.yml)

## Introduction
Aquarius is a modern c++ framework for network protocol applications. This repository includes basic code, some tools and some applications.

## Documentation
The full set of documentation can also be found in the respository under docs/.

## Dependece
+ Openssl 3 or later
+ Boost 1.81 or later
+ cmake 3.2 or later
+ gcc 13.3 or later or vs 2022 or later

## Quick Start
Make a TCP protocol as an example:
1. Create a TCP server, basic server has three params, that first one is port and second one is thread count and third one is server name.Maybe like this:
```
aquarius::tcp_server srv(8100,10, "test tcp server");
srv.run();
```
2. Regist a handler to handling TCP protocol requests. `AQUARIUS_HANDLER()` has three params, the first one is `Request` and second one is `Response` and the third one is name of handler(this is `ctx_test`). When you do this, that will generate a class named `ctx_test` and handling protocols. Maybe like this: 
```
AQUAIURS_HANDLER(request, response, ctx_test)
{
    // To do something
}
```
3. Create a TCP client, and connect server, when connect success, send a request and that will receive a response. Maybe like this
```
aquarius::io_context io{};
aquarius::tcp_client cli(io, 300ms);
co_await cli.async_connect("127.0.0.1", 8100);

auto resp = co_await cli.async_send<response>(request);
```

For more examples, please refer to [Here](samples/) dirs.

## Generate Protocol
define a virgo file for protocol and use lazytool to generate, like this:
```
lazytool test.virgo --cpp_out=. --protocol=tcp
```

For details, please refer to [Here](docs/virgo.md)

## Use MySQL
Aquarius provides some APIs to use MySQL.
1. Create a protocol for MySQL and use `lazytool` generate a code files.
```
model person
{
    integer id;
}
```
2. Define a MySQL config that named `mysql.ini`. It likes this:
```
[mysql]
host=xx
user=xx
password=xxx
...
```
For param details, please refer to [Here](include/aquarius/tbl/database_param.hpp)
3. Use APIs to execute sql. For examples:
```
person per{};
per.set_condition(1);

mpc_select(per);   // That will be equivalent to excute sql that is `select * from peson where id=1`
```

For details, please refer to [Here](docs/tbl.md)


## Use Lua for Handler
Coming Soon