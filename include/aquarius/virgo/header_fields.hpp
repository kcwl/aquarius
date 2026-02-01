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
			using key_t = std::string;

			using value_t = std::string;

			constexpr static auto __seq_number__ = "Seq-Number"sv;
			constexpr static auto __content_length__ = "Content-Length"sv;

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
			bool operator==(const header_fields& other) const
			{
				return fields_ == other.fields_;
			}

		public:
			void set_header_fields(header_fields hf)
			{
				*this = std::move(hf);
			}

			value_t find(const key_t& key) const
			{
				auto iter = fields_.find(key);

				if (iter == fields_.end())
					return value_t{};

				return iter->second;
			}

			void set_field(const key_t& key, const value_t& v)
			{
				fields_[key] = v;
			}

			uint32_t seq_number()
			{
				return to_integer<uint32_t>(find(std::string(__seq_number__)));
			}

			void seq_number(uint32_t v)
			{
				this->set_field(std::string(__seq_number__), std::to_string(v));
			}

			bool has_content_length() const
			{
				return !this->find(std::string(__content_length__)).empty();
			}

			void content_length(uint64_t len)
			{
				this->set_field(std::string(__content_length__), std::to_string(len));
			}

			uint64_t content_length()
			{
				auto value = this->find(std::string(__content_length__));

				if (value.empty())
					return 0;

				return to_integer<uint64_t>(value);
			}

		private:
			template <typename T>
			T to_integer(const std::string& value)
			{
				std::stringstream ss{};
				ss << value;

				T result;
				ss >> result;

				return result;
			}

		protected:
			std::map<std::string, std::string> fields_;
		};
	} // namespace virgo
} // namespace aquarius