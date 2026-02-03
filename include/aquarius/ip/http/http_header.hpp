#pragma once
#include <aquarius/serialize/http_json_serialize.hpp>

namespace aquarius
{
	class http_request_header : public http_json_serialize
	{
		using base = http_json_serialize;

	public:
		http_request_header()
			: base()
			, uuid_()
		{}

		virtual ~http_request_header() = default;

		http_request_header(const http_request_header& other)
			: base(other)
			, uuid_(other.uuid_)
		{}

		http_request_header& operator=(const http_request_header& other)
		{
			if (this != std::addressof(other))
			{
				base::operator=(other);
				uuid_ = other.uuid_;
			}

			return *this;
		}

		http_request_header(http_request_header&& other) noexcept
			: base(std::move(other))
			, uuid_(std::exchange(other.uuid_, 0))
		{}

		http_request_header& operator=(http_request_header&& other) noexcept
		{
			if (this != std::addressof(other))
			{
				base::operator=(std::move(other));
				uuid_ = std::exchange(other.uuid_, 0);
			}

			return *this;
		}

		bool operator==(const http_request_header& other) const
		{
			return uuid_ == other.uuid_;
		}

	public:
		virtual void serialize(flex_buffer& buffer) override
		{
			this->parse_to(*this, buffer);
		}

		virtual void deserialize(flex_buffer& buffer) override
		{
			*this = this->parse_from<http_request_header>(buffer);
		}

		uint64_t uuid() const
		{
			return uuid_;
		}

		void uuid(uint64_t v)
		{
			uuid_ = v;
		}

	private:
		uint64_t uuid_;
	};

	class http_response_header : public http_json_serialize
	{
		using base = http_json_serialize;

	public:
		http_response_header()
			: result_()
		{}

		virtual ~http_response_header() = default;

		http_response_header(const http_response_header& other)
			: base(other)
			, result_(other.result_)
		{}

		http_response_header& operator=(const http_response_header& other)
		{
			if (this != std::addressof(other))
			{
				base::operator=(other);
				result_ = other.result_;
			}

			return *this;
		}

		http_response_header(http_response_header&& other) noexcept
			: base(std::move(other))
			, result_(std::exchange(other.result_, 0))
		{}

		http_response_header& operator=(http_response_header&& other) noexcept
		{
			if (this != std::addressof(other))
			{
				base::operator=(std::move(other));
				result_ = std::exchange(other.result_, 0);
			}

			return *this;
		}

		bool operator==(const http_response_header& other) const
		{
			return result_ == other.result_;
		}

	public:
		virtual void serialize(flex_buffer& buffer) override
		{
			this->parse_to(*this, buffer);
		}

		virtual void deserialize(flex_buffer& buffer) override
		{
			*this = this->parse_from<http_response_header>(buffer);
		}

		int32_t result() const
		{
			return result_;
		}

		void result(int32_t v)
		{
			result_ = v;
		}

		uint64_t uuid() const
		{
			return uuid_;
		}

		void uuid(uint64_t v)
		{
			uuid_ = v;
		}

	private:
		uint64_t uuid_;

		int32_t result_;
	};

	inline std::ostream& operator<<(std::ostream& os, const http_request_header& header)
	{
		os << "uuid: " << header.uuid();

		return os;
	}

	inline std::ostream& operator<<(std::ostream& os, const http_response_header& header)
	{
		os << "result: " << header.result();

		return os;
	}

	inline void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const http_request_header& local)
	{
		auto& jv_obj = jv.emplace_object();
		jv_obj.emplace("uuid", local.uuid());
	}

	inline http_request_header tag_invoke(const aquarius::json::value_to_tag<http_request_header>&, const aquarius::json::value& jv)
	{
		http_request_header result{};
		auto obj = jv.try_as_object();
		if (obj->empty())
			return {};
		result.uuid(static_cast<uint64_t>(obj->at("uuid").as_int64()));

		return result;
	}

	inline void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const http_response_header& local)
	{
		auto& jv_obj = jv.emplace_object();
		jv_obj.emplace("uuid", local.uuid());
		jv_obj.emplace("result", local.result());
	}

	inline http_response_header tag_invoke(const aquarius::json::value_to_tag<http_response_header>&, const aquarius::json::value& jv)
	{
		http_response_header result{};
		auto obj = jv.try_as_object();
		if (obj->empty())
			return {};

		result.uuid(static_cast<uint64_t>(obj->at("uuid").as_int64()));
		result.result(static_cast<uint64_t>(obj->at("result").as_int64()));

		return result;
	}

} // namespace aquarius