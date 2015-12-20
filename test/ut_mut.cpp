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
    CPPUNIT_TEST(test_Add);
    CPPUNIT_TEST(test_MutSyst);
    CPPUNIT_TEST(test_Move);
    CPPUNIT_TEST(test_MutRmRecr);
    CPPUNIT_TEST(test_MutRmRecrInh);
    CPPUNIT_TEST(test_MutDepsRm);
    CPPUNIT_TEST(test_MutDepsRm2);
    CPPUNIT_TEST(test_MutDepsChilds1);
    CPPUNIT_TEST(test_MutDepsRmRef);
    CPPUNIT_TEST(test_MutInv1);
    CPPUNIT_TEST(test_MutInvRename);
    CPPUNIT_TEST(test_MutInvParent);
    CPPUNIT_TEST(test_MutInvImplicit);
    CPPUNIT_TEST(test_MutRmParent);
    CPPUNIT_TEST(test_MutRenameParent);
    CPPUNIT_TEST(test_Compact1);
    CPPUNIT_TEST(test_OptRmDeps);
    CPPUNIT_TEST(test_Compact2);
    CPPUNIT_TEST(test_Compact3);
    CPPUNIT_TEST(test_CompactRmDa);
    CPPUNIT_TEST(test_CompactRef1);
    CPPUNIT_TEST(test_CompactCont);
    CPPUNIT_TEST(test_TransfModif1);
    CPPUNIT_TEST(test_GetParentModifs);
    CPPUNIT_TEST_SUITE_END();
public:
    virtual void setUp();
    virtual void tearDown();
private:
    void test_Add();
    void test_MutSyst();
    void test_Move();
    void test_MutRmRecr();
    void test_MutRmRecrInh();
    void test_MutDepsRm();
    void test_MutDepsRm2();
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
private:
    Env* iEnv;
};

CPPUNIT_TEST_SUITE_REGISTRATION( Ut_mut );


void Ut_mut::setUp()
{
}

void Ut_mut::tearDown()
{
//    delete iEnv;
    CPPUNIT_ASSERT_EQUAL_MESSAGE("tearDown", 0, 0);
}

void Ut_mut::test_Add()
{
    printf("\n === Test#1 of mutation\n");

    iEnv = new Env("ut_mutadd_1.xml", "ut_mutadd_1.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ConstructSystem();
    Elem* root = iEnv->Root();
    // Check creation first
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    MElem* e2 = root->GetNode("./elem1/elem2");
    CPPUNIT_ASSERT_MESSAGE("Fail to get e2", e2 != 0);
    MElem* e4 = root->GetNode("./elem3/elem4");
    CPPUNIT_ASSERT_MESSAGE("Fail to get e4", e4 != 0);
    // Mutation of type "Adding node to current node"
    ChromoNode madd = root->Mutation().Root().AddChild(ENt_Node);
    madd.SetAttr(ENa_Id, "new_elem1");
    madd.SetAttr(ENa_Parent, "./elem5");
    root->Mutate();
    // Check the element added
    MElem* eadded = root->GetNode("./(elem5:)new_elem1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get elem added", eadded != 0);
    // Mutation of type "Delete node"
    ChromoNode mdel = root->Mutation().Root().AddChild(ENt_Rm);
    mdel.SetAttr(ENa_MutNode, "./new_elem1");
    root->Mutate();
    MElem* erem = root->GetNode("./(elem5:)new_elem1");
    CPPUNIT_ASSERT_MESSAGE("Fail to remove elem", erem == 0);
    // Check if we can add the node with the same name again
    ChromoNode madd1 = root->Mutation().Root().AddChild(ENt_Node);
    madd1.SetAttr(ENa_Id, "new_elem1");
    madd1.SetAttr(ENa_Parent, "./elem5");
    root->Mutate();
    // Check the element added
    MElem* eadded1 = root->GetNode("./new_elem1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get elem added", eadded1 != 0);
     // Mutation of type "Rename node"
    ChromoNode mren = root->Mutation().Root().AddChild(ENt_Change);
    mren.SetAttr(ENa_MutNode, "./elem5");
    mren.SetAttr(ENa_MutAttr, GUriBase::NodeAttrName(ENa_Id));
    mren.SetAttr(ENa_MutVal, "elem5_renamed");
    root->Mutate();
    MElem* eren = root->GetNode("./elem5_renamed");
    CPPUNIT_ASSERT_MESSAGE("Fail to rename node", eren != 0);
 
    delete iEnv;
}

void Ut_mut::test_MutSyst()
{
    printf("\n === Test of mutation of system\n");

    iEnv = new Env("ut_mut_syst.xml", "ut_mut_syst.txt");
    // Check creation first
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
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
    // Delete edge
    ChromoNode mdel = root->Mutation().Root().AddChild(ENt_Rm);
    mdel.SetAttr(ENa_MutNode, "./edge1");
    root->Mutate();
    MElem* erem = root->GetNode("./edge1");
    CPPUNIT_ASSERT_MESSAGE("Fail to remove edge", erem == 0);
    // Adding the edge again
    ChromoNode madd = root->Mutation().Root().AddChild(ENt_Node);
    madd.SetAttr(ENa_Id, "edge1");
    madd.SetAttr(ENa_Parent, "Edge");
    ChromoNode cnt1 = madd.AddChild(ENt_Cont);
    cnt1.SetAttr(ENa_MutNode, "./P1");
    cnt1.SetAttr(ENa_Ref, "./../../Syst1/cp");
    ChromoNode cnt2 = madd.AddChild(ENt_Cont);
    cnt2.SetAttr(ENa_MutNode, "./P2");
    cnt2.SetAttr(ENa_Ref, "./../../cp2");
    root->Mutate();
    // Check the element added
    MElem* eadded = root->GetNode("./edge1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get edge added after deleting", eadded != 0);
    MElem* cp_1 = root->GetNode("./Syst1/cp");
    CPPUNIT_ASSERT_MESSAGE("Fail to get Syst1/cp", cp_1 != 0);
    MVert* mcp_1 = cp_1->GetObj(mcp_1);
    CPPUNIT_ASSERT_MESSAGE("Fail to get Syst1/cp MVert", mcp_1 != 0);
    MVert* pair_1 = *(mcp_1->Pairs().begin());
    CPPUNIT_ASSERT_MESSAGE("Fail to get pair of Syst1/cp", pair_1 != 0);
    MElem* epair_1 = pair_1->EBase()->GetObj(epair_1);
    const string pname_1 = epair_1->Name();
    CPPUNIT_ASSERT_MESSAGE("Wrong name of pair of Syst1/cp", pname_1 == "cp2");
 
    delete iEnv;
}

void Ut_mut::test_Move()
{
    printf("\n === Test of mutation: moving node\n");

    iEnv = new Env("ut_mutmove_1.xml", "ut_mutmove_1.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ConstructSystem();
    Elem* root = iEnv->Root();
    // Check creation first
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    MElem* e2 = root->GetNode("./elem1/elem2");
    CPPUNIT_ASSERT_MESSAGE("Fail to get e2", e2 != 0);
    MElem* e4 = root->GetNode("./(elem1:)elem3/(elem2:)elem4");
    CPPUNIT_ASSERT_MESSAGE("Fail to get e4", e4 != 0);

    // Moving remote node
    ChromoNode mmovermt = root->Mutation().Root().AddChild(ENt_Move);
    mmovermt.SetAttr(ENa_Id, "file:../modules/syst.xml#");
    mmovermt.SetAttr(ENa_MutNode, "/test");
    root->Mutate();
    MElem* eext = root->GetNode("/test/SysComps/Extender");
    CPPUNIT_ASSERT_MESSAGE("Fail get extender from moved remote module", eext != NULL);

    // Moving local node
    ChromoNode mmove = root->Mutation().Root().AddChild(ENt_Move);
    mmove.SetAttr(ENa_Id, "./elem5");
    mmove.SetAttr(ENa_MutNode, "./elem3/elem4");
    root->Mutate();
    MElem* emoved = root->GetNode("./elem3/elem4/elem5");
    CPPUNIT_ASSERT_MESSAGE("Fail to move local node elem5", emoved != NULL);
 
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
    Elem* root = iEnv->Root();
     // Check creation first
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    ChromoNode mut = root->Mutation().Root().AddChild(ENt_Node);
    mut.SetAttr(ENa_Id, "v1");
    mut.SetAttr(ENa_Parent, "./v1p");
    root->Mutate();
    MElem* v1 = root->GetNode("./v1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get v1", v1 != 0);
    MElem* v1p_1i = root->GetNode("./v1/v1p_1i");
    CPPUNIT_ASSERT_MESSAGE("Fail to get v1p_1i", v1p_1i != 0);
    // Try to use ref to v1 and verify the ref is found correctly (no duplication occurs)
    MElem* e1 = root->GetNode("./edge1");
    ChromoNode mut1 = e1->Mutation().Root().AddChild(ENt_Cont);
    mut1.SetAttr(ENa_MutNode, "./P2");
    mut1.SetAttr(ENa_Ref, "./../../v1");
    e1->Mutate();
    MElem* p2 = e1->GetNode("./P2");
    string p2_cont;
    p2->GetCont(p2_cont);
    CPPUNIT_ASSERT_MESSAGE("Fail to set edge1/P2 with ref to v1", p2_cont == "./../../v1");
    iEnv->Root()->Chromos().Save("ut_mut_rm_recr_res.xml_");
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
    Elem* root = iEnv->Root();
     // Check creation first
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    MElem* v1 = root->GetNode("./v1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get v1", v1 != 0);
    // Verify the ref to v1/v1p_1 is found correctly (no duplication occurs)
    MElem* e1 = root->GetNode("./edge1");
    MElem* p2 = e1->GetNode("./P2");
    string p2_cont;
    p2->GetCont(p2_cont);
    CPPUNIT_ASSERT_MESSAGE("Fail to set edge1/P2 with ref", p2_cont == "./../../v1/v1p_1/v1p_1_1");
    iEnv->Root()->Chromos().Save("ut_mut_rm_recrinh_res.xml_");
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
    iEnv->ChMgr()->SetEnableCheckSafety(ETrue);
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ConstructSystem();
    Elem* root = iEnv->Root();
     // Check creation first
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    MElem* e2 = root->GetNode("./elem1/elem2");
    CPPUNIT_ASSERT_MESSAGE("Fail to get elem2", e2 != 0);
    MElem* e3 = root->GetNode("./(elem2:)elem3");
    CPPUNIT_ASSERT_MESSAGE("Fail to get elem3", e3 != 0);
    // Get major dep
    //Elem::TDep mdep = e2->GetMajorDep();
    TMDep mdep = e2->GetMajorDep();
    //CPPUNIT_ASSERT_MESSAGE("Fail to get elem2 major dep", mdep.first == e3 && mdep.second == -1);
    TBool e2dep_ok = mdep.first.first == root && mdep.first.second == e3->Chromos().Root().Handle() && mdep.second == ENa_Parent;
    CPPUNIT_ASSERT_MESSAGE("Fail to get elem2 major dep", e2dep_ok);
    // Try to remove elem2 from elem1 - unsafe mutation
    MElem* e1 = root->GetNode("./elem1");
    ChromoNode mut = e1->Mutation().Root().AddChild(ENt_Rm);
    mut.SetAttr(ENa_MutNode, "./elem2");
    e1->Mutate();
    // Check that the mutation is refused
    e2 = root->GetNode("./elem1/elem2");
    CPPUNIT_ASSERT_MESSAGE("Mutation -rm- of elem2 is not refused", e2 != NULL);
    // Try to remove elem2 from root - safe mutation
    mut = root->Mutation().Root().AddChild(ENt_Rm);
    mut.SetAttr(ENa_MutNode, "./elem1/elem2");
    root->Mutate();
    // Check that the mutation is refused because of having childs, to be unparent first
    e2 = root->GetNode("./elem1/elem2");
    CPPUNIT_ASSERT_MESSAGE("Root mutation -rm- of elem2 is not refused", e2 != NULL);
}

// Preventing of mutation braking model consistency
// Mutation - rm, dependency - object of change
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
    MElem* e2 = root->GetNode("./elem1/elem2");
    CPPUNIT_ASSERT_MESSAGE("Fail to get elem2", e2 != 0);
    // Get major dep
    TMDep mdep = e2->GetMajorDep();
    CPPUNIT_ASSERT_MESSAGE("Fail to get elem2 major dep", mdep.first.first == root);
    // Try to remove elem2 from elem1 - unsafe mutation
    MElem* e1 = root->GetNode("./elem1");
    ChromoNode mut = e1->Mutation().Root().AddChild(ENt_Rm);
    mut.SetAttr(ENa_MutNode, "./elem2");
    e1->Mutate();
    // Check that the mutation is refused
    e2 = root->GetNode("./elem1/elem2");
    CPPUNIT_ASSERT_MESSAGE("Mutation -rm- of elem2 is not refused", e2 != NULL);
    // Try to remove elem2 from root - safe mutation
    mut = root->Mutation().Root().AddChild(ENt_Rm);
    mut.SetAttr(ENa_MutNode, "./elem1/elem2");
    root->Mutate();
    // Check that the mutation is not refused
    e2 = root->GetNode("./elem1/elem2");
    CPPUNIT_ASSERT_MESSAGE("Root mutation -rm- of elem2 is refused", e2 == NULL);
}

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
    Elem* root = iEnv->Root();
     // Check creation first
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    MElem* e2 = root->GetNode("./elem1/elem2");
    CPPUNIT_ASSERT_MESSAGE("Fail to get elem2", e2 != 0);
    // Applying harmful mutation
    MElem* e1 = root->GetNode("./elem1");
    ChromoNode mut = e1->Mutation().Root().AddChild(ENt_Change);
    mut.SetAttr(ENa_MutNode, "./elem2");
    mut.SetAttr(ENa_MutAttr, "id");
    mut.SetAttr(ENa_MutVal, "elem2_renamed");
    e1->Mutate();
    // Check that the mutation is refused
    string e2_name = e2->Name();
    CPPUNIT_ASSERT_MESSAGE("Harmful mutation hasn't been refused", e2_name == "elem2");
}


// Preventing of mutation braking model consistency
// Mutation - rm, dependency - ref to node
void Ut_mut::test_MutDepsRmRef()
{
    printf("\n === Test of mutation consistency, mut -rm-, dep - ref to node \n");

    iEnv = new Env("ut_mut_dep_refs.xml", "ut_mut_dep_refs.txt");
    // Enabling pheno, because of model is basing on pheno
    iEnv->ChMgr()->SetEnablePhenoModif(ETrue);
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ConstructSystem();
    Elem* root = iEnv->Root();
     // Check creation first
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    MElem* v1_0 = root->GetNode("./v1/v1_0");
    CPPUNIT_ASSERT_MESSAGE("Fail to get v1/v1_0", v1_0 != 0);
    // Try to remove v1_0 from v1 - unsafe mutation
    MElem* v1 = root->GetNode("./v1");
    ChromoNode mut = v1->Mutation().Root().AddChild(ENt_Rm);
    mut.SetAttr(ENa_MutNode, "./v1_0");
    v1->Mutate(EFalse, ETrue, ETrue);
    // Check that the mutation is refused because not being safe -
    // there are deps with the rank higher than the rank of this mutatins,
    // so error will occur if node is deleted
    v1_0 = root->GetNode("./v1/v1_0");
    CPPUNIT_ASSERT_MESSAGE("Mutation -rm- of v1_0 is not refused", v1_0 != NULL);
    // Try to remove elem2 from root - safe mutation
    mut = root->Mutation().Root().AddChild(ENt_Rm);
    mut.SetAttr(ENa_MutNode, "./v1/v1_0");
    root->Mutate(EFalse, ETrue, ETrue);
    // Check that the mutation is refused because there is critical dep on the node being
    // removed. Even the rank of this dep is lower that the rank of this mutation, the
    // deps will generate error so the model will be inconsistent 
    v1_0 = root->GetNode("./v1/v1_0");
    CPPUNIT_ASSERT_MESSAGE("Mutation -rm- of v1_0 from root is not refused", v1_0 != NULL);
    // Remove critical dep first, then optimize the chromo, reload and try to remove v1_0 again
    mut = root->Mutation().Root().AddChild(ENt_Cont);
    mut.SetAttr(ENa_MutNode, "./edge1/P1");
    mut.SetAttr(ENa_Ref, "");
    root->Mutate(EFalse, ETrue, ETrue);
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
    ChromoNode mut1 = root->Mutation().Root().AddChild(ENt_Rm);
    mut1.SetAttr(ENa_MutNode, "./v1/v1_0");
    root->Mutate(EFalse, ETrue, ETrue);
    v1_0 = root->GetNode("./v1/v1_0");
    CPPUNIT_ASSERT_MESSAGE("Mutation -rm- of v1_0 from root is refused", v1_0 == NULL);
    iEnv->Root()->Chromos().Save("ut_mut_dep_refs_res1.xml_");
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
    Elem* root = iEnv->Root();
    // Check creation first
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != NULL);
    MElem* v1 = root->GetNode("./v1");
    MElem* v2 = root->GetNode("./v2");
    CPPUNIT_ASSERT_MESSAGE("Fail to get v1 and v2", v1 != NULL && v2 != NULL);
    // Check that the inital connection is set
    MVert* mv1 = v1->GetObj(mv1);
    MVert* mv2 = v2->GetObj(mv2);
    CPPUNIT_ASSERT_MESSAGE("Fail to connect v1 to v2", mv1->Pairs().count(mv2) == 1);
    // Try to mutate edge point P2 to change v2 to v3
    MElem* edge1 = root->GetNode("./edge1");
    ChromoNode mut = edge1->Mutation().Root().AddChild(ENt_Cont);
    mut.SetAttr(ENa_MutNode, "./P2");
    mut.SetAttr(ENa_Ref, "./../../v3");
    edge1->Mutate();
   
    // Save upated chromo 
    iEnv->Root()->Chromos().Save("ut_mut_inv_1_res.xml_");
    delete iEnv;
    // Shifting mutation introduced another unsafety for -p1-, checking if it will be 
    // resolved next model creation
    iEnv = new Env("ut_mut_inv_1_res.xml_", "ut_mut_inv_1_res.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env1", iEnv != 0);
    // Enabling mutation repositioning in order to resolve unsafety
    iEnv->ChMgr()->SetEnableReposMuts(ETrue);
    iEnv->ConstructSystem();
    root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != NULL);
    // Check that the renewed connection is set
    v1 = root->GetNode("./v1");
    mv1 = v1->GetObj(mv1);
    MElem* v3 = root->GetNode("./v3");
    MVert* mv3 = v3->GetObj(mv3);
    CPPUNIT_ASSERT_MESSAGE("Fail to connect v1 to v3", mv1->Pairs().count(mv3) == 1);
    // Check that secondary forward dependency was resolved
    MElem* p1 = root->GetNode("./p1");
    string p1_cont;
    p1->GetCont(p1_cont);
    CPPUNIT_ASSERT_MESSAGE("Fail to set p1 with ref to edge1/P1", p1_cont == "./../edge1/P1");
    // Save upated chromo 
    iEnv->Root()->Chromos().Save("ut_mut_inv_1_res2.xml_");
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
    Elem* root = iEnv->Root();
    // Check creation first
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != NULL);
    MElem* v1 = root->GetNode("./v1");
    MElem* v2 = root->GetNode("./v2");
    CPPUNIT_ASSERT_MESSAGE("Fail to get v1 and v2", v1 != NULL && v2 != NULL);
    // Check that the inital connection is set
    MVert* mv1 = v1->GetObj(mv1);
    MVert* mv2 = v2->GetObj(mv2);
    CPPUNIT_ASSERT_MESSAGE("Fail to connect v1 to v2", mv1->Pairs().count(mv2) == 1);
    // Try to mutate edge point P2 to change v2 to v3
    MElem* edge1 = root->GetNode("./edge1");
    ChromoNode mut = edge1->Mutation().Root().AddChild(ENt_Cont);
    mut.SetAttr(ENa_MutNode, "./P2");
    mut.SetAttr(ENa_Ref, "./../../v3");
    edge1->Mutate();
   
    // Save upated chromo 
    iEnv->Root()->Chromos().Save("ut_mut_inv_rnm_res.xml_");
    delete iEnv;
    // Shifting mutation introduced another unsafety for -p1-, checking if it will be 
    // resolved next model creation
    iEnv = new Env("ut_mut_inv_rnm_res.xml_", "ut_mut_inv_rnm_res.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env1", iEnv != 0);
    // Enabling mutation repositioning in order to resolve unsafety
    iEnv->ChMgr()->SetEnableReposMuts(ETrue);
    iEnv->ConstructSystem();
    root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != NULL);
    // Check that the renewed connection is set
    v1 = root->GetNode("./v1");
    mv1 = v1->GetObj(mv1);
    MElem* v3 = root->GetNode("./v3");
    MVert* mv3 = v3->GetObj(mv3);
    CPPUNIT_ASSERT_MESSAGE("Fail to connect v1 to v3", mv1->Pairs().count(mv3) == 1);
    // Check that secondary forward dependency was resolved
    MElem* p1 = root->GetNode("./p1");
    string p1_cont;
    p1->GetCont(p1_cont);
    CPPUNIT_ASSERT_MESSAGE("Fail to set p1 with ref to edge1/P1", p1_cont == "./../edge1/P1");
    // Save upated chromo 
    iEnv->Root()->Chromos().Save("ut_mut_inv_rnm_res2.xml_");
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
    Elem* root = iEnv->Root();
    // Check creation first
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != NULL);
    MElem* v1 = root->GetNode("./v1");
    MElem* v2 = root->GetNode("./v2");
    CPPUNIT_ASSERT_MESSAGE("Fail to get v1 and v2", v1 != NULL && v2 != NULL);
    // Try to mutate v1 by adding child of v2
    ChromoNode mut = v1->Mutation().Root().AddChild(ENt_Node);
    mut.SetAttr(ENa_Id, "v2_child");
    mut.SetAttr(ENa_Parent, "./../v2");
    v1->Mutate();
    // Check that the mutation applied
    MElem* v2_child = root->GetNode("./v1/v2_child");
    CPPUNIT_ASSERT_MESSAGE("Fail to create v2_child", v2_child != NULL);
    
    // Save upated chromo 
    iEnv->Root()->Chromos().Save("ut_mut_inv_prnt_res.xml_");
    delete iEnv;
    // Shifting mutation introduced another unsafety for -p1-, checking if it will be 
    // resolved next model creation
    iEnv = new Env("ut_mut_inv_prnt_res.xml_", "ut_mut_inv_prnt_res.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env1", iEnv != 0);
    // Enabling mutation repositioning in order to resolve unsafety
    iEnv->ChMgr()->SetEnableReposMuts(ETrue);
    iEnv->ConstructSystem();
    root = iEnv->Root();
    // Check that secondary forward dependency was resolved
    MElem* p1 = root->GetNode("./p1");
    string p1_cont;
    p1->GetCont(p1_cont);
    CPPUNIT_ASSERT_MESSAGE("Fail to set p1 with ref to v1", p1_cont == "./../v1");
    // Save upated chromo 
    iEnv->Root()->Chromos().Save("ut_mut_inv_prnt_res2.xml_");
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
    Elem* root = iEnv->Root();
    // Check creation first
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != NULL);
    MElem* ee = root->GetNode("./System/E");
    CPPUNIT_ASSERT_MESSAGE("Fail to get edge [E]", ee != NULL);
    // Try to mutate edge by setting ref to P2
    ChromoNode mut = ee->Mutation().Root().AddChild(ENt_Cont);
    mut.SetAttr(ENa_MutNode, "./P2");
    mut.SetAttr(ENa_Ref, "./../../Syst_B/Cp");
    ee->Mutate();
    // Check that the mutation applied
    MElem* sacp = root->GetNode("./System/Syst_A/Cp");
    MElem* sbcp = root->GetNode("./System/Syst_B/Cp");
    MVert* sacpv = sacp->GetObj(sacpv);
    MVert* sbcpv = sbcp->GetObj(sbcpv);
    MVert* pair = *(sacpv->Pairs().begin());
    CPPUNIT_ASSERT_MESSAGE("Fail to get pair of Syst_A Cp", pair != 0);
    CPPUNIT_ASSERT_MESSAGE("Wrong pair's name", pair->EBase() == sbcp);
    
    // Save upated chromo 
    iEnv->Root()->Chromos().Save("ut_mut_inv_impl_res.xml_");
    delete iEnv;
    // Recreate the system
    iEnv = new Env("ut_mut_inv_impl_res.xml_", "ut_mut_inv_impl_res.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env1", iEnv != 0);
    // Enabling mutation repositioning in order to resolve unsafety
    iEnv->ChMgr()->SetEnableReposMuts(ETrue);
    iEnv->ConstructSystem();
    root = iEnv->Root();
    // Check that the mutation applied
    sacp = root->GetNode("./System/Syst_A/Cp");
    sbcp = root->GetNode("./System/Syst_B/Cp");
    sacpv = sacp->GetObj(sacpv);
    sbcpv = sbcp->GetObj(sbcpv);
    pair = *(sacpv->Pairs().begin());
    CPPUNIT_ASSERT_MESSAGE("Fail to get pair of Syst_A Cp", pair != 0);
    CPPUNIT_ASSERT_MESSAGE("Wrong pair's name", pair->EBase() == sbcp);
    // Save upated chromo 
    iEnv->Root()->Chromos().Save("ut_mut_inv_impl_res2.xml_");
    delete iEnv;
}


// Handling of parents removing, ref uc_029
void Ut_mut::test_MutRmParent()
{
    printf("\n === Test of system consistency after parent removed\n");

    iEnv = new Env("ut_mutadd_1.xml", "ut_mut_rm_parent.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ConstructSystem();
    Elem* root = iEnv->Root();
     // Check creation first
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    MElem* e2 = root->GetNode("./elem1/elem2");
    CPPUNIT_ASSERT_MESSAGE("Fail to get elem2", e2 != 0);
    // Remove elem2, which is parent of elem5 
    ChromoNode mut = root->Mutation().Root().AddChild(ENt_Rm);
    mut.SetAttr(ENa_MutNode, "./elem1/elem2");
    root->Mutate();
    // Create child from elem5
    ChromoNode madd = root->Mutation().Root().AddChild(ENt_Node);
    madd.SetAttr(ENa_Id, "elem6");
    madd.SetAttr(ENa_Parent, "./elem5");
    root->Mutate();
    // Check that the mutation is performed
    MElem* e6 = root->GetNode("./elem6");
    CPPUNIT_ASSERT_MESSAGE("Creating elem6 as child of elem5 failed", e6 != NULL);
    // Check that the elem6 contains mutation from deleted parent elem2
    MElem* e2_1 = root->GetNode("./elem6/elem2_1");
    CPPUNIT_ASSERT_MESSAGE("elem6 containing deleted parent comp elem2_1 failed", e2_1 != NULL);

    // Trying to remove elem4 from elem3. elem4 doens't have direct deps but via comp elem4_1
    // System must deny removing elem4 from elem3
    MElem* e3 = root->GetNode("./elem3");
    mut = e3->Mutation().Root().AddChild(ENt_Rm);
    mut.SetAttr(ENa_MutNode, "./elem4");
    e3->Mutate();
    // Check that the mutation is dinied
    MElem* e4 = root->GetNode("./elem3/elem4");
    CPPUNIT_ASSERT_MESSAGE("Removing elem4 from elem3 isn't denied", e4 != NULL);

    // Remove elem3/elem4- owner of elem4_1, which is parent of elem4_1i 
    // The deps are mutations that referred to components of the node
    // System should not remove the node if there are inheritance deps, also from nodes comps
    mut = root->Mutation().Root().AddChild(ENt_Rm);
    mut.SetAttr(ENa_MutNode, "./elem3/elem4");
    root->Mutate();
    // Create child from elem4_1i
    madd = root->Mutation().Root().AddChild(ENt_Node);
    madd.SetAttr(ENa_Id, "elem7");
    madd.SetAttr(ENa_Parent, "./elem4_1i");
    root->Mutate();
    // Check that the mutation is performed
    MElem* e7 = root->GetNode("./elem7");
    CPPUNIT_ASSERT_MESSAGE("Creating elem7 as child of elem4_1i (comp of removed elem4) failed", e7 != NULL);
    root->Mutate();
}

// Handling of parents renaming, ref uc_031
void Ut_mut::test_MutRenameParent()
{
    printf("\n === Test of system consistency after parent renamed\n");

    iEnv = new Env("ut_mutadd_1.xml", "ut_mut_rn_parent.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ConstructSystem();
    Elem* root = iEnv->Root();
     // Check creation first
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    MElem* e1 = root->GetNode("./elem1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get elem1", e1 != 0);
    MElem* e2 = root->GetNode("./elem1/elem2");
    CPPUNIT_ASSERT_MESSAGE("Fail to get elem2", e2 != 0);
    // Rename elem2, which is parent of elem5 
    ChromoNode mut = e1->Mutation().Root().AddChild(ENt_Change);
    mut.SetAttr(ENa_MutNode, "./elem2");
    mut.SetAttr(ENa_MutAttr, GUriBase::NodeAttrName(ENa_Id));
    mut.SetAttr(ENa_MutVal, "elem2_renamed");
    root->Mutate();
    // Create child from elem5
    ChromoNode madd = root->Mutation().Root().AddChild(ENt_Node);
    madd.SetAttr(ENa_Id, "elem6");
    madd.SetAttr(ENa_Parent, "./elem5");
    root->Mutate();
    // Check that the mutation is performed
    MElem* e6 = root->GetNode("./elem6");
    CPPUNIT_ASSERT_MESSAGE("Creating elem6 as child of elem5 failed", e6 != NULL);
    // Check that the elem6 contains mutation from deleted parent elem2
    MElem* e2_1 = root->GetNode("./elem6/elem2_1");
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
    Elem* root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
     // Check creation first
    MElem* e3 = root->GetNode("./elem3");
    CPPUNIT_ASSERT_MESSAGE("Fail to get elem3", e3 != 0);
    // Compact chromo
    root->CompactChromo();
    // Save compacted chromo and recreate the model
    iEnv->Root()->Chromos().Save("ut_compact1_res.xml_");
    delete iEnv;

    iEnv = new Env("ut_compact1_res.xml_", "ut_compact1_res.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env for compacted system", iEnv != 0);
    iEnv->ConstructSystem();
     // Check if elem3 and elem4 are created ok with compacted chromo
    root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root after compacting", root != 0);
    e3 = root->GetNode("./elem3");
    CPPUNIT_ASSERT_MESSAGE("Fail to get elem3", e3 != 0);
    MElem* e4 = root->GetNode("./elem4");
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
    Elem* root = iEnv->Root();
     // Check creation first
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    MElem* v1 = root->GetNode("./v1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get v1", v1 != 0);
    // Try to remove v1_0 from v1 - unsafe mutation
    ChromoNode mut = root->Mutation().Root().AddChild(ENt_Rm);
    mut.SetAttr(ENa_MutNode, "./v1");
    root->Mutate(EFalse, ETrue, ETrue);
    // Check that the mutation is refused because not being safe -
    // there are deps with the rank higher than the rank of this mutatins,
    // so error will occur if node is deleted
    v1 = root->GetNode("./v1");
    CPPUNIT_ASSERT_MESSAGE("Mutation -rm- of v1 is not refused", v1 != NULL);
    // Remove critical deps and try to remove v1_0 again
    MElem* edge1 = root->GetNode("./edge1");
    mut = edge1->Mutation().Root().AddChild(ENt_Cont);
    mut.SetAttr(ENa_MutNode, "./P1");
    mut.SetAttr(ENa_Ref, "");
    edge1->Mutate(EFalse, ETrue, ETrue);
    mut = root->Mutation().Root().AddChild(ENt_Rm);
    mut.SetAttr(ENa_MutNode, "./v1_0_i1");
    root->Mutate(EFalse, ETrue, ETrue);
    ChromoNode mut1 = root->Mutation().Root().AddChild(ENt_Rm);
    mut1.SetAttr(ENa_MutNode, "./v1");
    root->Mutate(EFalse, ETrue, ETrue);
    v1 = root->GetNode("./v1");
    iEnv->Root()->Chromos().Save("ut_opt_rm_deps_res1.xml_");
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
    Elem* root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    // Compact chromo
    root->CompactChromo();
    // Save compacted chromo and recreate the model
    iEnv->Root()->Chromos().Save("ut_compact2_res.xml_");
    delete iEnv;

    iEnv = new Env("ut_compact2_res.xml_", "ut_compact2_res.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to re-create compacted system", iEnv != 0);
    iEnv->ConstructSystem();
     // Check if edge is set correctly
    root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root after compacting", root != 0);
    MElem* v1 = root->GetNode("./(Vert:)v1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get v1", v1 != 0);
    MVert* mv1 = v1->GetObj(mv1);
    CPPUNIT_ASSERT_MESSAGE("Fail to get mv1", mv1 != 0);
    MVert* pair = *(mv1->Pairs().begin());
    CPPUNIT_ASSERT_MESSAGE("Fail to get pair", pair != 0);
    MElem* v3 = root->GetNode("./(Vert:)v3");
    CPPUNIT_ASSERT_MESSAGE("Wrong pair's name", pair->EBase() == v3);

    delete iEnv;
}

// Compactization of chromo: mut removing node
void Ut_mut::test_Compact3()
{
    printf("\n === Test of compacting of chromo: removing\n");
    iEnv = new Env("ut_compact3.xml", "ut_compact3.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ConstructSystem();
    Elem* root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    // Compact chromo
    root->CompactChromo();
    // Save compacted chromo and recreate the model
    iEnv->Root()->Chromos().Save("ut_compact3_res.xml_");
    delete iEnv;

    iEnv = new Env("ut_compact3_res.xml_", "ut_compact3_res.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to re-create compacted system", iEnv != 0);
    iEnv->ConstructSystem();
     // Check that v3 is not created
    root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root after compacting", root != 0);
    MElem* v3 = root->GetNode("./(Vert:)v3");
    CPPUNIT_ASSERT_MESSAGE("V3 is not removed", v3 == NULL);
    ChromoNode::Iterator it = root->Chromos().Root().Begin(); 
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
    TInt compsnum = root->Comps().size();
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
    Elem* root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    // Compact chromo
    root->CompactChromo();
    // Save compacted chromo and recreate the model
    iEnv->Root()->Chromos().Save("ut_compact_rmda_res.xml_");
    delete iEnv;

    iEnv = new Env("ut_compact_rmda_res.xml_", "ut_compact_rmda_res.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to re-create compacted system", iEnv != 0);
    iEnv->ConstructSystem();
     // Check that -rm- mut is not optimized out
    root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root after compacting", root != 0);
    MElem* v3a1 = root->GetNode("./v3_a1");
    CPPUNIT_ASSERT_MESSAGE("v3_a1 is not created", v3a1 != NULL);
    ChromoNode::Iterator it = v3a1->Chromos().Root().Begin(); 
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
    Elem* root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    // Compact chromo
    root->CompactChromo();
    // Save compacted chromo and recreate the model
    iEnv->Root()->Chromos().Save("ut_compact_ref1_res.xml_");
    delete iEnv;

    iEnv = new Env("ut_compact_ref1_res.xml_", "ut_compact_ref1_res.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to re-create compacted system", iEnv != 0);
    iEnv->ConstructSystem();
     // Check if mut is not squeezed
    root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root after compacting", root != 0);
    ChromoNode rnode = root->Chromos().Root();
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
    Elem* root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    // Compact chromo
    root->CompactChromo();
    // Save compacted chromo and recreate the model
    iEnv->Root()->Chromos().Save("ut_compact_cont_res.xml_");
    delete iEnv;

    iEnv = new Env("ut_compact_cont_res.xml_", "ut_compact_cont_res.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to re-create compacted system", iEnv != 0);
    iEnv->ConstructSystem();
     // Check if mut is not squeezed
    root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root after compacting", root != 0);
    MElem* e2 = root->GetNode("./E2");
    MElem* p1 = root->GetNode("./E2/P1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get P1", p1 != NULL);
    string cp1;
    p1->GetCont(cp1);
    CPPUNIT_ASSERT_MESSAGE("Incorrect content of P1", cp1 == "P1_new_content");
    ChromoNode rnode = e2->Chromos().Root();
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

// Getting parents modifs
void Ut_mut::test_GetParentModifs()
{
    printf("\n === Test of getting parent modifs\n");
    iEnv = new Env("ut_parmod1.xml", "ut_parmod1.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ConstructSystem();
    Elem* root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    // Make the transformation
    MElem* vb1 = root->GetNode("./VB/VB_1");
    TBool hasModifs = vb1->HasParentModifs();
    vb1->CopyModifsFromParent();
    // Save transformed chromo and recreate the model
    iEnv->Root()->Chromos().Save("ut_parmod1_res.xml_");
    delete iEnv;

    iEnv = new Env("ut_parmod1_res.xml_", "ut_parmod1_res.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to re-create system after gettin parent modifs", iEnv != 0);
    iEnv->ConstructSystem();
     // Check if edge is set correctly
    root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root after transforming", root != 0);
    MElem* va1b1 = root->GetNode("./VB/VB_1/VA_1_B1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get ./VB/VB_1/VA_1_B1", va1b1 != 0);

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
    Elem* root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    // Make illegal modif and enable transformation
    MElem* va = root->GetNode("./VA");
    ChromoNode madd = va->Mutation().Root().AddChild(ENt_Node);
    madd.SetAttr(ENa_Id, "VA_1_1");
    madd.SetAttr(ENa_Parent, "Vert");
    root->Mutate();
    // Save transformed chromo and recreate the model
    iEnv->Root()->Chromos().Save("ut_transfmdf1_res.xml_");
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



