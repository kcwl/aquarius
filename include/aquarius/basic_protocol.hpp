#pragma once
#include <boost/core/empty_value.hpp>
#include <iostream>
#include <aquarius/detail/string_literal.hpp>

namespace aquarius
{
	template <detail::string_literal Router, typename Header, typename Body, typename Allocator>
	class basic_protocol : public boost::empty_value<Body>
	{
		static_assert(std::is_pointer_v<Body>, "body must be a regular pointer");

	public:
		using header_t = Header;

		using body_t = std::remove_pointer_t<Body>;

		using base_body = boost::empty_value<Body>;

		constexpr static auto router = detail::bind_param<Router>::value;

	public:
		basic_protocol()
			: basic_protocol(Allocator())
		{}

		basic_protocol(const Allocator& alloc)
			: header_ptr_()
			, alloc_(alloc)
		{
			this->get() = alloc_.allocate(1);

			::new (static_cast<void*>(this->get())) body_t();
		}

		basic_protocol(const basic_protocol&) = default;

		basic_protocol& operator=(const basic_protocol&) = default;

		basic_protocol(basic_protocol&& other) noexcept
			: base_body(boost::empty_init, std::exchange(other.get(), nullptr))
			, header_ptr_(std::exchange(other.header_ptr_, {}))
		{}

		basic_protocol& operator=(basic_protocol&& other) noexcept
		{
			if (this != std::addressof(other))
			{
				header_ptr_ = std::move(other.header_ptr_);
				this->get() = std::move(other.get());
				other.get() = nullptr;
				alloc_ = std::move(other.alloc_);
			}

			return *this;
		}

		virtual ~basic_protocol()
		{
			if (this->get())
			{
				alloc_.deallocate(this->get(), 1);
			}
		}

	public:
		bool operator==(const basic_protocol& other) const
		{
			return header() == other.header() && body() == other.body();
		}

		std::ostream& operator<<(std::ostream& os) const
		{
			os << header_ptr_ << body();

			return os;
		}

	public:
		header_t& header()
		{
			return header_ptr_;
		}

		const header_t& header() const
		{
			return header_ptr_;
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
		header_t header_ptr_;

		Allocator alloc_;
	};
} // namespace aquarius