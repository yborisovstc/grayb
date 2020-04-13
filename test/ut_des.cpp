#include <env.h>
#include <stdlib.h>
#include <elem.h>
#include <mvert.h>
#include <mdata.h>
#include <mdes.h>

#include <cppunit/extensions/HelperMacros.h>

/*
 * This test is for checking the functionality of creation graph
 */


class Ut_des : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(Ut_des);
    CPPUNIT_TEST(test_Cre1);
    CPPUNIT_TEST(test_Cre2);
    CPPUNIT_TEST(test_Cre4);
    CPPUNIT_TEST(test_Cre5mc);
    CPPUNIT_TEST(test_Cre6mcm);
    CPPUNIT_TEST(test_Cre6mcmu);
    CPPUNIT_TEST(test_Cre6mcmau);
    CPPUNIT_TEST(test_CreStatec);
    CPPUNIT_TEST_SUITE_END();
public:
    virtual void setUp();
    virtual void tearDown();
private:
    void test_Cre1();
    void test_Cre2();
    void test_Cre4();
    void test_Cre5mc();
    void test_Cre6mcm();
    void test_Cre6mcmu();
    void test_Cre6mcmau();
    void test_CreStatec();
private:
    Env* iEnv;
};

CPPUNIT_TEST_SUITE_REGISTRATION( Ut_des );
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(Ut_des, "Ut_des");


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

    for (int ct = 1; ct < 2; ct++) {
	const string specn("ut_des_cre1");
	string ext = (ct == 0) ? "xml" : "chs";
	string spec = specn + string(".") + ext;
	string log = specn + "_" + ext + ".log";
	iEnv = new Env(spec, log);
	CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
	iEnv->ImpsMgr()->ResetImportsPaths();
	iEnv->ImpsMgr()->AddImportsPaths("../modules");
	iEnv->ConstructSystem();
	MUnit* root = iEnv->Root();
	CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
	// Socket doesn't support obtaining iface thru its pins, so access via pin directly but not via extender
	MUnit* doutp = root->GetNode("./test/State1/Capsule/Out/Int/PinData");
	CPPUNIT_ASSERT_MESSAGE("Fail to get state out", doutp != 0);
	MDIntGet* doutpget = doutp->GetSIfit(doutpget);
	CPPUNIT_ASSERT_MESSAGE("Fail to get data out Get iface", doutpget != 0);
	CPPUNIT_ASSERT_MESSAGE("Fail to get value of data iface", doutpget->Value() == 0);
	// Sync the state
	MUnit* esync = root->GetNode("./test/State1/Capsule/Sync");
	CPPUNIT_ASSERT_MESSAGE("Fail to get input for Syncable iface", esync != 0);
	MDesSyncable* sync = (MDesSyncable*) esync->GetSIfi(MDesSyncable::Type());
	CPPUNIT_ASSERT_MESSAGE("Fail to get Syncable iface", sync != 0);
	// Do some ticks
	const TInt ticksnum = 5;
	for (TInt cnt = 0; cnt < ticksnum; cnt++) {
	    if (sync->IsActive()) {
		sync->Update();
	    }
	    if (sync->IsUpdated()) {
		sync->Confirm();
	    }
	}
	CPPUNIT_ASSERT_MESSAGE("Fail to get value of data iface", doutpget->Value() == 5);

	delete iEnv;
    }
}

void Ut_des::test_Cre2()
{
    printf("\n === Test of hier des and multiple connections\n");

    iEnv = new Env("ut_des_cre2.xml", "ut_des_cre2.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ImpsMgr()->ResetImportsPaths();
    iEnv->ImpsMgr()->AddImportsPaths("../modules");
    iEnv->ConstructSystem();
    MUnit* root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    // Socket doesn't support obtaining iface thru its pins, so access via pin directly but not via extender
    MUnit* doutp = root->GetNode("./TestDes/Capsule/Out/Int/PinData");
    CPPUNIT_ASSERT_MESSAGE("Fail to get DES out", doutp != 0);
    //MDIntGet* doutpget = doutp->GetObj(doutpget);
    MDIntGet* doutpget = (MDIntGet*) doutp->GetSIfi(MDIntGet::Type());
    CPPUNIT_ASSERT_MESSAGE("Fail to get data out Get iface", doutpget != 0);
    CPPUNIT_ASSERT_MESSAGE("Fail to get initial value of data iface", doutpget->Value() == 0);
    // Get Sync iface of DES
    MUnit* esync = root->GetNode("./TestDes/Capsule/Sync");
    CPPUNIT_ASSERT_MESSAGE("Fail to get DES's Syncable iface", esync != 0);
    MDesSyncable* sync = (MDesSyncable*) esync->GetSIfi(MDesSyncable::Type());
    CPPUNIT_ASSERT_MESSAGE("Fail to get Syncable iface", sync != 0);
    // Do some ticks
    const TInt ticksnum = 5;
    for (TInt cnt = 0; cnt < ticksnum; cnt++) {
	if (sync->IsActive()) {
	    sync->Update();
	}
	if (sync->IsUpdated()) {
	    sync->Confirm();
	}
    }
    CPPUNIT_ASSERT_MESSAGE("Fail to get value of data iface", doutpget->Value() == 5);

    delete iEnv;
}

void Ut_des::test_Cre4()
{
    printf("\n === Test of Snails racing\n");

    iEnv = new Env("ut_des_cre4.xml", "ut_des_cre4.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ImpsMgr()->ResetImportsPaths();
    iEnv->ImpsMgr()->AddImportsPaths("../modules");
    iEnv->ConstructSystem();
    MUnit* root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    MDIntGet* doutpget = (MDIntGet*) root->GetSIfi("./TestDes/Snail_1/Capsule/Out_Mass/Int/PinData", MDIntGet::Type());
    CPPUNIT_ASSERT_MESSAGE("Fail to get data out Get iface", doutpget != 0);
    CPPUNIT_ASSERT_MESSAGE("Fail to get initial value of data iface", doutpget->Value() == 1);
    // Get Sync iface of DES
    MUnit* esync = root->GetNode("./TestDes/Capsule/Sync");
    CPPUNIT_ASSERT_MESSAGE("Fail to get DES's Syncable iface", esync != 0);
    MDesSyncable* sync = (MDesSyncable*) esync->GetSIfi(MDesSyncable::Type());
    CPPUNIT_ASSERT_MESSAGE("Fail to get Syncable iface", sync != 0);
    // Do some ticks
    const TInt ticksnum = 11;
    for (TInt cnt = 0; cnt < ticksnum; cnt++) {
	iEnv->Logger()->WriteFormat("[Test] Tick #%4d", cnt);
	if (sync->IsActive()) {
	    sync->Update();
	}
	if (sync->IsUpdated()) {
	    sync->Confirm();
	}
    }
    CPPUNIT_ASSERT_MESSAGE("Fail to get final value of data iface", doutpget->Value() == 10);

    delete iEnv;
}

void Ut_des::test_Cre5mc()
{
    printf("\n === Test of creation of simple des: var, multicontent\n");

    iEnv = new Env("ut_des_cre1vmc.xml", "ut_des_cre1vmc.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ImpsMgr()->ResetImportsPaths();
    iEnv->ImpsMgr()->AddImportsPaths("../modules");
    iEnv->ConstructSystem();
    MUnit* root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    // Socket doesn't support obtaining iface thru its pins, so access via pin directly but not via extender
    MUnit* doutp = root->GetNode("./test/State1/Capsule/Out/Int/PinData");
    CPPUNIT_ASSERT_MESSAGE("Fail to get state out", doutp != 0);
    MDVarGet* doutpget = (MDVarGet*) doutp->GetSIfi(MDVarGet::Type());
    CPPUNIT_ASSERT_MESSAGE("Fail to get data out Get iface", doutpget != 0);
    MDIntGet* doutpgetp= (MDIntGet*) doutpget->DoGetDObj(MDIntGet::Type());
    CPPUNIT_ASSERT_MESSAGE("Fail to get data out IntGet iface", doutpgetp != 0);
    CPPUNIT_ASSERT_MESSAGE("Fail to get value of data iface", doutpgetp->Value() == 0);
    // Sync the state
    MUnit* esync = root->GetNode("./test/State1/Capsule/Sync");
    CPPUNIT_ASSERT_MESSAGE("Fail to get input for Syncable iface", esync != 0);
    MDesSyncable* sync = (MDesSyncable*) esync->GetSIfi(MDesSyncable::Type());
    CPPUNIT_ASSERT_MESSAGE("Fail to get Syncable iface", sync != 0);
    // Do some ticks
    const TInt ticksnum = 5;
    for (TInt cnt = 0; cnt < ticksnum; cnt++) {
	if (sync->IsActive()) {
	    sync->Update();
	}
	if (sync->IsUpdated()) {
	    sync->Confirm();
	}
    }
    CPPUNIT_ASSERT_MESSAGE("Fail to get value of data iface", doutpgetp->Value() == 5);

    delete iEnv;
}

void Ut_des::test_Cre6mcm()
{
    printf("\n === Test of creation of simple des: var, multicontent, monolitic extd\n");

    iEnv = new Env("ut_des_cre1vmcm.xml", "ut_des_cre1vmcm.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ImpsMgr()->ResetImportsPaths();
    iEnv->ImpsMgr()->AddImportsPaths("../modules");
    iEnv->ConstructSystem();
    MUnit* root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    // Socket doesn't support obtaining iface thru its pins, so access via pin directly but not via extender
    MUnit* doutp = root->GetNode("./test/State1/Capsule/Out/Int/PinData");
    CPPUNIT_ASSERT_MESSAGE("Fail to get state out", doutp != 0);
    MDVarGet* doutpget = (MDVarGet*) doutp->GetSIfi(MDVarGet::Type());
    CPPUNIT_ASSERT_MESSAGE("Fail to get data out Get iface", doutpget != 0);
    MDIntGet* doutpgetp= (MDIntGet*) doutpget->DoGetDObj(MDIntGet::Type());
    CPPUNIT_ASSERT_MESSAGE("Fail to get data out IntGet iface", doutpgetp != 0);
    CPPUNIT_ASSERT_MESSAGE("Fail to get value of data iface", doutpgetp->Value() == 0);
    // Sync the state
    MUnit* esync = root->GetNode("./test/State1/Capsule/Sync");
    CPPUNIT_ASSERT_MESSAGE("Fail to get input for Syncable iface", esync != 0);
    MDesSyncable* sync = (MDesSyncable*) esync->GetSIfi(MDesSyncable::Type());
    CPPUNIT_ASSERT_MESSAGE("Fail to get Syncable iface", sync != 0);
    // Do some ticks
    const TInt ticksnum = 5;
    for (TInt cnt = 0; cnt < ticksnum; cnt++) {
	if (sync->IsActive()) {
	    //iEnv->SetSBool(MEnv::ESb_EnIfTrace, ETrue);
	    sync->Update();
	    //iEnv->SetSBool(MEnv::ESb_EnIfTrace, EFalse);
	}
	if (sync->IsUpdated()) {
	    sync->Confirm();
	}
    }
    TInt val = doutpgetp->Value();
    CPPUNIT_ASSERT_MESSAGE("Fail to get value of data iface", val == 5);

    delete iEnv;
}

void Ut_des::test_Cre6mcmu()
{
    printf("\n === Test of creation of simple des: var, multicontent, monolitic extd, unit pins\n");

    iEnv = new Env("ut_des_cre1vmcmu.xml", "ut_des_cre1vmcmu.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ImpsMgr()->ResetImportsPaths();
    iEnv->ImpsMgr()->AddImportsPaths("../modules");
    iEnv->ConstructSystem();
    MUnit* root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    // Socket doesn't support obtaining iface thru its pins, so access via pin directly but not via extender
    MUnit* doutp = root->GetNode("./test/State1/Capsule/Out/Int/PinData");
    CPPUNIT_ASSERT_MESSAGE("Fail to get state out", doutp != 0);
    MDVarGet* doutpget = (MDVarGet*) doutp->GetSIfi(MDVarGet::Type());
    CPPUNIT_ASSERT_MESSAGE("Fail to get data out Get iface", doutpget != 0);
    MDIntGet* doutpgetp= (MDIntGet*) doutpget->DoGetDObj(MDIntGet::Type());
    CPPUNIT_ASSERT_MESSAGE("Fail to get data out IntGet iface", doutpgetp != 0);
    CPPUNIT_ASSERT_MESSAGE("Fail to get value of data iface", doutpgetp->Value() == 0);
    // Sync the state
    MUnit* esync = root->GetNode("./test/State1/Capsule/Sync");
    CPPUNIT_ASSERT_MESSAGE("Fail to get input for Syncable iface", esync != 0);
    MDesSyncable* sync = (MDesSyncable*) esync->GetSIfi(MDesSyncable::Type());
    CPPUNIT_ASSERT_MESSAGE("Fail to get Syncable iface", sync != 0);
    // Do some ticks
    const TInt ticksnum = 5;
    for (TInt cnt = 0; cnt < ticksnum; cnt++) {
	if (sync->IsActive()) {
	    //iEnv->SetSBool(MEnv::ESb_EnIfTrace, ETrue);
	    sync->Update();
	    //iEnv->SetSBool(MEnv::ESb_EnIfTrace, EFalse);
	}
	if (sync->IsUpdated()) {
	    sync->Confirm();
	}
    }
    TInt val = doutpgetp->Value();
    CPPUNIT_ASSERT_MESSAGE("Fail to get value of data iface", val == 5);

    delete iEnv;
}

void Ut_des::test_Cre6mcmau()
{
    printf("\n === Test of creation of simple des: var, multicontent, monolitic agent and cps, unit pins\n");

    iEnv = new Env("ut_des_cre1vmcmau.xml", "ut_des_cre1vmcmau.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ImpsMgr()->ResetImportsPaths();
    iEnv->ImpsMgr()->AddImportsPaths("../modules");
    iEnv->ConstructSystem();
    MUnit* root = iEnv->Root();
    MElem* eroot = root->GetObj(eroot);
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != NULL && eroot != NULL);
    // Socket doesn't support obtaining iface thru its pins, so access via pin directly but not via extender
    MUnit* doutp = root->GetNode("./test/State1/Capsule/Out/Int/PinData");
    CPPUNIT_ASSERT_MESSAGE("Fail to get state out", doutp != 0);
    MDVarGet* doutpget = (MDVarGet*) doutp->GetSIfi(MDVarGet::Type());
    CPPUNIT_ASSERT_MESSAGE("Fail to get data out Get iface", doutpget != 0);
    MDIntGet* doutpgetp= (MDIntGet*) doutpget->DoGetDObj(MDIntGet::Type());
    CPPUNIT_ASSERT_MESSAGE("Fail to get data out IntGet iface", doutpgetp != 0);
    CPPUNIT_ASSERT_MESSAGE("Fail to get value of data iface", doutpgetp->Value() == 0);
    // Sync the state
    MUnit* esync = root->GetNode("./test/State1/Capsule/Sync");
    CPPUNIT_ASSERT_MESSAGE("Fail to get input for Syncable iface", esync != 0);
    MDesSyncable* sync = (MDesSyncable*) esync->GetSIfi(MDesSyncable::Type());
    CPPUNIT_ASSERT_MESSAGE("Fail to get Syncable iface", sync != 0);
    // Do some ticks
    const TInt ticksnum = 5;
    for (TInt cnt = 0; cnt < ticksnum; cnt++) {
	if (sync->IsActive()) {
	    //iEnv->SetSBool(MEnv::ESb_EnIfTrace, ETrue);
	    sync->Update();
	    //iEnv->SetSBool(MEnv::ESb_EnIfTrace, EFalse);
	}
	if (sync->IsUpdated()) {
	    sync->Confirm();
	}
    }
    TInt val = doutpgetp->Value();
    CPPUNIT_ASSERT_MESSAGE("Fail to get value of data iface", val == 5);

    delete iEnv;
}

// The test uses similar model as test_Cre6mcmau
void Ut_des::test_CreStatec()
{
    printf("\n === Test of creation of simple des using AStatec\n");

    for (int ct = 1; ct < 2; ct++) {
	const string specn("ut_des_cre_statec");
	string ext = ct == 0 ? "xml" : "chs";
	string spec = specn + string(".") + ext;
	string log = specn + "_" + ext + ".log";
	iEnv = new Env(spec, log);
	CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
	iEnv->ImpsMgr()->ResetImportsPaths();
	iEnv->ImpsMgr()->AddImportsPaths("../modules");
	iEnv->ConstructSystem();
	MUnit* root = iEnv->Root();
	MElem* eroot = root->GetObj(eroot);
	CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != NULL && eroot != NULL);
	string saved_name = specn + "_saved." + ext;
	eroot->Chromos().Save(saved_name);

	MUnit* trans = root->GetNode("./test/Trans");
	CPPUNIT_ASSERT_MESSAGE("Fail to get trans", trans != 0);
	MUnit* doutp = root->GetNode("./test/State");
	CPPUNIT_ASSERT_MESSAGE("Fail to get state out", doutp != 0);
	MDVarGet* doutpget = (MDVarGet*) doutp->GetSIfi(MDVarGet::Type());
	CPPUNIT_ASSERT_MESSAGE("Fail to get data out Get iface", doutpget != 0);
	MDIntGet* doutpgetp= (MDIntGet*) doutpget->DoGetDObj(MDIntGet::Type());
	CPPUNIT_ASSERT_MESSAGE("Fail to get data out IntGet iface", doutpgetp != 0);
	CPPUNIT_ASSERT_MESSAGE("Fail to get value of data iface", doutpgetp->Value() == 0);
	// Sync the state
	MUnit* esync = root->GetNode("./test");
	CPPUNIT_ASSERT_MESSAGE("Fail to get input for Syncable iface", esync != NULL);
	MDesSyncable* sync = (MDesSyncable*) esync->GetSIfi(MDesSyncable::Type());
	CPPUNIT_ASSERT_MESSAGE("Fail to get Syncable iface", sync != NULL);
	// Natively set Incr2
	MUnit* incr2u = root->GetNode("./test/IncrData2");
	CPPUNIT_ASSERT_MESSAGE("Fail to get IncrData2 unit", incr2u != NULL);
	MDVarSet* incr2s = incr2u->GetObj(incr2s);
	CPPUNIT_ASSERT_MESSAGE("Fail to get IncrData2 MDVarSet iface", incr2s != NULL);
	MDIntSet* incr2is = (MDIntSet*)incr2s->DoGetSDObj(MDIntSet::Type());
	CPPUNIT_ASSERT_MESSAGE("Fail to get IncrData2 MDIntSet iface", incr2is != NULL);
	incr2is->SetValue(1);
	// Natively set Data3
	MUnit* d3u = root->GetNode("./test/Data3");
	CPPUNIT_ASSERT_MESSAGE("Fail to get Data3 unit", d3u != NULL);
	MDVarSet* d3s = d3u->GetObj(d3s);
	CPPUNIT_ASSERT_MESSAGE("Fail to get Data3 MDVarSet iface", d3s != NULL);
	MDtSet<Sdata<TInt>>* d3is = dynamic_cast<MDtSet<Sdata<TInt>>*>(d3s->DoGetSDObj(MDtSet<Sdata<TInt>>::Type()));
	CPPUNIT_ASSERT_MESSAGE("Fail to get Data3 MDIntSet iface", d3is != NULL);
	d3is->DtSet(5);

	// Do some ticks
	const TInt ticksnum = 5;
	for (TInt cnt = 0; cnt < ticksnum; cnt++) {
	    if (sync->IsActive()) {
		//iEnv->SetSBool(MEnv::ESb_EnIfTrace, ETrue);
		sync->Update();
		//iEnv->SetSBool(MEnv::ESb_EnIfTrace, EFalse);
	    }
	    if (sync->IsUpdated()) {
		sync->Confirm();
	    }
	}
	TInt val = doutpgetp->Value();
	CPPUNIT_ASSERT_MESSAGE("Fail to get value of data iface", val == 9);

	delete iEnv;
    }
}
