#include <env.h>
#include <stdlib.h>
#include <elem.h>
#include <mvert.h>
#include <mdata.h>
#include <medge.h>
#include <mdes.h>

#include <cppunit/extensions/HelperMacros.h>

/*
 * This test is for checking the functionality of interfaces caching
 */


class Ut_icache : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(Ut_icache);
    CPPUNIT_TEST(test_Inv1);
    CPPUNIT_TEST_SUITE_END();
public:
    virtual void setUp();
    virtual void tearDown();
private:
    void test_Inv1();
private:
    Env* iEnv;
};

CPPUNIT_TEST_SUITE_REGISTRATION( Ut_icache );


void Ut_icache::setUp()
{
}

void Ut_icache::tearDown()
{
//    delete iEnv;
    CPPUNIT_ASSERT_EQUAL_MESSAGE("tearDown", 0, 0);
}

void Ut_icache::test_Inv1()
{
    printf("\n === Tests invalidation of function add inputs ifaces cache after feedback edge has been connected\n");

    iEnv = new Env("Env", "ut_icache_inv1.xml", "ut_icache_inv1.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ImpsMgr()->ResetImportsPaths();
    iEnv->ImpsMgr()->AddImportsPaths("../modules");
    iEnv->ConstructSystem();
    Elem* root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    // Socket doesn't support obtaining iface thru its pins, so access via pin directly but not via extender
    Elem* doutp = root->GetNode("/Root/IncapsRoot/DesRoot/st/Capsule/Out/Int/PinData");
    CPPUNIT_ASSERT_MESSAGE("Fail to get state out", doutp != 0);
    MDVarGet* doutpget = (MDVarGet*) doutp->GetSIfi(MDVarGet::Type());
    CPPUNIT_ASSERT_MESSAGE("Fail to get data out Get iface", doutpget != 0);
    MDFloatGet* fget = doutpget->GetDObj(fget);
    //MDFloatGet* fget = (MDFloatGet*) doutpget->GetSIfi(MDFloatGet::Type(), NULL);
    CPPUNIT_ASSERT_MESSAGE("Wrong value of data iface", fget->Value() == 0);
    // Sync the state
    Elem* esync = root->GetNode("/Root/IncapsRoot/DesRoot/st/Capsule/Sync");
    CPPUNIT_ASSERT_MESSAGE("Fail to get input for Syncable iface", esync != 0);
    MDesSyncable* sync = (MDesSyncable*) esync->GetSIfi(MDesSyncable::Type(), NULL);
    CPPUNIT_ASSERT_MESSAGE("Fail to get Syncable iface", sync != 0);

    // Do some ticks just to fillout ifaces caches
    TInt ticksnum = 2;
    for (TInt cnt = 0; cnt < ticksnum; cnt++) {
	if (sync->IsActive()) {
	    sync->Update();
	}
	if (sync->IsUpdated()) {
	    sync->Confirm();
	}
    }

    // Connect feedback edge
    Elem* mnode = root->GetNode("/Root/IncapsRoot/DesRoot/E_back");
    ChromoNode mut = mnode->Mutation().Root().AddChild(ENt_Cont);
    mut.SetAttr(ENa_MutNode, "./P1");
    mut.SetAttr(ENa_Ref, "/Root/IncapsRoot/DesRoot/st/Capsule/Inp");
    mnode->Mutate();

    // Do some ticks
    ticksnum = 5;
    for (TInt cnt = 0; cnt < ticksnum; cnt++) {
	if (sync->IsActive()) {
	    sync->Update();
	}
	if (sync->IsUpdated()) {
	    sync->Confirm();
	}
    }

    CPPUNIT_ASSERT_MESSAGE("Wrong value of data iface", fget->Value() >= 0.6 && fget->Value() < 0.600001);

    delete iEnv;
}

