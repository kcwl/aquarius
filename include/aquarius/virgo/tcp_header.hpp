#pragma once
#include <cstdint>
#include <iostream>
#include <string_view>
#include <aquarius/detail/binary.hpp>

namespace aquarius
{
	namespace virgo
	{
		template<bool Request>
		class tcp_header;

		template<>
		class tcp_header<true>
		{
			struct implement
			{
				int32_t version_;
				int32_t transfer_;
				uint32_t rpc_;
				uint64_t length_;
				uint32_t crc_;
				int64_t timestamp_;
			};

		public:
			constexpr static auto impl_size = sizeof(implement);

		public:
			tcp_header() = default;

			tcp_header(const tcp_header&) = default;

		public:
			std::ostream& operator<<(std::ostream& os) const
			{
				os << "version: " << impl_.version_ << " transfer: " << impl_.transfer_ << " rpc: " << impl_.rpc_
				   << " length: " << impl_.length_ << " crc: " << impl_.crc_ << " timestamp: " << impl_.timestamp_;
				return os;
			}

		public:
			int32_t version() const
			{
				return impl_.version_;
			}

			void version(int32_t version)
			{
				impl_.version_ = version;
			}

			int32_t transfer() const
			{
				return impl_.transfer_;
			}

			void transfer(int32_t transfer)
			{
				impl_.transfer_ = transfer;
			}

			uint32_t rpc() const
			{
				return impl_.rpc_;
			}

			void rpc(uint32_t rpc_id)
			{
				impl_.rpc_ = rpc_id;
			}

			uint64_t length() const
			{
				return impl_.length_;
			}

			void length(uint64_t len)
			{
				impl_.length_ = len;
			}
			uint32_t crc() const
			{
				return impl_.crc_;
			}

			void crc(uint32_t c)
			{
				impl_.crc_ = c;
			}

			int64_t timestamp() const
			{
				return impl_.timestamp_;
			}

			void timestamp(int64_t timestamp)
			{
				impl_.timestamp_ = timestamp;
			}

			template<typename T>
			void mark(detail::flex_buffer<T>& buffer)
			{
				buffer.commit(sizeof(implement));
			}

			template <typename T>
			std::expected<bool, error_code> commit(detail::flex_buffer<T>& buffer)
			{
				parse_.to_datas(impl_.version_, buffer);
				parse_.to_datas(impl_.transfer_, buffer);
				parse_.to_datas(impl_.rpc_, buffer);
				parse_.to_datas(impl_.length_, buffer);
				parse_.to_datas(impl_.crc_, buffer);
				parse_.to_datas(impl_.timestamp_, buffer);

				return true;
			}

			template <typename T>
			std::expected<bool, error_code> consume(detail::flex_buffer<T>& buffer)
			{
				impl_.version_ = parse_.from_datas<int32_t>(buffer);
				impl_.transfer_ = parse_.from_datas<int32_t>(buffer);
				impl_.rpc_ = parse_.from_datas<uint32_t>(buffer);
				impl_.length_ = parse_.from_datas<uint64_t>(buffer);
				impl_.crc_ = parse_.from_datas<uint32_t>(buffer);
				impl_.timestamp_ = parse_.from_datas<int64_t>(buffer);

				return true;
			}

		private:
			implement impl_;

			detail::binary_parse parse_;
		};

		template<>
		class tcp_header<false>
		{
			struct implement
			{
				int32_t result_;
				int32_t version_;
				int32_t transfer_;
				uint32_t rpc_;
				uint64_t length_;
				uint32_t crc_;
				int64_t timestamp_;
			};

		public:
			constexpr static std::size_t impl_size = sizeof(implement);

		public:
			tcp_header() = default;

			tcp_header(const tcp_header&) = default;

		public:
			std::ostream& operator<<(std::ostream& os) const
			{
				os << "version: " << impl_.version_ << " transfer: " << impl_.transfer_ << " rpc: " << impl_.rpc_
					<< " length: " << impl_.length_ << " crc: " << impl_.crc_ << " timestamp: " << impl_.timestamp_;
				return os;
			}

			template <typename BufferSequence>
			void mark(BufferSequence& buffer)
			{
				buffer.resize(impl_size);
			}

		public:
			int32_t result() const
			{
				return impl_.result_;
			}

			void result(int32_t result)
			{
				impl_.result_ = result;
			}

			int32_t version() const
			{
				return impl_.version_;
			}

			void version(int32_t version)
			{
				impl_.version_ = version;
			}

			int32_t transfer() const
			{
				return impl_.transfer_;
			}

			void transfer(int32_t transfer)
			{
				impl_.transfer_ = transfer;
			}

			uint32_t rpc() const
			{
				return impl_.rpc_;
			}

			void rpc(uint32_t rpc_id)
			{
				impl_.rpc_ = rpc_id;
			}

			uint64_t length() const
			{
				return impl_.length_;
			}

			void length(uint64_t len)
			{
				impl_.length_ = len;
			}
			uint32_t crc() const
			{
				return impl_.crc_;
			}

			void crc(uint32_t c)
			{
				impl_.crc_ = c;
			}

			int64_t timestamp() const
			{
				return impl_.timestamp_;
			}

			void timestamp(int64_t timestamp)
			{
				impl_.timestamp_ = timestamp;
			}

			template<typename T>
			void mark(detail::flex_buffer<T>& buffer)
			{
				buffer.commit(sizeof(implement));
			}

			template <typename T>
			std::expected<bool,error_code> commit(detail::flex_buffer<T>& buffer)
			{
				parse_.to_datas(impl_.result_, buffer);
				parse_.to_datas(impl_.version_, buffer);
				parse_.to_datas(impl_.transfer_, buffer);
				parse_.to_datas(impl_.rpc_, buffer);
				parse_.to_datas(impl_.length_, buffer);
				parse_.to_datas(impl_.crc_, buffer);
				parse_.to_datas(impl_.timestamp_, buffer);

				return true;
			}

			template <typename T>
			std::expected<bool, error_code> consume(detail::flex_buffer<T>& buffer)
			{
				impl_.result_ = parse_.from_datas<int32_t>(buffer);
				impl_.version_ = parse_.from_datas<int32_t>(buffer);
				impl_.transfer_ = parse_.from_datas<int32_t>(buffer);
				impl_.rpc_ = parse_.from_datas<uint32_t>(buffer);
				impl_.length_ = parse_.from_datas<uint64_t>(buffer);
				impl_.crc_ = parse_.from_datas<uint32_t>(buffer);
				impl_.timestamp_ = parse_.from_datas<int64_t>(buffer);

				return true;
			}

		private:
			implement impl_;

			detail::binary_parse parse_;
		};
	} // namespace virgo
} // namespace aquarius
