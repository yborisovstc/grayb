#include <env.h>
#include <stdlib.h>
#include <elem.h>
#include <mvert.h>
#include <mdata.h>

#include <cppunit/extensions/HelperMacros.h>

/*
 * This test is for checking the functionality of creation graph
 */


class Ut_des : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(Ut_des);
    CPPUNIT_TEST(test_Cre1);
    CPPUNIT_TEST_SUITE_END();
public:
    virtual void setUp();
    virtual void tearDown();
private:
    void test_Cre1();
private:
    Env* iEnv;
};

CPPUNIT_TEST_SUITE_REGISTRATION( Ut_des );


void Ut_des::setUp()
{
}

void Ut_des::tearDown()
{
//    delete iEnv;
    CPPUNIT_ASSERT_EQUAL_MESSAGE("tearDown", 0, 0);
}

void Ut_des::test_Cre1()
{
    printf("\n === Test of creation of simple des\n");

    iEnv = new Env("Env", "ut_des_cre1.xml", "ut_des_cre1.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ConstructSystem();
    Elem* root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    Elem* doutp = root->GetNode("Incaps:test/StateInt:State1/Elem:Capsule/Extender:Out");
    CPPUNIT_ASSERT_MESSAGE("Fail to get state out", doutp != 0);
    MDIntGet* doutpget = doutp->GetObj(doutpget);
    CPPUNIT_ASSERT_MESSAGE("Fail to get data out Get iface", doutpget != 0);
    CPPUNIT_ASSERT_MESSAGE("Fail to get value of data iface", doutpget->Value() == 34);
    MVert* mdoutpv = doutp->GetObj(mdoutpv);
    CPPUNIT_ASSERT_MESSAGE("Fail to get data out vertex", mdoutpv != 0);
    MVert* pair = *(mdoutpv->Pairs().begin());
    CPPUNIT_ASSERT_MESSAGE("Fail to get pair", pair != 0);
    Elem* efuninp = root->GetNode("Incaps:test/FuncIncInt:Incr/Elem:Capsule/ConnPoint:inp");
    CPPUNIT_ASSERT_MESSAGE("Fail to get fun inp", efuninp != NULL);
    MVert* mpairt = efuninp->GetObj(mpairt);
    CPPUNIT_ASSERT_MESSAGE("Wrong pair", pair == mpairt);

    Elem* foutp = root->GetNode("Incaps:test/FuncIncInt:Incr2/Elem:Capsule/ConnPoint:out");
    CPPUNIT_ASSERT_MESSAGE("Fail to get func out", foutp != 0);
    MDIntGet* foutpget = foutp->GetObj(doutpget);
    CPPUNIT_ASSERT_MESSAGE("Fail to get func out Get iface", foutpget != 0);
    TInt fres = foutpget->Value();
    CPPUNIT_ASSERT_MESSAGE("Incorrect func result", fres == 36);

    delete iEnv;
}
