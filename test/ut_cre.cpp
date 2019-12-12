#include <stdlib.h>
#include <elem.h>
#include <mvert.h>
#include <mdata.h>
#include <env.h>

#include <cppunit/extensions/HelperMacros.h>

/*
 * This test is for checking the functionality of creation graph
 */

class AgentObserver: public MAgentObserver
{
    public:
	AgentObserver(MEnv* aEnv): mEnv(aEnv) {};
	// From MAgentObserver
	virtual void OnCompDeleting(MUnit& aComp, TBool aSoft = ETrue, TBool aModif = EFalse);
	virtual void OnCompAdding(MUnit& aComp, TBool aModif = EFalse);
	virtual TBool OnCompChanged(MUnit& aComp, const string& aContName = string(), TBool aModif = EFalse);
	virtual TBool OnChanged(MUnit& aComp);
	virtual TBool OnCompRenamed(MUnit& aComp, const string& aOldName);
	virtual void OnCompMutated(const MUnit* aNode);
	MIface* Call(const string& aSpec, string& aRes) override { return NULL;};
	string Mid() const override { return string();};
    private:
	MEnv* mEnv; 
};

class Ut_cre : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(Ut_cre);
    CPPUNIT_TEST(test_Cre);
    CPPUNIT_TEST(test_Creu);
    CPPUNIT_TEST(test_BaseApis1);
    CPPUNIT_TEST(test_Content);
    CPPUNIT_TEST(test_CreGr);
    CPPUNIT_TEST(test_CreSyst);
    CPPUNIT_TEST(test_CreSystu);
    CPPUNIT_TEST(test_CreIncaps);
    CPPUNIT_TEST(test_CreData);
    CPPUNIT_TEST(test_Iface);
    CPPUNIT_TEST(test_Notif);
    CPPUNIT_TEST_SUITE_END();
public:
    virtual void setUp();
    virtual void tearDown();
private:
    void test_Cre();
    void test_Creu();
    void test_CreGr();
    void test_CreSyst();
    void test_CreSystu();
    void test_CreIncaps();
    void test_CreData();
    void test_BaseApis1();
    void test_Content();
    void test_Iface();
    void test_Notif();
private:
    Env* iEnv;
    AgentObserver* mAgtObs;
};

CPPUNIT_TEST_SUITE_REGISTRATION( Ut_cre );
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(Ut_cre, "Ut_cre");



void AgentObserver::OnCompDeleting(MUnit& aComp, TBool aSoft, TBool aModif)
{
}

void AgentObserver::OnCompAdding(MUnit& aComp, TBool aModif)
{
    string uid = aComp.Uid();
    cout << "AgentObserver::OnCompAdding " << uid << endl;
    MIface* comp = mEnv->IfaceResolver()->GetIfaceByUid(uid);
    CPPUNIT_ASSERT_MESSAGE("Fail to get comp iface by UID", comp != NULL);
}

TBool AgentObserver::OnCompChanged(MUnit& aComp, const string& aContName, TBool aModif)
{
    string uid = aComp.Uid();
    cout << "AgentObserver::OnCompChanged " << uid << endl;
    MIface* comp = mEnv->IfaceResolver()->GetIfaceByUid(uid);
    CPPUNIT_ASSERT_MESSAGE("Fail to get comp iface by UID", comp != NULL);
}

TBool AgentObserver::OnChanged(MUnit& aComp)
{
}

TBool AgentObserver::OnCompRenamed(MUnit& aComp, const string& aOldName)
{
    string uid = aComp.Uid();
    cout << "AgentObserver::OnCompRenamed " << uid << endl;
    MIface* comp = mEnv->IfaceResolver()->GetIfaceByUid(uid);
    CPPUNIT_ASSERT_MESSAGE("Fail to get comp iface by UID", comp != NULL);
}

void AgentObserver::OnCompMutated(const MUnit* aNode)
{
}


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
    MUnit* root = iEnv->Root();
    MUnit* ee = root->GetNode(":Elem");
    MElem* eroot = root->GetObj(eroot);
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    eroot->Chromos().Save("ut_cre_1_saved.xml");
    MUnit* e2 = root->GetNode("./elem1/elem2");
    CPPUNIT_ASSERT_MESSAGE("Fail to get e2", e2 != 0);
    MUnit* e4 = root->GetNode("./elem3/(elem2:)elem4");
    CPPUNIT_ASSERT_MESSAGE("Fail to get e4", e4 != 0);
    MUnit* e4_he1 = root->GetNode("./elem3/(elem1:)elem4");
#if 0 // TODO Should creation of elem4 be really refused. Why? Disabled atm
    CPPUNIT_ASSERT_MESSAGE("Creation of (elem1:)/elem4 is not refused", e4_he1 == 0);
    Rank rk_e4;
    e4->GetRank(rk_e4, e4->Chromos().Root());
    string srk_e4 = rk_e4.ToString();
    Rank rk_e2;
    e2->GetRank(rk_e2, e2->Chromos().Root());
    string srk_e2 = rk_e2.ToString();
    //CPPUNIT_ASSERT_MESSAGE("Wrong rank of e4 or e2", srk_e4 == "2.1" && srk_e2 == "1.0");
    TBool cmp1 = rk_e2 < rk_e4;
    TBool cmp2 = rk_e4 > rk_e2;
    TBool cmp3 = rk_e4 == rk_e4;
    //CPPUNIT_ASSERT_MESSAGE("Wrong rank comparition of e4 and e2", cmp1 && cmp2 && cmp3);
#endif
    delete iEnv;

    // Recreate the model with osm style chromo
    iEnv = new Env("ut_cre_1_saved.xml", "ut_cre_1_saved.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env for ut_cre_1_saved.xml", iEnv != 0);
    iEnv->ImpsMgr()->ResetImportsPaths();
    iEnv->ImpsMgr()->AddImportsPaths("../modules");
    iEnv->ConstructSystem();
    root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
}

void Ut_cre::test_Creu()
{
    printf("\n === Test of creation of simple model with units\n");

    iEnv = new Env("ut_creu_1.xml", "ut_creu_1.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ImpsMgr()->ResetImportsPaths();
    iEnv->ImpsMgr()->AddImportsPaths("../modules");
    iEnv->ConstructSystem();
    MUnit* root = iEnv->Root();
    MUnit* ee = root->GetNode(":Elem");
    MElem* eroot = root == NULL ? NULL : root->GetObj(eroot);
    CPPUNIT_ASSERT_MESSAGE("Fail to get eroot", eroot != NULL);
    MUnit* unit1_1 = root->GetNode("./elem1/unit1_1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get unit1_1", unit1_1 != NULL);
    MUnit* unit1_1_1 = unit1_1->GetNode("./unit1_1_1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get unit1_1_1", unit1_1_1 != NULL);

    // Saving chromo
    eroot->Chromos().Save("ut_creu_1_saved.xml");
    MUnit* e2 = root->GetNode("./elem1/elem2");
    CPPUNIT_ASSERT_MESSAGE("Fail to get e2", e2 != 0);
    MUnit* e4 = root->GetNode("./elem3/(elem2:)elem4");
    CPPUNIT_ASSERT_MESSAGE("Fail to get e4", e4 != 0);
    MUnit* e4_he1 = root->GetNode("./elem3/(elem1:)elem4");
    delete iEnv;

    // Recreate the model with osm style chromo
    iEnv = new Env("ut_creu_1_saved.xml", "ut_creu_1_saved.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env for ut_creu_1_saved.xml", iEnv != 0);
    iEnv->ImpsMgr()->ResetImportsPaths();
    iEnv->ImpsMgr()->AddImportsPaths("../modules");
    iEnv->ConstructSystem();
    root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    unit1_1_1 = root->GetNode("./elem1/unit1_1/unit1_1_1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get unit1_1_1", unit1_1_1 != NULL);
}

void Ut_cre::test_CreGr()
{
    printf("\n === Test of creation of graph\n");

    iEnv = new Env("ut_cre_gr.xml", "ut_cre_gr.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ImpsMgr()->ResetImportsPaths();
    iEnv->ImpsMgr()->AddImportsPaths("../modules");
    iEnv->ConstructSystem();
    MUnit* root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    MUnit* v1 = root->GetNode("./(Vert:)v1");
    //Elem* av1 = v1->GetNode("(Elem:)Agents");
    CPPUNIT_ASSERT_MESSAGE("Fail to get v1", v1 != 0);
    MVert* mv1 = v1->GetObj(mv1);
    CPPUNIT_ASSERT_MESSAGE("Fail to get mv1", mv1 != 0);
    MVert* pair = mv1->GetPair(0);
    CPPUNIT_ASSERT_MESSAGE("Fail to get pair", pair != 0);
    MUnit* epair = pair->GetObj(epair);
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
    MUnit* root = iEnv->Root();
    MElem* eroot = root->GetObj(eroot);
    root->ChangeCont("root_prop", EFalse);
    root->ChangeCont("yes", EFalse, "Debug.Enable_trace");
    root->ChangeCont("no", EFalse, "Debug.Enable_dbg");
    string cont = root->GetContent("", ETrue);
    TBool cont_ok = (cont == "{'root_prop' About:'' Debug:{ Enable_trace:'yes' Enable_dbg:'no'}}");
    CPPUNIT_ASSERT_MESSAGE("Wrong root content", cont_ok);
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    eroot->Chromos().Save("ut_cre_syst1_saved.xml");
    MUnit* cp1 = root->GetNode("./cp1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get cp1", cp1 != 0);
    MVert* mcp1 = cp1->GetObj(mcp1);
    CPPUNIT_ASSERT_MESSAGE("Fail to get mcp1", mcp1 != 0);
    MVert* pair = mcp1->GetPair(0);
    CPPUNIT_ASSERT_MESSAGE("Fail to get pair", pair != 0);
    MUnit* epair = pair->GetObj(epair);
    const string pname = epair->Name();
    CPPUNIT_ASSERT_MESSAGE("Wrong pair's name", pname == "cp2");
    MUnit* e2 = root->GetNode("./E2");
    MUnit* e2_p1 = e2->GetNode("./P1");
    MElem* ee2 = e2 == NULL ? NULL : e2->GetObj(ee2);
    TBool isatt = ee2->IsCompAttached(e2_p1);
    //CPPUNIT_ASSERT_MESSAGE("e2->IsAttached(e2_p1) returns false", isatt);
    MUnit* e1 = root->GetNode("./E1");
    string cont_e1 = e1->GetContent();

    // Checking extenstion agent
    MUnit* ep = root->GetNode("./Syst1/ep");
    MCompatChecker* ep_cchk = (MCompatChecker*) ep->GetSIfiC(MCompatChecker::Type());
    CPPUNIT_ASSERT_MESSAGE("Fail to get MCompatChecker from ep", ep_cchk != 0);
    MUnit* cp3 = root->GetNode("./cp3");
    MVert* cp3v = cp3->GetObj(cp3v);
    CPPUNIT_ASSERT_MESSAGE("Fail to get cp3", cp3 != 0);
    MVert* cp3v_pair = cp3v->GetPair(0);
    CPPUNIT_ASSERT_MESSAGE("Fail to get pair of cp3", cp3v_pair != 0);
    // Verifying that cp3/Provided chromo is empty, ref ds_uac_snmc
    MUnit* cp3prov = root->GetNode("./cp3/Provided");
    CPPUNIT_ASSERT_MESSAGE("Fail to get cp3/Provided", cp3prov != 0);
    MElem* ecp3prov = cp3prov->GetObj(ecp3prov);
    TInt cp3prov_chr_cnt = ecp3prov->Chromos().Root().Count();
    CPPUNIT_ASSERT_MESSAGE("cp3/Provided chromo is not empty", cp3prov_chr_cnt == 0);
    // Verifying that Syst1/Syst1_1 chromo is empty
    MUnit* s1_1 = root->GetNode("./Syst1/Syst1_1");
    MElem* es1_1 = s1_1->GetObj(es1_1);
    CPPUNIT_ASSERT_MESSAGE("Fail to get ./Syst1/Syst1_1", s1_1 != NULL);
    TInt s1_1_chr_cnt = es1_1->Chromos().Root().Count();
    CPPUNIT_ASSERT_MESSAGE("./Syst1/Syst1_1 chromo is not empty", s1_1_chr_cnt == 0);

    delete iEnv;
}

void Ut_cre::test_CreSystu()
{
    printf("\n === Test of creation of system using unit based components\n");

    iEnv = new Env("ut_cre_systu_1.xml", "ut_cre_systu_1.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ImpsMgr()->ResetImportsPaths();
    iEnv->ImpsMgr()->AddImportsPaths("../modules");
    iEnv->ConstructSystem();
    MUnit* root = iEnv->Root();
    MElem* eroot = root == NULL ? NULL : root->GetObj(eroot);
    CPPUNIT_ASSERT_MESSAGE("Fail to get eroot", eroot != NULL);
    root->ChangeCont("root_prop", EFalse);
    root->ChangeCont("yes", EFalse, "Debug.Enable_trace");
    root->ChangeCont("no", EFalse, "Debug.Enable_dbg");
    string cont = root->GetContent("", ETrue);
    TBool cont_ok = (cont == "{'root_prop' About:'' Debug:{ Enable_trace:'yes' Enable_dbg:'no'}}");
    CPPUNIT_ASSERT_MESSAGE("Wrong root content", cont_ok);
    eroot->Chromos().Save("ut_cre_systu_1_saved.xml");
    MUnit* cp1 = root->GetNode("./cp1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get cp1", cp1 != 0);
    MUnit* s1_scp1 = root->GetNode("./Syst1/scp1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get Syst1/scp1", s1_scp1 != 0);
    MVert* mcp1 = cp1->GetObj(mcp1);
    CPPUNIT_ASSERT_MESSAGE("Fail to get mcp1", mcp1 != 0);
    MVert* pair = mcp1->GetPair(0);
    CPPUNIT_ASSERT_MESSAGE("Fail to get pair", pair != 0);
    MUnit* epair = pair->GetObj(epair);
    const string pname = epair->Name();
    CPPUNIT_ASSERT_MESSAGE("Wrong pair's name", pname == "cp2");
    MUnit* e2 = root->GetNode("./E2");
    MUnit* e2_p1 = e2->GetNode("./P1");
    MElem* ee2 = e2 == NULL ? NULL : e2->GetObj(ee2);
    TBool isatt = ee2->IsCompAttached(e2_p1);
    //CPPUNIT_ASSERT_MESSAGE("e2->IsAttached(e2_p1) returns false", isatt);
    MUnit* e1 = root->GetNode("./E1");
    string cont_e1 = e1->GetContent();

    // Checking extenstion agent
    MUnit* ep = root->GetNode("./Syst1/ep");
    MCompatChecker* ep_cchk = (MCompatChecker*) ep->GetSIfiC(MCompatChecker::Type());
    CPPUNIT_ASSERT_MESSAGE("Fail to get MCompatChecker from ep", ep_cchk != 0);
    MUnit* cp3 = root->GetNode("./cp3");
    MVert* cp3v = cp3->GetObj(cp3v);
    CPPUNIT_ASSERT_MESSAGE("Fail to get cp3", cp3 != 0);
    MVert* cp3v_pair = cp3v->GetPair(0);
    CPPUNIT_ASSERT_MESSAGE("Fail to get pair of cp3", cp3v_pair != 0);
    // Verifying that cp3/Provided chromo is empty, ref ds_uac_snmc
    MUnit* cp3prov = root->GetNode("./cp3/Provided");
    CPPUNIT_ASSERT_MESSAGE("Fail to get cp3/Provided", cp3prov != 0);
    MElem* ecp3prov = cp3prov->GetObj(ecp3prov);
    TInt cp3prov_chr_cnt = ecp3prov->Chromos().Root().Count();
    CPPUNIT_ASSERT_MESSAGE("cp3/Provided chromo is not empty", cp3prov_chr_cnt == 0);
    // Verifying that Syst1/Syst1_1 chromo is empty
    MUnit* s1_1 = root->GetNode("./Syst1/Syst1_1");
    MElem* es1_1 = s1_1->GetObj(es1_1);
    CPPUNIT_ASSERT_MESSAGE("Fail to get ./Syst1/Syst1_1", s1_1 != NULL);
    TInt s1_1_chr_cnt = es1_1->Chromos().Root().Count();
    CPPUNIT_ASSERT_MESSAGE("./Syst1/Syst1_1 chromo is not empty", s1_1_chr_cnt == 0);

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
    MUnit* root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    MUnit* cp = root->GetNode("./sysmod/ConnPoint");
    MElem* ecp = cp->GetObj(ecp);
    ecp->Chromos().Save("ut_cre_incaps_cp_saved.xml");
    MUnit* ep1 = root->GetNode("./test/Ics1/Capsule/ep");
    CPPUNIT_ASSERT_MESSAGE("Fail to get ep1", ep1 != 0);
    MVert* mep1 = ep1->GetObj(mep1);
    CPPUNIT_ASSERT_MESSAGE("Fail to get mep1", mep1 != 0);
    MVert* pair = mep1->GetPair(0);
    CPPUNIT_ASSERT_MESSAGE("Fail to get pair", pair != 0);
    MUnit* epairt = root->GetNode("./test/(Incaps:)Ics2/Capsule/ep");
    MVert* mpairt = epairt->GetObj(mpairt);
    CPPUNIT_ASSERT_MESSAGE("Wrong pair", pair == mpairt);
    MUnit* wep1 = root->GetNode("./test/Ics1/cp_int");
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
    MUnit* root = iEnv->Root();
    MElem* eroot = root->GetObj(eroot);
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    eroot->Chromos().Save("ut_cre_data_saved.xml");
    MUnit* doutp = root->GetNode("./(Incaps:)test/(DataSInt:)DataS_Int_1/Capsule/out");
    CPPUNIT_ASSERT_MESSAGE("Fail to get data out", doutp != 0);
    MDIntGet* doutpget = (MDIntGet*) doutp->GetSIfi(MDIntGet::Type());
    CPPUNIT_ASSERT_MESSAGE("Fail to get data out Get iface", doutpget != 0);
    TInt val = doutpget->Value();
    CPPUNIT_ASSERT_MESSAGE("Fail to get value of data iface", val == 34);
    MVert* mdoutpv = doutp->GetObj(mdoutpv);
    CPPUNIT_ASSERT_MESSAGE("Fail to get data out vertex", mdoutpv != 0);
    MVert* pair = mdoutpv->GetPair(0);
    CPPUNIT_ASSERT_MESSAGE("Fail to get pair", pair != 0);
    MUnit* efuninp = root->GetNode("./test/Incr/Capsule/inp");
    CPPUNIT_ASSERT_MESSAGE("Fail to get fun inp", efuninp != NULL);
    MVert* mpairt = efuninp->GetObj(mpairt);
    CPPUNIT_ASSERT_MESSAGE("Wrong pair", pair == mpairt);

    MUnit* foutp = root->GetNode("./test/Incr/Capsule/out");
    CPPUNIT_ASSERT_MESSAGE("Fail to get func out", foutp != 0);
    MDIntGet* foutpget = (MDIntGet*) foutp->GetSIfi(MDIntGet::Type());
    CPPUNIT_ASSERT_MESSAGE("Fail to get func out Get iface", foutpget != 0);
    TInt fres = foutpget->Value();
    CPPUNIT_ASSERT_MESSAGE("Incorrect func result", fres == 35);

    delete iEnv;

    // Recreate the model with osm style chromo
    iEnv = new Env("ut_cre_data_saved.xml", "ut_cre_data_saved.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env for ut_cre_data_saved.xml", iEnv != 0);
    iEnv->ImpsMgr()->ResetImportsPaths();
    iEnv->ImpsMgr()->AddImportsPaths("../modules");
    iEnv->ConstructSystem();
    root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);

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
    MUnit* root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    // Make illegal modif and enable transformation
    MUnit* vb = root->GetNode("./VB");
    MUnit* va1b1 = root->GetNode("./VB/VA_1/VA_1_B1");
    //Elem* evb = vb->GetObj(evb);
    //bool isdirinh1 = evb->IsDirectInheritedComp(va1b1);
    //CPPUNIT_ASSERT_MESSAGE("Wrong result of IsDirectInheritedComp for VA_1_B1", !isdirinh1);

    delete iEnv;
}

void Ut_cre::test_Iface()
{
    printf("\n === Test of Iface resolution\n");

    iEnv = new Env("ut_cre_iface.xml", "ut_cre_iface.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ImpsMgr()->ResetImportsPaths();
    iEnv->ImpsMgr()->AddImportsPaths("../modules");
    iEnv->ConstructSystem();
    MUnit* root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    MUnit* cp1 = root->GetNode("./cp1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get cp1", cp1 != 0);
    string uid = cp1->Uid();
    MIface* cp1r = iEnv->IfaceResolver()->GetIfaceByUid(uid);
    CPPUNIT_ASSERT_MESSAGE("cp1 resolution failed", cp1r == cp1);

    delete iEnv;
}

void Ut_cre::test_Content()
{
    printf("\n === Test of base agent content apis\n");
    iEnv = new Env("ut_content.xml", "ut_content.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ImpsMgr()->ResetImportsPaths();
    iEnv->ImpsMgr()->AddImportsPaths("../modules");
    iEnv->ConstructSystem();
    MUnit* root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    // Setting of content
    root->ChangeCont("debug_content", EFalse, "Debug");
    root->ChangeCont("yes", EFalse, "Debug.Enable_trace");
    root->ChangeCont("no", EFalse, "Debug.Enable_dbg");
    TInt rc_debug_count = root->GetContCount("Debug");
    CPPUNIT_ASSERT_MESSAGE("Wrong root debug content count", rc_debug_count == 2);
    for (TInt ct = 0; ct < rc_debug_count; ct++) {
	string cname = root->GetContComp("Debug", ct);
	string cval = root->GetContent(cname);
	cout << "Ind: " << ct << ", Content_name: " << cname << ", Value: " << cval << endl;
    }
    string cont = root->GetContent(string(), ETrue);
    cout << "Root content >>" << endl;
    cout << cont << endl;
    TBool cont_ok = (cont == "{ About:'' Debug:{'debug_content' Enable_trace:'yes' Enable_dbg:'no'}}");
    CPPUNIT_ASSERT_MESSAGE("Wrong root content", cont_ok);
    // Setting of structured content: without comps
    TBool ccres = root->ChangeCont("{'Without comps'}", EFalse, "Test_WOC");
    CPPUNIT_ASSERT_MESSAGE("Failed on setting content (without comps)", ccres);
    cont = root->GetContent("Test_WOC", ETrue);
    // Setting of structured content: with comps
    ccres = root->ChangeCont("{'With comps' Comp1:'Comp1_Value' Comp2:'Comp2_Value'}", EFalse, "Test_WC");
    CPPUNIT_ASSERT_MESSAGE("Failed on setting content (with comps)", ccres);
    cout << "Content of Test_WC:" << endl;
    cont = root->GetContent("Test_WC", ETrue);
    cout << cont << endl;
    // Nested component
    ccres = root->ChangeCont("{'Value of Test_WC_nested' Test_WC_nested_1:{'Value of Test_WC_nested_1' comp:'test'}}",
	    EFalse, "Test_WC_nested");
    cout << "Content of Test_WC_nested:" << endl;
    cont = root->GetContent("Test_WC_nested", ETrue);
    cout << cont << endl;
    // Deleting comp of content
    cout << "Deleting [Enable_trace] comp from [Debug] ..." << endl;
    root->ChangeCont("{ Enable_trace:- }", EFalse, "Debug");
    cont = root->GetContent("Debug", ETrue);
    cout << "[Debug] content after deleting [Enable_trace]:" << endl;
    cout << cont << endl;
    cont_ok = (cont == "{'debug_content' Enable_dbg:'no'}");
    CPPUNIT_ASSERT_MESSAGE("Wrong root content after deleting comp", cont_ok);
    // Predefined categories
    root->ChangeCont("{ @Debug @Readonly comp:{'some_value'}}", EFalse, "Debug");
    cont = root->GetContent("Debug", ETrue);
    cout << "[Debug] content after adding category @Debug:" << endl;
    cout << cont << endl;

    delete iEnv;
}

// Ref ds_di_cnfr
void Ut_cre::test_Notif()
{
    printf("\n === Test of notification of observers\n");

    iEnv = new Env("ut_cre_notif.xml", "ut_cre_notif.txt");
    mAgtObs = new AgentObserver(iEnv);
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ImpsMgr()->ResetImportsPaths();
    iEnv->ImpsMgr()->AddImportsPaths("../modules");
    iEnv->ConstructSystem();
    MUnit* root = iEnv->Root();
    MElem* eroot = root->GetObj(eroot);
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    iEnv->SetObserver(mAgtObs);
    eroot->AppendMutation(TMut("node,id:Syst2,parent:./Syst1"));
    TNs ns; MutCtx mctx(NULL, ns);
    eroot->Mutate(false, false, true, mctx);

    iEnv->UnsetObserver(mAgtObs);
    delete mAgtObs;
    delete iEnv;
}
