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
    CPPUNIT_TEST(test_FuncSeq3);
    CPPUNIT_TEST(test_FuncSeq4);
    CPPUNIT_TEST(test_FuncVar1);
    CPPUNIT_TEST(test_FuncVar1mc);
    CPPUNIT_TEST(test_FuncVar2);
    CPPUNIT_TEST_SUITE_END();
public:
    virtual void setUp();
    virtual void tearDown();
private:
    void test_FuncSeq1();
    void test_FuncSeq2();
    void test_FuncSeq3();
    void test_FuncSeq4();
    void test_FuncVar1();
    void test_FuncVar1mc();
    void test_FuncVar2();
private:
    Env* iEnv;
};

CPPUNIT_TEST_SUITE_REGISTRATION( Ut_func );
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(Ut_func, "Ut_func");


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

    iEnv = new Env("ut_func_seq1.xml", "ut_func_seq1.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ImpsMgr()->ResetImportsPaths();
    iEnv->ImpsMgr()->AddImportsPaths("../modules");
    iEnv->ConstructSystem();
    MUnit* root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    MUnit* doutp = root->GetNode("./test/DataS_Int_1/Capsule/out");
    CPPUNIT_ASSERT_MESSAGE("Fail to get data out", doutp != 0);
    MDIntGet* doutpget = (MDIntGet*) doutp->GetSIfi(MDIntGet::Type());
    CPPUNIT_ASSERT_MESSAGE("Fail to get data out Get iface", doutpget != 0);
    CPPUNIT_ASSERT_MESSAGE("Fail to get value of data iface", doutpget->Value() == 34);
    MVert* mdoutpv = doutp->GetObj(mdoutpv);
    CPPUNIT_ASSERT_MESSAGE("Fail to get data out vertex", mdoutpv != 0);
    MVert* pair = mdoutpv->GetPair(0);
    CPPUNIT_ASSERT_MESSAGE("Fail to get pair", pair != 0);
    MUnit* efuninp = root->GetNode("./test/Incr/Capsule/inp");
    CPPUNIT_ASSERT_MESSAGE("Fail to get fun inp", efuninp != NULL);
    MVert* mpairt = efuninp->GetObj(mpairt);
    CPPUNIT_ASSERT_MESSAGE("Wrong pair", pair == mpairt);

    MUnit* foutp = root->GetNode("./test/Incr2/Capsule/out");
    CPPUNIT_ASSERT_MESSAGE("Fail to get func out", foutp != 0);
    MDIntGet* foutpget = (MDIntGet*) foutp->GetSIfi(MDIntGet::Type());
    CPPUNIT_ASSERT_MESSAGE("Fail to get func out Get iface", foutpget != 0);
    TInt fres = foutpget->Value();
    CPPUNIT_ASSERT_MESSAGE("Incorrect func result", fres == 36);

    MUnit* resdata = root->GetNode("./test/ResData/Capsule/out");
    CPPUNIT_ASSERT_MESSAGE("Fail to get res data out", resdata != 0);
    MDIntGet* rdataget = (MDIntGet*) resdata->GetSIfi(MDIntGet::Type());
    CPPUNIT_ASSERT_MESSAGE("Fail to get rdata out Get iface", rdataget != 0);
    TInt rdataval = rdataget->Value();
    CPPUNIT_ASSERT_MESSAGE("Incorrect rdata value", rdataval == 36);
    MUnit* resdataprop = root->GetNode("./test/ResData/Value");
    CPPUNIT_ASSERT_MESSAGE("Fail to get rdata value property", resdataprop != 0);
    MProp* rdmprop = resdataprop->GetObj(rdmprop);
    const string& rdval = rdmprop->Value();
    CPPUNIT_ASSERT_MESSAGE("Incorrect rdata prop value", rdval == "36");

    // Checking the result update on update of input
    // Mutate the input data first
    MUnit* dinp = root->GetNode("./test/DataS_Int_1");
    MElem* edinp = dinp->GetObj(edinp);
    ChromoNode nchange = edinp->AppendMutation(ENt_Cont);
    nchange.SetAttr(ENa_MutNode, "./Value");
    nchange.SetAttr(ENa_MutVal, "57");
    TNs ns; MutCtx mctx(NULL, ns);
    edinp->Mutate(false, false, true, mctx);
    // Check the function output
    MUnit* foutp1 = root->GetNode("./test/Incr2/Capsule/out");
    CPPUNIT_ASSERT_MESSAGE("Fail to get func out when inp data changed", foutp1 != 0);
    MDIntGet* foutpget1 = (MDIntGet*) foutp1->GetSIfi(MDIntGet::Type());
    CPPUNIT_ASSERT_MESSAGE("Fail to get func out Get iface when input data changed", foutpget1 != 0);
    TInt fres1 = foutpget1->Value();
    CPPUNIT_ASSERT_MESSAGE("Incorrect func result after input data change", fres1 == 59);
    // Check the output data
    MUnit* resdataprop1 = root->GetNode("./test/ResData/Value");
    CPPUNIT_ASSERT_MESSAGE("Fail to get result data value property when inp data changed", resdataprop1 != 0);
    MProp* rdmprop1 = resdataprop1->GetObj(rdmprop1);
    const string& rdval1 = rdmprop1->Value();
    CPPUNIT_ASSERT_MESSAGE("Incorrect result data prop value when inp data changed", rdval1 == "59");

    delete iEnv;
}

void Ut_func::test_FuncSeq2()
{
    printf("\n === Test of multiargument function: simple addition\n");

    iEnv = new Env("ut_func_seq2.xml", "ut_func_seq2.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ImpsMgr()->ResetImportsPaths();
    iEnv->ImpsMgr()->AddImportsPaths("../modules");
    iEnv->ConstructSystem();
    MUnit* root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    MUnit* doutp = root->GetNode("./test/DataS_Int_1/Capsule/out");
    CPPUNIT_ASSERT_MESSAGE("Fail to get data out", doutp != 0);
    MDIntGet* doutpget = (MDIntGet*) doutp->GetSIfi(MDIntGet::Type());
    CPPUNIT_ASSERT_MESSAGE("Fail to get data out Get iface", doutpget != 0);
    CPPUNIT_ASSERT_MESSAGE("Fail to get value of data iface", doutpget->Value() == 34);
    MVert* mdoutpv = doutp->GetObj(mdoutpv);
    CPPUNIT_ASSERT_MESSAGE("Fail to get data out vertex", mdoutpv != 0);
    MVert* pair = mdoutpv->GetPair(0);
    CPPUNIT_ASSERT_MESSAGE("Fail to get pair", pair != 0);
    MUnit* efuninp = root->GetNode("./test/Add/Capsule/inp");
    CPPUNIT_ASSERT_MESSAGE("Fail to get fun inp", efuninp != NULL);
    MVert* mpairt = efuninp->GetObj(mpairt);
    CPPUNIT_ASSERT_MESSAGE("Wrong pair", pair == mpairt);

    MUnit* foutp = root->GetNode("./test/Add/Capsule/out");
    CPPUNIT_ASSERT_MESSAGE("Fail to get func out", foutp != 0);
    MDIntGet* foutpget = (MDIntGet*) foutp->GetSIfi(MDIntGet::Type());
    CPPUNIT_ASSERT_MESSAGE("Fail to get func out Get iface", foutpget != 0);
    TInt fres = foutpget->Value();
    CPPUNIT_ASSERT_MESSAGE("Incorrect func result", fres == 60);

    MUnit* resdata = root->GetNode("./test/ResData/Capsule/out");
    CPPUNIT_ASSERT_MESSAGE("Fail to get res data out", resdata != 0);
    MDIntGet* rdataget = (MDIntGet*) resdata->GetSIfi(MDIntGet::Type());
    CPPUNIT_ASSERT_MESSAGE("Fail to get rdata out Get iface", rdataget != 0);
    TInt rdataval = rdataget->Value();
    CPPUNIT_ASSERT_MESSAGE("Incorrect rdata value", rdataval == 60);
    MUnit* resdataprop = root->GetNode("./test/ResData/Value");
    CPPUNIT_ASSERT_MESSAGE("Fail to get rdata value property", resdataprop != 0);
    MProp* rdmprop = resdataprop->GetObj(rdmprop);
    const string& rdval = rdmprop->Value();
    CPPUNIT_ASSERT_MESSAGE("Incorrect rdata prop value", rdval == "60");

    // Checking the result update on update of input
    // Mutate the input data first
    MUnit* dinp = root->GetNode("./test/DataS_Int_1");
    MElem* edinp = dinp->GetObj(edinp);
    ChromoNode nchange = edinp->AppendMutation(ENt_Cont);
    nchange.SetAttr(ENa_MutNode, "./Value");
    nchange.SetAttr(ENa_MutVal, "57");
    TNs ns; MutCtx mctx(NULL, ns);
    edinp->Mutate(false, false, true, mctx);
    // Check the function output
    MUnit* foutp1 = root->GetNode("./test/Add/Capsule/out");
    CPPUNIT_ASSERT_MESSAGE("Fail to get func out when inp data changed", foutp1 != 0);
    MDIntGet* foutpget1 = (MDIntGet*) foutp1->GetSIfi(MDIntGet::Type());
    CPPUNIT_ASSERT_MESSAGE("Fail to get func out Get iface when input data changed", foutpget1 != 0);
    TInt fres1 = foutpget1->Value();
    CPPUNIT_ASSERT_MESSAGE("Incorrect func result after input data change", fres1 == 83);
    // Check the output data
    MUnit* resdataprop1 = root->GetNode("./test/ResData/Value");
    CPPUNIT_ASSERT_MESSAGE("Fail to get result data value property when inp data changed", resdataprop1 != 0);
    MProp* rdmprop1 = resdataprop1->GetObj(rdmprop1);
    const string& rdval1 = rdmprop1->Value();
    CPPUNIT_ASSERT_MESSAGE("Incorrect result data prop value when inp data changed", rdval1 == "83");

    delete iEnv;
}

void Ut_func::test_FuncSeq3()
{
    printf("\n === Test of multiargument and multitype function: simple addition\n");

    iEnv = new Env("ut_func_seq3.xml", "ut_func_seq3.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ImpsMgr()->ResetImportsPaths();
    iEnv->ImpsMgr()->AddImportsPaths("../modules");
    iEnv->ConstructSystem();
    MUnit* root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    MUnit* doutp = root->GetNode("./test/DataS_Int_1/Capsule/out");
    CPPUNIT_ASSERT_MESSAGE("Fail to get data out", doutp != 0);
    MDIntGet* doutpget = (MDIntGet*) doutp->GetSIfi(MDIntGet::Type());
    CPPUNIT_ASSERT_MESSAGE("Fail to get data out Get iface", doutpget != 0);
    CPPUNIT_ASSERT_MESSAGE("Fail to get value of data iface", doutpget->Value() == 34);
    MVert* mdoutpv = doutp->GetObj(mdoutpv);
    CPPUNIT_ASSERT_MESSAGE("Fail to get data out vertex", mdoutpv != 0);
    MVert* pair = mdoutpv->GetPair(0);
    CPPUNIT_ASSERT_MESSAGE("Fail to get pair", pair != 0);
    MUnit* efuninp = root->GetNode("./test/Add/Capsule/inp");
    CPPUNIT_ASSERT_MESSAGE("Fail to get fun inp", efuninp != NULL);
    MVert* mpairt = efuninp->GetObj(mpairt);
    CPPUNIT_ASSERT_MESSAGE("Wrong pair", pair == mpairt);

    MUnit* foutp = root->GetNode("./test/Add/Capsule/out");
    CPPUNIT_ASSERT_MESSAGE("Fail to get func out", foutp != 0);
    MDIntGet* foutpget = (MDIntGet*) foutp->GetSIfi(MDIntGet::Type());
    CPPUNIT_ASSERT_MESSAGE("Fail to get func out Get iface", foutpget != 0);
    TInt fres = foutpget->Value();
    CPPUNIT_ASSERT_MESSAGE("Incorrect func result", fres == 60);

    MUnit* resdata = root->GetNode("./test/ResData/Capsule/out");
    CPPUNIT_ASSERT_MESSAGE("Fail to get res data out", resdata != 0);
    MDIntGet* rdataget = (MDIntGet*) resdata->GetSIfi(MDIntGet::Type());
    CPPUNIT_ASSERT_MESSAGE("Fail to get rdata out Get iface", rdataget != 0);
    TInt rdataval = rdataget->Value();
    CPPUNIT_ASSERT_MESSAGE("Incorrect rdata value", rdataval == 60);
    MUnit* resdataprop = root->GetNode("./test/ResData/Value");
    CPPUNIT_ASSERT_MESSAGE("Fail to get rdata value property", resdataprop != 0);
    MProp* rdmprop = resdataprop->GetObj(rdmprop);
    const string& rdval = rdmprop->Value();
    CPPUNIT_ASSERT_MESSAGE("Incorrect rdata prop value", rdval == "60");

    // Checking the result update on update of input
    // Mutate the input data first
    MUnit* dinp = root->GetNode("./test/DataS_Int_1");
    MElem* edinp = dinp->GetObj(edinp);
    ChromoNode nchange = edinp->AppendMutation(ENt_Cont);
    nchange.SetAttr(ENa_MutNode, "./Value");
    nchange.SetAttr(ENa_MutVal, "57");
    TNs ns; MutCtx mctx(NULL, ns);
    edinp->Mutate(false, false, true, mctx);
    // Check the function output
    MUnit* foutp1 = root->GetNode("./test/Add/Capsule/out");
    CPPUNIT_ASSERT_MESSAGE("Fail to get func out when inp data changed", foutp1 != 0);
    MDIntGet* foutpget1 = (MDIntGet*) foutp1->GetSIfi(MDIntGet::Type());
    CPPUNIT_ASSERT_MESSAGE("Fail to get func out Get iface when input data changed", foutpget1 != 0);
    TInt fres1 = foutpget1->Value();
    CPPUNIT_ASSERT_MESSAGE("Incorrect func result after input data change", fres1 == 83);
    // Check the output data
    MUnit* resdataprop1 = root->GetNode("./test/ResData/Value");
    CPPUNIT_ASSERT_MESSAGE("Fail to get result data value property when inp data changed", resdataprop1 != 0);
    MProp* rdmprop1 = resdataprop1->GetObj(rdmprop1);
    const string& rdval1 = rdmprop1->Value();
    CPPUNIT_ASSERT_MESSAGE("Incorrect result data prop value when inp data changed", rdval1 == "83");

    delete iEnv;
}

void Ut_func::test_FuncSeq4()
{
    printf("\n === Test of convolution function with separate working function\n");

    iEnv = new Env("ut_func_seq4.xml", "ut_func_seq4.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ImpsMgr()->ResetImportsPaths();
    iEnv->ImpsMgr()->AddImportsPaths("../modules");
    iEnv->ConstructSystem();
    MUnit* root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);

    MUnit* resdata = root->GetNode("./test/ResData/Capsule/out");
    CPPUNIT_ASSERT_MESSAGE("Fail to get res data out", resdata != 0);
    MDIntGet* rdataget = (MDIntGet*) resdata->GetSIfi(MDIntGet::Type());
    CPPUNIT_ASSERT_MESSAGE("Fail to get rdata out Get iface", rdataget != 0);
    TInt rdataval = rdataget->Value();
    CPPUNIT_ASSERT_MESSAGE("Incorrect rdata value", rdataval == 1);
    MUnit* resdataprop = root->GetNode("./test/ResData/Value");
    CPPUNIT_ASSERT_MESSAGE("Fail to get rdata value property", resdataprop != 0);
    MProp* rdmprop = resdataprop->GetObj(rdmprop);
    const string& rdval = rdmprop->Value();
    CPPUNIT_ASSERT_MESSAGE("Incorrect rdata prop value", rdval == "1");

    // Checking the result update on update of input
    // Mutate the input data first
    MUnit* dinp = root->GetNode("./test/Data_SelfCoord");
    MElem* edinp = dinp->GetObj(edinp);
    ChromoNode nchange = edinp->AppendMutation(ENt_Cont);
    nchange.SetAttr(ENa_MutNode, "./Value");
    nchange.SetAttr(ENa_MutVal, "80");
    TNs ns; MutCtx mctx(NULL, ns);
    edinp->Mutate(false, false, true, mctx);
    // Check the output data
    MUnit* resdataprop1 = root->GetNode("./test/ResData/Value");
    CPPUNIT_ASSERT_MESSAGE("Fail to get result data value property when inp data changed", resdataprop1 != 0);
    MProp* rdmprop1 = resdataprop1->GetObj(rdmprop1);
    const string& rdval1 = rdmprop1->Value();
    CPPUNIT_ASSERT_MESSAGE("Incorrect result data prop value when inp data changed", rdval1 == "3");

    // Verifying second part of test - division
    MUnit* resdata2 = root->GetNode("./test/DivResData/Capsule/out");
    CPPUNIT_ASSERT_MESSAGE("Fail to get res data out", resdata2 != 0);
    MDIntGet* rdataget2 = (MDIntGet*) resdata2->GetSIfi(MDIntGet::Type());
    CPPUNIT_ASSERT_MESSAGE("Fail to get rdata out Get iface", rdataget2 != 0);
    TInt rdataval2 = rdataget2->Value();
    CPPUNIT_ASSERT_MESSAGE("Incorrect rdata value", rdataval2 == 4);

    delete iEnv;
}

void Ut_func::test_FuncVar1()
{
    printf("\n === Test of functions: Addition of variable data\n");

    for (int ct = 0; ct < 1; ct++) {
	const string specn("ut_func_var1");
	string ext = ct == 0 ? "xml" : "chs";
	string spec = specn + string(".") + ext;
	string log = specn + "_" + ext + ".log";
	iEnv = new Env(spec, log);
	CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
	iEnv->ImpsMgr()->ResetImportsPaths();
	iEnv->ImpsMgr()->AddImportsPaths("../modules");
	iEnv->ConstructSystem();
	MUnit* root = iEnv->Root();
	CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);

	MUnit* dir = root->GetNode("./Start/Incaps_root/e2");
	MElem* edir = dir->GetObj(edir);
	ChromoNode mut1 = edir->AppendMutation(ENt_Cont);
	mut1.SetAttr(ENa_MutNode, "./P1");
	mut1.SetAttr(ENa_Ref, "/Root/Start/Incaps_root/Inp_data1/Capsule/out");
	TNs ns; MutCtx mctx(NULL, ns);
	edir->Mutate(false, false, true, mctx);

	// Check the output data
	MUnit* resdataprop1 = root->GetNode("/Root/Start/Incaps_root/Dt/Value");
	CPPUNIT_ASSERT_MESSAGE("Fail to get result data value property when inp data changed", resdataprop1 != 0);
	MProp* rdmprop1 = resdataprop1->GetObj(rdmprop1);
	const string& rdval1 = rdmprop1->Value();
	CPPUNIT_ASSERT_MESSAGE("Incorrect result data prop value when inp data changed", rdval1 == "F 4");
	MUnit* resdata1 = root->GetNode("/Root/Start/Incaps_root/Dt/Capsule/out");
	MDVarGet* resdata1g = (MDVarGet*) resdata1->GetSIfi(MDVarGet::Type());
	CPPUNIT_ASSERT_MESSAGE("Cannot get MDVarGet from result data", resdata1g != NULL);
	MDFloatGet* resdata1f = resdata1g->GetDObj(resdata1f);
	CPPUNIT_ASSERT_MESSAGE("Cannot get MDFloatGet from result data", resdata1f != NULL);
	float res1 = resdata1f->Value();
	CPPUNIT_ASSERT_MESSAGE("Incorrect result data", res1 == 4.0);
	delete iEnv;
    }
}

void Ut_func::test_FuncVar1mc()
{
    printf("\n === Test of functions: Addition of variable data (multicontent)\n");

    for (int ct = 1; ct < 2; ct++) {
	const string specn("ut_func_var1mc");
	string ext = ct == 0 ? "xml" : "chs";
	string spec = specn + string(".") + ext;
	string log = specn + string(ct == 0 ? "_xml" : "_chs") + ".log";
	iEnv = new Env(spec, log);
	CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
	iEnv->ImpsMgr()->ResetImportsPaths();
	iEnv->ImpsMgr()->AddImportsPaths("../modules");
	iEnv->ConstructSystem();
	MUnit* root = iEnv->Root();
	CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != NULL);
	MElem* eroot = root->GetObj(eroot);
	CPPUNIT_ASSERT_MESSAGE("Fail to get eroot", eroot != NULL);

	string savedfn = specn + "_saved." + ext;
	eroot->Chromos().Save(savedfn);

	MUnit* datas = root->GetNode("/Root/Modules/DataComps/DataS");
	MElem* edatas = datas->GetObj(edatas);

	MUnit* dir = root->GetNode("./Start/Incaps_root/e2");
	MElem* edir = dir->GetObj(edir);
	ChromoNode mut1 = edir->AppendMutation(ENt_Cont);
	mut1.SetAttr(ENa_MutNode, "./P1");
	mut1.SetAttr(ENa_Ref, "/Root/Start/Incaps_root/Inp_data1/Capsule/out");
	TNs ns; MutCtx mctx(NULL, ns);
	edir->Mutate(false, false, true, mctx);

	// Check the output data
	MUnit* resdataprop1 = root->GetNode("/Root/Start/Incaps_root/Dt/Value");
	CPPUNIT_ASSERT_MESSAGE("Fail to get result data value property when inp data changed", resdataprop1 != 0);
	MProp* rdmprop1 = resdataprop1->GetObj(rdmprop1);
	const string& rdval1 = rdmprop1->Value();
	CPPUNIT_ASSERT_MESSAGE("Incorrect result data prop value when inp data changed", rdval1 == "F 4");
	MUnit* resdata1 = root->GetNode("/Root/Start/Incaps_root/Dt/Capsule/out");
	MDVarGet* resdata1g = (MDVarGet*) resdata1->GetSIfi(MDVarGet::Type());
	CPPUNIT_ASSERT_MESSAGE("Cannot get MDVarGet from result data", resdata1g != NULL);
	MDFloatGet* resdata1f = resdata1g->GetDObj(resdata1f);
	CPPUNIT_ASSERT_MESSAGE("Cannot get MDFloatGet from result data", resdata1f != NULL);
	float res1 = resdata1f->Value();
	CPPUNIT_ASSERT_MESSAGE("Incorrect result data", res1 == 4.0);
	delete iEnv;
    }
}

// This ut verifies two-directional negotiation of type
void Ut_func::test_FuncVar2()
{
    printf("\n === Test of functions: Two-directional negotiation of type\n");

    iEnv = new Env("ut_func_var2.xml", "ut_func_var2.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ImpsMgr()->ResetImportsPaths();
    iEnv->ImpsMgr()->AddImportsPaths("../modules");
    iEnv->ConstructSystem();
    MUnit* root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);

    /*
       MUnit* dir = root->GetNode("Start/Incaps_root");
       ChromoNode mut1 = dir->AppendMutation(ENt_Cont);
       mut1.SetAttr(ENa_MutNode, "e2/P1");
       mut1.SetAttr(ENa_Ref, "/Root/Start/Incaps_root/Inp_data1/Capsule/out");
       dir->Mutate();
       */


    // Check the output data
    MUnit* resdataprop1 = root->GetNode("/Root/Start/Incaps_root/Dt/Value");
    CPPUNIT_ASSERT_MESSAGE("Fail to get result data value property when inp data changed", resdataprop1 != 0);
    MProp* rdmprop1 = resdataprop1->GetObj(rdmprop1);
    const string& rdval1 = rdmprop1->Value();
    CPPUNIT_ASSERT_MESSAGE("Incorrect result data prop value when inp data changed", rdval1 == "B false");
    MUnit* resdata1 = root->GetNode("/Root/Start/Incaps_root/Dt/Capsule/out");
    MDVarGet* resdata1g = (MDVarGet*) resdata1->GetSIfi(MDVarGet::Type());
    CPPUNIT_ASSERT_MESSAGE("Cannot get MDVarGet from result data", resdata1g != NULL);
    MDBoolGet* resdata1f = resdata1g->GetDObj(resdata1f);
    CPPUNIT_ASSERT_MESSAGE("Cannot get MDFloatGet from result data", resdata1f != NULL);
    TBool res1 = resdata1f->Value();
    CPPUNIT_ASSERT_MESSAGE("Incorrect result data", res1 == EFalse);

    // Checking the type establishing
    // Mutate the result data first
    MUnit* dinp = root->GetNode("./Start/Incaps_root/Inp_data1");
    MElem* edinp = dinp->GetObj(edinp);
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    ChromoNode nchange = edinp->AppendMutation(ENt_Cont);
    nchange.SetAttr(ENa_MutNode, "./Value");
    nchange.SetAttr(ENa_MutVal, "I 80");
    TNs ns; MutCtx mctx(NULL, ns);
    edinp->Mutate(false, false, true, mctx);

    delete iEnv;
}
