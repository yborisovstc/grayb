#include <env.h>
#include <stdlib.h>
#include <elem.h>
#include <mvert.h>
#include <mdata.h>

#include <cppunit/extensions/HelperMacros.h>

/*
 * This test is for checking the functionality of mutation
 */


class Ut_mut : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(Ut_mut);
    CPPUNIT_TEST(test_TMut);
    CPPUNIT_TEST(test_Add);
//    CPPUNIT_TEST(test_DcpChromo1);
    CPPUNIT_TEST(test_MutSyst);
    CPPUNIT_TEST(test_MutSyst2);
    CPPUNIT_TEST(test_Move);
    CPPUNIT_TEST(test_MutRmRecr);
    CPPUNIT_TEST(test_MutRmRecrInh);
    CPPUNIT_TEST(test_MutDepsRm);
//    CPPUNIT_TEST(test_MutDepsRm2);
//    CPPUNIT_TEST(test_MutDepsChilds1);
    CPPUNIT_TEST(test_MutDepsRmRef);
//    CPPUNIT_TEST(test_MutInv1);
//    CPPUNIT_TEST(test_MutInvRename);
//    CPPUNIT_TEST(test_MutInvParent);
//    CPPUNIT_TEST(test_MutInvImplicit);
    CPPUNIT_TEST(test_MutRmParent);
    CPPUNIT_TEST(test_MutRenameParent);
//    CPPUNIT_TEST(test_Compact1); //TODO [YB] To enable
    CPPUNIT_TEST(test_OptRmDeps);
//    CPPUNIT_TEST(test_Compact2); //TODO [YB] To enable
//    CPPUNIT_TEST(test_Compact3); //TODO [YB] To enable
//    CPPUNIT_TEST(test_CompactRmDa); //TODO [YB] To enable
//    CPPUNIT_TEST(test_CompactRef1); //TODO [YB] To enable
//    CPPUNIT_TEST(test_CompactCont); //TODO [YB] To enable
//    CPPUNIT_TEST(test_TransfModif1);
    CPPUNIT_TEST(test_GetParentModifs);
    CPPUNIT_TEST(test_ParentMut);
    CPPUNIT_TEST(test_ParentMut2);
    CPPUNIT_TEST_SUITE_END();
public:
    virtual void setUp();
    virtual void tearDown();
private:
    void test_TMut();
    void test_Add();
    void test_DcpChromo1();
    void test_MutSyst();
    void test_MutSyst2();
    void test_Move();
    void test_MutRmRecr();
    void test_MutRmRecrInh();
    void test_MutDepsRm();
    //void test_MutDepsRm2();
    void test_MutDepsChilds1();
    void test_MutDepsRmRef();
    void test_MutInv1();
    void test_MutInvRename();
    void test_MutInvParent();
    void test_MutInvImplicit();
    void test_MutRmParent();
    void test_MutRenameParent();
    void test_Compact1();
    void test_OptRmDeps();
    void test_Compact2();
    void test_Compact3();
    void test_CompactRmDa();
    void test_CompactRef1();
    void test_CompactCont();
    void test_TransfModif1();
    void test_GetParentModifs();
    void test_ParentMut();
    void test_ParentMut2();
private:
    Env* iEnv;
};

CPPUNIT_TEST_SUITE_REGISTRATION( Ut_mut );
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(Ut_mut, "Ut_mut");


void Ut_mut::setUp()
{
}

void Ut_mut::tearDown()
{
//    delete iEnv;
    CPPUNIT_ASSERT_EQUAL_MESSAGE("tearDown", 0, 0);
}

void Ut_mut::test_TMut()
{
    printf("\n === Test of TMut APIs\n");

    TMut mut1(ENt_Node);
    string mut1s = mut1;
    CPPUNIT_ASSERT_MESSAGE("Fail to create mut1", mut1s == "node");
    TMut mut2(ENt_Node, ENa_Id, "Parent");
    string mut2s = mut2;
    CPPUNIT_ASSERT_MESSAGE("Fail to create mut2", mut2s == "node,id:Parent");
    string mut3spec("node,id:elem"); 
    TMut mut3(mut3spec);
    string mut3s = mut3;
    CPPUNIT_ASSERT_MESSAGE("Fail to create mut3", mut3s == mut3spec);
}

void Ut_mut::test_Add()
{
    printf("\n === Test#1 of mutation\n");

    for (int ct = 1; ct < 2; ct++) {
	const string specn("ut_mutadd_1");
	string ext = ct == 0 ? "xml" : "chs";
	string spec = specn + string(".") + ext;
	string log = specn + "_" + ext + ".log";
	iEnv = new Env(spec, log);
	CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
	// TODO Temporarily, to remove
	iEnv->ChMgr()->SetEnableFixErrors(ETrue);
	//iEnv->ChMgr()->SetEnableCheckSafety(EFalse);
	//iEnv->ChMgr()->SetEnablePhenoModif(ETrue);
	iEnv->ConstructSystem();
	MUnit* root = iEnv->Root();
	MElem* eroot = root ? root->GetObj(eroot) : nullptr;
	CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != NULL && eroot != NULL);

	MUnit* e2 = root->GetNode("./elem1/elem2");
	CPPUNIT_ASSERT_MESSAGE("Fail to get e2", e2 != 0);
	MUnit* e4 = root->GetNode("./elem3/elem4");
	MElem* ee4 = e4->GetObj(ee4);
	CPPUNIT_ASSERT_MESSAGE("Fail to get e4", e4 != 0);
	MUnit* e3_2_1 = root->GetNode("./elem3/elem2/elem2_1");
	MElem* ee3_2_1 = e3_2_1->GetObj(ee3_2_1);
	CPPUNIT_ASSERT_MESSAGE("Fail to get elem3/elem2/elem2_1", e3_2_1 != 0);
	MUnit* e3 = root->GetNode("./elem3");
	CPPUNIT_ASSERT_MESSAGE("Fail to get e3", e3 != 0);
	MUnit* e6_2 = root->GetNode("./elem6/elem2/elem6_1/elem6_2");
	CPPUNIT_ASSERT_MESSAGE("Fail to get elem6_2", e6_2 != NULL);
	MUnit* e4_ao = ee4->GetAttachedMgr();
	CPPUNIT_ASSERT_MESSAGE("Wrong attached owner of e4", e4_ao == e4);
	MUnit* e3_2_1_ao = ee3_2_1->GetAttachedMgr();
	CPPUNIT_ASSERT_MESSAGE("Wrong attached owner of elem3/elem2/elem2_1", e3_2_1_ao == e3);
	MUnit* e4_ato = ee4->GetAowner();
	CPPUNIT_ASSERT_MESSAGE("Wrong attaching owner of e4", e4_ato == e3);
	// Mutation of type "Adding node to current node"
	//root->AppendMutation(TMut(ENt_Node, ENa_Id, "new_elem1", ENa_Parent, "./elem5"));
	eroot->AppendMutation(TMut("node,id:new_elem1,parent:./elem5"));
	TNs ns; MutCtx mctx(NULL, ns);
	eroot->Mutate(false, false, false, mctx);
	// Check the element added
	MUnit* eadded = root->GetNode("./(elem5:)new_elem1");
	CPPUNIT_ASSERT_MESSAGE("Fail to get elem added", eadded != 0);
	// Mutation of type "Delete node"
	eroot->AppendMutation(TMut(ENt_Rm, ENa_MutNode, "./new_elem1"));
	eroot->Mutate(false, false, false, mctx);
	MUnit* erem = root->GetNode("./(elem5:)new_elem1");
#if 0  // Name duplication is disabled, ref ds_mut_nm
	CPPUNIT_ASSERT_MESSAGE("Fail to remove elem", erem == 0);
	// Check if we can add the node with the same name again
	ChromoNode madd1 = root->AppendMutation(ENt_Node);
	madd1.SetAttr(ENa_Id, "new_elem1");
	madd1.SetAttr(ENa_Parent, "./elem5");
	root->Mutate();
	// Check the element added
	MUnit* eadded1 = root->GetNode("./new_elem1");
	CPPUNIT_ASSERT_MESSAGE("Fail to get elem added", eadded1 != 0);
#endif
	// Mutation of type "Rename node"
	eroot->AppendMutation(TMut(ENt_Change, ENa_Targ, "./elem5", ENa_MutAttr, TMut::NodeAttrName(ENa_Id),
		    ENa_MutVal, "elem5_renamed"));
	eroot->Mutate(false, false, false, mctx);
	MUnit* eren = root->GetNode("./elem5_renamed");
	CPPUNIT_ASSERT_MESSAGE("Fail to rename node", eren != 0);

	// Check getting chromo 
	string saved_name = specn + "_saved." + ext;
	eroot->Chromos().Save(saved_name);

	delete iEnv;
    }
}

/**
 * Test of decoupled chromo, ref ds_daa_chrc_va
 * 1. Checking if [node] inderect mutation (pheno) is taken into chromo
 */
void Ut_mut::test_DcpChromo1()
{
    printf("\n === Test#1 of decoupled chromo\n");

    iEnv = new Env("ut_dcp_chromo_1.xml", "ut_dcp_chromo_1.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    // TODO Temporarily, to remove
    //iEnv->ChMgr()->SetEnableCheckSafety(EFalse);
    iEnv->ConstructSystem();
    MUnit* root = iEnv->Root();
    MElem* eroot = root->GetObj(eroot);
    // Check creation first
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    // Check getting chromo 
    MChromo& chromo = eroot->Chromos();
    chromo.Save("ut_dcp_chromo_1_saved.xml_");
    // Checking that "node" indirect mutation is taken into chromo
    GUri ne22_uri("./elem3/elem2_2");
    ChromoNode ne22 = chromo.Root().GetNode(ne22_uri);
    CPPUNIT_ASSERT_MESSAGE("Fail to get node [elem2_2] from result chromo", ne22.Handle() != NULL);
    GUri ne221_uri("./elem3/elem2_2/elem2_2_1");
    ChromoNode ne221 = chromo.Root().GetNode(ne221_uri);
    CPPUNIT_ASSERT_MESSAGE("Fail to get node [elem2_2_1] from result chromo", ne221.Handle() != NULL);

    delete iEnv;
}

void Ut_mut::test_MutSyst()
{
    printf("\n === Test of mutation of system\n");

    for (int ct = 0; ct < 1; ct++) {
	const string specn("ut_mut_syst");
	string spec = specn + string(".") + string(ct == 0 ? "xml" : "chs");
	string log = specn + string(ct == 0 ? "_xml" : "_chs") + ".log";
	iEnv = new Env(spec, log);
	// Check creation first
	CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
	iEnv->ChMgr()->SetEnableFixErrors(ETrue);
	iEnv->ImpsMgr()->ResetImportsPaths();
	iEnv->ImpsMgr()->AddImportsPaths("../modules");
	iEnv->ConstructSystem();
	MUnit* root = iEnv->Root();
	MElem* eroot = root->GetObj(eroot);
	CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
	MUnit* cp1 = root->GetNode("./cp1");
	CPPUNIT_ASSERT_MESSAGE("Fail to get cp1", cp1 != 0);
	MVert* mcp1 = cp1->GetObj(mcp1);
	CPPUNIT_ASSERT_MESSAGE("Fail to get mcp1", mcp1 != 0);
	MVert* pair = mcp1->GetPair(0);
	CPPUNIT_ASSERT_MESSAGE("Fail to get pair", pair != 0);
	MUnit* epair = pair->GetObj(epair);
	const string pname = epair->Name();
	CPPUNIT_ASSERT_MESSAGE("Wrong pair's name", pname == "cp2");
	// Delete edge
	MUnit* edge1 = root->GetNode("./edge1");
	CPPUNIT_ASSERT_MESSAGE("Cannot get edge1", edge1 != 0);
	ChromoNode mdel = eroot->AppendMutation(ENt_Rm);
	mdel.SetAttr(ENa_MutNode, "./edge1");
	TNs ns; MutCtx mctx(NULL, ns);
	eroot->Mutate(false, false, false, mctx);
	MUnit* erem = root->GetNode("./edge1", false);
	CPPUNIT_ASSERT_MESSAGE("Fail to remove edge", erem == 0);
	// Adding the edge again
	ChromoNode madd = eroot->AppendMutation(ENt_Node);
	madd.SetAttr(ENa_Id, "edge1_new");
	madd.SetAttr(ENa_Parent, "Edge");
	ChromoNode cnt1 = madd.AddChild(ENt_Cont);
	cnt1.SetAttr(ENa_MutNode, "./P1");
	cnt1.SetAttr(ENa_Ref, "./../../Syst1/cp");
	ChromoNode cnt2 = madd.AddChild(ENt_Cont);
	cnt2.SetAttr(ENa_MutNode, "./P2");
	cnt2.SetAttr(ENa_Ref, "./../../cp2");
	eroot->Mutate(false, false, false, mctx);
	// Check the element added
	MUnit* eadded = root->GetNode("./edge1_new");
	CPPUNIT_ASSERT_MESSAGE("Fail to get edge added after deleting", eadded != 0);
	MUnit* cp_1 = root->GetNode("./Syst1/cp");
	CPPUNIT_ASSERT_MESSAGE("Fail to get Syst1/cp", cp_1 != 0);
	MVert* mcp_1 = cp_1->GetObj(mcp_1);
	CPPUNIT_ASSERT_MESSAGE("Fail to get Syst1/cp MVert", mcp_1 != 0);
	MVert* pair_1 = mcp_1->GetPair(0);
	CPPUNIT_ASSERT_MESSAGE("Fail to get pair of Syst1/cp", pair_1 != 0);
	MUnit* epair_1 = pair_1->GetObj(epair_1);
	const string pname_1 = epair_1->Name();
	CPPUNIT_ASSERT_MESSAGE("Wrong name of pair of Syst1/cp", pname_1 == "cp2");

	delete iEnv;
    }
}

void Ut_mut::test_MutSyst2()
{
    printf("\n === Test of mutation of system using custom mutations\n");

    for (int ct = 1; ct < 2; ct++) {
	const string specn("ut_mut_syst2");
	string ext = ct == 0 ? "xml" : "chs";
	string spec = specn + string(".") + ext;
	string log = specn + "_" + ext + ".log";
	iEnv = new Env(spec, log);
	// Check creation first
	CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
	iEnv->ChMgr()->SetEnableFixErrors(ETrue);
	iEnv->ImpsMgr()->ResetImportsPaths();
	iEnv->ImpsMgr()->AddImportsPaths("../modules");
	iEnv->ConstructSystem();
	MUnit* root = iEnv->Root();
	CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
	MElem* eroot = root->GetObj(eroot);
	CPPUNIT_ASSERT_MESSAGE("Fail to get eroot", eroot != 0);
	MUnit* cp1 = root->GetNode("./cp1");
	CPPUNIT_ASSERT_MESSAGE("Fail to get cp1", cp1 != 0);
	MVert* mcp1 = cp1->GetObj(mcp1);
	CPPUNIT_ASSERT_MESSAGE("Fail to get mcp1", mcp1 != 0);
	MVert* pair = mcp1->GetPair(0);
	CPPUNIT_ASSERT_MESSAGE("Fail to get pair", pair != 0);
	MUnit* epair = pair->GetObj(epair);
	const string pname = epair->Name();
	CPPUNIT_ASSERT_MESSAGE("Wrong pair's name", pname == "cp2");
	// Verifying system custom mutation for connecting
	MUnit* cp3 = root->GetNode("./cp3");
	CPPUNIT_ASSERT_MESSAGE("Fail to get cp3", cp3 != NULL);
	MVert* vcp3 = cp3->GetObj(vcp3);
	CPPUNIT_ASSERT_MESSAGE("Fail to get vcp3", vcp3 != NULL);
	MVert* vcp3_pair = vcp3->GetPair(0);
	CPPUNIT_ASSERT_MESSAGE("Fail to get vcp3 pair", vcp3_pair != NULL);
	MUnit* vcp3_upair = vcp3_pair->GetObj(vcp3_upair);
	const string vcp3_pname = vcp3_upair->Name();
	CPPUNIT_ASSERT_MESSAGE("Wrong vcp3 pair's name", vcp3_pname == "Int");
	// Verifying system custom mutation Disconnect
	eroot->AppendMutation(TMut(ENt_Disconn, ENa_P, "./Syst1/ep/Int", ENa_Q, "cp3"));
	TNs ns; MutCtx mctx(NULL, ns);
	eroot->Mutate(false, false, false, mctx);
	vcp3_pair = vcp3->GetPair(0);
	CPPUNIT_ASSERT_MESSAGE("vcp3 pair is not disconnected", vcp3_pair == NULL);

	delete iEnv;
    }
}

void Ut_mut::test_Move()
{
    printf("\n === Test of mutation: moving node\n");

    iEnv = new Env("ut_mutmove_1.xml", "ut_mutmove_1.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ConstructSystem();
    MUnit* root = iEnv->Root();
    MElem* eroot = root->GetObj(eroot);
    // Check creation first
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    MUnit* e2 = root->GetNode("./elem1/elem2");
    CPPUNIT_ASSERT_MESSAGE("Fail to get e2", e2 != 0);
    MUnit* e4 = root->GetNode("./(elem1:)elem3/(elem2:)elem4");
    CPPUNIT_ASSERT_MESSAGE("Fail to get e4", e4 != 0);

    // Moving remote node
    ChromoNode mmovermt = eroot->AppendMutation(ENt_Move);
    mmovermt.SetAttr(ENa_Id, "file:../modules/syst.xml#");
    mmovermt.SetAttr(ENa_MutNode, "/test");
    TNs ns; MutCtx mctx(NULL, ns);
    eroot->Mutate(false, false, false, mctx);
    MUnit* eext = root->GetNode("/test/SysComps/Extender");
    CPPUNIT_ASSERT_MESSAGE("Fail get extender from moved remote module", eext != NULL);

    // Moving local node
    ChromoNode mmove = eroot->AppendMutation(ENt_Move);
    mmove.SetAttr(ENa_Id, "/test/elem5");
    mmove.SetAttr(ENa_MutNode, "./elem3/elem4");
    eroot->Mutate(false, false, false, mctx);
    MUnit* emoved = root->GetNode("./elem3/elem4/elem5");
    CPPUNIT_ASSERT_MESSAGE("Fail to move local node elem5", emoved != NULL);

    eroot->Chromos().Save("ut_mutmove_1_saved.xml_");

    delete iEnv;
}

// Test of removing node and recreation node with the same name
void Ut_mut::test_MutRmRecr()
{
    printf("\n === Test of creating node with same name as removed one\n");

    iEnv = new Env("ut_mut_rm_recr.xml", "ut_mut_rm_recr.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    // Enabling mutation repositioning in order to resolve unsafety
    iEnv->ChMgr()->SetEnableReposMuts(ETrue);
    iEnv->ConstructSystem();
    MUnit* root = iEnv->Root();
    MElem* eroot = root->GetObj(eroot);
    // Check creation first
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    ChromoNode mut = eroot->AppendMutation(ENt_Node);
    mut.SetAttr(ENa_Id, "v1_new");
    mut.SetAttr(ENa_Parent, "./v1p");
    TNs ns; MutCtx mctx(NULL, ns);
    eroot->Mutate(false, false, false, mctx);
    MUnit* v1 = root->GetNode("./v1_new");
    CPPUNIT_ASSERT_MESSAGE("Fail to get v1_new", v1 != 0);
    MUnit* v1p_1i = root->GetNode("./v1_new/v1p_1i");
    CPPUNIT_ASSERT_MESSAGE("Fail to get v1p_1i", v1p_1i != 0);
    // Try to use ref to v1 and verify the ref is found correctly (no duplication occurs)
    MUnit* e1 = root->GetNode("./edge1");
    MElem* ee1 = e1->GetObj(ee1);
    ChromoNode mut1 = ee1->AppendMutation(ENt_Cont);
    mut1.SetAttr(ENa_MutNode, "./P2");
    mut1.SetAttr(ENa_Ref, "./../../v1_new");
    ee1->Mutate(false, false, false, mctx);
    MUnit* p2 = e1->GetNode("./P2");
    string p2_cont = p2->GetContent();
    CPPUNIT_ASSERT_MESSAGE("Fail to set edge1/P2 with ref to v1_new", p2_cont == "./../../v1_new");
    eroot->Chromos().Save("ut_mut_rm_recr_res.xml_");
    delete iEnv;
}

// Test of removing node and recreation inherited node with the same name
void Ut_mut::test_MutRmRecrInh()
{
    printf("\n === Test of creating node with same name as removed one\n");

    iEnv = new Env("ut_mut_rm_recrinh.xml", "ut_mut_rm_recrinh.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    // Enabling mutation repositioning in order to resolve unsafety
    iEnv->ChMgr()->SetEnableReposMuts(ETrue);
    iEnv->ConstructSystem();
    MUnit* root = iEnv->Root();
    MElem* eroot = root->GetObj(eroot);
    // Check creation first
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    MUnit* v1 = root->GetNode("./v1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get v1", v1 != 0);
    // Verify the ref to v1/v1p_1 is found correctly (no duplication occurs)
    MUnit* e1 = root->GetNode("./edge1");
    MUnit* p2 = e1->GetNode("./P2");
    string p2_cont = p2->GetContent();
    CPPUNIT_ASSERT_MESSAGE("Fail to set edge1/P2 with ref", p2_cont == "./../../v1/v1p_1_new/v1p_1_1");
    eroot->Chromos().Save("ut_mut_rm_recrinh_res.xml_");
    delete iEnv;
}

// Preventing of mutation braking model consistency
// Mutation - rm, dependency - child
void Ut_mut::test_MutDepsRm()
{
    printf("\n === Test of mutation consistency\n");

    iEnv = new Env("ut_mut_dep_1.xml", "ut_mut_dep_1.txt");
    // Enabling pheno, because of model is basing on pheno
    iEnv->ChMgr()->SetEnablePhenoModif(ETrue);
    //iEnv->ChMgr()->SetEnableCheckSafety(ETrue);
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ConstructSystem();
    MUnit* root = iEnv->Root();
    MElem* eroot = root->GetObj(eroot);
    // Check creation first
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    MUnit* e2 = root->GetNode("./elem1/elem2");
    CPPUNIT_ASSERT_MESSAGE("Fail to get elem2", e2 != 0);
    MUnit* e3 = root->GetNode("./(elem2:)elem3");
    CPPUNIT_ASSERT_MESSAGE("Fail to get elem3", e3 != 0);
    // Get major dep
    //Elem::TDep mdep = e2->GetMajorDep();
    /*
       TMDep mdep = e2->GetMajorDep();
       ChromoNode depmut = e2->Chromos().CreateNode(mdep.first.second);
       TBool e2dep_ok = mdep.first.first == root && depmut.Name() == e3->Name() && mdep.second == ENa_Parent;
       CPPUNIT_ASSERT_MESSAGE("Fail to get elem2 major dep", e2dep_ok);
       */
    // Try to remove elem2 from elem1 - unsafe mutation
    MUnit* e1 = root->GetNode("./elem1");
    MElem* ee1 = e1->GetObj(ee1);
    ChromoNode mut = ee1->AppendMutation(ENt_Rm);
    mut.SetAttr(ENa_MutNode, "./elem2");
    TNs ns; MutCtx mctx(NULL, ns);
    ee1->Mutate(false, false, false, mctx);
    // Check that the mutation is refused
    e2 = root->GetNode("./elem1/elem2");
    CPPUNIT_ASSERT_MESSAGE("Mutation -rm- of elem2 is not refused", e2 != NULL);
    // Try to remove elem2 from root - safe mutation
    ChromoNode mut2 = eroot->AppendMutation(ENt_Rm);
    mut2.SetAttr(ENa_MutNode, "./elem1/elem2");
    eroot->Mutate(false, false, false, mctx);
    // Check that the mutation is refused because of having childs, to be unparent first
    e2 = root->GetNode("./elem1/elem2");
    CPPUNIT_ASSERT_MESSAGE("Root mutation -rm- of elem2 is not refused", e2 != NULL);
}

// Preventing of mutation braking model consistency
// Mutation - rm, dependency - object of change
#if 0
void Ut_mut::test_MutDepsRm2()
{
    printf("\n === Test of mutation consistency, mut -rm-, dep - object \n");

    iEnv = new Env("ut_mut_dep_2.xml", "ut_mut_dep_2.txt");
    // Enabling pheno, because of model is basing on pheno
    iEnv->ChMgr()->SetEnablePhenoModif(ETrue);
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ConstructSystem();
    Elem* root = iEnv->Root();
    // Check creation first
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    MUnit* e2 = root->GetNode("./elem1/elem2");
    CPPUNIT_ASSERT_MESSAGE("Fail to get elem2", e2 != 0);
    // Get major dep
    TMDep mdep = e2->GetMajorDep();
    CPPUNIT_ASSERT_MESSAGE("Fail to get elem2 major dep", mdep.first.first == root);
    // Try to remove elem2 from elem1 - unsafe mutation
    MUnit* e1 = root->GetNode("./elem1");
    ChromoNode mut = e1->AppendMutation(ENt_Rm);
    mut.SetAttr(ENa_MutNode, "./elem2");
    e1->Mutate();
    // Check that the mutation is refused
    e2 = root->GetNode("./elem1/elem2");
    CPPUNIT_ASSERT_MESSAGE("Mutation -rm- of elem2 is not refused", e2 != NULL);
    // Try to remove elem2 from root - safe mutation
    mut = root->AppendMutation(ENt_Rm);
    mut.SetAttr(ENa_MutNode, "./elem1/elem2");
    root->Mutate();
    // Check that the mutation is not refused
    e2 = root->GetNode("./elem1/elem2");
    CPPUNIT_ASSERT_MESSAGE("Root mutation -rm- of elem2 is refused", e2 == NULL);
}
#endif

// Mutation consistency
// Dependency - via childs
void Ut_mut::test_MutDepsChilds1()
{
    printf("\n === Test of mutation consistency, deps via childs \n");

    iEnv = new Env("ut_mut_dep_3.xml", "ut_mut_dep_3.txt");
    // Enabling pheno, because of model is basing on pheno
    iEnv->ChMgr()->SetEnablePhenoModif(ETrue);
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ConstructSystem();
    MUnit* root = iEnv->Root();
    // Check creation first
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    MUnit* e2 = root->GetNode("./elem1/elem2");
    CPPUNIT_ASSERT_MESSAGE("Fail to get elem2", e2 != 0);
    // Applying harmful mutation
    MUnit* e1 = root->GetNode("./elem1");
    MElem* ee1 = e1->GetObj(ee1);
    ChromoNode mut = ee1->AppendMutation(ENt_Change);
    mut.SetAttr(ENa_MutNode, "./elem2");
    mut.SetAttr(ENa_MutAttr, "id");
    mut.SetAttr(ENa_MutVal, "elem2_renamed");
    TNs ns; MutCtx mctx(NULL, ns);
    ee1->Mutate(false, false, false, mctx);
    // Check that the mutation is refused
    string e2_name = e2->Name();
    CPPUNIT_ASSERT_MESSAGE("Harmful mutation hasn't been refused", e2_name == "elem2");
}


// Preventing of mutation breaking model consistency
// Mutation - rm, dependency - ref to node
void Ut_mut::test_MutDepsRmRef()
{
    printf("\n === Test of mutation consistency, mut -rm-, dep - ref to node \n");

    iEnv = new Env("ut_mut_dep_refs.xml", "ut_mut_dep_refs.txt");
    // Enabling pheno, because of model is basing on pheno
    iEnv->ChMgr()->SetEnablePhenoModif(ETrue);
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ConstructSystem();
    MUnit* root = iEnv->Root();
    MElem* eroot = root->GetObj(eroot);
    // Check creation first
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    MUnit* v1_0 = root->GetNode("./v1/v1_0");
    CPPUNIT_ASSERT_MESSAGE("Fail to get v1/v1_0", v1_0 != 0);
    // Try to remove v1_0 from v1 - unsafe mutation
    MUnit* v1 = root->GetNode("./v1");
    MElem* ev1 = v1->GetObj(ev1);
    ChromoNode mut = ev1->AppendMutation(ENt_Rm);
    mut.SetAttr(ENa_MutNode, "./v1_0");
    TNs ns; MutCtx mctx(NULL, ns);
    ev1->Mutate(EFalse, EFalse, ETrue, mctx);
    // Check that the mutation is refused because not being safe -
    // there are deps with the rank higher than the rank of this mutatins,
    // so error will occur if node is deleted
    v1_0 = root->GetNode("./v1/v1_0");
    CPPUNIT_ASSERT_MESSAGE("Mutation -rm- of v1_0 is not refused", v1_0 != NULL);
    // Try to remove elem2 from root - safe mutation
    ChromoNode mut2 = eroot->AppendMutation(ENt_Rm);
    mut2.SetAttr(ENa_MutNode, "./v1/v1_0");
    eroot->Mutate(EFalse, EFalse, ETrue, mctx);
    // Check that the mutation is refused because there is critical dep on the node being
    // removed. Even the rank of this dep is lower that the rank of this mutation, the
    // deps will generate error so the model will be inconsistent 
    v1_0 = root->GetNode("./v1/v1_0");
    CPPUNIT_ASSERT_MESSAGE("Mutation -rm- of v1_0 from root is not refused", v1_0 != NULL);
    // Remove critical dep first, then optimize the chromo, reload and try to remove v1_0 again
    mut2 = eroot->AppendMutation(ENt_Cont);
    mut2.SetAttr(ENa_MutNode, "./edge1/P1");
    mut2.SetAttr(ENa_Ref, "");
    eroot->Mutate(EFalse, EFalse, ETrue, mctx);
    /*
       root->CompactChromo();
       iEnv->Root()->Chromos().Save("ut_mut_dep_refs_res1.xml_");
       delete iEnv;
       iEnv = new Env("ut_mut_dep_refs_res1.xml_", "ut_mut_dep_refs_res1.txt");
       CPPUNIT_ASSERT_MESSAGE("Fail to create Env for compacted system", iEnv != 0);
       iEnv->ChMgr()->SetEnablePhenoModif(ETrue);
       iEnv->ConstructSystem();
       root = iEnv->Root();
       CPPUNIT_ASSERT_MESSAGE("Fail to get root after compacting", root != 0);
       */
    ChromoNode mut1 = eroot->AppendMutation(ENt_Rm);
    mut1.SetAttr(ENa_MutNode, "./v1/v1_0");
    eroot->Mutate(EFalse, EFalse, ETrue, mctx);
    v1_0 = root->GetNode("./v1/v1_0");
    CPPUNIT_ASSERT_MESSAGE("Mutation -rm- of v1_0 from root is refused", v1_0 == NULL);
    eroot->Chromos().Save("ut_mut_dep_refs_res1.xml_");
    delete iEnv;
}

// Suppotring invariance with respect to mutations position
void Ut_mut::test_MutInv1()
{
    printf("\n === Test of mutation invariance with respect to mutations rank, dep: ref to node \n");

    iEnv = new Env("ut_mut_inv_1.xml", "ut_mut_inv_1.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    // Enabling mutation repositioning in order to resolve unsafety
    iEnv->ChMgr()->SetEnableReposMuts(ETrue);
    iEnv->ConstructSystem();
    MUnit* root = iEnv->Root();
    MElem* eroot = root->GetObj(eroot);
    // Check creation first
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != NULL);
    MUnit* v1 = root->GetNode("./v1");
    MUnit* v2 = root->GetNode("./v2");
    CPPUNIT_ASSERT_MESSAGE("Fail to get v1 and v2", v1 != NULL && v2 != NULL);
    // Check that the inital connection is set
    MVert* mv1 = v1->GetObj(mv1);
    MVert* mv2 = v2->GetObj(mv2);
    CPPUNIT_ASSERT_MESSAGE("Fail to connect v1 to v2", mv1->IsPair(mv2));
    // Try to mutate edge point P2 to change v2 to v3
    MUnit* edge1 = root->GetNode("./edge1");
    MElem* eedge1 = edge1->GetObj(eedge1);
    ChromoNode mut = eedge1->AppendMutation(ENt_Cont);
    mut.SetAttr(ENa_MutNode, "./P2");
    mut.SetAttr(ENa_Ref, "./../../v3");
    TNs ns; MutCtx mctx(NULL, ns);
    eedge1->Mutate(false, false, false, mctx);

    // Save upated chromo 
    eroot->Chromos().Save("ut_mut_inv_1_res.xml_");
    delete iEnv;
    // Shifting mutation introduced another unsafety for -p1-, checking if it will be 
    // resolved next model creation
    iEnv = new Env("ut_mut_inv_1_res.xml_", "ut_mut_inv_1_res.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env1", iEnv != 0);
    // Enabling mutation repositioning in order to resolve unsafety
    iEnv->ChMgr()->SetEnableReposMuts(ETrue);
    iEnv->ConstructSystem();
    root = iEnv->Root();
    eroot = root->GetObj(eroot);
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != NULL);
    // Check that the renewed connection is set
    v1 = root->GetNode("./v1");
    mv1 = v1->GetObj(mv1);
    MUnit* v3 = root->GetNode("./v3");
    MVert* mv3 = v3->GetObj(mv3);
    CPPUNIT_ASSERT_MESSAGE("Fail to connect v1 to v3", mv1->IsPair(mv3));
    // Check that secondary forward dependency was resolved
    MUnit* p1 = root->GetNode("./p1");
    string p1_cont = p1->GetContent();
    CPPUNIT_ASSERT_MESSAGE("Fail to set p1 with ref to edge1/P1", p1_cont == "./../edge1/P1");
    // Save upated chromo 
    eroot->Chromos().Save("ut_mut_inv_1_res2.xml_");
    delete iEnv;
}

// Suppotring invariance with respect to mutations position
void Ut_mut::test_MutInvRename()
{
    printf("\n === Test of mutation invariance with respect to mutations rank, dep: renaming of referred node\n");

    iEnv = new Env("ut_mut_inv_rnm.xml", "ut_mut_inv_rnm.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    // Enabling mutation repositioning in order to resolve unsafety
    iEnv->ChMgr()->SetEnableReposMuts(ETrue);
    iEnv->ConstructSystem();
    MUnit* root = iEnv->Root();
    MElem* eroot = root->GetObj(eroot);
    // Check creation first
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != NULL);
    MUnit* v1 = root->GetNode("./v1");
    MUnit* v2 = root->GetNode("./v2");
    CPPUNIT_ASSERT_MESSAGE("Fail to get v1 and v2", v1 != NULL && v2 != NULL);
    // Check that the inital connection is set
    MVert* mv1 = v1->GetObj(mv1);
    MVert* mv2 = v2->GetObj(mv2);
    CPPUNIT_ASSERT_MESSAGE("Fail to connect v1 to v2", mv1->IsPair(mv2));
    // Try to mutate edge point P2 to change v2 to v3
    MUnit* edge1 = root->GetNode("./edge1");
    MElem* eedge1 = edge1->GetObj(eedge1);
    ChromoNode mut = eedge1->AppendMutation(ENt_Cont);
    mut.SetAttr(ENa_MutNode, "./P2");
    mut.SetAttr(ENa_Ref, "./../../v3");
    TNs ns; MutCtx mctx(NULL, ns);
    eedge1->Mutate(false, false, false, mctx);

    // Save upated chromo 
    eroot->Chromos().Save("ut_mut_inv_rnm_res.xml_");
    delete iEnv;
    // Shifting mutation introduced another unsafety for -p1-, checking if it will be 
    // resolved next model creation
    iEnv = new Env("ut_mut_inv_rnm_res.xml_", "ut_mut_inv_rnm_res.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env1", iEnv != 0);
    // Enabling mutation repositioning in order to resolve unsafety
    iEnv->ChMgr()->SetEnableReposMuts(ETrue);
    iEnv->ConstructSystem();
    root = iEnv->Root();
    eroot = root->GetObj(eroot);
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != NULL);
    // Check that the renewed connection is set
    v1 = root->GetNode("./v1");
    mv1 = v1->GetObj(mv1);
    MUnit* v3 = root->GetNode("./v3");
    MVert* mv3 = v3->GetObj(mv3);
    CPPUNIT_ASSERT_MESSAGE("Fail to connect v1 to v3", mv1->IsPair(mv3));
    // Check that secondary forward dependency was resolved
    MUnit* p1 = root->GetNode("./p1");
    string p1_cont = p1->GetContent();
    CPPUNIT_ASSERT_MESSAGE("Fail to set p1 with ref to edge1/P1", p1_cont == "./../edge1/P1");
    // Save upated chromo 
    eroot->Chromos().Save("ut_mut_inv_rnm_res2.xml_");
    delete iEnv;
}

// Suppotring invariance with respect to mutations position
// Dependency that contradicts to the native order: ref to parent
void Ut_mut::test_MutInvParent()
{
    printf("\n === Test of mutation invariance with respect to mutations rank, dep: ref to parent \n");

    iEnv = new Env("ut_mut_inv_prnt.xml", "ut_mut_inv_prnt.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    // Enabling mutation repositioning in order to resolve unsafety
    iEnv->ChMgr()->SetEnableReposMuts(ETrue);
    iEnv->ConstructSystem();
    MUnit* root = iEnv->Root();
    MElem* eroot = root->GetObj(eroot);
    // Check creation first
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != NULL);
    MUnit* v1 = root->GetNode("./v1");
    MElem* ev1 = v1->GetObj(ev1);
    MUnit* v2 = root->GetNode("./v2");
    CPPUNIT_ASSERT_MESSAGE("Fail to get v1 and v2", v1 != NULL && v2 != NULL);
    // Try to mutate v1 by adding child of v2
    ChromoNode mut = ev1->AppendMutation(ENt_Node);
    mut.SetAttr(ENa_Id, "v2_child");
    mut.SetAttr(ENa_Parent, "./../v2");
    TNs ns; MutCtx mctx(NULL, ns);
    ev1->Mutate(false, false, false, mctx);
    // Check that the mutation applied
    MUnit* v2_child = root->GetNode("./v1/v2_child");
    CPPUNIT_ASSERT_MESSAGE("Fail to create v2_child", v2_child != NULL);

    // Save upated chromo 
    eroot->Chromos().Save("ut_mut_inv_prnt_res.xml_");
    delete iEnv;
    // Shifting mutation introduced another unsafety for -p1-, checking if it will be 
    // resolved next model creation
    iEnv = new Env("ut_mut_inv_prnt_res.xml_", "ut_mut_inv_prnt_res.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env1", iEnv != 0);
    // Enabling mutation repositioning in order to resolve unsafety
    iEnv->ChMgr()->SetEnableReposMuts(ETrue);
    iEnv->ConstructSystem();
    root = iEnv->Root();
    eroot = root->GetObj(eroot);
    // Check that secondary forward dependency was resolved
    MUnit* p1 = root->GetNode("./p1");
    string p1_cont = p1->GetContent();
    CPPUNIT_ASSERT_MESSAGE("Fail to set p1 with ref to v1", p1_cont == "./../v1");
    // Save upated chromo 
    eroot->Chromos().Save("ut_mut_inv_prnt_res2.xml_");
    delete iEnv;
}

// Suppotring invariance with respect to mutations position
// Imlicit dependency, ref uc_050, ds_indp_mutord_impl
void Ut_mut::test_MutInvImplicit()
{
    printf("\n === Test of mutation invariance with respect to mutations rank, implicit dep: Connectable \n");

    iEnv = new Env("ut_mut_inv_impl.xml", "ut_mut_inv_impl.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    // Enabling mutation repositioning in order to resolve unsafety
    iEnv->ChMgr()->SetEnableReposMuts(ETrue);
    iEnv->ConstructSystem();
    MUnit* root = iEnv->Root();
    MElem* eroot = root->GetObj(eroot);
    // Check creation first
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != NULL);
    MUnit* ee = root->GetNode("./System/E");
    MElem* eee = ee->GetObj(eee);
    CPPUNIT_ASSERT_MESSAGE("Fail to get edge [E]", ee != NULL);
    // Try to mutate edge by setting ref to P2
    ChromoNode mut = eee->AppendMutation(ENt_Cont);
    mut.SetAttr(ENa_MutNode, "./P2");
    mut.SetAttr(ENa_Ref, "./../../Syst_B/Cp");
    TNs ns; MutCtx mctx(NULL, ns);
    eee->Mutate(false, false, false, mctx);
    // Check that the mutation applied
    MUnit* sacp = root->GetNode("./System/Syst_A/Cp");
    MUnit* sbcp = root->GetNode("./System/Syst_B/Cp");
    MVert* sacpv = sacp->GetObj(sacpv);
    MVert* sbcpv = sbcp->GetObj(sbcpv);
    MVert* pair = sacpv->GetPair(0);
    CPPUNIT_ASSERT_MESSAGE("Fail to get pair of Syst_A Cp", pair != 0);
    MUnit* pmelem = pair->GetObj(pmelem);
    CPPUNIT_ASSERT_MESSAGE("Wrong pair's name", pmelem == sbcp);

    // Save upated chromo 
    eroot->Chromos().Save("ut_mut_inv_impl_res.xml_");
    delete iEnv;
    // Recreate the system
    iEnv = new Env("ut_mut_inv_impl_res.xml_", "ut_mut_inv_impl_res.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env1", iEnv != 0);
    // Enabling mutation repositioning in order to resolve unsafety
    iEnv->ChMgr()->SetEnableReposMuts(ETrue);
    iEnv->ConstructSystem();
    root = iEnv->Root();
    eroot = root->GetObj(eroot);
    // Check that the mutation applied
    sacp = root->GetNode("./System/Syst_A/Cp");
    sbcp = root->GetNode("./System/Syst_B/Cp");
    sacpv = sacp->GetObj(sacpv);
    sbcpv = sbcp->GetObj(sbcpv);
    pair = sacpv->GetPair(0);
    CPPUNIT_ASSERT_MESSAGE("Fail to get pair of Syst_A Cp", pair != 0);
    pmelem = pair->GetObj(pmelem);
    CPPUNIT_ASSERT_MESSAGE("Wrong pair's name", pmelem== sbcp);
    // Save upated chromo 
    eroot->Chromos().Save("ut_mut_inv_impl_res2.xml_");
    delete iEnv;
}


// Handling of parents removing, ref uc_029
void Ut_mut::test_MutRmParent()
{
    printf("\n === Test of system consistency after parent removed\n");

    iEnv = new Env("ut_mutadd_1.xml", "ut_mut_rm_parent.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ConstructSystem();
    MUnit* root = iEnv->Root();
    MElem* eroot = root->GetObj(eroot);
    // Check creation first
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    MUnit* e2 = root->GetNode("./elem1/elem2");
    CPPUNIT_ASSERT_MESSAGE("Fail to get elem2", e2 != 0);
    // Remove elem2, which is parent of elem5 
    ChromoNode mut = eroot->AppendMutation(ENt_Rm);
    mut.SetAttr(ENa_MutNode, "./elem1/elem2");
    TNs ns; MutCtx mctx(NULL, ns);
    eroot->Mutate(false, false, false, mctx);
    // Create child from elem5
    ChromoNode madd = eroot->AppendMutation(ENt_Node);
    madd.SetAttr(ENa_Id, "elem6");
    madd.SetAttr(ENa_Parent, "./elem5");
    eroot->Mutate(false, false, false, mctx);
    // Check that the mutation is performed
    MUnit* e6 = root->GetNode("./elem6");
    CPPUNIT_ASSERT_MESSAGE("Creating elem6 as child of elem5 failed", e6 != NULL);
    // Check that the elem6 contains mutation from deleted parent elem2
    MUnit* e2_1 = root->GetNode("./elem6/elem2_1");
    CPPUNIT_ASSERT_MESSAGE("elem6 containing deleted parent comp elem2_1 failed", e2_1 != NULL);

    // Trying to remove elem4 from elem3. elem4 doens't have direct deps but via comp elem4_1
    // System must deny removing elem4 from elem3
    MUnit* e3 = root->GetNode("./elem3");
    MElem* ee3 = e3->GetObj(ee3);
    ChromoNode mut2 = ee3->AppendMutation(ENt_Rm);
    mut2.SetAttr(ENa_MutNode, "./elem4");
    ee3->Mutate(false, false, false, mctx);
    // Check that the mutation is dinied
    MUnit* e4 = root->GetNode("./elem3/elem4");
    CPPUNIT_ASSERT_MESSAGE("Removing elem4 from elem3 isn't denied", e4 != NULL);

    // Remove elem3/elem4- owner of elem4_1, which is parent of elem4_1i 
    // The deps are mutations that referred to components of the node
    // System should not remove the node if there are inheritance deps, also from nodes comps
    ChromoNode mut3 = eroot->AppendMutation(ENt_Rm);
    mut3.SetAttr(ENa_MutNode, "./elem3/elem4");
    eroot->Mutate(false, false, false, mctx);
    // Create child from elem4_1i
    madd = eroot->AppendMutation(ENt_Node);
    madd.SetAttr(ENa_Id, "elem7");
    madd.SetAttr(ENa_Parent, "./elem4_1i");
    eroot->Mutate(false, false, false, mctx);
    // Check that the mutation is performed
    MUnit* e7 = root->GetNode("./elem7");
    CPPUNIT_ASSERT_MESSAGE("Creating elem7 as child of elem4_1i (comp of removed elem4) failed", e7 != NULL);
    eroot->Mutate(false, false, false, mctx);
}

// Handling of parents renaming, ref uc_031
void Ut_mut::test_MutRenameParent()
{
    printf("\n === Test of system consistency after parent renamed\n");

    iEnv = new Env("ut_mutadd_1.xml", "ut_mut_rn_parent.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ConstructSystem();
    MUnit* root = iEnv->Root();
    MElem* eroot = root->GetObj(eroot);
    // Check creation first
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    MUnit* e1 = root->GetNode("./elem1");
    MElem* ee1 = e1->GetObj(ee1);
    CPPUNIT_ASSERT_MESSAGE("Fail to get elem1", e1 != 0);
    MUnit* e2 = root->GetNode("./elem1/elem2");
    CPPUNIT_ASSERT_MESSAGE("Fail to get elem2", e2 != 0);
    // Rename elem2, which is parent of elem5 
    ChromoNode mut = ee1->AppendMutation(ENt_Change);
    mut.SetAttr(ENa_MutNode, "./elem2");
    mut.SetAttr(ENa_MutAttr, TMut::NodeAttrName(ENa_Id));
    mut.SetAttr(ENa_MutVal, "elem2_renamed");
    TNs ns; MutCtx mctx(NULL, ns);
    eroot->Mutate(false, false, false, mctx);
    // Create child from elem5
    ChromoNode madd = eroot->AppendMutation(ENt_Node);
    madd.SetAttr(ENa_Id, "elem6");
    madd.SetAttr(ENa_Parent, "./elem5");
    eroot->Mutate(false, false, false, mctx);
    // Check that the mutation is performed
    MUnit* e6 = root->GetNode("./elem6");
    CPPUNIT_ASSERT_MESSAGE("Creating elem6 as child of elem5 failed", e6 != NULL);
    // Check that the elem6 contains mutation from deleted parent elem2
    MUnit* e2_1 = root->GetNode("./elem6/elem2_1");
    CPPUNIT_ASSERT_MESSAGE("elem6 containing deleted parent comp elem2_1 failed", e2_1 != NULL);
}

// Compactization of chromo
void Ut_mut::test_Compact1()
{
    printf("\n === Test of compacting of chromo: renaming\n");

    iEnv = new Env("ut_compact1.xml", "ut_compact1.txt");
    // Enabling pheno, because of model is basing on pheno
    iEnv->ChMgr()->SetEnablePhenoModif(ETrue);
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ConstructSystem();
    MUnit* root = iEnv->Root();
    MElem* eroot = root->GetObj(eroot);
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    // Check creation first
    MUnit* e3 = root->GetNode("./elem3");
    CPPUNIT_ASSERT_MESSAGE("Fail to get elem3", e3 != 0);
    // Compact chromo
    eroot->CompactChromo();
    // Save compacted chromo and recreate the model
    eroot->Chromos().Save("ut_compact1_res.xml_");
    delete iEnv;

    iEnv = new Env("ut_compact1_res.xml_", "ut_compact1_res.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env for compacted system", iEnv != 0);
    iEnv->ConstructSystem();
    // Check if elem3 and elem4 are created ok with compacted chromo
    root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root after compacting", root != 0);
    e3 = root->GetNode("./elem3");
    CPPUNIT_ASSERT_MESSAGE("Fail to get elem3", e3 != 0);
    MUnit* e4 = root->GetNode("./elem4");
    CPPUNIT_ASSERT_MESSAGE("Fail to get elem4", e4 != 0);

    delete iEnv;
}

// Optimization of chromo: -rm- with deps -parent- and -ref-
// Resolving deps, optimized on fly, checking out that -rm- is optimized
void Ut_mut::test_OptRmDeps()
{
    printf("\n === Test of mutation consistency, mut -rm-, dep - ref to node \n");
    iEnv = new Env("ut_opt_rm_deps.xml", "ut_opt_rm_deps.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ConstructSystem();
    MUnit* root = iEnv->Root();
    MElem* eroot = root->GetObj(eroot);
    // Check creation first
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    MUnit* v1 = root->GetNode("./v1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get v1", v1 != 0);
    // Try to remove v1_0 from v1 - unsafe mutation
    ChromoNode mut = eroot->AppendMutation(ENt_Rm);
    mut.SetAttr(ENa_MutNode, "./v1");
    TNs ns; MutCtx mctx(NULL, ns);
    eroot->Mutate(EFalse, EFalse, ETrue, mctx);
    // Check that the mutation is refused because not being safe -
    // there are deps with the rank higher than the rank of this mutatins,
    // so error will occur if node is deleted
    v1 = root->GetNode("./v1");
    CPPUNIT_ASSERT_MESSAGE("Mutation -rm- of v1 is not refused", v1 != NULL);
    // Remove critical deps and try to remove v1_0 again
    MUnit* edge1 = root->GetNode("./edge1");
    MElem* eedge1 = edge1->GetObj(eedge1);
    ChromoNode mut2 = eedge1->AppendMutation(ENt_Cont);
    mut2.SetAttr(ENa_MutNode, "./P1");
    mut2.SetAttr(ENa_Ref, "");
    eedge1->Mutate(EFalse, EFalse, ETrue, mctx);
    ChromoNode mut3 = eroot->AppendMutation(ENt_Rm);
    mut3.SetAttr(ENa_MutNode, "./v1_0_i1");
    eroot->Mutate(EFalse, EFalse, ETrue, mctx);
    ChromoNode mut1 = eroot->AppendMutation(ENt_Rm);
    mut1.SetAttr(ENa_MutNode, "./v1");
    eroot->Mutate(EFalse, EFalse, ETrue, mctx);
    v1 = root->GetNode("./v1");
    eroot->Chromos().Save("ut_opt_rm_deps_res1.xml_");
    CPPUNIT_ASSERT_MESSAGE("Mutation -rm- of v1 from root is refused", v1 == NULL);
    delete iEnv;
}

// Optimization of chromo
void Ut_mut::test_Compact2()
{
    printf("\n === Test of compacting of chromo: renaming\n");
    iEnv = new Env("ut_compact2.xml", "ut_compact2.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ConstructSystem();
    MUnit* root = iEnv->Root();
    MElem* eroot = root->GetObj(eroot);
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    // Compact chromo
    eroot->CompactChromo();
    // Save compacted chromo and recreate the model
    eroot->Chromos().Save("ut_compact2_res.xml_");
    delete iEnv;

    iEnv = new Env("ut_compact2_res.xml_", "ut_compact2_res.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to re-create compacted system", iEnv != 0);
    iEnv->ConstructSystem();
    // Check if edge is set correctly
    root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root after compacting", root != 0);
    MUnit* v1 = root->GetNode("./(Vert:)v1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get v1", v1 != 0);
    MVert* mv1 = v1->GetObj(mv1);
    CPPUNIT_ASSERT_MESSAGE("Fail to get mv1", mv1 != 0);
    MVert* pair = mv1->GetPair(0);
    CPPUNIT_ASSERT_MESSAGE("Fail to get pair", pair != 0);
    MUnit* v3 = root->GetNode("./(Vert:)v3");
    MUnit* pmelem = pair->GetObj(pmelem);
    CPPUNIT_ASSERT_MESSAGE("Wrong pair's name", pmelem == v3);

    delete iEnv;
}

// Compactization of chromo: mut removing node
void Ut_mut::test_Compact3()
{
    printf("\n === Test of compacting of chromo: removing\n");
    iEnv = new Env("ut_compact3.xml", "ut_compact3.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ConstructSystem();
    MUnit* root = iEnv->Root();
    MElem* eroot = root->GetObj(eroot);
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    // Compact chromo
    eroot->CompactChromo();
    // Save compacted chromo and recreate the model
    eroot->Chromos().Save("ut_compact3_res.xml_");
    delete iEnv;

    iEnv = new Env("ut_compact3_res.xml_", "ut_compact3_res.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to re-create compacted system", iEnv != 0);
    iEnv->ConstructSystem();
    // Check that v3 is not created
    root = iEnv->Root();
    eroot = root->GetObj(eroot);
    CPPUNIT_ASSERT_MESSAGE("Fail to get root after compacting", root != 0);
    MUnit* v3 = root->GetNode("./(Vert:)v3");
    CPPUNIT_ASSERT_MESSAGE("V3 is not removed", v3 == NULL);
    ChromoNode::Iterator it = eroot->Chromos().Root().Begin(); 
    it++; it++;
    ChromoNode mut = *it;
    TBool is_mut_correct = mut.Type() == ENt_Node && mut.Attr(ENa_Id) == "v3";
    TBool is_mut_deact = !mut.IsActive();
    CPPUNIT_ASSERT_MESSAGE("Mut of creation v3 is not squeezed", is_mut_correct && is_mut_deact);
    delete iEnv;

    // Verifying disabling of processing of optimization
    iEnv = new Env("ut_compact3_res.xml_", "ut_compact3_res.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to re-create compacted system", iEnv != 0);
    iEnv->ChMgr()->SetEnableOptimization(EFalse);
    iEnv->ConstructSystem();
    // Check that v3 is not created
    root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root after compacting", root != 0);
    v3 = root->GetNode("./(Vert:)v3");
    TInt compsnum = root->CompsCount();
    CPPUNIT_ASSERT_MESSAGE("V3 is not removed", v3 == NULL);
    CPPUNIT_ASSERT_MESSAGE("Mut of creation v3 is missing in opt disabling mode", compsnum == 5);
    delete iEnv;

}

// Compactization of chromo: mut removing deattached node
void Ut_mut::test_CompactRmDa()
{
    printf("\n === Test of compacting of chromo: removing deattached node\n");
    iEnv = new Env("ut_compact_rmda.xml", "ut_compact_rmda.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ConstructSystem();
    MUnit* root = iEnv->Root();
    MElem* eroot = root->GetObj(eroot);
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    // Compact chromo
    eroot->CompactChromo();
    // Save compacted chromo and recreate the model
    eroot->Chromos().Save("ut_compact_rmda_res.xml_");
    delete iEnv;

    iEnv = new Env("ut_compact_rmda_res.xml_", "ut_compact_rmda_res.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to re-create compacted system", iEnv != 0);
    iEnv->ConstructSystem();
    // Check that -rm- mut is not optimized out
    root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root after compacting", root != 0);
    MUnit* v3a1 = root->GetNode("./v3_a1");
    MElem* ev3a1 = v3a1->GetObj(ev3a1);
    CPPUNIT_ASSERT_MESSAGE("v3_a1 is not created", v3a1 != NULL);
    ChromoNode::Iterator it = ev3a1->Chromos().Root().Begin(); 
    ChromoNode mut = *it;
    TBool is_mut_correct = mut.Type() == ENt_Rm;
    TBool is_mut_act = mut.IsActive();
    CPPUNIT_ASSERT_MESSAGE("Mut of removing v3_c1 is optimized out", is_mut_correct && is_mut_act);
    delete iEnv;
}

// Compactization of chromo. Cont ref if ref rank is greater than that of init mut.
void Ut_mut::test_CompactRef1()
{
    printf("\n === Test of compacting of chromo: ref if ref rank is greater than that of init mut\n");
    iEnv = new Env("ut_compact_ref1.xml", "ut_compact_ref1.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ConstructSystem();
    MUnit* root = iEnv->Root();
    MElem* eroot = root->GetObj(eroot);
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    // Compact chromo
    eroot->CompactChromo();
    // Save compacted chromo and recreate the model
    eroot->Chromos().Save("ut_compact_ref1_res.xml_");
    delete iEnv;

    iEnv = new Env("ut_compact_ref1_res.xml_", "ut_compact_ref1_res.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to re-create compacted system", iEnv != 0);
    iEnv->ConstructSystem();
    // Check if mut is not squeezed
    root = iEnv->Root();
    eroot = root->GetObj(eroot);
    CPPUNIT_ASSERT_MESSAGE("Fail to get root after compacting", root != 0);
    ChromoNode rnode = eroot->Chromos().Root();
    ChromoNode::Reverse_Iterator it = rnode.Rbegin(); 
    it++; it++;
    ChromoNode mut = *it;
    TBool is_cont_ref = mut.Type() == ENt_Cont && mut.Attr(ENa_MutNode) == "./Edge1/P2";
    TBool is_mut_deact = !mut.IsActive();
    CPPUNIT_ASSERT_MESSAGE("Fail to optmize mut out", is_cont_ref && is_mut_deact);


    delete iEnv;
}

// Compactization of chromo. Content change
void Ut_mut::test_CompactCont()
{
    printf("\n === Test of compacting of chromo: Content change\n");
    iEnv = new Env("ut_compact_cont.xml", "ut_compact_cont.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ConstructSystem();
    MUnit* root = iEnv->Root();
    MElem* eroot = root->GetObj(eroot);
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    // Compact chromo
    eroot->CompactChromo();
    // Save compacted chromo and recreate the model
    eroot->Chromos().Save("ut_compact_cont_res.xml_");
    delete iEnv;

    iEnv = new Env("ut_compact_cont_res.xml_", "ut_compact_cont_res.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to re-create compacted system", iEnv != 0);
    iEnv->ConstructSystem();
    // Check if mut is not squeezed
    root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root after compacting", root != 0);
    MUnit* e2 = root->GetNode("./E2");
    MElem* ee2 = e2->GetObj(ee2);
    MUnit* p1 = root->GetNode("./E2/P1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get P1", p1 != NULL);
    string cp1 = p1->GetContent();
    CPPUNIT_ASSERT_MESSAGE("Incorrect content of P1", cp1 == "P1_new_content");
    ChromoNode rnode = ee2->Chromos().Root();
    ChromoNode::Iterator it = rnode.Begin(); 
    ChromoNode mut = *it;
    TBool is_mut_deact = mut.Type() == ENt_Cont && mut.AttrExists(ENa_Inactive);
    CPPUNIT_ASSERT_MESSAGE("Init cont mutation is not optimized out", is_mut_deact);
    it++;
    mut = *it;
    is_mut_deact = mut.Type() == ENt_Cont && mut.AttrExists(ENa_Inactive);
    CPPUNIT_ASSERT_MESSAGE("Cont mutation 2 is not optimized out", is_mut_deact);
    it++;
    mut = *it;
    is_mut_deact = mut.Type() == ENt_Cont && mut.AttrExists(ENa_Inactive);
    CPPUNIT_ASSERT_MESSAGE("Cont mutation 3 is not optimized out", is_mut_deact);

    delete iEnv;
}

// Getting parents modifs, ref ds_mut_osm_linchr_comdp
// TODO [YB] Seems this use-case is not complete. To consider use-case
// where A-B--C-D (- attached, -- deattached) and D is mutated. 
// Currently mutation from D will be copied to C, that is wrong
void Ut_mut::test_GetParentModifs()
{
    printf("\n === Test of getting parent modifs\n");
    iEnv = new Env("ut_parmod1.xml", "ut_parmod1.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ConstructSystem();
    MUnit* root = iEnv->Root();
    MElem* eroot = root->GetObj(eroot);
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    MUnit* vb1 = root->GetNode("./VB/VB_1");
    eroot->Chromos().Save("ut_parmod1_res_saved.xml");
    delete iEnv;

    iEnv = new Env("ut_parmod1_res_saved.xml", "ut_parmod1_res_saved.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to re-create system after gettin parent modifs", iEnv != 0);
    iEnv->ConstructSystem();
    root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root after transforming", root != 0);
    vb1 = root->GetNode("./VB/VB_1");
    MElem* evb1 = vb1->GetObj(evb1);
    evb1->Chromos().Save("ut_parmod1_res_vb1_saved.xml");
    MUnit* va1b1 = root->GetNode("./VB/VB_1/VP_1/VM_1/VA_1_1_B1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get ./VB/VB_1/VP_1/VM_1/VA_1_1_B1", va1b1 != 0);
    MUnit* vb1a1 = root->GetNode("./VB/VA_1");
    MElem* evb1a1 = vb1a1->GetObj(evb1a1);
    evb1a1->Chromos().Save("ut_parmod1_res_vb1a1_saved.xml");

    delete iEnv;

}

void Ut_mut::test_ParentMut()
{
    printf("\n === Test of mutating of parent\n");
    iEnv = new Env("ut_par_mut1.xml", "ut_par_mut1.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != NULL);
    iEnv->ConstructSystem();
    MUnit* root = iEnv->Root();
    MElem* eroot = root->GetObj(eroot);
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != NULL);
    MUnit* va = root->GetNode("./VA");
    /*
       ChromoNode madd = va->AppendMutation(ENt_Node);
       madd.SetAttr(ENa_Id, "VA_New");
       madd.SetAttr(ENa_Parent, "./../VP");
       va->Mutate();
       */
    // Check propagation of mutation to VR_1
    // R-VV-VB--VA_1-VA_1_1--VR_1
    MUnit* vr1 = root->GetNode("./VA/VA_1/VA_1_1/VR_1");
    ChromoNode m2 = eroot->AppendMutation(ENt_Node);
    m2.SetAttr(ENa_Targ, "./VA/VA_1/VA_1_1/VR_1");
    m2.SetAttr(ENa_Id, "VR_1_1_New");
    m2.SetAttr(ENa_Parent, "Vert");
    TNs ns; MutCtx mctx(NULL, ns);
    eroot->Mutate(false, false, true, mctx);
    eroot->Chromos().Save("ut_par_mut1_res.xml_");
    //MUnit* van = root->GetNode("./VV/VB/VA_New");
    //CPPUNIT_ASSERT_MESSAGE("Fail to get ./VB/VA_New", van != NULL);
    MUnit* vr11n = root->GetNode("./VV/VB/VA_1/VA_1_1/VR_1/VR_1_1_New");
    CPPUNIT_ASSERT_MESSAGE("Fail to get ./VV/VB/VA_1/VA_1_1/VR_1/VR_1_1_New", vr11n != NULL);
    delete iEnv;
}

// Verified that parent's mutation are propageted as run-time modifications, ref ds_mut_osmlc_pm
// TODO [YB] To verify error on model assembling phase
void Ut_mut::test_ParentMut2()
{
    printf("\n === Test of mutating of parent\n");
    iEnv = new Env("ut_par_mut2.xml", "ut_par_mut2.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != NULL);
    iEnv->ConstructSystem();
    MUnit* root = iEnv->Root();
    MElem* eroot = root->GetObj(eroot);
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != NULL);
    eroot->Chromos().Save("ut_par_mut2_res.xml_");
    MUnit* b1 = root->GetNode("./B1");
    MElem* eb1 = b1->GetObj(eb1);
    CPPUNIT_ASSERT_MESSAGE("Fail to get B1", b1 != NULL);
    eb1->Chromos().Save("ut_par_mut2_res_b1.xml_");
    MUnit* c1 = root->GetNode("./C1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get C1", c1 != NULL);
    delete iEnv;
}

// Transform modif to mutation
void Ut_mut::test_TransfModif1()
{
    printf("\n === Test of transforming modif to mut\n");
    iEnv = new Env("ut_transfmdf1.xml", "ut_transfmdf1.txt");
    iEnv->ChMgr()->SetEnableFixErrors(ETrue);
    iEnv->ChMgr()->SetEnableReposMuts(ETrue);
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ConstructSystem();
    MUnit* root = iEnv->Root();
    MElem* eroot = root->GetObj(eroot);
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    // Make illegal modif and enable transformation
    MUnit* va = root->GetNode("./VA");
    MElem* eva = va->GetObj(eva);
    ChromoNode madd = eva->AppendMutation(ENt_Node);
    madd.SetAttr(ENa_Id, "VA_1_1");
    madd.SetAttr(ENa_Parent, "Vert");
    TNs ns; MutCtx mctx(NULL, ns);
    eroot->Mutate(false, false, true, mctx);
    // Save transformed chromo and recreate the model
    eroot->Chromos().Save("ut_transfmdf1_res.xml_");
    delete iEnv;

    iEnv = new Env("ut_transfmdf1_res.xml_", "ut_transfmdf1_res.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to re-create system after transfortation of modifs", iEnv != 0);
    iEnv->ConstructSystem();
    // Check if edge is set correctly
    root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root after transforming", root != 0);
    //Elem* va1b1 = root->GetNode("./VB/VB_1/VA_1_B1");
    //CPPUNIT_ASSERT_MESSAGE("Fail to get ./VB/VB_1/VA_1_B1", va1b1 != 0);

    delete iEnv;

}



