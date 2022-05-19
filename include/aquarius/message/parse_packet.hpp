#pragma once
#include <cstddef>
#include "../stream.hpp"

namespace aquarius
{
	namespace msg
	{
		template<std::size_t N>
		struct parse{};

		template<>
		struct parse<0>
		{
			template<typename T>
			static bool parse_bytes(std::shared_ptr<T> message_ptr, aquarius::ftstream& archive)
			{
				constexpr auto header_size = sizeof(typename T::header_type);

				if (header_size > archive.active())
					return false;

				typename T::header_type* header = (typename T::header_type*)archive.read_pointer();

				message_ptr->header() = *header;

				archive.consume(header_size);

				if (archive.active() < header->size_)
					return false;

				message_ptr->body().ParseFromArray(archive.read_pointer(), header_size);

				return true;
			}

			template<typename T>
			static bool to_bytes(std::shared_ptr<T> message_ptr, aquarius::ftstream& archive)
			{
				auto header = message_ptr->header();

				archive.write(&header, sizeof(T::header_type));

				int body_size = message_ptr->body().ByteSize();

				message_ptr->body().SerializeToArray(archive.write_pointer(), body_size);

				archive.commit(body_size);

				return true;
			}
		};
	}
}