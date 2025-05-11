#pragma once
#include <aquarius/detail/flex_buffer.hpp>
#include <map>
#include <list>

namespace aquarius
{
	class package_base
	{
	public:
		enum class mvcc
		{
			header = 0x0,
			middle = 0x1,
			complete = 0x2,
			normal = middle | complete
		};

		struct sequence
		{
			struct block
			{
				std::size_t seq;

				flex_buffer_t buffer;
			};

			std::size_t total;

			bool has_complete;

			std::list<block> buffers;

			bool check_complete()
			{
				return total == buffers.size();
			}
		};

	public:
		package_base() = default;

	public:
		template<typename T>
		T from_binary(flex_buffer_t& buffer)
		{
			return T{};
		}

		template<typename T>
		bool to_binary(T value, flex_buffer_t& buffer)
		{
			return true;
		}

	protected:
		std::map<std::size_t, sequence> buffers_;
	};
}