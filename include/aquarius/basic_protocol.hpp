#pragma once
#include <aquarius/detail/flex_buffer.hpp>
#include <aquarius/detail/string_literal.hpp>
#include <aquarius/serialize/binary.hpp>
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

	public:
		basic_protocol()
			: basic_protocol(Allocator())
		{}
		basic_protocol(const Allocator& alloc)
			: base_body()
			, header_()
			, alloc_(alloc)
			, method_(0)
		{
			this->get() = alloc_.allocate(1);
			::new (static_cast<void*>(this->get())) body_t();
		}

		basic_protocol(const basic_protocol& other) = delete;

		basic_protocol& operator=(const basic_protocol& other) = delete;

		basic_protocol(basic_protocol&& other) noexcept
			: header_(std::exchange(other.header_, {}))
			, alloc_(std::move(other.alloc_))
			, method_(std::exchange(other.method_, 0))
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
				method_ = std::exchange(other.method_, 0);
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
		virtual error_code commit(flex_buffer&)
		{
			return error_code{};
		}

		virtual bool consume(flex_buffer& buffer)
		{
			if (this->header().deserialize(buffer))
			{
				return false;
			}

			this->body().deserialize(buffer);

			return true;
		}

		void method(int m)
		{
			method_ = m;
		}

		int method() const
		{
			return method_;
		}

		int& method()
		{
			return method_;
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

	private:
		header_t header_;

		Allocator alloc_;

		int method_;
	};

	struct null_header
	{
		bool serialize(flex_buffer&)
		{
			return true;
		}
		bool deserialize(flex_buffer&)
		{
			return true;
		}
		std::size_t content_length()
		{
			return 0;
		}
		std::size_t sequence()
		{
			return 0;
		}
		void sequence(uint32_t)
		{}
	};

	struct null_body
	{
		bool serialize(flex_buffer&)
		{
			return true;
		}
		bool deserialize(flex_buffer&)
		{
			return true;
		}
	};
} // namespace aquarius