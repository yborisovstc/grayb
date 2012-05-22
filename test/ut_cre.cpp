#include <env.h>
#include <stdlib.h>
#include <elem.h>
#include <mvert.h>
#include <mdata.h>

#include <cppunit/extensions/HelperMacros.h>

/*
 * This test is for checking the functionality of creation graph
 */


class Ut_cre : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(Ut_cre);
    CPPUNIT_TEST(test_Cre);
    CPPUNIT_TEST(test_CreGr);
    CPPUNIT_TEST(test_CreSyst);
    CPPUNIT_TEST(test_CreIncaps);
    CPPUNIT_TEST(test_CreData);
    CPPUNIT_TEST_SUITE_END();
public:
    virtual void setUp();
    virtual void tearDown();
private:
    void test_Cre();
    void test_CreGr();
    void test_CreSyst();
    void test_CreIncaps();
    void test_CreData();
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
    printf("\n === Test of creation of hier\n");

    iEnv = new Env("Env", "ut_cre_1.xml", "ut_cre_1.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ConstructSystem();
    Elem* root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    Elem* e2 = root->GetNode("Elem:elem1/Elem:elem2");
    CPPUNIT_ASSERT_MESSAGE("Fail to get e2", e2 != 0);
    Elem* e4 = root->GetNode("elem1:elem3/elem2:elem4");
    CPPUNIT_ASSERT_MESSAGE("Fail to get e4", e4 != 0);

    delete iEnv;
}

void Ut_cre::test_CreGr()
{
    printf("\n === Test of creation of graph\n");

    iEnv = new Env("Env", "ut_cre_gr.xml", "ut_cre_gr.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ConstructSystem();
    Elem* root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    Elem* v1 = root->GetNode("Vert:v1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get v1", v1 != 0);
    MVert* mv1 = v1->GetObj(mv1);
    CPPUNIT_ASSERT_MESSAGE("Fail to get mv1", mv1 != 0);
    MVert* pair = *(mv1->Pairs().begin());
    CPPUNIT_ASSERT_MESSAGE("Fail to get pair", pair != 0);
    const string pname = pair->EBase()->Name();
    CPPUNIT_ASSERT_MESSAGE("Wrong pair's name", pname == "v2");

    delete iEnv;
}

void Ut_cre::test_CreSyst()
{
    printf("\n === Test of creation of system\n");

    iEnv = new Env("Env", "ut_cre_syst1.xml", "ut_cre_syst.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ConstructSystem();
    Elem* root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    Elem* cp1 = root->GetNode("ConnPoint:cp1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get cp1", cp1 != 0);
    MVert* mcp1 = cp1->GetObj(mcp1);
    CPPUNIT_ASSERT_MESSAGE("Fail to get mcp1", mcp1 != 0);
    MVert* pair = *(mcp1->Pairs().begin());
    CPPUNIT_ASSERT_MESSAGE("Fail to get pair", pair != 0);
    const string pname = pair->EBase()->Name();
    CPPUNIT_ASSERT_MESSAGE("Wrong pair's name", pname == "cp2");

    delete iEnv;
}

void Ut_cre::test_CreIncaps()
{
    printf("\n === Test of creation of incapsulated system\n");

    iEnv = new Env("Env", "ut_cre_incaps.xml", "ut_cre_incaps.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ConstructSystem();
    Elem* root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    Elem* ep1 = root->GetNode("Incaps:test/Incaps:Ics1/Elem:Capsule/Extender:ep");
    CPPUNIT_ASSERT_MESSAGE("Fail to get ep1", ep1 != 0);
    MVert* mep1 = ep1->GetObj(mep1);
    CPPUNIT_ASSERT_MESSAGE("Fail to get mep1", mep1 != 0);
    MVert* pair = *(mep1->Pairs().begin());
    CPPUNIT_ASSERT_MESSAGE("Fail to get pair", pair != 0);
    Elem* epairt = root->GetNode("Incaps:test/Incaps:Ics2/Elem:Capsule/Extender:ep");
    MVert* mpairt = epairt->GetObj(mpairt);
    CPPUNIT_ASSERT_MESSAGE("Wrong pair", pair == mpairt);
    Elem* wep1 = root->GetNode("Incaps:test/Incaps:Ics1/ConnPoint:cp_int");
    CPPUNIT_ASSERT_MESSAGE("Fail to get wrong edge ep1", wep1 != NULL);

    delete iEnv;
}

void Ut_cre::test_CreData()
{
    printf("\n === Test of creation of data system\n");

    iEnv = new Env("Env", "ut_cre_data.xml", "ut_cre_data.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ConstructSystem();
    Elem* root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    Elem* doutp = root->GetNode("Incaps:test/DataSInt:DataS_Int_1/Elem:Capsule/ConnPoint:out");
    CPPUNIT_ASSERT_MESSAGE("Fail to get data out", doutp != 0);
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

    Elem* foutp = root->GetNode("Incaps:test/FuncIncInt:Incr/Elem:Capsule/ConnPoint:out");
    CPPUNIT_ASSERT_MESSAGE("Fail to get func out", foutp != 0);
    MDIntGet* foutpget = foutp->GetObj(doutpget);
    CPPUNIT_ASSERT_MESSAGE("Fail to get func out Get iface", foutpget != 0);
    TInt fres = foutpget->Value();
    CPPUNIT_ASSERT_MESSAGE("Incorrect func result", fres == 35);

    // Check getting node via edge
    Elem* fdoutp = root->GetNode("test/Incr/Capsule/inp/Edge:*/ConnPoint:*");
    CPPUNIT_ASSERT_MESSAGE("Fail to get data out via func-data connection", fdoutp != 0);

    // Check serching node, one level search
    Elem* srdoutp = root->GetNode("test/Incr/*/inp");
    CPPUNIT_ASSERT_MESSAGE("Fail to get data out via func-data connection", fdoutp != 0);

    delete iEnv;
}
