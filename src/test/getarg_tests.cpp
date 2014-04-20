#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-CCC");
    BOOST_CHECK(GetBoolArg("-CCC"));
    BOOST_CHECK(GetBoolArg("-CCC", false));
    BOOST_CHECK(GetBoolArg("-CCC", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-CCCo"));
    BOOST_CHECK(!GetBoolArg("-CCCo", false));
    BOOST_CHECK(GetBoolArg("-CCCo", true));

    ResetArgs("-CCC=0");
    BOOST_CHECK(!GetBoolArg("-CCC"));
    BOOST_CHECK(!GetBoolArg("-CCC", false));
    BOOST_CHECK(!GetBoolArg("-CCC", true));

    ResetArgs("-CCC=1");
    BOOST_CHECK(GetBoolArg("-CCC"));
    BOOST_CHECK(GetBoolArg("-CCC", false));
    BOOST_CHECK(GetBoolArg("-CCC", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noCCC");
    BOOST_CHECK(!GetBoolArg("-CCC"));
    BOOST_CHECK(!GetBoolArg("-CCC", false));
    BOOST_CHECK(!GetBoolArg("-CCC", true));

    ResetArgs("-noCCC=1");
    BOOST_CHECK(!GetBoolArg("-CCC"));
    BOOST_CHECK(!GetBoolArg("-CCC", false));
    BOOST_CHECK(!GetBoolArg("-CCC", true));

    ResetArgs("-CCC -noCCC");  // -CCC should win
    BOOST_CHECK(GetBoolArg("-CCC"));
    BOOST_CHECK(GetBoolArg("-CCC", false));
    BOOST_CHECK(GetBoolArg("-CCC", true));

    ResetArgs("-CCC=1 -noCCC=1");  // -CCC should win
    BOOST_CHECK(GetBoolArg("-CCC"));
    BOOST_CHECK(GetBoolArg("-CCC", false));
    BOOST_CHECK(GetBoolArg("-CCC", true));

    ResetArgs("-CCC=0 -noCCC=0");  // -CCC should win
    BOOST_CHECK(!GetBoolArg("-CCC"));
    BOOST_CHECK(!GetBoolArg("-CCC", false));
    BOOST_CHECK(!GetBoolArg("-CCC", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--CCC=1");
    BOOST_CHECK(GetBoolArg("-CCC"));
    BOOST_CHECK(GetBoolArg("-CCC", false));
    BOOST_CHECK(GetBoolArg("-CCC", true));

    ResetArgs("--noCCC=1");
    BOOST_CHECK(!GetBoolArg("-CCC"));
    BOOST_CHECK(!GetBoolArg("-CCC", false));
    BOOST_CHECK(!GetBoolArg("-CCC", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-CCC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-CCC", "eleven"), "eleven");

    ResetArgs("-CCC -bar");
    BOOST_CHECK_EQUAL(GetArg("-CCC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-CCC", "eleven"), "");

    ResetArgs("-CCC=");
    BOOST_CHECK_EQUAL(GetArg("-CCC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-CCC", "eleven"), "");

    ResetArgs("-CCC=11");
    BOOST_CHECK_EQUAL(GetArg("-CCC", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-CCC", "eleven"), "11");

    ResetArgs("-CCC=eleven");
    BOOST_CHECK_EQUAL(GetArg("-CCC", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-CCC", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-CCC", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-CCC", 0), 0);

    ResetArgs("-CCC -bar");
    BOOST_CHECK_EQUAL(GetArg("-CCC", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-CCC=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-CCC", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-CCC=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-CCC", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--CCC");
    BOOST_CHECK_EQUAL(GetBoolArg("-CCC"), true);

    ResetArgs("--CCC=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-CCC", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noCCC");
    BOOST_CHECK(!GetBoolArg("-CCC"));
    BOOST_CHECK(!GetBoolArg("-CCC", true));
    BOOST_CHECK(!GetBoolArg("-CCC", false));

    ResetArgs("-noCCC=1");
    BOOST_CHECK(!GetBoolArg("-CCC"));
    BOOST_CHECK(!GetBoolArg("-CCC", true));
    BOOST_CHECK(!GetBoolArg("-CCC", false));

    ResetArgs("-noCCC=0");
    BOOST_CHECK(GetBoolArg("-CCC"));
    BOOST_CHECK(GetBoolArg("-CCC", true));
    BOOST_CHECK(GetBoolArg("-CCC", false));

    ResetArgs("-CCC --noCCC");
    BOOST_CHECK(GetBoolArg("-CCC"));

    ResetArgs("-noCCC -CCC"); // CCC always wins:
    BOOST_CHECK(GetBoolArg("-CCC"));
}

BOOST_AUTO_TEST_SUITE_END()
