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

    iEnv = new Env("ut_des_cre1.xml", "ut_des_cre1.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ImpsMgr()->ResetImportsPaths();
    iEnv->ImpsMgr()->AddImportsPaths("../modules");
    iEnv->ConstructSystem();
    MUnit* root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    // Socket doesn't support obtaining iface thru its pins, so access via pin directly but not via extender
    MUnit* doutp = root->GetNode("./test/State1/Capsule/Out/Int/PinData");
    CPPUNIT_ASSERT_MESSAGE("Fail to get state out", doutp != 0);
//    MDIntGet* doutpget = doutp->GetObj(doutpget);
    MDIntGet* doutpget = (MDIntGet*) doutp->GetSIfi(MDIntGet::Type());
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
	    sync->Update();
	}
	if (sync->IsUpdated()) {
	    sync->Confirm();
	}
    }
    CPPUNIT_ASSERT_MESSAGE("Fail to get value of data iface", doutpget->Value() == 5);

    delete iEnv;
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
