#pragma once

namespace aquarius
{
	template <typename _Archive>
	class interface_iarchive
	{
	protected:
		interface_iarchive() = default;

	public:
		template <typename _Ty>
		_Archive& operator>>(_Ty& t)
		{
			this->_this()->load_override(t);

			return *this->_this();
		}

		template <typename _Ty>
		_Archive& operator&(_Ty& t)
		{
			return *(this->_this()) >> t;
		}

	private:
		_Archive* _this()
		{
			return static_cast<_Archive*>(this);
		}
	};

	template <typename _Archive>
	class interface_oarchive
	{
	protected:
		interface_oarchive() = default;

	public:
		template <typename _Ty>
		_Archive& operator<<(_Ty&& t)
		{
			this->_this()->save_override(std::forward<_Ty>(t));
			return *this->_this();
		}

		template <typename _Ty>
		_Archive& operator&(_Ty&& t)
		{
			return *this->_this() << std::forward<_Ty>(t);
		}

	private:
		_Archive* _this()
		{
			return static_cast<_Archive*>(this);
		}
	};
} // namespace elastic