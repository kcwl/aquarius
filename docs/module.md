# Aquarius Modules — README (English)

This document explains how to use the Aquarius module framework located in `include/aquarius/module`. It covers module definition, registration, scheduling and running.

## Contents
- [Overview](#overview)
- [Quick Start](#quick-start)
- [API Overview](#api-overview)
- [Examples](#examples)
- [Notes](#notes)

## Overview

The framework consists of:

- `module_base` / `basic_module<T>`: base interface and CRTP helper for modules.
- `module_router`: singleton router that stores, looks up and dispatches calls to modules.
- `AQUARIUS_MODULE`: macro to declare and optionally auto-register a module.
- `mpc_call` / `mpc_async_call`: helpers to call member functions across module boundaries using pointer-to-member.

These components allow managing module instances by type and safely invoking methods synchronously or asynchronously.

## Quick Start

1. Define a module (recommended to use `AQUARIUS_MODULE`):

```cpp
AQUARIUS_MODULE(player_module)
{
public:
    player_module() = default;

    void insert(std::size_t id, std::shared_ptr<player_base> p);
    template<typename T> std::shared_ptr<T> get(std::size_t id);
    void erase(std::size_t id);
};
```

2. Register manually (if not using auto-registration):

```cpp
module_router::get_mutable_instance().regist<player_module>();
```

3. Schedule a synchronous call to a module:

```cpp
auto result = module_router::get_mutable_instance().schedule<player_module, std::shared_ptr<player_base>>(
    [](player_module* m) { return m->get<player_base>(42); }
);
```

4. Asynchronous call (inside a co_await context):

```cpp
co_await module_router::get_mutable_instance().async_schedule<player_module, SomeType>(
    [&](player_module* m) -> asio::awaitable<SomeType> { co_return co_await m->async_op(); }
);
```

5. Use `mpc_call` to simplify pointer-to-member calls:

```cpp
auto p = mpc_call<&player_module::get<player_base>>(42);
```

6. Run all registered modules:

```cpp
io_service_pool pool{ /* thread count */ };
module_router::get_mutable_instance().run(pool);
```

## API Overview

- `module_base`: pure virtual interface with `name()`, `init()`, `stop()`, `enable()`, `timer()`, `run()`.
- `basic_module<T>`: provides defaults and `visit` / `async_visit` helpers.
- `module_router::regist<Module>()`: register a module instance (keyed by `name()`).
- `module_router::schedule<T, R>(Func&& f)`: synchronous schedule returning `R`.
- `module_router::async_schedule<T, R>(Func&& f)`: asynchronous schedule returning `asio::awaitable<R>`.
- `mpc_call` / `mpc_async_call`: deduce target class/return type from member pointer and dispatch.

## Notes

- `AQUARIUS_MODULE` registers a module instance during static initialization. If static init order issues occur, call `regist<Module>()` explicitly during program startup.
- `schedule` / `async_schedule` return `R{}` when the target module is not found or type mismatches; always check return values.
- Ensure logger and environment are initialized before calling `module_router::run`.

---

See the headers under `include/aquarius/` for more details and the `player_module.hpp` example.
