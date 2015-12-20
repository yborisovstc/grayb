#include <stdlib.h>
#include <elem.h>
#include <mvert.h>
#include <mdata.h>
#include <env.h>

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
    CPPUNIT_TEST(test_BaseApis1);
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
    void test_BaseApis1();
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

    iEnv = new Env("ut_cre_1.xml", "ut_cre_1.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ImpsMgr()->ResetImportsPaths();
    iEnv->ImpsMgr()->AddImportsPaths("../modules");
    iEnv->ConstructSystem();
    Elem* root = iEnv->Root();
    MElem* ee = root->GetNode(":Elem");
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    MElem* e2 = root->GetNode("./elem1/elem2");
    CPPUNIT_ASSERT_MESSAGE("Fail to get e2", e2 != 0);
    MElem* e4 = root->GetNode("./elem3/(elem2:)elem4");
    CPPUNIT_ASSERT_MESSAGE("Fail to get e4", e4 != 0);
    Rank rk_e4;
    e4->GetRank(rk_e4, e4->Chromos().Root());
    string srk_e4 = rk_e4.ToString();
    Rank rk_e2;
    e2->GetRank(rk_e2, e2->Chromos().Root());
    string srk_e2 = rk_e2.ToString();
    CPPUNIT_ASSERT_MESSAGE("Wrong rank of e4 or e2", srk_e4 == "2.2" && srk_e2 == "1.0");
    TBool cmp1 = rk_e2 < rk_e4;
    TBool cmp2 = rk_e4 > rk_e2;
    TBool cmp3 = rk_e4 == rk_e4;
    CPPUNIT_ASSERT_MESSAGE("Wrong rank comparition of e4 and e2", cmp1 && cmp2 && cmp3);

    delete iEnv;
}

void Ut_cre::test_CreGr()
{
    printf("\n === Test of creation of graph\n");

    iEnv = new Env("ut_cre_gr.xml", "ut_cre_gr.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ImpsMgr()->ResetImportsPaths();
    iEnv->ImpsMgr()->AddImportsPaths("../modules");
    iEnv->ConstructSystem();
    Elem* root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    MElem* v1 = root->GetNode("./(Vert:)v1");
    //Elem* av1 = v1->GetNode("(Elem:)Agents");
    CPPUNIT_ASSERT_MESSAGE("Fail to get v1", v1 != 0);
    MVert* mv1 = v1->GetObj(mv1);
    CPPUNIT_ASSERT_MESSAGE("Fail to get mv1", mv1 != 0);
    MVert* pair = *(mv1->Pairs().begin());
    CPPUNIT_ASSERT_MESSAGE("Fail to get pair", pair != 0);
    MElem* epair = pair->EBase()->GetObj(epair);
    const string pname = epair->Name();
    CPPUNIT_ASSERT_MESSAGE("Wrong pair's name", pname == "v2");

    delete iEnv;
}

void Ut_cre::test_CreSyst()
{
    printf("\n === Test of creation of system\n");

    iEnv = new Env("ut_cre_syst1.xml", "ut_cre_syst.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ImpsMgr()->ResetImportsPaths();
    iEnv->ImpsMgr()->AddImportsPaths("../modules");
    iEnv->ConstructSystem();
    Elem* root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    MElem* cp1 = root->GetNode("./cp1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get cp1", cp1 != 0);
    MVert* mcp1 = cp1->GetObj(mcp1);
    CPPUNIT_ASSERT_MESSAGE("Fail to get mcp1", mcp1 != 0);
    MVert* pair = *(mcp1->Pairs().begin());
    CPPUNIT_ASSERT_MESSAGE("Fail to get pair", pair != 0);
    MElem* epair = pair->EBase()->GetObj(epair);
    const string pname = epair->Name();
    CPPUNIT_ASSERT_MESSAGE("Wrong pair's name", pname == "cp2");

    delete iEnv;
}

void Ut_cre::test_CreIncaps()
{
    printf("\n === Test of creation of incapsulated system\n");

    iEnv = new Env("ut_cre_incaps.xml", "ut_cre_incaps.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ImpsMgr()->ResetImportsPaths();
    iEnv->ImpsMgr()->AddImportsPaths("../modules");
    iEnv->ConstructSystem();
    Elem* root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    MElem* ep1 = root->GetNode("./test/Ics1/Capsule/ep");
    CPPUNIT_ASSERT_MESSAGE("Fail to get ep1", ep1 != 0);
    MVert* mep1 = ep1->GetObj(mep1);
    CPPUNIT_ASSERT_MESSAGE("Fail to get mep1", mep1 != 0);
    MVert* pair = *(mep1->Pairs().begin());
    CPPUNIT_ASSERT_MESSAGE("Fail to get pair", pair != 0);
    MElem* epairt = root->GetNode("./test/(Incaps:)Ics2/Capsule/ep");
    MVert* mpairt = epairt->GetObj(mpairt);
    CPPUNIT_ASSERT_MESSAGE("Wrong pair", pair == mpairt);
    MElem* wep1 = root->GetNode("./test/Ics1/cp_int");
    CPPUNIT_ASSERT_MESSAGE("Fail to get wrong edge ep1", wep1 != NULL);

    delete iEnv;
}

void Ut_cre::test_CreData()
{
    printf("\n === Test of creation of data system\n");

    iEnv = new Env("ut_cre_data.xml", "ut_cre_data.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ImpsMgr()->ResetImportsPaths();
    iEnv->ImpsMgr()->AddImportsPaths("../modules");
    iEnv->ConstructSystem();
    Elem* root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    MElem* doutp = root->GetNode("./(Incaps:)test/(DataSInt:)DataS_Int_1/Capsule/out");
    CPPUNIT_ASSERT_MESSAGE("Fail to get data out", doutp != 0);
    MDIntGet* doutpget = (MDIntGet*) doutp->GetSIfi(MDIntGet::Type());
    CPPUNIT_ASSERT_MESSAGE("Fail to get data out Get iface", doutpget != 0);
    CPPUNIT_ASSERT_MESSAGE("Fail to get value of data iface", doutpget->Value() == 34);
    MVert* mdoutpv = doutp->GetObj(mdoutpv);
    CPPUNIT_ASSERT_MESSAGE("Fail to get data out vertex", mdoutpv != 0);
    MVert* pair = *(mdoutpv->Pairs().begin());
    CPPUNIT_ASSERT_MESSAGE("Fail to get pair", pair != 0);
    MElem* efuninp = root->GetNode("./test/Incr/Capsule/inp");
    CPPUNIT_ASSERT_MESSAGE("Fail to get fun inp", efuninp != NULL);
    MVert* mpairt = efuninp->GetObj(mpairt);
    CPPUNIT_ASSERT_MESSAGE("Wrong pair", pair == mpairt);

    MElem* foutp = root->GetNode("./test/Incr/Capsule/out");
    CPPUNIT_ASSERT_MESSAGE("Fail to get func out", foutp != 0);
    MDIntGet* foutpget = (MDIntGet*) foutp->GetSIfi(MDIntGet::Type());
    CPPUNIT_ASSERT_MESSAGE("Fail to get func out Get iface", foutpget != 0);
    TInt fres = foutpget->Value();
    CPPUNIT_ASSERT_MESSAGE("Incorrect func result", fres == 35);

    // Check getting node via edge
    // TODO [YB] Incorrect URI, this is hier URI, no possibility to access CP thru edge
//    Elem* fdoutp = root->GetNode("test/Incr/Capsule/inp/Edge:*/ConnPoint:*");
//    CPPUNIT_ASSERT_MESSAGE("Fail to get data out via func-data connection", fdoutp != 0);

    // Check serching node, one level search
    MElem* srdoutp = root->GetNode("./test/Incr/*/inp");
    CPPUNIT_ASSERT_MESSAGE("Fail to get data out via func-data connection", srdoutp != 0);

    delete iEnv;
}

// Base agent apis: nets
void Ut_cre::test_BaseApis1()
{
    printf("\n === Test of base agent apis\n");
    iEnv = new Env("ut_baseapis1.xml", "ut_baseapis1.txt");
    iEnv->ChMgr()->SetEnableFixErrors(ETrue);
    iEnv->ChMgr()->SetEnableReposMuts(ETrue);
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ImpsMgr()->ResetImportsPaths();
    iEnv->ImpsMgr()->AddImportsPaths("../modules");
    iEnv->ConstructSystem();
    Elem* root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    // Make illegal modif and enable transformation
    MElem* vb = root->GetNode("./VB");
    MElem* va1b1 = root->GetNode("./VB/VA_1/VA_1_B1");
    Elem* evb = vb->GetObj(evb);
    bool isdirinh1 = evb->IsDirectInheritedComp(va1b1);
    CPPUNIT_ASSERT_MESSAGE("Wrong result of IsDirectInheritedComp for VA_1_B1", !isdirinh1);
    delete iEnv;
}


