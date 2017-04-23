#ifndef __sane_complex_h__
#define __sane_complex_h__

#include <cmath>
#include <string>

	// comp is an int64_t but 0x8000_0000_0000_0000 is NaN
	//typedef int64_t complex;

namespace SANE {

	struct complex {

	public:
		const uint64_t NaN = 0x8000000000000000;

		complex(const complex &rhs) = default;

		explicit complex(uint64_t rhs) : _data(rhs)
		{}

		template <class T>
		explicit complex(T t)
		{
			*this = t;
		}

		bool isnan() const
		{
			return _data == NaN;
		}

		complex &operator=(const complex &rhs) = default;

		complex &operator=(uint64_t rhs)
		{
			_data = rhs;
			return *this;
		}

		template <class T>
		complex &operator=(T ld)
		{
			using std::fpclassify;
			using std::signbit;

			switch(fpclassify(ld))
			{
				case FP_NAN:
					_data = NaN;
					break;
				case FP_INFINITE:
					if (signbit(ld))
					{
						_data = -INT64_MAX;
					}
					else
					{
						_data = INT64_MAX;
					}
					break;
				default:
					_data = ld;
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



	std::string to_string(const complex &c)
	{
		if (c.isnan()) return std::string("nan");

		return std::to_string((int64_t)c);
	}

	inline int fpclassify(const complex &c) {
		if (c.isnan()) return FP_NAN;
		if ((uint64_t)c == (uint64_t)0) return FP_ZERO;
		return FP_NORMAL;
	}

	inline int signbit(const complex &c) {
		if (c.isnan()) return 0;
		return ((int64_t)c < (int64_t)0) ? 1 : 0;
	}

	inline int isnan(const complex &c) {
		return c.isnan();
	}

	inline int isinf(const complex &c) {
		return false;
	}

	inline int isfinite(complex c) {
		if (c.isnan()) return false;
		return true;
	}

	inline int isnormal(const complex &c) {
		if (c.isnan()) return false;
		if ((uint64_t)c == 0) return false;
		return true;
	}

	inline complex abs(const complex &c) {
		using std::abs;
		if (c.isnan()) return c;
		return complex(abs((int64_t)c));
	}
}

#endif
