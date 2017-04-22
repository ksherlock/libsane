#ifndef __sane_fpinfo_h__
#define __sane_fpinfo_h__

#include <cstdint>

namespace SANE {

	struct fpinfo {

		bool sign = false;
		bool one = false;
		int exp = 0;
		uint64_t sig = 0;

		bool nan = false;
		bool inf = false;

		fpinfo(const float &f);
		fpinfo(const double &d);
		fpinfo(const long double &ld);
	};

}

#endif
