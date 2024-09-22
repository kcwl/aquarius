#pragma once

namespace aquarius
{
	namespace detail
	{
		template <typename _Archive>
		class interface_iarchive
		{
		public:
			template <typename _Ty>
			_Archive& operator>>(_Ty&& t)
			{
				this->_this()->load_override(std::forward<_Ty>(t));

				return *this->_this();
			}

			template <typename _Ty>
			_Archive& operator&(_Ty&& t)
			{
				return operator>>(std::forward<_Ty>(t));
			}

		private:
			_Archive* _this() noexcept
			{
				return static_cast<_Archive*>(this);
			}
		};
	} // namespace detail
} // namespace aquarius