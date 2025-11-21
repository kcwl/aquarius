#pragma once
#include <aquarius.hpp>
struct person
{
public:
	bool sex;
	uint32 addr;
	int32 age;
	uint64 telephone;
	int64 score;
	float hp;
	double mana;
	string name;
	bytes orders;
};
class login_req_body
{
public:
	person per_req;
};
class login_resp_body
{
public:
	person per_resp;
};
class new_http_login_req_body
{
public:
	int32 uuid;
	person per_req;
};
class new_http_login_resp_body
{
public:
	int32 uuid;
	person per_resp;
};
class http_test_get_req_body
{
public:
	int32 user;
	string passwd;
};
class http_test_get_resp_body
{
public:
	int32 user;
	string passwd;
};
