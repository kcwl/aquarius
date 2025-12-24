# TBL
The TBL is a scheme language for sql table generation. That contains three parts: key,index and attribute. the table scheme will like this:

```
model person -> encoding[note: utf8 or gbk]
{
    field_type field_name [@key or ^index or $attr];
}
```
`model` describe a table, the `person` is table name,  `encoding` is a encoding, the fields is included within curly braces.

# Constraint
Key, Index and Attribute is the constraint of sql field.

## Key
The key consists of two parts that prefix `@` and name.

|type|usage|note|
|----------|
|primary key| @primary|
|foreign key| @foreign(ref)|`ref` is the reference of other key|
|unique key| @unique|

## Index
The index consists of two parts that prefix `^` and name.

|type|usage|note|
|----------|
|primary index| ^primary|
|normal index| ^index |
|unique index| ^unique|
|fulltext index| ^fulltext|
|spatial index| ^spatial|

## Attribute
The attribute consists of two parts that prefix `^` and name.

|type|usage|note|
|----------|
|not null value| @not_null|
|default value| @default(value)|`value` is the default value|
|auto increament value| @auto_increment|
|auto update timestamp| $auto_timestamp|
|comment note| $comment(note)| `note` is note text|

## Usage
That id is a unique key, it has unique index , it will auto increase and it is not null

```
model test_table -> utf8
{
    int id @unique ^unique $auto_increment $not_null;
}
```

# Type 
## integer
+ int1
+ int2
+ int3
+ int4
+ int8

## float
+ float
+ double

## decimal
+ decimal(暂未支持)

## string
+ char
+ varchar
+ text

## binary
+ blob(暂未支持)

## enum
+ enum(暂未支持)

## set
+ set(暂未支持)

## datetime
+ year(暂未支持)
+ timestamp(暂未支持)
+ date(暂未支持)
+ datetime
+ time(暂未支持)