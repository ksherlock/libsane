#define CATCH_CONFIG_MAIN

#include "catch.hpp"

#include <sane/sane.h>
#include <math.h>

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

}
