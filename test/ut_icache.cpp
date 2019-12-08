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
    CPPUNIT_TEST(test_InvMAgent);
    CPPUNIT_TEST_SUITE_END();
public:
    virtual void setUp();
    virtual void tearDown();
private:
    void test_Inv1();
    void test_InvMAgent();
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

    iEnv = new Env("ut_icache_inv1.xml", "ut_icache_inv1.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ImpsMgr()->ResetImportsPaths();
    iEnv->ImpsMgr()->AddImportsPaths("../modules");
    iEnv->ConstructSystem();
    MUnit* root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    // Socket doesn't support obtaining iface thru its pins, so access via pin directly but not via extender
    MUnit* doutp = root->GetNode("/Root/IncapsRoot/DesRoot/st/Capsule/Out/Int/PinData");
    CPPUNIT_ASSERT_MESSAGE("Failed to get state out", doutp != 0);
    MDVarGet* doutpget = (MDVarGet*) doutp->GetSIfi(MDVarGet::Type());
    CPPUNIT_ASSERT_MESSAGE("Failed to get data out Get iface", doutpget != 0);
    MDFloatGet* fget = doutpget->GetDObj(fget);
    //MDFloatGet* fget = (MDFloatGet*) doutpget->GetSIfi(MDFloatGet::Type(), NULL);
    CPPUNIT_ASSERT_MESSAGE("Failed to get data iface", fget != 0);
    CPPUNIT_ASSERT_MESSAGE("Wrong value of data iface", fget->Value() == 0);
    // Sync the state
    MUnit* esync = root->GetNode("/Root/IncapsRoot/DesRoot/st/Capsule/Sync");
    CPPUNIT_ASSERT_MESSAGE("Fail to get input for Syncable iface", esync != 0);
    MDesSyncable* sync = (MDesSyncable*) esync->GetSIfi(MDesSyncable::Type());
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
    MUnit* mnode = root->GetNode("/Root/IncapsRoot/DesRoot/E_back");
    MElem* emnode = mnode->GetObj(emnode);
    ChromoNode mut = emnode->AppendMutation(ENt_Cont);
    mut.SetAttr(ENa_MutNode, "./P1");
    mut.SetAttr(ENa_Ref, "/Root/IncapsRoot/DesRoot/st/Capsule/Inp");
    emnode->Mutate(false, false, true, MutCtx());

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

/**
 * @brief Verifies if iface cache invalidates for MAgent when MAgent comp adding/deleting
 */
void Ut_icache::test_InvMAgent()
{
    printf("\n === Tests invalidation for MAgent when MAgent comp adding/deleting\n");

    iEnv = new Env("ut_icache_inv_magent.xml", "ut_icache_inv_magent.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ImpsMgr()->ResetImportsPaths();
    iEnv->ImpsMgr()->AddImportsPaths("../modules");
    iEnv->ConstructSystem();
    MUnit* root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);

    // Mutate the input data first
    MUnit* test = root->GetNode("./test");
    MElem* etest = test->GetObj(etest);
    ChromoNode madd = etest->AppendMutation(ENt_Node);
    madd.SetAttr(ENa_Id, "MAExteder");
    madd.SetAttr(ENa_Parent, "ExtenderAgent");

    // Cache contains only one item - incaps agent
    MIfProv::TIfRange rg = test->GetIfi(MAgent::Type());
    TInt cnt = 0;
    for (MIfProv::TIfIter it = rg.first; it != rg.second; it++, cnt++);
    CPPUNIT_ASSERT_MESSAGE("Wrong number of MAgent ifaces on first request", cnt == 1);
    MAgent* agt = (MAgent*) *rg.first;
    MUnit* agte = dynamic_cast<MUnit*>(agt->DoGetIface(MUnit::Type()));
    CPPUNIT_ASSERT_MESSAGE("Wrong MAgent iface on first request", agte->GetUri(NULL,true) == "/testroot/test/MACompsObserver");

    etest->Mutate(false, false, true, MutCtx());

    // Cache should be invalidated here, so GetIfi refreshes cache and add two agetns to cache - incaps and extender
    rg = test->GetIfi(MAgent::Type());
    cnt = 0;
    for (MIfProv::TIfIter it = rg.first; it != rg.second; it++, cnt++);
    CPPUNIT_ASSERT_MESSAGE("Wrong number of MAgent ifaces on second request", cnt == 2);
    MIfProv::TIfIter it = rg.first;
    agt = (MAgent*) *it;
    agte = dynamic_cast<MUnit*>(agt->DoGetIface(MUnit::Type()));
    CPPUNIT_ASSERT_MESSAGE("Wrong MAgent 1-st iface on second request", agte->GetUri(NULL,true) == "/testroot/test/MACompsObserver");
    it++;
    agt = (MAgent*) *it;
    agte = dynamic_cast<MUnit*>(agt->DoGetIface(MUnit::Type()));
    CPPUNIT_ASSERT_MESSAGE("Wrong MAgent 2-nd iface on second request", agte->GetUri(NULL,true) == "/testroot/test/MAExteder");


    delete iEnv;
}
