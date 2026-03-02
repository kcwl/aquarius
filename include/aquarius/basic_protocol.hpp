#pragma once
#include <aquarius/detail/string_literal.hpp>
#include <aquarius/detail/uuid_generator.hpp>
#include <aquarius/serialize/flex_buffer.hpp>
#include <boost/core/empty_value.hpp>
#include <iostream>

namespace aquarius
{
	template <typename Header, typename Body, typename Allocator>
	class basic_protocol : public boost::empty_value<Body>
	{
		static_assert(std::is_pointer_v<Body>, "body must be a regular pointer");

	public:
		using header_t = Header;
		using body_t = std::remove_pointer_t<Body>;
		using base_body = boost::empty_value<Body>;
		using seq_t = uint32_t;
		using version_t = int;

	public:
		basic_protocol()
			: basic_protocol(Allocator())
		{}
		basic_protocol(const Allocator& alloc)
			: base_body()
			, header_()
			, alloc_(alloc)
			, version_(0)
			, seq_number_(detail::uuid_generator()())
		{
			this->get() = alloc_.allocate(1);
			::new (static_cast<void*>(this->get())) body_t();
		}

		basic_protocol(const basic_protocol& other)
			: base_body(other)
			, header_(other.header_)
			, alloc_(other.alloc_)
			, version_(other.version_)
			, seq_number_(other.seq_number_)
		{}

		basic_protocol& operator=(const basic_protocol& other)
		{
			if (this != std::addressof(other))
			{
				this->get() = other.get();
				header_ = other.header_;
				alloc_ = other.alloc_;
				version_ = other.version_;
				seq_number_ = other.seq_number_;
			}

			return *this;
		}
		basic_protocol(basic_protocol&& other) noexcept
			: header_(std::exchange(other.header_, {}))
			, alloc_(std::move(other.alloc_))
			, version_(std::move(other.version_))
		{
			this->get() = std::exchange(other.get(), nullptr);
		}
		basic_protocol& operator=(basic_protocol&& other) noexcept
		{
			if (this != std::addressof(other))
			{
				header_ = std::move(other.header_);
				this->get() = std::exchange(other.get(), nullptr);
				alloc_ = std::move(other.alloc_);
				version_ = std::move(other.version_);
				seq_number_ = std::move(other.seq_number_);
			}
			return *this;
		}
		virtual ~basic_protocol()
		{
			if (this->get())
			{
				alloc_.deallocate(this->get(), 1);
				this->get() = nullptr;
			}
		}

	public:
		bool operator==(const basic_protocol& other) const
		{
			return header() == other.header() && body() == other.body();
		}

		std::ostream& operator<<(std::ostream& os) const
		{
			os << this->header() << " " << this->body();

			return os;
		}

	public:
		virtual bool commit(flex_buffer& buffer) = 0;

		virtual bool consume_header(flex_buffer& buffer) = 0;

		virtual bool consume_body(flex_buffer& buffer)
		{
			this->body().deserialize(buffer);
			return true;
		}

	public:
		header_t& header()
		{
			return header_;
		}
		const header_t& header() const
		{
			return header_;
		}
		body_t& body()
		{
			return *this->get();
		}
		const body_t& body() const
		{
			return *this->get();
		}

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

		void seq_number(seq_t v)
		{
			seq_number_ = v;
		}

		seq_t seq_number() const
		{
			return seq_number_;
		}

		seq_t& seq_number()
		{
			return seq_number_;
		}

	private:
		header_t header_;

		Allocator alloc_;

		int version_;

		seq_t seq_number_;
	};

	struct null_protocol
	{
		bool commit(flex_buffer&)
		{
			return false;
		}

		bool consume_header(flex_buffer&)
		{
			return false;
		}

		bool consume_body(flex_buffer&)
		{
			return false;
		}

		std::size_t content_length()
		{
			return 0;
		}
	};
} // namespace aquarius