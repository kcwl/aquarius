# Virgo

## Introduction
Virgo is a protocol processing component for aquarius. The purpose of Virgo is to provide a universal sulotion for most protocol that as long as simple declarations are used, a certain type of protocol can be supported.The document is used to describe the layout of types on serialize or deserialize flow. The flow includes two kinds, one is protocol that on the network, and the other is local that on the sqls.

## Types

### Scalar Types
As of now, the scalar types support all of following types. The table shows the types and how to represent on different parts.

|Type|TCP|HTTP|Sqls|
|-|-|-|-|
|int32|Uses variable-length encoding. Inefficient for encoding negative numbers.|Json number|corresponding to sql int or bytes less than int|
|uint32|Uses variable-length encoding.|Json number|corresponding to sql int or bytes less than int|
|sint32|Uses Zig-Zag encoding and transform to int32. Efficient for encoding negative numbers.|Json number|corresponding to sql int or bytes less than int|
|int64|Uses variable-length encoding. Inefficient for encoding negative numbers.|Json number|corresponding to sql int or bytes less than bigint|
|uint64|Uses variable-length encoding.|Json number|corresponding to sql integer or bytes less than bigint|
|sint64|Uses Zig-Zag encoding and transform to int64. Efficient for encoding negative numbers.|Json number|corresponding to sql int or bytes less than bigint|
|fixed32|Always four bytes.|Json number|corresponding to sql int|
|fixed64|Always eight bytes.|Json number|corresponding to sql bigint|
|bool|turn to int32|Json boolean|sql int|
|float|Uses IEEE 754 single-precision format.|Json Number|sql float|
|double|Uses IEEE 754 double-precision format.|Json Number|sql double|
|string|A string must always contain 7-bit ASCII text|Json String|corresponding to sql char,varchar,text(text group)|
|bytes|May contain any arbitrary sequence of bytes|Json Array|corresponding to sql blob(blob group)|
|date|Turn to timestamp and use int64 ot encoding|Turn to timestamp and then use json number|Describe a date, its format is 'YYYY-MM-DD', corresponding to sql date|
|time|Turn to timestamp and use int64 ot encoding|Turn to timestamp and then use json number|Describe a date, its format is 'YYYY-MM-DD', corresponding to sql time|
|datetime|Turn to timestamp and use int64 ot encoding|Turn to timestamp and then use json number|Describe a date, its format is 'YYYY-MM-DD', corresponding to sql datetime|

|Type|c++|
|-|-|
|int32|uint32_t|
|uint32_t|uint32_t|
|sint32|int32_t|
|int64|uint64_t|
|uint64|uint64_t|
|sint64|int64_t|
|fiex32|uint32_t|
|fixed64|uint32_t|
|bool|bool|
|float|float|
|double|double|
|string|std::string|
|bytes|std::vector<char>|
|date|std::chrono::time_point|
|time|std::chrono::time_point|
|datetime|std::chrono::time_point|

### Composite Type
Composite Type is the way to describe complex type. As of now, composite types support following keys to realize.
#### repeated
The `repeated` is the way to represent a array, it means multiple of the same type can form a sequence. Such as, when use `repeated int32`, that means a group of types that calls `int32,int32,...`.

Usage:
```
repeated int32 values;
```
#### map
The 'map' is the way to represent a pair of two types. Such as, when use `map int32,int32`, that means a group of types that calls `<int32,int32>,<int32,int32>,...`

Usage:
```
map int32,int32 keys;
```

### Aggregation Type
The aggregation type is base unit of virgo. Every aggregation type has its own mission.Such as:
|Type|Notes|
|enum|one of its fields to only have one of a predefined list of values. |
|struct|includes multi scalar types|
|message|includes two parts that `request` and `response`, describe a protocol on network|
|model|describe a table on sql|

#### enum
In the following example, we define a `enum` named `color` to decribe a color list.
```
enum color
{
    red;
    yellow;
    green;
}
```

#### struct
In the following example, we define a `struct` named `person` to describe a person class.
```
struct person
{
    int32 age;
    string name;
}
```

#### message
In the following example, we define a `message` named `login` to describe a protocol.
```
message login
{
    request
    {
        string account;
        bytes password;
    }
    response
    {
        int32 result;
    }
}
```

#### model
In the following example, we define a `model` named `person` to describe a table.
```
model person
{
    int32 age;
    string name;
}
```

## Encoding
Virgo uses four ways to encoding all the types. So, that is `varint`, `len-str`,`i64` and `i32`.

|Name|Used For|
|-|-|
|varint|int32,uint32,sint32,int64,uint64,sint64,bool,enum|
|len-str|string,bytes,repeated,map|
|i64|fixed64,double|
|i32|fixed32,flaot|

### Varint
Variable-width integers, or varint, that allow encoding unsigned 64-bit integers using anywhere between one and ten bytes, with small values using fewer bytes. Every byte contains one sign-bit and 7-bit effective value. So max of uint64 will use ten bytes and max of uint32 will use five bytes.

```
Note:  If your number is always greater than 2^28 and less than 2^32, the fixed32 will rather than int32 and uint32, similarly if your number is always greater than 2^56 and less than 2^64, fixed64 wiil rather than int64 and uint64.
```

#### More Integer Types

##### bool and enum
bool and enum will transform to int32. bool always is `00`or `01` and enum will start with `0`(as of now, enum can not support custom initial number).

##### sint32 and sint64
sintN uses the “ZigZag” encoding instead of two’s complement to encode negative integers. Positive integers are encoded as (the even numbers), while negative integers are encoded as (the odd numbers). The encoding thus “zig-zags” between positive and negative numbers.

|Signed Original|Encode As|
|-|-|
|0|0|
|-1|1|
|1|2|
|-2|3|
|...|...|
|0x7fffffff|0xfffffffe|
|-0x80000000|0xffffffff|

So, each value `n` is encoded with:
```
for sint32:  (n << 1) ^ (n >> 31)
```
and 
```
for sint64: (n << 1) ^ (n >> 63)
```
When decode, that will decode to sintN `n` with:
```
for sint32: (n >> 1) ^ (~(n&0x1))
```

#### datetime and date and time
Some types that releated to time will store with timestamp and turn timestamp to int64, use the way of int64 to encode. 


### Len-Str
The sequences that is uncertain-length always use **Length Prefixs** to encode.

#### string
As now, we have a string that is `hello word!`, that will be encoded:
```
c          68   65    6c    6c    6f    20      77    6f    72    6c    64    21
^··str-len ^··h  ^··e  ^··l  ^··l  ^··o  ^··' '  ^··w  ^··o  ^··r  ^··l  ^··d  ^··!
```

#### repeated
As now, we have a repeated integers that is `{1, 2, 3, 4, 5}`, that will be encoded:
```
6               1    2    3    4    5
^··repeated-len ^··1 ^··2 ^··3 ^··4 ^··5
```

#### map
As now, we have a key-value sequence that is `{{1,2},{3,4},{5,6}}`, that will be encoded:
```
3           [1 2]        [3 4]        [5 6]
^··map-len    ^··value1    ^··value2    ^··value3
```