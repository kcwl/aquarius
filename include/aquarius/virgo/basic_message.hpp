#pragma once
#include <boost/core/empty_value.hpp>
#include <iostream>

namespace aquarius
{
	namespace virgo
	{
		template <typename Body, typename Allocator>
		class basic_message : public boost::empty_value<Body>
		{
			static_assert(std::is_pointer_v<Body>, "body must be a regular pointer");

		public:
			using body_type = std::remove_pointer_t<Body>;

			using base_body = boost::empty_value<Body>;

			using base_type = basic_message;

		public:
			basic_message()
				: alloc_()
			{
				this->get() = alloc_.allocate(1);

				::new (static_cast<void*>(this->get())) body_type();
			}

			basic_message(const Allocator& alloc)
				: alloc_(alloc)
			{}

			basic_message(const basic_message&) = default;

			basic_message& operator=(const basic_message&) = default;

			basic_message(basic_message&& other) noexcept
				: base_body(boost::empty_init, std::exchange(other.get(), nullptr))
			{}

			basic_message& operator=(basic_message&& other) noexcept
			{
				if (this != std::addressof(other))
				{
					this->get() = std::move(other.get());
					other.get() = nullptr;
					alloc_ = std::move(other.alloc_);
				}

				return *this;
			}

			virtual ~basic_message()
			{
				if (this->get())
				{
					alloc_.deallocate(this->get(), 1);
				}
			}

		public:
			bool operator==(const basic_message& other) const
			{
				return body() == other.body();
			}

			std::ostream& operator<<(std::ostream& os) const
			{
				os << body();

				return os;
			}

		public:
			body_type& body()
			{
				return *this->get();
			}

			const body_type& body() const
			{
				return *this->get();
			}

		private:
			Allocator alloc_;
		};
	} // namespace virgo
} // namespace aquarius