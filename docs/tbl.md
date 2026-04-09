# TBL — SQL Query Builder & Async Executor

## Introduction

TBL is a compile-time SQL query builder and async MySQL executor for aquarius. It provides a DSL-style API that lets you compose `SELECT`, `INSERT`, `UPDATE`, and `DELETE` statements using C++ pipe operators (`|`) and execute them asynchronously through Boost.MySQL connection pools.

Key features:

- **Compile-time reflection** — struct field names are extracted at compile time via `boost::pfr`, no manual mapping needed.
- **Pipe-style composition** — build queries by piping views together: `select<T> | grep | enter`.
- **Async execution** — all database operations return `asio::awaitable` and run on Boost.Asio coroutines.
- **Type-safe conditions** — `grep_view` binds conditions to struct member pointers, catching mismatches at compile time.

## Header

Include the umbrella header to access all TBL components:

```cpp
#include <aquarius/tbl.hpp>
```

## Define a Model

A model is a plain aggregate struct. TBL uses `boost::pfr` to reflect field names and values at compile time. No macros or registration are required.

```cpp
struct person
{
    int id;
    std::string name;
    int age;
};
```

> **Note:** The struct must be a simple aggregate (no user-defined constructors, no private members, no virtual functions).

## SQL Configure

Before executing any SQL, configure the MySQL connection:

```cpp
#include <aquarius/resource/global_resource.hpp>

// Configure MySQL connection
aquarius::global_resource::get_mutable_instance().mysql().host = "localhost";
aquarius::global_resource::get_mutable_instance().mysql().port = 3306;
aquarius::global_resource::get_mutable_instance().mysql().db = "mydb";
aquarius::global_resource::get_mutable_instance().mysql().user = "root";
aquarius::global_resource::get_mutable_instance().mysql().password = "password";

```

## API Reference

### insert — Insert a Record

`insert_view` generates an `INSERT INTO` statement from a struct instance. Field names and values are extracted automatically.

```cpp
person p{1, "Alice", 25};

// Build the SQL string
aquarius::insert_view iv;
iv(p);
std::string sql = static_cast<std::string>(iv);
// => "insert into person(id,name,age) values(1,Alice,25)"
```

Use the global `insert` variable for a more concise style:

```cpp
aquarius::insert(p);
```

### select — Query Records

`select_view<T>` generates a `SELECT` statement. Without member pointer arguments, it selects all fields.

```cpp
// select * from person
aquarius::select_view<person> sv;
std::string sql = static_cast<std::string>(sv);
// => "select * from person"
```

Use the global `select` variable:

```cpp
aquarius::select<person>
```

### update — Update Records

`update_view` generates an `UPDATE ... SET` statement from a struct instance.

```cpp
person p{1, "Bob", 30};

aquarius::update_view uv;
uv(p);
std::string sql = static_cast<std::string>(uv);
// => "update person set id=1 and name=Bob and age=30"
```

### remove — Delete Records

`remove_view` generates a `DELETE FROM` statement.

```cpp
aquarius::remove_view rv;
rv(person{});
std::string sql = static_cast<std::string>(rv);
// => "delete from person"
```

### grep — Build Conditions

`grep_view<Ptr>` builds a `WHERE` condition bound to a specific struct member pointer. It supports the following comparison methods:

| Method | SQL |
|---|---|
| `equal(v)` / `operator==` | `field = v` |
| `less(v)` / `operator<` | `field < v` |
| `less_equal(v)` / `operator<=` | `field <= v` |
| `greater(v)` | `field > v` |
| `greater_equal(v)` / `operator>=` | `field >= v` |

```cpp
// Create a condition: id = 1
aquarius::grep_view<&person::id> g;
g.equal(1);
// or use the global variable
auto g = aquarius::grep<&person::id>();
g.equal(1);
```

All methods return `*this`, so you can reassign conditions:

```cpp
auto g = aquarius::grep<&person::id>();
g.equal(1);   // id = 1
g.less(10);   // now: id < 10  (overwrites previous condition)
```

### Pipe Operator `|` — Attach Conditions

Use the pipe operator `|` to attach `grep_view` conditions to any SQL view. The first condition adds `WHERE`, subsequent conditions add `AND`.

```cpp
// update person set id=1 and name=Bob and age=30 where id = 1
aquarius::update_view uv;
uv(person{1, "Bob", 30});

auto g = aquarius::grep<&person::id>();
g.equal(1);

uv | g;
```

Multiple conditions:

```cpp
// select * from person where ... and id = 1 and age < 30
aquarius::select_view<person> sv;

auto g1 = aquarius::grep<&person::id>();
g1.equal(1);

auto g2 = aquarius::grep<&person::age>();
g2.less(30);

sv | g1;
sv | g2;
```

### OR Operator `||` — Combine Conditions

Use `||` to combine two `grep_view` conditions with `OR`. The result is wrapped in parentheses.

```cpp
auto g1 = aquarius::grep<&person::id>();
g1.equal(1);

auto g2 = aquarius::grep<&person::id>();
g2.equal(2);

g1 || g2;
// g1 is now: "(id = 1 or id = 2)"
```

### enter — Execute SQL

The global `enter_view` object `enter` sends the composed SQL to the database asynchronously. Pipe a view into `enter` to execute it.

All `enter` operations are coroutines and must be used inside an `asio::awaitable` context.

```cpp
// Insert
auto affected = co_await (aquarius::insert(p) | aquarius::enter);

// Select
auto results = co_await (aquarius::select<person> | aquarius::enter);
// results is std::vector<person>

// Update
auto affected = co_await (aquarius::update(p) | aquarius::enter);

// Delete
auto affected = co_await (aquarius::remove(p) | aquarius::enter);
```

### trans_guard — Transaction RAII Guard

`trans_guard` provides a RAII wrapper that calls `begin()` on construction and `end()` on destruction.

```cpp
{
    aquarius::trans_guard guard(&service);
    // begin() called here
    // ... do work ...
} // end() called here
```

CTAD (Class Template Argument Deduction) is supported:

```cpp
aquarius::trans_guard guard(&service);  // T deduced automatically
```

### MySQL Type Aliases

The `aquarius::mysql` namespace provides type aliases that map to MySQL column types:

| Alias | C++ Type |
|---|---|
| `mysql::string` | `std::string` |
| `mysql::uint64_t` | `std::uint64_t` |
| `mysql::int64_t` | `std::int64_t` |
| `mysql::blob` | `std::vector<uint8_t>` |
| `mysql::date` | `boost::mysql::date` |
| `mysql::datetime` | `boost::mysql::datetime` |
| `mysql::time` | `boost::mysql::time` |
| `float`|`float`|
| `double`|`double`|

## Complete Example

```cpp
#include <aquarius/tbl.hpp>
#include <aquarius/io_service_pool.hpp>

struct person
{
    int id;
    std::string name;
    int age;
};

// Inside a coroutine context:
auto example() -> aquarius::asio::awaitable<void>
{
    person p{1, "Alice", 25};

    // INSERT
    auto rows = co_await (aquarius::insert(p) | aquarius::enter);

    // SELECT all
    auto all = co_await (aquarius::select<person> | aquarius::enter);

    // SELECT with condition
    aquarius::select_view<person> sv;
    auto g = aquarius::grep<&person::age>();
    g.less(30);
    sv | g;
    auto young = co_await (sv | aquarius::enter);

    // UPDATE with condition
    aquarius::update_view uv;
    uv(person{1, "Alice", 26});
    auto g_id = aquarius::grep<&person::id>();
    g_id.equal(1);
    uv | g_id;
    auto updated = co_await (uv | aquarius::enter);

    // DELETE with condition
    aquarius::remove_view rv;
    rv(person{});
    auto g_del = aquarius::grep<&person::id>();
    g_del.equal(1);
    rv | g_del;
    auto deleted = co_await (rv | aquarius::enter);

    co_return;
}
```

## File Structure

```
include/aquarius/tbl/
├── concepts.hpp              # Type traits: remove_member_pointer, member_pointer_struct, iterator_t, push_t
├── member_pointer.hpp        # Compile-time member pointer name extraction
├── grep_view.hpp             # WHERE condition builder
├── insert_view.hpp           # INSERT SQL generator
├── update_view.hpp           # UPDATE SQL generator
├── remove_view.hpp           # DELETE SQL generator
├── select_view.hpp           # SELECT SQL generator
├── operator.hpp              # Pipe operators (|, ||) for composing views
├── enter_view.hpp            # Async SQL executor via sql_module
├── transaction.hpp           # RAII transaction guard
├── mysql_type.hpp            # MySQL type aliases and type_cast utilities
├── mysql_config_register.hpp # MySQL configuration macro
├── sql_op.hpp                # sql_op type alias (mysql_io_service)
└── detail/
    └── mysql_io_service.hpp  # Boost.MySQL connection pool wrapper
```
