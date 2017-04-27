#ifndef __sane_comp_h__
#define __sane_comp_h__

#include <cmath>
#include <string>
#include <cstdlib>

	// comp is an int64_t but 0x8000_0000_0000_0000 is NaN
	//typedef int64_t comp;

namespace SANE {

	struct comp {

	public:
		const uint64_t NaN = 0x8000000000000000;

		comp(const comp &rhs) = default;

		explicit comp(uint64_t rhs) : _data(rhs)
		{}

		template <class T>
		explicit comp(T t)
		{
			*this = t;
		}

		bool isnan() const
		{
			return _data == NaN;
		}

		comp &operator=(const comp &rhs) = default;

		comp &operator=(uint64_t rhs)
		{
			_data = rhs;
			return *this;
		}

		template <class T>
		comp &operator=(T ld)
		{
			switch(std::fpclassify(ld))
			{
				case FP_NAN:
				case FP_INFINITE:
					_data = NaN;
					break;
				default:
					if (ld > INT64_MAX || ld < -INT64_MAX) {
						_data = NaN;
					} else {
						_data = ld;
					}
					break;
			}

			return *this;
		}


		explicit operator uint64_t() const {
			return _data;
		}

		explicit operator int64_t() const {
			return _data;
		}

		explicit operator long double() const {
			if (_data == NaN)
				return NAN;
			return _data;
		}

		explicit operator double() const {
			if (_data == NaN)
				return NAN;
			return _data;
		}

		explicit operator float() const {
			if (_data == NaN)
				return NAN;
			return _data;
		}


	private:
		int64_t _data = 0;

	};



	std::string to_string(const comp &c)
	{
		if (c.isnan()) return std::string("nan");

		return std::to_string((int64_t)c);
	}

	inline int fpclassify(const comp &c) {
		if (c.isnan()) return FP_NAN;
		if ((uint64_t)c == (uint64_t)0) return FP_ZERO;
		return FP_NORMAL;
	}

	inline int signbit(const comp &c) {
		if (c.isnan()) return 0;
		return ((int64_t)c < (int64_t)0) ? 1 : 0;
	}

	inline int isnan(const comp &c) {
		return c.isnan();
	}

	inline int isinf(const comp &c) {
		return false;
	}

	inline int isfinite(comp c) {
		if (c.isnan()) return false;
		return true;
	}

	inline int isnormal(const comp &c) {
		if (c.isnan()) return false;
		if ((uint64_t)c == 0) return false;
		return true;
	}

	inline comp abs(const comp &c) {
		using std::abs;
		if (c.isnan()) return c;
		return comp(std::abs((int64_t)c));
	}
}

#endif
