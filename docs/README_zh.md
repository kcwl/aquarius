# Aquarius
[![codecov](https://codecov.io/gh/kcwl/aquarius/graph/badge.svg?token=4ACYCIPH8B)](https://codecov.io/gh/kcwl/aquarius)
[![ubuntu(gcc)](https://github.com/kcwl/aquarius/actions/workflows/cmake-ubuntu-gcc.yml/badge.svg)](https://github.com/kcwl/aquarius/actions/workflows/cmake-ubuntu-gcc.yml)
[![visual stadio](https://github.com/kcwl/aquarius/actions/workflows/cmake-windows-vs.yml/badge.svg)](https://github.com/kcwl/aquarius/actions/workflows/cmake-windows-vs.yml)

Aquarius 是用于网络协议应用的现代 C++ 框架。本仓库包含核心库、协议工具（lazytool）、示例应用以及若干开发/测试辅助代码。

状态：活跃开发（请参见仓库中的 CI 状态徽章和 docs/）

主要特性
- 模块化模块系统（module_router / AQUARIUS_MODULE）
- 基于 virgo 的协议定义与代码生成（lazytool）
- TCP 客户端/服务器、同步/异步调度支持（asio 协程友好）
- 数据库（MySQL）访问辅助 API

依赖
- OpenSSL >= 3
- Boost >= 1.81
- CMake >= 3.2
- GCC >= 13.3 或 MSVC (Visual Studio 2022/2024)

目录结构（概要）
- include/aquarius/    — 框架头文件（模块、网络、工具、资源、tbl 等）
- src/                 — 框架实现与示例程序
- docs/                — 模块、协议、tbl 等文档（详见 docs/）
- samples/             — 示例工程和使用示例（协议、服务器、客户端）
- tools/lazytool       — 协议生成器（将 .virgo 生成为 C++）

快速开始（Linux / Windows）
1. 克隆仓库：

   git clone https://github.com/kcwl/aquarius.git

2. 使用 CMake 构建：

   mkdir build && cd build
   cmake ..
   cmake --build . --config Release

   在 Windows 下可打开生成的 Visual Studio 解决方案进行调试/开发。

3. 运行示例：

   构建完成后，samples/ 目录内有示例二进制（或在 docs 中查看具体示例）。

TCP 服务示例（简要）

```cpp
aquarius::tcp_server srv(8100, 10, "test tcp server");
srv.run();
```

注册处理器（宏示例）

```cpp
AQUARIUS_HANDLER(request_t, response_t, ctx_test)
{
    // 处理逻辑
}
```

异步客户端示例（协程）

```cpp
aquarius::io_context io{};
aquarius::tcp_client cli(io, std::chrono::milliseconds(300));
co_await cli.async_connect("127.0.0.1", 8100);
auto resp = co_await cli.async_send<response_t>(request);
```

协议生成（lazytool）

1. 使用 .virgo 文件描述协议模型：

   model person { integer id; }

2. 生成 C++ 代码：

   lazytool test.virgo --cpp_out=. --protocol=tcp

更多信息请参阅 docs/virgo.md。

MySQL 支持

1. 在 .virgo 中定义表模型并使用 lazytool 生成对应代码。
2. 在配置文件（例如 mysql.ini）中填写连接信息（参见 include/aquarius/tbl/database_param.hpp）。
3. 使用框架提供的 API（例如 mpc_select）进行 CRUD 操作。

模块系统简介

Aquarius 提供 module_router、basic_module、AQUARIUS_MODULE 宏以及 mpc_call 辅助函数，用于跨模块的同步/异步方法调用与管理。详情见 docs/module.md 与 include/aquarius/module/ 下的头文件。

贡献
- 欢迎提交 issue 与 PR。请遵循仓库的 code style 与测试策略。
- 在开新特性分支前建议先通过 issue 讨论设计。

许可
请参见仓库根目录的 LICENSE 文件。

更多文档
- docs/ 目录包含模块、协议（virgo）、tbl（数据库模型）等详细说明。  

联系方式
- 仓库主页：https://github.com/kcwl/aquarius

---
此 README 为仓库概要说明；如需更详细的部署/开发步骤，请参阅 docs/ 下的具体文档。
