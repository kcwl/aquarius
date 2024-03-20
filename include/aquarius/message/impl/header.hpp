#pragma once
#include <aquarius/elastic.hpp>

namespace aquarius
{
	namespace impl
	{
		class basic_header
		{
		public:
			basic_header()
				: length_()
			{}

			virtual ~basic_header() = default;

		public:
			void add_length(std::size_t length)
			{
				length_ += length;
			}

			void swap(basic_header& other)
			{
				std::swap(this->length_, other.length_);
			}

		protected:
			error_code from_binary(flex_buffer_t& stream, error_code& ec)
			{
				if (!elastic::from_binary(length_, stream))
					return ec = system_errc::invalid_stream;

				if (length_ > stream.size())
					return ec = system_errc::wait_for_query;

				return ec = error_code{};;
			}

			error_code to_binary(flex_buffer_t& stream, error_code& ec)
			{
				if (!elastic::to_binary(length_, stream))
					return ec = system_errc::invalid_stream;

				return {};
			}

		private:
			std::size_t length_;
		};
	} // namespace impl
} // namespace aquarius