#pragma once
#include <aquarius/core/elastic.hpp>

namespace aquarius
{
	namespace impl
	{
		class basic_header
		{
		public:
			using virtual_base_type = basic_header;

		public:
			basic_header()
				: length_()
			{}

			virtual ~basic_header() = default;

		public:
			basic_header(basic_header&& other) noexcept
				: length_(other.length_)
			{
				other.length_ = 0;
			}

		public:
			void add_length(std::size_t length)
			{
				length_ += length;
			}

			std::size_t length() const
			{
				return length_;
			}

			void swap(basic_header& other)
			{
				std::swap(this->length_, other.length_);
			}

		protected:
			bool from_binary(flex_buffer_t& stream)
			{
				auto cur = stream.size();

				if (!elastic::from_binary(length_, stream))
					return false;

				cur -= stream.size();

				if (length_ > stream.size())
				{
					stream.consume(-(static_cast<int>(cur)));

					return false;
				}

				return true;
			}

			bool to_binary(flex_buffer_t& stream)
			{
				return elastic::to_binary(length_, stream);
			}

		private:
			std::size_t length_;
		};
	} // namespace impl
} // namespace aquarius