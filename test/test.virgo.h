#pragma once
#include <aquarius.hpp>

struct person
{
    bool sex;
    uint32_t addr;
    int32_t age;
    uint64_t telephone;
    int64_t score;
    float hp;
    double mana;
    std::string name;
    std::vector<char> orders;

};
inline bool operator==(const person& lhs, const person& rhs)
	{
		return lhs.sex == rhs.sex && lhs.addr == rhs.addr && lhs.age == rhs.age && lhs.telephone == rhs.telephone && lhs.score == rhs.score && lhs.hp == rhs.hp && lhs.mana == rhs.mana && lhs.name == rhs.name && lhs.orders == rhs.orders;
	}

	inline std::ostream& operator<<(std::ostream& os, const person& other)
	{
		os << other.sex << other.addr << other.age << other.telephone << other.score << other.hp << other.mana << other.name << "[";
		for (auto& s : other.orders)
		{
			os << s << ", ";
		}
		os.seekp(-2, std::ios::cur);
		os << "]";
		return os;
	}
class login_req_header final : public aquarius::serialize::tcp_serialize
{
public:
	login_req_header() = default; 
	virtual ~login_req_header() = default; 

public:
	virtual void serialize(aquarius::detail::flex_buffer<char>& buffer) override; 

	virtual void deserialize(aquarius::detail::flex_buffer<char>& buffer) override; 
	int32_t uuid;
};

class login_req_body final : public aquarius::serialize::tcp_serialize
{
public:
	login_req_body() = default; 
	virtual ~login_req_body() = default; 

public:
	virtual void serialize(aquarius::detail::flex_buffer<char>& buffer) override; 

	virtual void deserialize(aquarius::detail::flex_buffer<char>& buffer) override; 
	person per_req;
};

class login_resp_header final : public aquarius::serialize::tcp_serialize
{
public:
	login_resp_header() = default; 
	virtual ~login_resp_header() = default; 

public:
	virtual void serialize(aquarius::detail::flex_buffer<char>& buffer) override; 

	virtual void deserialize(aquarius::detail::flex_buffer<char>& buffer) override; 
	int32_t uuid;
};

class login_resp_body final : public aquarius::serialize::tcp_serialize
{
public:
	login_resp_body() = default; 
	virtual ~login_resp_body() = default; 

public:
	virtual void serialize(aquarius::detail::flex_buffer<char>& buffer) override; 

	virtual void deserialize(aquarius::detail::flex_buffer<char>& buffer) override; 
	person per_resp;
};

class http_login_req_header final : public aquarius::serialize::http_serialize
{
public:
	http_login_req_header() = default; 
	virtual ~http_login_req_header() = default; 

public:
	virtual void serialize(aquarius::detail::flex_buffer<char>& buffer) override; 

	virtual void deserialize(aquarius::detail::flex_buffer<char>& buffer) override; 
	int32_t uuid;
};

class http_login_req_body final : public aquarius::serialize::http_serialize
{
public:
	http_login_req_body() = default; 
	virtual ~http_login_req_body() = default; 

public:
	virtual void serialize(aquarius::detail::flex_buffer<char>& buffer) override; 

	virtual void deserialize(aquarius::detail::flex_buffer<char>& buffer) override; 
	friend void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const http_login_req_body& local);

	friend http_login_req_body tag_invoke(const aquarius::json::value_to_tag<http_login_req_body>&, const aquarius::json::value& jv);
	person per_req;
};

class http_login_resp_header final : public aquarius::serialize::http_serialize
{
public:
	http_login_resp_header() = default; 
	virtual ~http_login_resp_header() = default; 

public:
	virtual void serialize(aquarius::detail::flex_buffer<char>& buffer) override; 

	virtual void deserialize(aquarius::detail::flex_buffer<char>& buffer) override; 
	int32_t uuid;
};

class http_login_resp_body final : public aquarius::serialize::http_serialize
{
public:
	http_login_resp_body() = default; 
	virtual ~http_login_resp_body() = default; 

public:
	virtual void serialize(aquarius::detail::flex_buffer<char>& buffer) override; 

	virtual void deserialize(aquarius::detail::flex_buffer<char>& buffer) override; 
	friend void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const http_login_resp_body& local);

	friend http_login_resp_body tag_invoke(const aquarius::json::value_to_tag<http_login_resp_body>&, const aquarius::json::value& jv);
	person per_resp;
};






using login_request = aquarius::virgo::tcp_request<"10001", login_req_header, login_req_body>;
using login_response = aquarius::virgo::tcp_response<"10001", login_resp_header, login_resp_body>;
using http_login_request = aquarius::virgo::http_request<"/login", http_login_req_header, http_login_req_body>;
using http_login_response = aquarius::virgo::http_response<"/login", http_login_resp_header, http_login_resp_body>;
