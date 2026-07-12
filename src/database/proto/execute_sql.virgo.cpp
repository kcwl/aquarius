#include "execute_sql.virgo.h"
#include <aquarius/serialize/binary.hpp>
#include <aquarius/serialize/json.hpp>
#include <aquarius/serialize/kv.hpp>

using namespace aquarius;

struct query_sql_req_body::impl
{
	string sql;
AQUARIUS_SERIALIZE_STRUCT(query_sql_req_body::impl, (sql))
};
query_sql_req_body::query_sql_req_body()
	: impl_ptr_(std::make_shared<impl>())
{}

query_sql_req_body::~query_sql_req_body(){}

void query_sql_req_body::to_binary(flex_buffer& buffer)
{
	binary_parse::to_datas(impl_ptr_->sql, buffer);
}
void query_sql_req_body::from_binary(flex_buffer& buffer)
{
	impl_ptr_->sql = binary_parse::from_datas<string>(buffer);
}
void query_sql_req_body::to_json(flex_buffer& buffer)
{
	json::value jv{};
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("sql", impl_ptr_->sql);
	auto json_str = json::serialize(jv);
	buffer.sputn(json_str.data(), json_str.size());
}
void query_sql_req_body::from_json(flex_buffer& buffer)
{
	json::value jv = json::parse(std::string((char*)buffer.data().data(), buffer.size()));
	auto jv_obj = jv.try_as_object();
	if(jv_obj.has_error())
	{
		return;
	}
	impl_ptr_->sql = static_cast<string>(jv_obj->at("sql").as_string());
}
void query_sql_req_body::to_kv(flex_buffer& buffer)
{
	buffer.sputc('?');
	kv_parse::to_datas(impl_ptr_->sql, buffer);
}
void query_sql_req_body::from_kv(flex_buffer& buffer)
{
	impl_ptr_->sql = kv_parse::from_datas<string>(buffer, impl_ptr_->sql);
}
string query_sql_req_body::sql() const
{
	return impl_ptr_->sql;
}
string& query_sql_req_body::sql()
{
	return impl_ptr_->sql;
}
struct query_sql_resp_body::impl
{
	std::vector<string> results;
AQUARIUS_SERIALIZE_STRUCT(query_sql_resp_body::impl, (results))
};
query_sql_resp_body::query_sql_resp_body()
	: impl_ptr_(std::make_shared<impl>())
{}

query_sql_resp_body::~query_sql_resp_body(){}

void query_sql_resp_body::to_binary(flex_buffer& buffer)
{
	binary_parse::to_datas(impl_ptr_->results, buffer);
}
void query_sql_resp_body::from_binary(flex_buffer& buffer)
{
	impl_ptr_->results = binary_parse::from_datas<std::vector<string>>(buffer);
}
void query_sql_resp_body::to_json(flex_buffer& buffer)
{
	json::value jv{};
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("results", aquarius::json_value_from_object<std::vector<string>>(impl_ptr_->results));
	auto json_str = json::serialize(jv);
	buffer.sputn(json_str.data(), json_str.size());
}
void query_sql_resp_body::from_json(flex_buffer& buffer)
{
	json::value jv = json::parse(std::string((char*)buffer.data().data(), buffer.size()));
	auto jv_obj = jv.try_as_object();
	if(jv_obj.has_error())
	{
		return;
	}
	impl_ptr_->results = aquarius::json::value_to<std::vector<string>>(jv_obj->at("results"));
}
void query_sql_resp_body::to_kv(flex_buffer& buffer)
{
	buffer.sputc('?');
	kv_parse::to_datas(impl_ptr_->results, buffer);
}
void query_sql_resp_body::from_kv(flex_buffer& buffer)
{
	impl_ptr_->results = kv_parse::from_datas<std::vector<string>>(buffer, impl_ptr_->results);
}
std::vector<string> query_sql_resp_body::results() const
{
	return impl_ptr_->results;
}
std::vector<string>& query_sql_resp_body::results()
{
	return impl_ptr_->results;
}

struct execute_sql_req_body::impl
{
	string sql;
AQUARIUS_SERIALIZE_STRUCT(execute_sql_req_body::impl, (sql))
};
execute_sql_req_body::execute_sql_req_body()
	: impl_ptr_(std::make_shared<impl>())
{}

execute_sql_req_body::~execute_sql_req_body(){}

void execute_sql_req_body::to_binary(flex_buffer& buffer)
{
	binary_parse::to_datas(impl_ptr_->sql, buffer);
}
void execute_sql_req_body::from_binary(flex_buffer& buffer)
{
	impl_ptr_->sql = binary_parse::from_datas<string>(buffer);
}
void execute_sql_req_body::to_json(flex_buffer& buffer)
{
	json::value jv{};
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("sql", impl_ptr_->sql);
	auto json_str = json::serialize(jv);
	buffer.sputn(json_str.data(), json_str.size());
}
void execute_sql_req_body::from_json(flex_buffer& buffer)
{
	json::value jv = json::parse(std::string((char*)buffer.data().data(), buffer.size()));
	auto jv_obj = jv.try_as_object();
	if(jv_obj.has_error())
	{
		return;
	}
	impl_ptr_->sql = static_cast<string>(jv_obj->at("sql").as_string());
}
void execute_sql_req_body::to_kv(flex_buffer& buffer)
{
	buffer.sputc('?');
	kv_parse::to_datas(impl_ptr_->sql, buffer);
}
void execute_sql_req_body::from_kv(flex_buffer& buffer)
{
	impl_ptr_->sql = kv_parse::from_datas<string>(buffer, impl_ptr_->sql);
}
string execute_sql_req_body::sql() const
{
	return impl_ptr_->sql;
}
string& execute_sql_req_body::sql()
{
	return impl_ptr_->sql;
}
struct execute_sql_resp_body::impl
{
	int32 affected;
AQUARIUS_SERIALIZE_STRUCT(execute_sql_resp_body::impl, (affected))
};
execute_sql_resp_body::execute_sql_resp_body()
	: impl_ptr_(std::make_shared<impl>())
{}

execute_sql_resp_body::~execute_sql_resp_body(){}

void execute_sql_resp_body::to_binary(flex_buffer& buffer)
{
	binary_parse::to_datas(impl_ptr_->affected, buffer);
}
void execute_sql_resp_body::from_binary(flex_buffer& buffer)
{
	impl_ptr_->affected = binary_parse::from_datas<int32>(buffer);
}
void execute_sql_resp_body::to_json(flex_buffer& buffer)
{
	json::value jv{};
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("affected", impl_ptr_->affected);
	auto json_str = json::serialize(jv);
	buffer.sputn(json_str.data(), json_str.size());
}
void execute_sql_resp_body::from_json(flex_buffer& buffer)
{
	json::value jv = json::parse(std::string((char*)buffer.data().data(), buffer.size()));
	auto jv_obj = jv.try_as_object();
	if(jv_obj.has_error())
	{
		return;
	}
	impl_ptr_->affected = static_cast<int32>(jv_obj->at("affected").as_int64());
}
void execute_sql_resp_body::to_kv(flex_buffer& buffer)
{
	buffer.sputc('?');
	kv_parse::to_datas(impl_ptr_->affected, buffer);
}
void execute_sql_resp_body::from_kv(flex_buffer& buffer)
{
	impl_ptr_->affected = kv_parse::from_datas<int32>(buffer, impl_ptr_->affected);
}
int32 execute_sql_resp_body::affected() const
{
	return impl_ptr_->affected;
}
int32& execute_sql_resp_body::affected()
{
	return impl_ptr_->affected;
}
