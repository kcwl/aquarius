#pragma once
#include <aquarius/detail/flex_buffer.hpp>
#include <aquarius/error_code.hpp>
#include <map>
#include <ranges>
#include <span>
#include <string>
#include <string_view>

using namespace std::string_view_literals;

namespace aquarius
{
	class http_header
	{
		constexpr static auto crlf = "\r\n"sv;
		constexpr static auto seq_number = "Source-Seq"sv;
		constexpr static auto http_content_length = "Content-Length"sv;
		constexpr static auto http_content_type = "Content-Type"sv;
		constexpr static auto http_connect = "Connection"sv;
		constexpr static auto http_connect_alive = "keep-alive"sv;
		constexpr static auto http_connect_close = "close"sv;

		using key_t = std::string;

		using value_t = std::string;

	public:
		http_header()
			: fields_()
		{}

		virtual ~http_header() = default;

		http_header(const http_header& other)
			: fields_(other.fields_)
		{}

		http_header(http_header&& other) noexcept
			: fields_(std::move(other.fields_))
		{}

		http_header& operator=(const http_header& other)
		{
			if (this != std::addressof(other))
			{
				fields_ = other.fields_;
			}

			return *this;
		}

		http_header& operator=(http_header&& other) noexcept
		{
			if (this != std::addressof(other))
			{
				fields_ = std::move(other.fields_);
			}

			return *this;
		}

	public:
		virtual void serialize(flex_buffer& buffer)
		{
			std::string headline{};

			for (auto& s : fields_)
			{
				headline += std::format("{}: {}\r\n", s.first, s.second);
			}

			headline += "\r\n";

			buffer.sputn(headline.data(), headline.size());
		}

		virtual void deserialize(flex_buffer& buffer)
		{
			auto span_buffer = std::string_view((char*)buffer.data().data(), buffer.size());

			auto headers = span_buffer | std::views::split(crlf);

			std::size_t length = 0;

			for (const auto& header : headers)
			{
				auto str = std::string_view(header);

				if (str.empty())
				{
					break;
				}

				length += (str.size() + crlf.size());

				auto pos = str.find(":");

				if (pos == std::string_view::npos)
				{
					throw std::runtime_error("header field syntax failed");
				}

				auto key = str.substr(0, pos);
				auto value = str.substr(pos + 1, str.size() - pos - 1);

				fields_.emplace(key_t(key), value_t(value.substr(1)));
			}

			buffer.consume(length + crlf.size());
		}

	public:
		bool keep_alive() const
		{
			auto value = this->find(key_t(http_connect));

			return value == "keep-alive" ? true : false;
		}

		void keep_alive(bool k)
		{
			this->set_field(key_t(http_connect), value_t(k ? http_connect_alive : http_connect_close));
		}

		std::string content_type() const
		{
			return find(key_t(http_content_type));
		}

		void content_type(const std::string& type)
		{
			return set_field(key_t(http_content_type), type);
		}

		void set_field(const key_t& key, const value_t& v)
		{
			fields_[key] = v;
		}

		uint32_t sequence() const
		{
			return to_integer<uint32_t>(find(key_t(seq_number)));
		}

		void sequence(uint32_t v)
		{
			set_field(key_t(seq_number), std::to_string(v));
		}

		void content_length(uint64_t len)
		{
			set_field(key_t(http_content_length), std::to_string(len));
		}

		uint64_t content_length() const
		{
			auto value = find(key_t(http_content_length));

			if (value.empty())
				return 0;

			return to_integer<uint64_t>(value);
		}

		value_t find(const key_t& key) const
		{
			auto iter = fields_.find(key);

			if (iter == fields_.end())
				return value_t{};

			return iter->second;
		}

	private:
		template <typename T>
		T to_integer(const std::string& value) const
		{
			std::stringstream ss{};
			ss << value;

			T result;
			ss >> result;

			return result;
		}

	private:
		std::unordered_map<key_t, value_t> fields_;
	};

} // namespace aquarius