本文说明如何使用elastic语言来构建elastic数据，包括.elc文件语法以及如何从.elc文件生成数据访问类。 

这是一份参考指南 - 有关使用本文档中描述的许多功能的分步示例，请参阅对应语言的教程。

## elastic支持哪些语言？
+ c++: 会生成 .h和.cpp文件
+ java: 会生成.java文件
+ c#: 会生成一个.cs文件
+ lua： 会生成一个.lua文件

## 关键字
|名称|用途|是否启用|
|----|----|--------|
|package|命名空间|启用|
|package...package|多级命名空间|未启用|
|import|导入文件|启用|
|syntax|版本标识|未启用|
|enum|声明枚举变量|启用|
|message|声明对象集合|启用|

## 定义消息类型
定义一个简单的查询消息格式，其中包括一个工厂名的字符串，工厂车间的数量以及每个车间的员工数。以下是对应定义：
```
message QueryRequest = 1000
{
  string factory_name;
  int32 home_number;
  uint32 number_of_home;
}
```
+ `message`为集合声明关键字,后续大括号中的内容为一个集合。第一行结尾的数字为集合的唯一编号， 此编号可选，如不使用此序号，则不提供消息注册

+ `QueryRequest` 定义了三个字段，每个字段都由`类型+字段名`组成，其中的类型有很多种，除标量类型外，还可以指定复合类型以及枚举类型。具体类型如下：

## 字段类型
| 原型  | 注释| c++类型|java类型|c#类型|lua类型|默认值|
|                                                     |
| double||double|double|doule|double|0|
| float |float|float|float|double|0|
| int32 |使用可变长度编码。ZigZag编码格式|int32|int|int|int|0|
| int64 |使用可变长度编码。ZigZag编码格式|int64|long|long|long|0|
| uint32|使用可变长度编码。|uint32|int|uint|uint|0|
| uint64|使用可变长度编码。|uint64|long|ulong|uint64|0|
| fixed32|固定4字节长度，如果大于2^28,则比uint32高效。|uint32|int|uint|uint|0|
| fixed64|固定8字节长度，如果大于2^56,则比uint64高效。|uint64|long|ulong|long|0|
| bool | |bool|boolean|bool|bool|false|
| string|字符串必须是utf8或者7-bit ASCII，并且长度不大于2^32|string|ByteString|ByteString|str|""|
| bytes| | vector<uint8>|ByteString|ByteString|byte|""|

## 复合字段类型
消息字段可以是以下之一：
  + optional: optional字段可能处于以下两种状态之一：
    + 该字段已设置， 并且包含显示设置的值，则将会被执行序列化
    + 该字段未设置，将返回默认值，但不会被序列化
  + repeated: 此字段类型可以在类型正确的情况下，重复领次或多次，顺序不变
  + 未知类型无法参与序列化

## 使用其他类型
### 枚举enum

`enum`关键字可以非常简单的通过在消息定义中定义一个变量来枚举变量。下面示例中添加了一个枚举变量。
```
enum Kind
{
    Kind1;
    Kind2;
    Kind3;
}

message QueryRequest = 1000
{
  string factory_name;
  int32 home_number;
  uint32 number_of_home;
  Kind k;
}
```

## 注释类型
如添加注释，请采用c++样式风格的注释，即：`//` 或`/**/`