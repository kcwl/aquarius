#pragma once
#include <aquarius/defines.hpp>
#include <aquarius/flex_buffer.hpp>
#include <cstdint>

namespace aquarius
{
	struct tcp_header
	{
		uint32_t magic_;
		int32_t size_;
		uint32_t uid_;
		uint32_t append_;

		void set_size(std::size_t sz)
		{
			size_ = static_cast<uint32_t>(sz);
		}

		void clone(const tcp_header& header)
		{
			uid_ = header.uid_;
			append_ = header.append_;
		}

	private:
		friend class elastic::access;

		template <typename _Archive>
		void serialize(_Archive& ar, [[maybe_unused]] uint32_t file_version = 0)
		{
			ar& magic_;
			ar& size_;
			ar& uid_;
			ar& append_;
		}
	};

	struct tcp_request_header : tcp_header
	{
		uint32_t reserve_;

	private:
		friend class elastic::access;

		template <typename _Archive>
		void serialize(_Archive& ar, [[maybe_unused]] uint32_t file_version = 0)
		{
			ar& elastic::base_object<tcp_header>(*this);
			ar& reserve_;
		}

	public:
		read_handle_result parse_bytes(flex_buffer_t& stream)
		{
			elastic::from_binary(*this, stream);

			return read_handle_result::ok;
		}

		read_handle_result to_bytes(flex_buffer_t& stream)
		{
			elastic::to_binary(*this, stream);

			return read_handle_result::ok;
		}
	};

	struct tcp_response_header : tcp_header
	{
		int32_t result_;

	private:
		friend class elastic::access;

		template <typename _Archive>
		void serialize(_Archive& ar, [[maybe_unused]] uint32_t file_version = 0)
		{
			ar& elastic::base_object<tcp_header>(*this);
			ar& result_;
		}

	public:
		read_handle_result parse_bytes(flex_buffer_t& stream)
		{
			elastic::from_binary(*this, stream);

			return read_handle_result::ok;
		}

		read_handle_result to_bytes(flex_buffer_t& stream)
		{
			elastic::to_binary(*this, stream);

			return read_handle_result::ok;
		}
	};
} // namespace aquarius