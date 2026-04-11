#pragma once
#include <aquarius/basic_protocol.hpp>

namespace aquarius
{
	namespace virgo
	{
		template <bool Request, typename Header, typename Body, typename Allocator = std::allocator<Body>>
		class basic_tcp_protocol : public basic_protocol<Header, std::add_pointer_t<Body>, Allocator>
		{
		public:
			using base = basic_protocol<Header, std::add_pointer_t<Body>, Allocator>;

			using typename base::header_t;

			using typename base::body_t;

			using typename base::version_t;

			constexpr static auto has_request = Request;

		public:
			basic_tcp_protocol() = default;

			virtual ~basic_tcp_protocol() = default;

			basic_tcp_protocol(const basic_tcp_protocol& other) = delete;

			basic_tcp_protocol& operator=(const basic_tcp_protocol& other) = delete;

			basic_tcp_protocol(basic_tcp_protocol&& other) noexcept = default;

			basic_tcp_protocol& operator=(basic_tcp_protocol&& other) noexcept = default;

		public:
			virtual error_code commit(flex_buffer& buffer) override
			{
				buffer.commit(sizeof(uint32_t));

				commit_command_header(buffer);

				binary_parse().to_datas(this->version(), buffer);

				this->header().serialize(buffer);

				this->body().serialize(buffer);

				auto size = buffer.size() - sizeof(uint32_t);

				buffer.pubseekpos(0, std::ios::in);

				buffer.sputn((char*)&size, sizeof(uint32_t));

				buffer.pubseekoff(size, std::ios::cur, std::ios::in);

				return error_code{};
			}

			virtual bool consume(flex_buffer& buffer, int = 0)
			{
				this->version() = binary_parse().from_datas<version_t>(buffer);

				if (this->header().deserialize(buffer))
				{
					return false;
				}

				this->body().deserialize(buffer);

				return true;
			}

		protected:
			virtual void commit_command_header(flex_buffer&)
			{
				return;
			}
		};

		template <typename Header, typename Body, typename Allocator>
		class basic_tcp_protocol<false, Header, Body, Allocator>
			: public basic_protocol<Header, std::add_pointer_t<Body>, Allocator>
		{
		public:
			using base = basic_protocol<Header, std::add_pointer_t<Body>, Allocator>;

			using typename base::header_t;

			using typename base::body_t;

			using version_t = int32_t;

			using result_t = int32_t;

			constexpr static auto has_request = false;

			//   using length_offset_t = uint16_t;

		public:
			basic_tcp_protocol()
				: base()
				, version_(0)
				, result_()
			{}

			virtual ~basic_tcp_protocol() = default;

			basic_tcp_protocol(const basic_tcp_protocol& other) = delete;

			basic_tcp_protocol& operator=(const basic_tcp_protocol& other) = delete;

			basic_tcp_protocol(basic_tcp_protocol&& other) noexcept
				: base(std::move(other))
				, version_(std::exchange(other.version_, 0))
				, result_(std::exchange(other.result_, 0))
			{}

			basic_tcp_protocol& operator=(basic_tcp_protocol&& other) noexcept
			{
				if (this != std::addressof(other))
				{
					base::operator=(std::move(other));
					version_ = std::exchange(other.version_, 0);
					result_ = std::exchange(other.result_, 0);
				}

				return *this;
			}

		public:
			void version(version_t v)
			{
				version_ = v;
			}

			version_t version() const
			{
				return version_;
			}

			version_t& version()
			{
				return version_;
			}

			void result(result_t v)
			{
				result_ = v;
			}

			result_t result() const
			{
				return result_;
			}

			result_t& result()
			{
				return result_;
			}

		public:
			virtual error_code commit(flex_buffer& buffer) override
			{
				buffer.commit(sizeof(uint32_t));

				commit_command_header(buffer);

				this->header().serialize(buffer);

				this->body().serialize(buffer);

				auto size = buffer.size() - sizeof(uint32_t);

				buffer.pubseekpos(0, std::ios::in);

				buffer.sputn((char*)&size, sizeof(uint32_t));

				buffer.pubseekoff(size, std::ios::cur, std::ios::in);

				return error_code{};
			}

			virtual bool consume(flex_buffer& buffer, int = 0) override
			{
				this->version() = binary_parse().from_datas<version_t>(buffer);

				this->result() = binary_parse().from_datas<result_t>(buffer);

				if (this->header().deserialize(buffer))
				{
					return false;
				}

				this->body().deserialize(buffer);

				return true;
			}

		protected:
			virtual void commit_command_header(flex_buffer&) = 0;

		private:
			version_t version_;

			result_t result_;
		};
	} // namespace virgo
} // namespace aquarius