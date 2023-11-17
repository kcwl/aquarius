#pragma once
#include <aquarius/defines.hpp>
#include <aquarius/flex_buffer.hpp>
#include <cstdint>

namespace aquarius
{
	constexpr static int32_t unknown_req_number = 0;
	constexpr static int32_t unknown_resp_number = 1;
	constexpr static int32_t message_base_number = 1000;

	class command_header
	{
	public:
		void clone(const command_header& header)
		{
			total_seq_ = header.total_seq_;
			sequences_ = header.sequences_;
			reserve_ = header.reserve_;
		}

	public:
		int32_t total_seq_;
		int32_t sequences_;
		uint32_t reserve_;
	};

	class request_header : public command_header
	{
	public:
		read_handle_result parse_bytes(flex_buffer_t& stream)
		{
			//elastic::from_binary(*this, stream);

			return read_handle_result::ok;
		}

		read_handle_result to_bytes(flex_buffer_t& stream)
		{
			//elastic::to_binary(*this, stream);

			return read_handle_result::ok;
		}

	//private:
	//	friend class elastic::access;

	//	template <typename _Archive>
	//	void serialize(_Archive& ar)
	//	{
	//		ar& elastic::base_object<command_header>(*this);
	//		ar& uid_;
	//		ar& session_id_;
	//	}

	public:
		uint32_t uid_;
		uint32_t session_id_;
	};

	class response_header : public command_header
	{
	public:
		read_handle_result parse_bytes(flex_buffer_t& stream)
		{
			//elastic::from_binary(*this, stream);

			return read_handle_result::ok;
		}

		read_handle_result to_bytes(flex_buffer_t& stream)
		{
			//elastic::to_binary(*this, stream);

			return read_handle_result::ok;
		}

	//private:
	//	friend class elastic::access;

	//	template <typename _Archive>
	//	void serialize(_Archive& ar)
	//	{
	//		ar& elastic::base_object<command_header>(*this);
	//		ar& result_;
	//	}

	public:
		int32_t result_;
	};
} // namespace aquarius