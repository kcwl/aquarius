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
class login_req_header final : public aquarius::virgo::tcp_serialize
{
public:
	login_req_header() = default; 
	virtual ~login_req_header() = default; 

public:
	virtual void serialize(aquarius::detail::flex_buffer<char>& buffer) override; 

	virtual void deserialize(aquarius::detail::flex_buffer<char>& buffer) override; 
	int32_t uuid;
};

class login_req_body final : public aquarius::virgo::tcp_serialize
{
public:
	login_req_body() = default; 
	virtual ~login_req_body() = default; 

public:
	virtual void serialize(aquarius::detail::flex_buffer<char>& buffer) override; 

	virtual void deserialize(aquarius::detail::flex_buffer<char>& buffer) override; 
	person per_req;
};

class login_resp_header final : public aquarius::virgo::tcp_serialize
{
public:
	login_resp_header() = default; 
	virtual ~login_resp_header() = default; 

public:
	virtual void serialize(aquarius::detail::flex_buffer<char>& buffer) override; 

	virtual void deserialize(aquarius::detail::flex_buffer<char>& buffer) override; 
	int32_t uuid;
};

class login_resp_body final : public aquarius::virgo::tcp_serialize
{
public:
	login_resp_body() = default; 
	virtual ~login_resp_body() = default; 

public:
	virtual void serialize(aquarius::detail::flex_buffer<char>& buffer) override; 

	virtual void deserialize(aquarius::detail::flex_buffer<char>& buffer) override; 
	person per_resp;
};






using login_request = aquarius::virgo::tcp_request<"10001", login_req_header, login_req_body>;
using login_response = aquarius::virgo::tcp_response<"10001", login_resp_header, login_resp_body>;
