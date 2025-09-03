#pragma once
#include <map>
#include <optional>
#include <ranges>
#include <string>
#include <string_view>

using namespace std::string_view_literals;

namespace aquarius
{
	namespace virgo
	{
		class header_fields
		{
		public:
			std::ostream& operator<<(std::ostream& os) const
			{
				for (auto& s : fields_)
				{
					os << s.first << ":" << s.second << "\r\n";
				}

				os << "\r\n";

				return os;
			}

		public:
			std::string_view find(std::string_view f) const
			{
				auto iter = fields_.find(f);

				if (iter == fields_.end())
					return {};

				return iter->second;
			}

			template <typename T>
			void set_field(std::string_view f, T v)
			{
				fields_[f] = v;
			}

			bool has_content_length() const
			{
				return find("Content-Length"sv).empty();
			}

			void content_length(std::optional<uint64_t> len)
			{
				if (!len.has_value())
					return;

				fields_["Content-Length"sv] = std::to_string(*len);
			}

			std::optional<uint64_t> content_length()
			{
				auto iter = fields_.find("Content-Length"sv);
				if (iter == fields_.end())
					return std::nullopt;

				return std::atoi(iter->second.data());
			}

			bool keep_alive()
			{
				auto iter = fields_.find("Connection"sv);
				if (iter == fields_.end())
					return false;

				return iter->second == "keep-alive" ? true : false;
			}

			void keep_alive(bool k)
			{
				fields_["Connection"sv] = k ? "keep-alive" : "close";
			}

		public:
			template <typename BufferSequence>
			void commit(BufferSequence& buffer)
			{}

			std::span<char> consume(std::span<char> buffer)
			{
				auto [header_span, next_span] = prase_header_span(buffer);

				constexpr auto delm = "\r\n"sv;

				for (const auto& line : std::views::split(header_span, delm))
				{
					auto line_view = line | std::views::split(':');

					auto iter = line_view.begin();
					if (iter == line_view.end())
						return {};

					auto key = std::string_view(*iter);

					if (++iter == line_view.end())
						return {};

					fields_[key] = std::string_view(*iter);
				}

				return next_span;
			}

		private:
			auto prase_header_span(std::span<char> buffer) -> std::pair<std::span<char>, std::span<char>>
			{
				constexpr auto delm = "\r\n\r\n"sv;

				constexpr auto delm_size = delm.size();

				auto buf_view = std::views::slide(buffer, delm_size);

				auto iter = std::ranges::find_if(buf_view,
												 [&](const auto& v)
												 {
													 if (v.size() < delm_size)
														 return false;

													 return std::string_view(v) == delm;
												 });

				if (iter == buf_view.end())
					return {};

				auto pos = std::distance(buf_view.begin(), iter);

				auto result_span = buffer.subspan(0, pos);

				auto next_span = buffer.subspan(pos + 2);

				return { result_span, next_span };
			}

		private:
			std::map<std::string_view, std::string_view> fields_;
		};
	} // namespace virgo
} // namespace aquarius

namespace std
{
	inline std::ostream& operator<<(std::ostream& os, const virgo::http::detail::header_fields& other)
	{
		other << os;

		return os;
	}
} // namespace std
