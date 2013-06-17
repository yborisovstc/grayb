#include <env.h>
#include <stdlib.h>
#include <elem.h>
#include <mvert.h>
#include <mdata.h>
#include <mprop.h>

#include <cppunit/extensions/HelperMacros.h>

/*
 * This test is for checking the functionality of creation graph
 */


class Ut_func : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(Ut_func);
    CPPUNIT_TEST(test_FuncSeq1);
    CPPUNIT_TEST(test_FuncSeq2);
    CPPUNIT_TEST_SUITE_END();
public:
    virtual void setUp();
    virtual void tearDown();
private:
    void test_FuncSeq1();
    void test_FuncSeq2();
private:
    Env* iEnv;
};

CPPUNIT_TEST_SUITE_REGISTRATION( Ut_func );


void Ut_func::setUp()
{
}

void Ut_func::tearDown()
{
//    delete iEnv;
    CPPUNIT_ASSERT_EQUAL_MESSAGE("tearDown", 0, 0);
}

void Ut_func::test_FuncSeq1()
{
    printf("\n === Test of simple sequence of functions\n");

    iEnv = new Env("Env", "ut_func_seq1.xml", "ut_func_seq1.txt");
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

    Elem* foutp = root->GetNode("Incaps:test/FuncIncInt:Incr2/Elem:Capsule/ConnPoint:out");
    CPPUNIT_ASSERT_MESSAGE("Fail to get func out", foutp != 0);
    MDIntGet* foutpget = foutp->GetObj(foutpget);
    CPPUNIT_ASSERT_MESSAGE("Fail to get func out Get iface", foutpget != 0);
    TInt fres = foutpget->Value();
    CPPUNIT_ASSERT_MESSAGE("Incorrect func result", fres == 36);

    Elem* resdata = root->GetNode("Incaps:test/DataSInt:ResData/Elem:Capsule/ConnPoint:out");
    CPPUNIT_ASSERT_MESSAGE("Fail to get res data out", resdata != 0);
    MDIntGet* rdataget = resdata->GetObj(rdataget);
    CPPUNIT_ASSERT_MESSAGE("Fail to get rdata out Get iface", rdataget != 0);
    TInt rdataval = rdataget->Value();
    CPPUNIT_ASSERT_MESSAGE("Incorrect rdata value", rdataval == 36);
    Elem* resdataprop = root->GetNode("Incaps:test/DataSInt:ResData/Prop:Value");
    CPPUNIT_ASSERT_MESSAGE("Fail to get rdata value property", resdataprop != 0);
    MProp* rdmprop = resdataprop->GetObj(rdmprop);
    const string& rdval = rdmprop->Value();
    CPPUNIT_ASSERT_MESSAGE("Incorrect rdata prop value", rdval == "36");

    // Checking the result update on update of input
    // Mutate the input data first
    Elem* dinp = root->GetNode("Incaps:test/DataSInt:DataS_Int_1");
    ChromoNode nchange = dinp->Mutation().Root().AddChild(ENt_Cont);
    nchange.SetAttr(ENa_MutNode, "Prop:Value");
    nchange.SetAttr(ENa_MutVal, "57");
    dinp->Mutate();
    // Check the function output
    Elem* foutp1 = root->GetNode("Incaps:test/FuncIncInt:Incr2/Elem:Capsule/ConnPoint:out");
    CPPUNIT_ASSERT_MESSAGE("Fail to get func out when inp data changed", foutp1 != 0);
    MDIntGet* foutpget1 = foutp1->GetObj(foutpget1);
    CPPUNIT_ASSERT_MESSAGE("Fail to get func out Get iface when input data changed", foutpget1 != 0);
    TInt fres1 = foutpget1->Value();
    CPPUNIT_ASSERT_MESSAGE("Incorrect func result after input data change", fres1 == 59);
    // Check the output data
    Elem* resdataprop1 = root->GetNode("Incaps:test/DataSInt:ResData/Prop:Value");
    CPPUNIT_ASSERT_MESSAGE("Fail to get result data value property when inp data changed", resdataprop1 != 0);
    MProp* rdmprop1 = resdataprop1->GetObj(rdmprop1);
    const string& rdval1 = rdmprop1->Value();
    CPPUNIT_ASSERT_MESSAGE("Incorrect result data prop value when inp data changed", rdval1 == "59");

   


    delete iEnv;
}

void Ut_func::test_FuncSeq2()
{
    printf("\n === Test of multiargument function: simple addition\n");

    iEnv = new Env("Env", "ut_func_seq2.xml", "ut_func_seq2.txt");
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
    Elem* efuninp = root->GetNode("Incaps:test/FuncAddInt:Add/Elem:Capsule/ConnPoint:inp");
    CPPUNIT_ASSERT_MESSAGE("Fail to get fun inp", efuninp != NULL);
    MVert* mpairt = efuninp->GetObj(mpairt);
    CPPUNIT_ASSERT_MESSAGE("Wrong pair", pair == mpairt);

    Elem* foutp = root->GetNode("Incaps:test/FuncAddInt:Add/Elem:Capsule/ConnPoint:out");
    CPPUNIT_ASSERT_MESSAGE("Fail to get func out", foutp != 0);
    MDIntGet* foutpget = foutp->GetObj(foutpget);
    CPPUNIT_ASSERT_MESSAGE("Fail to get func out Get iface", foutpget != 0);
    TInt fres = foutpget->Value();
    CPPUNIT_ASSERT_MESSAGE("Incorrect func result", fres == 60);

    Elem* resdata = root->GetNode("Incaps:test/DataSInt:ResData/Elem:Capsule/ConnPoint:out");
    CPPUNIT_ASSERT_MESSAGE("Fail to get res data out", resdata != 0);
    MDIntGet* rdataget = resdata->GetObj(rdataget);
    CPPUNIT_ASSERT_MESSAGE("Fail to get rdata out Get iface", rdataget != 0);
    TInt rdataval = rdataget->Value();
    CPPUNIT_ASSERT_MESSAGE("Incorrect rdata value", rdataval == 60);
    Elem* resdataprop = root->GetNode("Incaps:test/DataSInt:ResData/Prop:Value");
    CPPUNIT_ASSERT_MESSAGE("Fail to get rdata value property", resdataprop != 0);
    MProp* rdmprop = resdataprop->GetObj(rdmprop);
    const string& rdval = rdmprop->Value();
    CPPUNIT_ASSERT_MESSAGE("Incorrect rdata prop value", rdval == "60");

    // Checking the result update on update of input
    // Mutate the input data first
    Elem* dinp = root->GetNode("Incaps:test/DataSInt:DataS_Int_1");
    ChromoNode nchange = dinp->Mutation().Root().AddChild(ENt_Cont);
    nchange.SetAttr(ENa_MutNode, "Prop:Value");
    nchange.SetAttr(ENa_MutVal, "57");
    dinp->Mutate();
    // Check the function output
    Elem* foutp1 = root->GetNode("Incaps:test/FuncAddInt:Add/Elem:Capsule/ConnPoint:out");
    CPPUNIT_ASSERT_MESSAGE("Fail to get func out when inp data changed", foutp1 != 0);
    MDIntGet* foutpget1 = foutp1->GetObj(foutpget1);
    CPPUNIT_ASSERT_MESSAGE("Fail to get func out Get iface when input data changed", foutpget1 != 0);
    TInt fres1 = foutpget1->Value();
    CPPUNIT_ASSERT_MESSAGE("Incorrect func result after input data change", fres1 == 83);
    // Check the output data
    Elem* resdataprop1 = root->GetNode("Incaps:test/DataSInt:ResData/Prop:Value");
    CPPUNIT_ASSERT_MESSAGE("Fail to get result data value property when inp data changed", resdataprop1 != 0);
    MProp* rdmprop1 = resdataprop1->GetObj(rdmprop1);
    const string& rdval1 = rdmprop1->Value();
    CPPUNIT_ASSERT_MESSAGE("Incorrect result data prop value when inp data changed", rdval1 == "83");

   


    delete iEnv;
}
