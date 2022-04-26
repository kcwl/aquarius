#pragma once
#include "field.hpp"
#include "header.hpp"
#include "header_value.hpp"

#pragma warning(disable:4100)

namespace aquarius
{
	namespace msg
	{
		template<bool Request, typename Body, std::size_t Number>
		class message 
			: public std::enable_shared_from_this<message<Request, Body, Number>>
			, private header<Request,header_fields>
			, private detail::empty_value<Body>
		{
		public:
			enum { NUMBER = Number };

		public:
			using header_type = header<Request, header_fields>;
			using body_type = Body;

		public:
			message() = default;

			message(message&&) = default;

			message(message const&) = default;

			message& operator=(message&&) = default;

			message& operator=(message const&) = default;

		public:
			typename header_type::value_type& header()noexcept 
			{
				return header_type::get();
			}

			typename header_type::value_type& header() const noexcept
			{
				return header_type::get();
			}

			Body& body() noexcept
			{
				return this->detail::empty_value<body_type>::get();
			}

			Body& body() const noexcept
			{
				return this->detail::empty_value<body_type>::get();
			}

			template<typename Context>
			int accept(std::shared_ptr<Context> ctx_ptr)
			{
				if (ctx_ptr == nullptr)
					return 1;

				return ctx_ptr->visit(this->shared_from_this());
			}
		};
	}
}

template<typename Body, std::size_t Number>
aquarius::ftstream& operator<<(aquarius::ftstream& stream, aquarius::msg::message<true, Body, Number>& req)
{
	auto& header = req.header();

	stream << header;

	auto& body = req.body();

	body.SerializeToArray(stream.data(), static_cast<int>(stream.size()));

	return stream;
}

template<typename Body, std::size_t Number>
aquarius::ftstream& operator<<(aquarius::ftstream& stream, aquarius::msg::message<false, Body, Number>& req)
{
	auto& header = req.header();

	stream << header;

	auto& body = req.body();

	body.SerializeToArray(stream.data(), static_cast<int>(stream.size()));

	return stream;
}

template<typename Body, std::size_t Number>
aquarius::ftstream& operator>>(aquarius::ftstream& stream, aquarius::msg::message<true, Body, Number>& req)
{
	auto& header = req.header();

	stream >> header;

	auto& body = req.body();

	body.ParseFromArray(stream.data(), static_cast<int>(stream.size()));

	return stream;
}

template<typename Body, std::size_t Number>
aquarius::ftstream& operator>>(aquarius::ftstream& stream, aquarius::msg::message<false, Body, Number>& req)
{
	auto& header = req.header();

	stream >> header;

	auto& body = req.body();

	body.ParseFromArray(stream.data(), static_cast<int>(stream.size()));

	return stream;
}