#pragma once
#include <cstdint>
#include <iostream>
#include <aquarius/detail/binary.hpp>

namespace aquarius
{
	namespace virgo
	{
		class custom_request_header
		{
		public:
			uint64_t uuid() const
			{
				return uuid_;
			}
			void uuid(uint64_t id)
			{
				uuid_ = id;
			}

		public:
			std::ostream& operator<<(std::ostream& os) const
			{
				os << uuid_;

				return os;
			}

			template <typename T>
			std::expected<bool, error_code> commit(detail::flex_buffer<T>& buffer)
			{
				parse_.to_datas(uuid_, buffer);

				return true;
			}

			template <typename T>
			std::expected<bool, error_code> consume(detail::flex_buffer<T>& buffer)
			{
				uuid_ = parse_.from_datas<uint64_t>(buffer);

				return true;
			}

		private:
			uint64_t uuid_;

			detail::binary_parse parse_;
		};

		class custom_response_header
		{
		public:
			std::ostream& operator<<(std::ostream& os) const
			{
				os << uuid_ << '\t' << result_;

				return os;
			}

		public:
			uint64_t uuid() const
			{
				return uuid_;
			}
			void uuid(uint64_t id)
			{
				uuid_ = id;
			}
			int32_t result() const
			{
				return result_;
			}
			void result(int32_t r)
			{
				result_ = r;
			}

			template <typename BuffSequence>
			std::expected<bool, error_code> commit(BuffSequence& buffer)
			{
				parse_.to_datas(uuid_, buffer);

				parse_.to_datas(result_, buffer);

				return true;
			}

			template <typename BuffSequence>
			std::expected<bool, error_code> consume(BuffSequence& buffer)
			{
				uuid_ = parse_.from_datas<uint64_t>(buffer);

				result_ = parse_.from_datas<int32_t>(buffer);

				return true;
			}

		private:
			uint64_t uuid_;

			int32_t result_;

			detail::binary_parse parse_;
		};
	} // namespace virgo
} // namespace aquarius