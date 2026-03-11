struct person
{
	bool operator==(const person & other) const; 

	bool sex;
	uint32 addr;
	int32 age;
	uint64 telephone;
	int64 score;
	float hp;
	double mana;
	string name;
	bytes orders;
struct login : public aquarius::tcp_binary_serialize
{
public:
	login();
	virtual ~login() = default;

public:
	bool operator==(const login & other) const; 

public:
	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;

public:
	person per_req;
struct login : public aquarius::tcp_binary_serialize
{
public:
	login();
	virtual ~login() = default;

public:
	bool operator==(const login & other) const; 

public:
	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;

public:
	person per_resp;
struct http_test_post : public aquarius::http_json_serialize
{
public:
	http_test_post();
	virtual ~http_test_post() = default;

public:
	bool operator==(const http_test_post & other) const; 

public:
	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;

public:
	int32 uuid;
	person per_req;
struct http_test_post : public aquarius::http_json_serialize
{
public:
	http_test_post();
	virtual ~http_test_post() = default;

public:
	bool operator==(const http_test_post & other) const; 

public:
	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;

public:
	int32 uuid;
	person per_resp;
struct http_test_get : public aquarius::http_kv_serialize
{
public:
	http_test_get();
	virtual ~http_test_get() = default;

public:
	bool operator==(const http_test_get & other) const; 

public:
	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;

public:
	int32 user;
	string passwd;
struct http_test_get : public aquarius::http_json_serialize
{
public:
	http_test_get();
	virtual ~http_test_get() = default;

public:
	bool operator==(const http_test_get & other) const; 

public:
	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;

public:
	int32 user;
	string passwd;
