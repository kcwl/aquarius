
# virgo

## What is Virgo?
Virgo is a protocol processing component for aquarius. The purpose of Virgo is to provide a universal sulotion for most protocol that as long as simple declarations are used, a certain type of protocol can be supported.

## How many protocols are supported?
Ceurently, Virgo supports the following protocols:
+ TCP
+ HTTP

A universal and extended solution will be provided in the future

## Composition of Virgo?
Virgo is composed of three parts:
+ structure
+ protocol
+ domain

### Structure
Virgo supports three keywords of structure: that is `enum`, `struct` and `protocol`.

#### enum
Virgo uses `enum` to define a enum type. So, define a enum to discribe a color, it likes this:
```
enum color
{
    red;
    green;
    blue;
}
```

#### struct
Virgo uses `struct` to define a struct type. So, define a struct to discribe a user info, it likes this:
```
struct UserInfo 
{
    int id;
    string name;
}
```

#### protocol
Virgo uses `protocol` to define a protocol type. So, define a protocol to discribe a TCP protocol, it likes this:
```
protocol SomeTCPProtocol 
{
    ...
}
```

## How to use Virgo?
Virgo provides a schema languages to describe a protocol. The schema language is called `Virgo Schema Language` (VSL).We use keyword that called `protocol` to define a protocol.
It likes this:
```
protocol SomeProtocol {};
```

The protocol structure contains three parts: `router`,`request` and `response`; Among them, the `router` is nessary, Virgo will use it to match the protocol. According the different protocols, the `request` and `response` may be omitted. It likes this:
```
protocol SomeProtocol -> router: value 
{
    request
    {
    header:
        type: value;
        ...

    body:
        type: value;
        ...
    }

    response
    {
    header:
        type: value;
        ...

    body:
        type: value;
        ...
    }
};
```
`request` and `response` are implement of `protocol`, any of them may be omitted. they have the same structure. The `header` and `body` are domain of protocol implement are composed of `key + value`, they are ending with `;`. The value of them is the custom protocol infos of users. 

### Router
The naming of `router` consists of two parts, the one is protocol type and the other is the type of router value. Currently, Virgo supports the following types:
+ tcp-key  // router value is a key of TCP packet
+ http-regex  // router value is a regex of HTTP path

So, define a TCP protocol, it likes this:
```
protocol SomeTCPProtocol -> tcp-key: 1000
{
    
};
```
define a HTTP protocol, it likes this:
```
protocol SomeTCPProtocol -> http-regex: ^/login$
{
    
};
```

### Compile Virgo Schema
Virgo provides a command line tool that called `lazytool` to compile VSL into C++ code.
Use the following command to compile VSL:
`lazytool --input test.virgo --cpp_out ./test`
that will be generated a C++ file in `./test` directory.Most usages of `lazytool` can be found in `lazytool --help`.