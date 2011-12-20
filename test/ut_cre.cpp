#include <env.h>
#include <stdlib.h>

#include <cppunit/extensions/HelperMacros.h>

/*
 * This test is for checking the functionality of creation graph
 */


class Ut_cre : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(Ut_cre);
    CPPUNIT_TEST(test_Cre);
    CPPUNIT_TEST_SUITE_END();
public:
    virtual void setUp();
    virtual void tearDown();
private:
    void test_Cre();
private:
    Env* iEnv;
};

CPPUNIT_TEST_SUITE_REGISTRATION( Ut_cre );


void Ut_cre::setUp()
{
}

void Ut_cre::tearDown()
{
//    delete iEnv;
    CPPUNIT_ASSERT_EQUAL_MESSAGE("tearDown", 0, 0);
}

void Ut_cre::test_Cre()
{
    printf("\n === Test of creation of graph\n");

    iEnv = new Env("Env", "ut_cre_1.xml", "ut_cre_1.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ConstructSystem();

    delete iEnv;
}
