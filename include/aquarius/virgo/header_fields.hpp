#pragma once
#include <iostream>
#include <map>
#include <string>

namespace aquarius
{
	namespace virgo
	{
		class header_fields
		{
		public:
			header_fields() = default;

			virtual ~header_fields() = default;

		public:
			header_fields(const header_fields& other)
				: fields_(other.fields_)
			{}

			header_fields& operator=(const header_fields& other)
			{
				if (this != std::addressof(other))
				{
					fields_ = other.fields_;
				}

				return *this;
			}

			header_fields(header_fields&& other) noexcept
				: fields_(std::move(other.fields_))
			{}

			header_fields& operator=(header_fields&& other) noexcept
			{
				if (this != std::addressof(other))
				{
					fields_ = std::move(other.fields_);
				}

				return *this;
			}

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
			std::string find(const std::string& f) const
			{
				auto iter = fields_.find(f);

				if (iter == fields_.end())
					return {};

				return iter->second;
			}

			void set_field(const std::string& f, const std::string& v)
			{
				fields_[f] = v;
			}

			std::map<std::string, std::string> fields() const
			{
				return fields_;
			}

		protected:
			std::map<std::string, std::string> fields_;
		};
	} // namespace virgo
} // namespace aquarius