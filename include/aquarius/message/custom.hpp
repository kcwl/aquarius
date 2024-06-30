#pragma once
#include <aquarius/core/elastic.hpp>
#include <cstdint>


namespace aquarius
{
	class custom_tcp_request_header
	{
	public:
		custom_tcp_request_header() = default;
		virtual ~custom_tcp_request_header() = default;

		custom_tcp_request_header(custom_tcp_request_header&& other)
			: session_id_(other.session_id_)
		{
			other.session_id_ = 0;
		}

		custom_tcp_request_header(const custom_tcp_request_header&) = default;
		custom_tcp_request_header& operator=(const custom_tcp_request_header&) = default;

	public:
		void swap(custom_tcp_request_header& other)
		{
			std::swap(session_id_, other.session_id_);
		}

		void set_session(uint32_t id) noexcept
		{
			session_id_ = id;
		}

		uint32_t session() const
		{
			return session_id_;
		}

		AQUARIUS_ACCESS(session_id_);

	private:
		uint32_t session_id_;
	};

	class custom_tcp_response_header : public custom_tcp_request_header
	{
	public:
		custom_tcp_response_header() = default;
		~custom_tcp_response_header() = default;

	public:
		custom_tcp_response_header(custom_tcp_response_header&& other)
			: custom_tcp_request_header(std::forward<custom_tcp_response_header>(other))
			, result_(other.result_)
		{
			other.result_ = 0;
		}

		custom_tcp_response_header(const custom_tcp_response_header&) = default;
		custom_tcp_response_header& operator=(const custom_tcp_response_header&) = default;

	public:
		void swap(custom_tcp_response_header& other)
		{
			custom_tcp_request_header::swap(other);

			std::swap(result_, other.result_);
		}

		uint32_t result() noexcept
		{
			return result_;
		}

		void set_result(uint32_t result)
		{
			result_ = result;
		}

		AQUARIUS_ACCESS_IF(custom_tcp_request_header, result_);

	private:
		uint32_t result_;
	};
} // namespace aquarius