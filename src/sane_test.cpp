#define CATCH_CONFIG_MAIN

#include "catch.hpp"

#include <sane/sane.h>
#include <sane/floating_point.h>
#include <sane/comp.h>

#include <cmath>

using std::abs;
using std::fpclassify;
using std::isinf;
using std::isnan;
using std::signbit;

using namespace SANE;
namespace fp = SANE::floating_point;

#define DOUBLE_C(v) (v)
#define FLOAT_C(v) (v ## f)
#define LONG_DOUBLE_C(v) (v ## l)


TEST_CASE( "Dec2Str(Float)", "[dec2str]") {

	// page 34, table 3-6


	std::string s;

	SECTION( " 1.23e+0" ) {
		SANE::decimal d{ 0, -2, "123" };
		SANE::decform df{ SANE::decform::FLOATDECIMAL, 3};

		dec2str(df, d, s);
		REQUIRE(s == " 1.23e+0");
	}

	SECTION( "-1.23e-2" ) {
		SANE::decimal d{ 1, -4, "123" };
		SANE::decform df{ SANE::decform::FLOATDECIMAL, 3};

		dec2str(df, d, s);
		REQUIRE(s == "-1.23e-2");
	}

	SECTION( " 1.23e+202" ) {
		SANE::decimal d{ 0, 200, "123" };
		SANE::decform df{ SANE::decform::FLOATDECIMAL, 1};

		dec2str(df, d, s);
		REQUIRE(s == " 1.23e+202");
	}

	SECTION( "-1.2300e+1002" ) {
		SANE::decimal d{ 1, 1000, "123" };
		SANE::decform df{ SANE::decform::FLOATDECIMAL, 5};

		dec2str(df, d, s);
		REQUIRE(s == "-1.2300e+1002");
	}

	SECTION( " 4e-30" ) {
		SANE::decimal d{ 0, -30, "4" };
		SANE::decform df{ SANE::decform::FLOATDECIMAL, 1};

		dec2str(df, d, s);
		REQUIRE(s == " 4e-30");
	}

	SECTION( "-0e+0" ) {
		SANE::decimal d{ 1, 0, "0" };
		SANE::decform df{ SANE::decform::FLOATDECIMAL, 1};

		dec2str(df, d, s);
		REQUIRE(s == "-0e+0");
	}

	SECTION( "-1.00000000000000000000000000000e+0" ) {
		SANE::decimal d{ 0, 0, "1" };
		SANE::decform df{ SANE::decform::FLOATDECIMAL, 30};

		dec2str(df, d, s);
		REQUIRE(s == " 1.00000000000000000000000000000e+0");
	}

	SECTION( "?" ) {
		SANE::decimal d{ 0, 0, "1" };
		SANE::decform df{ SANE::decform::FLOATDECIMAL, 76};

		dec2str(df, d, s);
		REQUIRE(s == "?");
	}

	SECTION( "?" ) {
		SANE::decimal d{ 1, 0, "1" };
		SANE::decform df{ SANE::decform::FLOATDECIMAL, 76};

		dec2str(df, d, s);
		REQUIRE(s == "?");
	}

	SECTION( " NAN(036)" ) {
		SANE::decimal d{ 0, -98, "N0024" };
		SANE::decform df{ SANE::decform::FLOATDECIMAL, 5};

		dec2str(df, d, s);
		REQUIRE(s == " NAN(036)");
	}

	SECTION( "-NAN(021)" ) {
		SANE::decimal d{ 1, 103, "N0015" };
		SANE::decform df{ SANE::decform::FLOATDECIMAL, 2};

		dec2str(df, d, s);
		REQUIRE(s == "-NAN(021)");
	}

	SECTION( " INF" ) {
		SANE::decimal d{ 0, 0, "I" };
		SANE::decform df{ SANE::decform::FLOATDECIMAL, 2};

		dec2str(df, d, s);
		REQUIRE(s == " INF");
	}

	SECTION( "-INF" ) {
		SANE::decimal d{ 1, -217, "I" };
		SANE::decform df{ SANE::decform::FLOATDECIMAL, 2};

		dec2str(df, d, s);
		REQUIRE(s == "-INF");
	}

}

TEST_CASE( "Dec2Str(Fixed)", "[dec2str]") {

	// page 34, table 3-6

	std::string s;

	SECTION( "12.345" ) {
		SANE::decimal d{ 0, -3, "12345" };
		SANE::decform df{ SANE::decform::FIXEDDECIMAL, 3};

		dec2str(df, d, s);
		REQUIRE(s == "12.345");
	}

	SECTION( "-12.345" ) {
		SANE::decimal d{ 1, -3, "12345" };
		SANE::decform df{ SANE::decform::FIXEDDECIMAL, 3};

		dec2str(df, d, s);
		REQUIRE(s == "-12.345");
	}

	SECTION( "12.34500" ) {
		SANE::decimal d{ 0, -3, "12345" };
		SANE::decform df{ SANE::decform::FIXEDDECIMAL, 5};

		dec2str(df, d, s);
		REQUIRE(s == "12.34500");
	}


	SECTION( "-12.34567" ) {
		SANE::decimal d{ 1, -5, "1234567" };
		SANE::decform df{ SANE::decform::FIXEDDECIMAL, 5};

		dec2str(df, d, s);
		REQUIRE(s == "-12.34567");
	}

	SECTION( "12345" ) {
		SANE::decimal d{ 0, 0, "12345" };
		SANE::decform df{ SANE::decform::FIXEDDECIMAL, 0};

		dec2str(df, d, s);
		REQUIRE(s == "12345");
	}

	SECTION( "-12345000" ) {
		SANE::decimal d{ 1, 3, "12345" };
		SANE::decform df{ SANE::decform::FIXEDDECIMAL, 0};

		dec2str(df, d, s);
		REQUIRE(s == "-12345000");
	}

	SECTION( "1234500" ) {
		SANE::decimal d{ 0, 2, "12345" };
		SANE::decform df{ SANE::decform::FIXEDDECIMAL, -2};

		dec2str(df, d, s);
		REQUIRE(s == "1234500");
	}

	SECTION( "-123450" ) {
		SANE::decimal d{ 1, 1, "12345" };
		SANE::decform df{ SANE::decform::FIXEDDECIMAL, -2};

		dec2str(df, d, s);
		REQUIRE(s == "-123450");
	}

	SECTION( "-0.000" ) {
		SANE::decimal d{ 1, 0, "0" };
		SANE::decform df{ SANE::decform::FIXEDDECIMAL, 3};

		dec2str(df, d, s);
		REQUIRE(s == "-0.000");
	}

	SECTION( "-0" ) {
		SANE::decimal d{ 1, 0, "0" };
		SANE::decform df{ SANE::decform::FIXEDDECIMAL, -3};

		dec2str(df, d, s);
		REQUIRE(s == "-0");
	}

	SECTION( "?" ) {
		SANE::decimal d{ 0, 74, "1" };
		SANE::decform df{ SANE::decform::FIXEDDECIMAL, 5};

		dec2str(df, d, s);
		REQUIRE(s == "?");
	}

	SECTION( "?" ) {
		SANE::decimal d{ 1, 74, "1" };
		SANE::decform df{ SANE::decform::FIXEDDECIMAL, 4};

		dec2str(df, d, s);
		REQUIRE(s == "?");
	}

	SECTION( "NAN(036)" ) {
		SANE::decimal d{ 0, -98, "N0024" };
		SANE::decform df{ SANE::decform::FIXEDDECIMAL, 5};

		dec2str(df, d, s);
		REQUIRE(s == "NAN(036)");
	}

	SECTION( "-NAN(021)" ) {
		SANE::decimal d{ 1, 103, "N0015" };
		SANE::decform df{ SANE::decform::FIXEDDECIMAL, 2};

		dec2str(df, d, s);
		REQUIRE(s == "-NAN(021)");
	}

	SECTION( "INF)" ) {
		SANE::decimal d{ 0, 0, "I" };
		SANE::decform df{ SANE::decform::FIXEDDECIMAL, 2};

		dec2str(df, d, s);
		REQUIRE(s == "INF");
	}

	SECTION( "-INF)" ) {
		SANE::decimal d{ 1, -217, "I" };
		SANE::decform df{ SANE::decform::FIXEDDECIMAL, 2};

		dec2str(df, d, s);
		REQUIRE(s == "-INF");
	}

}

TEST_CASE( "Str2Dec", "[str2dec]" ) {

	// page 30, table 3-3

	uint16_t index;
	uint16_t valid;
	SANE::decimal d;

	SECTION( "Str2Dec('12')" ) {
		index = 0;
		SANE::str2dec("12", index, d, valid);
		REQUIRE(index == 2);
		REQUIRE(valid);

		REQUIRE(d.sig == "12");
		REQUIRE(d.sgn == 0);
		REQUIRE(d.exp == 0);
	}

	SECTION( "Str2Dec('12E')" ) {
		index = 0;
		SANE::str2dec("12E", index, d, valid);
		REQUIRE(index == 2);
		REQUIRE(valid);
		REQUIRE(d.sig == "12");
	}

	SECTION( "Str2Dec('12E-')" ) {
		index = 0;
		SANE::str2dec("12E-", index, d, valid);
		REQUIRE(index == 2);
		REQUIRE(valid);

		REQUIRE(d.sig == "12");
		REQUIRE(d.sgn == 0);
		REQUIRE(d.exp == 0);
	}

	SECTION( "Str2Dec('12E-3')" ) {
		index = 0;
		SANE::str2dec("12E-3", index, d, valid);
		REQUIRE(index == 5);
		REQUIRE(valid);

		REQUIRE(d.sig == "12");
		REQUIRE(d.sgn == 0);
		REQUIRE(d.exp == -3);
	}

	SECTION( "Str2Dec('12E-X')" ) {
		index = 0;
		SANE::str2dec("12E-X", index, d, valid);
		REQUIRE(index == 2);
		REQUIRE(!valid);

		REQUIRE(d.sig == "12");
		REQUIRE(d.exp == 0);
		REQUIRE(d.exp == 0);
	}

	SECTION( "Str2Dec('12E-3X')" ) {
		index = 0;
		SANE::str2dec("12E-3X", index, d, valid);
		REQUIRE(index == 5);
		REQUIRE(!valid);

		REQUIRE(d.sig == "12");
		REQUIRE(d.sgn == 0);
		REQUIRE(d.exp == -3);
	}

	SECTION( "Str2Dec('x12E-3')" ) {
		index = 1;
		SANE::str2dec("x12E-3", index, d, valid);
		REQUIRE(index == 6);
		REQUIRE(valid);

		REQUIRE(d.sig == "12");
		REQUIRE(d.sgn == 0);
		REQUIRE(d.exp == -3);
	}

	SECTION( "Str2Dec('IN')" ) {
		index = 0;
		SANE::str2dec("IN", index, d, valid);
		REQUIRE(index == 0);
		REQUIRE(valid);

		REQUIRE(d.sig == "N");
		REQUIRE(d.sgn == 0);
		REQUIRE(d.exp == 0);
	}

	SECTION( "Str2Dec('INF')" ) {
		index = 0;
		SANE::str2dec("INF", index, d, valid);
		REQUIRE(index == 3);
		REQUIRE(valid);

		REQUIRE(d.sig == "I");
		REQUIRE(d.sgn == 0);
		REQUIRE(d.exp == 0);

	}


	SECTION( "Str2Dec('NAN(036)')" ) {
		index = 0;
		SANE::str2dec("NAN(036)", index, d, valid);
		REQUIRE(index == 8);
		REQUIRE(valid);

		REQUIRE(d.sig == "N0024");
		REQUIRE(d.sgn == 0);
		REQUIRE(d.exp == 0);
	}

	SECTION( "Str2Dec('-NAN(021)')" ) {
		index = 0;
		SANE::str2dec("-NAN(021)", index, d, valid);
		REQUIRE(index == 9);
		REQUIRE(valid);

		REQUIRE(d.sig == "N0015");
		REQUIRE(d.sgn == 1);
		REQUIRE(d.exp == 0);
	}

}


TEST_CASE( "Dec2X", "[dec2x]" ) {

	SECTION( "0" ) {
		SANE::decimal d{ 0, 0, "0"};
		long double dd = dec2x(d);
		REQUIRE(dd == 0.0);
	}

	SECTION( "1" ) {
		SANE::decimal d{ 0, 0, "1"};
		long double dd = dec2x(d);
		REQUIRE(dd == 1.0);
	}


	SECTION( "10" ) {
		SANE::decimal d{ 0, 1, "1"};
		long double dd = dec2x(d);
		REQUIRE(dd == 10.0);
	}


	SECTION( "Infinity" ) {
		SANE::decimal d{ 0, 0, "I"};
		long double dd = dec2x(d);
		REQUIRE(isinf(dd));
		REQUIRE(signbit(dd) == 0);
	}

	SECTION( "-Infinity" ) {
		SANE::decimal d{ 1, 0, "I"};
		long double dd = dec2x(d);
		REQUIRE(isinf(dd));
		REQUIRE(signbit(dd) != 0);
	}


	SECTION( "NaN" ) {
		SANE::decimal d{ 0, 0, "N0001"};
		long double dd = dec2x(d);
		REQUIRE(isnan(dd));
		REQUIRE(signbit(dd) == 0);
	}

	SECTION( "-NaN" ) {
		SANE::decimal d{ 1, 0, "N0001"};
		long double dd = dec2x(d);
		REQUIRE(isnan(dd));
		REQUIRE(signbit(dd) != 0);
	}

	SECTION( "HugeVal" ) {
		SANE::decimal d{ 0, 5000, "1"};
		long double dd = dec2x(d);
		REQUIRE(isinf(dd));
		REQUIRE(signbit(dd) == 0);
	}

	SECTION( "-HugeVal" ) {
		SANE::decimal d{ 1, 5000, "1"};
		long double dd = dec2x(d);
		REQUIRE(isinf(dd));
		REQUIRE(signbit(dd) != 0);
	}

	// page 38 special cases.
	SECTION( "0913") {
		SANE::decimal d{ 0, 0, "0913"};
		long double dd = dec2x(d);
		REQUIRE(dd == 0.0);
	}

}


TEST_CASE( "truncation" "[truncate]") {

	SECTION( "99 -> 1e2" ) {
		SANE::decimal d { 0, 0, "99" };
		SANE::truncate(d, 1);
		CHECK(d.sgn == 0);
		CHECK(d.exp == 2);
		CHECK(d.sig == "1");
	}

	SECTION( "101 -> 1e2" ) {
		SANE::decimal d { 0, 0, "101" };
		SANE::truncate(d, 1);
		CHECK(d.sgn == 0);
		CHECK(d.exp == 2);
		CHECK(d.sig == "1");
	}

}

TEST_CASE( "double/extended conversion", "[floating_point]") {


	SECTION( "0.0f -> ??" ) {
		fp::info fpi(FLOAT_C(0.0));
		CHECK((float)fpi == FLOAT_C(0.0));
		CHECK((double)fpi == DOUBLE_C(0.0));
		CHECK((long double)fpi == LONG_DOUBLE_C(0.0));
	}

	SECTION( "0.0d -> ??" ) {
		fp::info fpi(DOUBLE_C(0.0));
		CHECK((float)fpi == FLOAT_C(0.0));
		CHECK((double)fpi == DOUBLE_C(0.0));
		CHECK((long double)fpi == LONG_DOUBLE_C(0.0));
	}

	SECTION( "0.0l -> ??" ) {
		fp::info fpi(LONG_DOUBLE_C(0.0));
		CHECK((float)fpi == FLOAT_C(0.0));
		CHECK((double)fpi == DOUBLE_C(0.0));
		CHECK((long double)fpi == LONG_DOUBLE_C(0.0));
	}

	SECTION( "1.0f -> ??" ) {
		fp::info fpi(FLOAT_C(1.0));
		CHECK((float)fpi == FLOAT_C(1.0));
		CHECK((double)fpi == DOUBLE_C(1.0));
		CHECK((long double)fpi == LONG_DOUBLE_C(1.0));
	}

	SECTION( "1.0d -> ??" ) {
		fp::info fpi(DOUBLE_C(1.0));
		CHECK((float)fpi == FLOAT_C(1.0));
		CHECK((double)fpi == DOUBLE_C(1.0));
		CHECK((long double)fpi == LONG_DOUBLE_C(1.0));
	}

	SECTION( "1.0l -> ??" ) {
		fp::info fpi(LONG_DOUBLE_C(1.0));
		CHECK((float)fpi == FLOAT_C(1.0));
		CHECK((double)fpi == DOUBLE_C(1.0));
		CHECK((long double)fpi == LONG_DOUBLE_C(1.0));
	}

	SECTION( "-1.0f -> ??" ) {
		fp::info fpi(FLOAT_C(-1.0));
		CHECK((float)fpi == FLOAT_C(-1.0));
		CHECK((double)fpi == DOUBLE_C(-1.0));
		CHECK((long double)fpi == LONG_DOUBLE_C(-1.0));
	}

	SECTION( "-1.0d -> ??" ) {
		fp::info fpi(DOUBLE_C(-1.0));
		CHECK((float)fpi == FLOAT_C(-1.0));
		CHECK((double)fpi == DOUBLE_C(-1.0));
		CHECK((long double)fpi == LONG_DOUBLE_C(-1.0));
	}

	SECTION( "-1.0l -> ??" ) {
		fp::info fpi(LONG_DOUBLE_C(-1.0));
		CHECK((float)fpi == FLOAT_C(-1.0));
		CHECK((double)fpi == DOUBLE_C(-1.0));
		CHECK((long double)fpi == LONG_DOUBLE_C(-1.0));
	}



	SECTION( "HUGE_VALF  -> ??" ) {
		fp::info fpi(HUGE_VALF);
		CHECK((float)fpi == HUGE_VALF);
		CHECK((double)fpi == HUGE_VAL);
		CHECK((long double)fpi == HUGE_VALL);
	}


	SECTION( "HUGE_VAL  -> ??" ) {
		fp::info fpi(HUGE_VAL);
		CHECK((float)fpi == HUGE_VALF);
		CHECK((double)fpi == HUGE_VAL);
		CHECK((long double)fpi == HUGE_VALL);
	}

	SECTION( "HUGE_VALL  -> ??" ) {
		fp::info fpi(HUGE_VALL);
		CHECK((float)fpi == HUGE_VALF);
		CHECK((double)fpi == HUGE_VAL);
		CHECK((long double)fpi == HUGE_VALL);
	}



}


TEST_CASE("comp", "[comp]") {

	SECTION("NAN") {
		SANE::comp c(NAN);
		CHECK(SANE::isnan(c));
		CHECK(SANE::fpclassify(c) == FP_NAN);
	}

	SECTION("0.0") {
		SANE::comp c(0.0);
		CHECK((float)c == FLOAT_C(0.0));
		CHECK((double)c == DOUBLE_C(0.0));
		CHECK((long double)c == LONG_DOUBLE_C(0.0));
		CHECK(SANE::fpclassify(c) == FP_ZERO);
	}

	SECTION("1.0") {
		SANE::comp c(1.0);
		CHECK((float)c == FLOAT_C(1.0));
		CHECK((double)c == DOUBLE_C(1.0));
		CHECK((long double)c == LONG_DOUBLE_C(1.0));
		CHECK(SANE::fpclassify(c) == FP_NORMAL);
	}

	SECTION("HUGE_VAL") {
		SANE::comp c(HUGE_VAL);
		CHECK(SANE::isnan(c));
		CHECK(SANE::fpclassify(c) == FP_NAN);
	}


	SECTION("abs") {
		SANE::comp c(-123);
		CHECK(SANE::abs(c) == SANE::comp(123));
	}

}


TEST_CASE("make_nan", "[nan]") {

	SECTION("make_nan<float>") {
		auto tmp = make_nan<float>(1);
		CHECK(std::isnan(tmp));
		CHECK(SANE::floating_point::info(tmp).sig == 1);
	}

	SECTION("make_nan<double>") {
		auto tmp = make_nan<double>(1);
		CHECK(std::isnan(tmp));
		CHECK(SANE::floating_point::info(tmp).sig == 1);
	}

	SECTION("make_nan<long double>") {
		auto tmp = make_nan<long double>(1);
		CHECK(std::isnan(tmp));
		CHECK(SANE::floating_point::info(tmp).sig == 1);
	}

	SECTION("make_nan<comp>") {
		CHECK(SANE::isnan(make_nan<SANE::comp>(0)));
	}

	SECTION("make_nan<decimal>") {
		SANE::decimal d = make_nan<SANE::decimal>(0xff);
		CHECK(d.sig == "N00ff");
	}

}


TEST_CASE("x2dec", "[x2dec]") {
	// page 33.

	SANE::decform df{SANE::decform::FIXEDDECIMAL, 0};
	SANE::decimal d = SANE::x2dec(120, df);
	d.exp -= 2; // divide by 100
	df.digits = 2;
	std::string s;
	dec2str(df, d, s);
	CHECK(s == "1.20");
}
