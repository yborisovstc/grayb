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
    CPPUNIT_TEST(test_MutDepsRm);
    CPPUNIT_TEST(test_MutDepsRm2);
    CPPUNIT_TEST(test_MutDepsChilds1);
    CPPUNIT_TEST(test_MutDepsRmRef);
    CPPUNIT_TEST(test_MutRmParent);
    CPPUNIT_TEST(test_MutRenameParent);
    CPPUNIT_TEST(test_Compact1);
    CPPUNIT_TEST(test_Compact2);
    CPPUNIT_TEST(test_Compact3);
    CPPUNIT_TEST(test_CompactRef1);
    CPPUNIT_TEST(test_CompactCont);
    CPPUNIT_TEST_SUITE_END();
public:
    virtual void setUp();
    virtual void tearDown();
private:
    void test_Add();
    void test_MutSyst();
    void test_Move();
    void test_MutRmRecr();
    void test_MutDepsRm();
    void test_MutDepsRm2();
    void test_MutDepsChilds1();
    void test_MutDepsRmRef();
    void test_MutRmParent();
    void test_MutRenameParent();
    void test_Compact1();
    void test_Compact2();
    void test_Compact3();
    void test_CompactRef1();
    void test_CompactCont();
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

    iEnv = new Env("Env", "ut_mutadd_1.xml", "ut_mutadd_1.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ConstructSystem();
    Elem* root = iEnv->Root();
    // Check creation first
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    Elem* e2 = root->GetNode("./elem1/elem2");
    CPPUNIT_ASSERT_MESSAGE("Fail to get e2", e2 != 0);
    Elem* e4 = root->GetNode("./elem3/elem4");
    CPPUNIT_ASSERT_MESSAGE("Fail to get e4", e4 != 0);
    // Mutation of type "Adding node to current node"
    ChromoNode madd = root->Mutation().Root().AddChild(ENt_Node);
    madd.SetAttr(ENa_Id, "new_elem1");
    madd.SetAttr(ENa_Parent, "./elem5");
    root->Mutate();
    // Check the element added
    Elem* eadded = root->GetNode("./(elem5:)new_elem1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get elem added", eadded != 0);
    // Mutation of type "Delete node"
    ChromoNode mdel = root->Mutation().Root().AddChild(ENt_Rm);
    mdel.SetAttr(ENa_MutNode, "./new_elem1");
    root->Mutate();
    Elem* erem = root->GetNode("./(elem5:)new_elem1");
    CPPUNIT_ASSERT_MESSAGE("Fail to remove elem", erem == 0);
    // Check if we can add the node with the same name again
    ChromoNode madd1 = root->Mutation().Root().AddChild(ENt_Node);
    madd1.SetAttr(ENa_Id, "new_elem1");
    madd1.SetAttr(ENa_Parent, "./elem5");
    root->Mutate();
    // Check the element added
    Elem* eadded1 = root->GetNode("./new_elem1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get elem added", eadded1 != 0);
     // Mutation of type "Rename node"
    ChromoNode mren = root->Mutation().Root().AddChild(ENt_Change);
    mren.SetAttr(ENa_MutNode, "./elem5");
    mren.SetAttr(ENa_MutAttr, GUriBase::NodeAttrName(ENa_Id));
    mren.SetAttr(ENa_MutVal, "elem5_renamed");
    root->Mutate();
    Elem* eren = root->GetNode("./elem5_renamed");
    CPPUNIT_ASSERT_MESSAGE("Fail to rename node", eren != 0);
 
    delete iEnv;
}

void Ut_mut::test_MutSyst()
{
    printf("\n === Test of mutation of system\n");

    iEnv = new Env("Env", "ut_mut_syst.xml", "ut_mut_syst.txt");
    // Check creation first
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ConstructSystem();
    Elem* root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    Elem* cp1 = root->GetNode("./cp1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get cp1", cp1 != 0);
    MVert* mcp1 = cp1->GetObj(mcp1);
    CPPUNIT_ASSERT_MESSAGE("Fail to get mcp1", mcp1 != 0);
    MVert* pair = *(mcp1->Pairs().begin());
    CPPUNIT_ASSERT_MESSAGE("Fail to get pair", pair != 0);
    const string pname = pair->EBase()->Name();
    CPPUNIT_ASSERT_MESSAGE("Wrong pair's name", pname == "cp2");
    // Delete edge
    ChromoNode mdel = root->Mutation().Root().AddChild(ENt_Rm);
    mdel.SetAttr(ENa_MutNode, "./edge1");
    root->Mutate();
    Elem* erem = root->GetNode("./edge1");
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
    Elem* eadded = root->GetNode("./edge1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get edge added after deleting", eadded != 0);
    Elem* cp_1 = root->GetNode("./Syst1/cp");
    CPPUNIT_ASSERT_MESSAGE("Fail to get Syst1/cp", cp_1 != 0);
    MVert* mcp_1 = cp_1->GetObj(mcp_1);
    CPPUNIT_ASSERT_MESSAGE("Fail to get Syst1/cp MVert", mcp_1 != 0);
    MVert* pair_1 = *(mcp_1->Pairs().begin());
    CPPUNIT_ASSERT_MESSAGE("Fail to get pair of Syst1/cp", pair_1 != 0);
    const string pname_1 = pair_1->EBase()->Name();
    CPPUNIT_ASSERT_MESSAGE("Wrong name of pair of Syst1/cp", pname_1 == "cp2");
 
    delete iEnv;
}

void Ut_mut::test_Move()
{
    printf("\n === Test of mutation: moving node\n");

    iEnv = new Env("Env", "ut_mutmove_1.xml", "ut_mutmove_1.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ConstructSystem();
    Elem* root = iEnv->Root();
    // Check creation first
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    Elem* e2 = root->GetNode("./elem1/elem2");
    CPPUNIT_ASSERT_MESSAGE("Fail to get e2", e2 != 0);
    Elem* e4 = root->GetNode("./(elem1:)elem3/(elem2:)elem4");
    CPPUNIT_ASSERT_MESSAGE("Fail to get e4", e4 != 0);

    // Moving remote node
    ChromoNode mmovermt = root->Mutation().Root().AddChild(ENt_Move);
    mmovermt.SetAttr(ENa_Id, "file:../modules/syst.xml#");
    mmovermt.SetAttr(ENa_MutNode, "/test");
    root->Mutate();
    Elem* eext = root->GetNode("/test/SysComps/Extender");
    CPPUNIT_ASSERT_MESSAGE("Fail get extender from moved remote module", eext != NULL);

    // Moving local node
    ChromoNode mmove = root->Mutation().Root().AddChild(ENt_Move);
    mmove.SetAttr(ENa_Id, "./elem5");
    mmove.SetAttr(ENa_MutNode, "./elem3/elem4");
    root->Mutate();
    Elem* emoved = root->GetNode("./elem3/elem4/elem5");
    CPPUNIT_ASSERT_MESSAGE("Fail to move local node elem5", emoved != NULL);
 
    delete iEnv;
}

// Test of removing node and recreation node with the same name
void Ut_mut::test_MutRmRecr()
{
    printf("\n === Test of creating node with same name as removed one\n");

    iEnv = new Env("Env", "ut_mut_rm_recr.xml", "ut_mut_rm_recr.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ConstructSystem();
    Elem* root = iEnv->Root();
     // Check creation first
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    Elem* v1 = root->GetNode("./v1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get v1", v1 != 0);
    delete iEnv;
}

// Preventing of mutation braking model consistency
// Mutation - rm, dependency - child
void Ut_mut::test_MutDepsRm()
{
    printf("\n === Test of mutation consistency\n");

    iEnv = new Env("Env", "ut_mut_dep_1.xml", "ut_mut_dep_1.txt");
    // Enabling pheno, because of model is basing on pheno
    iEnv->ChMgr()->SetEnablePhenoModif(ETrue);
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ConstructSystem();
    Elem* root = iEnv->Root();
     // Check creation first
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    Elem* e2 = root->GetNode("./elem1/elem2");
    CPPUNIT_ASSERT_MESSAGE("Fail to get elem2", e2 != 0);
    Elem* e3 = root->GetNode("./(elem2:)elem3");
    CPPUNIT_ASSERT_MESSAGE("Fail to get elem3", e3 != 0);
    // Check major child
    Rank rmc;
    Elem* mc = e2->GetMajorChild(rmc);
    CPPUNIT_ASSERT_MESSAGE("Fail to get elem2 major child", mc == e3);
    // Get major dep
    //Elem::TDep mdep = e2->GetMajorDep();
    Elem::TMDep mdep = e2->GetMajorDep();
    //CPPUNIT_ASSERT_MESSAGE("Fail to get elem2 major dep", mdep.first == e3 && mdep.second == -1);
    TBool e2dep_ok = mdep.first.first == root && mdep.first.second == e3->Chromos().Root().Handle() && mdep.second == ENa_Parent;
    CPPUNIT_ASSERT_MESSAGE("Fail to get elem2 major dep", e2dep_ok);
    // Try to remove elem2 from elem1 - unsafe mutation
    Elem* e1 = root->GetNode("./elem1");
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

// Preventing of mutation braking model consistency
// Mutation - rm, dependency - object of change
void Ut_mut::test_MutDepsRm2()
{
    printf("\n === Test of mutation consistency, mut -rm-, dep - object \n");

    iEnv = new Env("Env", "ut_mut_dep_2.xml", "ut_mut_dep_2.txt");
    // Enabling pheno, because of model is basing on pheno
    iEnv->ChMgr()->SetEnablePhenoModif(ETrue);
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ConstructSystem();
    Elem* root = iEnv->Root();
     // Check creation first
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    Elem* e2 = root->GetNode("./elem1/elem2");
    CPPUNIT_ASSERT_MESSAGE("Fail to get elem2", e2 != 0);
    // Checking rank calculation
    Rank re;
    e2->GetRank(re);
    ChromoNode e2r = e2->Chromos().Root();
    Rank rec;
    e2r.GetRank(rec);
    CPPUNIT_ASSERT_MESSAGE("Fail to calculate elem2 rank", re == rec);
    // Get major dep
    Elem::TMDep mdep = e2->GetMajorDep();
    CPPUNIT_ASSERT_MESSAGE("Fail to get elem2 major dep", mdep.first.first == root);
    // Try to remove elem2 from elem1 - unsafe mutation
    Elem* e1 = root->GetNode("./elem1");
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

    iEnv = new Env("Env", "ut_mut_dep_3.xml", "ut_mut_dep_3.txt");
    // Enabling pheno, because of model is basing on pheno
    iEnv->ChMgr()->SetEnablePhenoModif(ETrue);
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ConstructSystem();
    Elem* root = iEnv->Root();
     // Check creation first
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    Elem* e2 = root->GetNode("./elem1/elem2");
    CPPUNIT_ASSERT_MESSAGE("Fail to get elem2", e2 != 0);
    // Applying harmful mutation
    Elem* e1 = root->GetNode("./elem1");
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

    iEnv = new Env("Env", "ut_mut_dep_refs.xml", "ut_mut_dep_refs.txt");
    // Enabling pheno, because of model is basing on pheno
    iEnv->ChMgr()->SetEnablePhenoModif(ETrue);
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ConstructSystem();
    Elem* root = iEnv->Root();
     // Check creation first
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    Elem* v1_0 = root->GetNode("./v1/v1_0");
    CPPUNIT_ASSERT_MESSAGE("Fail to get v1/v1_0", v1_0 != 0);
    // Try to remove v1_0 from v1 - unsafe mutation
    Elem* v1 = root->GetNode("./v1");
    ChromoNode mut = v1->Mutation().Root().AddChild(ENt_Rm);
    mut.SetAttr(ENa_MutNode, "./v1_0");
    v1->Mutate();
    // Check that the mutation is refused
    v1_0 = root->GetNode("./v1/v1_0");
    CPPUNIT_ASSERT_MESSAGE("Mutation -rm- of v1_0 is not refused", v1_0 != NULL);
    // Try to remove elem2 from root - safe mutation
    mut = root->Mutation().Root().AddChild(ENt_Rm);
    mut.SetAttr(ENa_MutNode, "./v1/v1_0");
    root->Mutate();
    // Check that the mutation is not refused
    v1_0 = root->GetNode("./v1/v1_0");
    CPPUNIT_ASSERT_MESSAGE("Mutation -rm- of v1_0 from root is refused", v1_0 == NULL);
}


// Handling of parents removing, ref uc_029
void Ut_mut::test_MutRmParent()
{
    printf("\n === Test of system consistency after parent removed\n");

    iEnv = new Env("Env", "ut_mutadd_1.xml", "ut_mut_rm_parent.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ConstructSystem();
    Elem* root = iEnv->Root();
     // Check creation first
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    Elem* e2 = root->GetNode("./elem1/elem2");
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
    Elem* e6 = root->GetNode("./elem6");
    CPPUNIT_ASSERT_MESSAGE("Creating elem6 as child of elem5 failed", e6 != NULL);
    // Check that the elem6 contains mutation from deleted parent elem2
    Elem* e2_1 = root->GetNode("./elem6/elem2_1");
    CPPUNIT_ASSERT_MESSAGE("elem6 containing deleted parent comp elem2_1 failed", e2_1 != NULL);

    // Trying to remove elem4 from elem3. elem4 doens't have direct deps but via comp elem4_1
    // System must deny removing elem4 from elem3
    Elem* e3 = root->GetNode("./elem3");
    mut = e3->Mutation().Root().AddChild(ENt_Rm);
    mut.SetAttr(ENa_MutNode, "./elem4");
    e3->Mutate();
    // Check that the mutation is dinied
    Elem* e4 = root->GetNode("./elem3/elem4");
    CPPUNIT_ASSERT_MESSAGE("Removing elem4 from elem3 isn't denied", e4 != NULL && !e4->IsRemoved());

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
    Elem* e7 = root->GetNode("./elem7");
    CPPUNIT_ASSERT_MESSAGE("Creating elem7 as child of elem4_1i (comp of removed elem4) failed", e7 != NULL);
    root->Mutate();
}

// Handling of parents renaming, ref uc_031
void Ut_mut::test_MutRenameParent()
{
    printf("\n === Test of system consistency after parent renamed\n");

    iEnv = new Env("Env", "ut_mutadd_1.xml", "ut_mut_rn_parent.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ConstructSystem();
    Elem* root = iEnv->Root();
     // Check creation first
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    Elem* e2 = root->GetNode("./elem1/elem2");
    CPPUNIT_ASSERT_MESSAGE("Fail to get elem2", e2 != 0);
    // Rename elem2, which is parent of elem5 
    ChromoNode mut = root->Mutation().Root().AddChild(ENt_Change);
    mut.SetAttr(ENa_MutNode, "./elem1/elem2");
    mut.SetAttr(ENa_MutAttr, GUriBase::NodeAttrName(ENa_Id));
    mut.SetAttr(ENa_MutVal, "elem2_renamed");
    root->Mutate();
    // Create child from elem5
    ChromoNode madd = root->Mutation().Root().AddChild(ENt_Node);
    madd.SetAttr(ENa_Id, "elem6");
    madd.SetAttr(ENa_Parent, "./elem5");
    root->Mutate();
    // Check that the mutation is performed
    Elem* e6 = root->GetNode("./elem6");
    CPPUNIT_ASSERT_MESSAGE("Creating elem6 as child of elem5 failed", e6 != NULL);
    // Check that the elem6 contains mutation from deleted parent elem2
    Elem* e2_1 = root->GetNode("./elem6/elem2_1");
    CPPUNIT_ASSERT_MESSAGE("elem6 containing deleted parent comp elem2_1 failed", e2_1 != NULL);
}

// Compactization of chromo
void Ut_mut::test_Compact1()
{
    printf("\n === Test of compacting of chromo: renaming\n");

    iEnv = new Env("Env", "ut_compact1.xml", "ut_compact1.txt");
    // Enabling pheno, because of model is basing on pheno
    iEnv->ChMgr()->SetEnablePhenoModif(ETrue);
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ConstructSystem();
    Elem* root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
     // Check creation first
    Elem* e3 = root->GetNode("./elem3");
    CPPUNIT_ASSERT_MESSAGE("Fail to get elem3", e3 != 0);
    // Compact chromo
    root->CompactChromo();
    // Save compacted chromo and recreate the model
    iEnv->Root()->Chromos().Save("ut_compact1_res.xml_");
    delete iEnv;

    iEnv = new Env("Env", "ut_compact1_res.xml_", "ut_compact1_res.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env for compacted system", iEnv != 0);
    iEnv->ConstructSystem();
     // Check if elem3 and elem4 are created ok with compacted chromo
    root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root after compacting", root != 0);
    e3 = root->GetNode("./elem3");
    CPPUNIT_ASSERT_MESSAGE("Fail to get elem3", e3 != 0);
    Elem* e4 = root->GetNode("./elem4");
    CPPUNIT_ASSERT_MESSAGE("Fail to get elem4", e4 != 0);

    delete iEnv;
}

// Compactization of chromo
void Ut_mut::test_Compact2()
{
    printf("\n === Test of compacting of chromo: renaming\n");
    iEnv = new Env("Env", "ut_compact2.xml", "ut_compact2.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ConstructSystem();
    Elem* root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    // Compact chromo
    root->CompactChromo();
    // Save compacted chromo and recreate the model
    iEnv->Root()->Chromos().Save("ut_compact2_res.xml_");
    delete iEnv;

    iEnv = new Env("Env", "ut_compact2_res.xml_", "ut_compact2_res.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to re-create compacted system", iEnv != 0);
    iEnv->ConstructSystem();
     // Check if edge is set correctly
    root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root after compacting", root != 0);
    Elem* v1 = root->GetNode("./(Vert:)v1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get v1", v1 != 0);
    MVert* mv1 = v1->GetObj(mv1);
    CPPUNIT_ASSERT_MESSAGE("Fail to get mv1", mv1 != 0);
    MVert* pair = *(mv1->Pairs().begin());
    CPPUNIT_ASSERT_MESSAGE("Fail to get pair", pair != 0);
    Elem* v3 = root->GetNode("./(Vert:)v3");
    CPPUNIT_ASSERT_MESSAGE("Wrong pair's name", pair->EBase() == v3);

    delete iEnv;
}

// Compactization of chromo: mut removing node
void Ut_mut::test_Compact3()
{
    printf("\n === Test of compacting of chromo: removing\n");
    iEnv = new Env("Env", "ut_compact3.xml", "ut_compact3.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ConstructSystem();
    Elem* root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    // Compact chromo
    //root->CompactChromo();
    // Save compacted chromo and recreate the model
    iEnv->Root()->Chromos().Save("ut_compact3_res.xml_");
    delete iEnv;

    iEnv = new Env("Env", "ut_compact3_res.xml_", "ut_compact3_res.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to re-create compacted system", iEnv != 0);
    iEnv->ConstructSystem();
     // Check if edge is set correctly
    root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root after compacting", root != 0);
    Elem* v3 = root->GetNode("(Vert:)v3");
    CPPUNIT_ASSERT_MESSAGE("V3 is not removed", v3 == NULL);

    delete iEnv;
}

// Compactization of chromo. Cont ref if ref rank is greater than that of init mut.
void Ut_mut::test_CompactRef1()
{
    printf("\n === Test of compacting of chromo: ref if ref rank is greater than that of init mut\n");
    iEnv = new Env("Env", "ut_compact_ref1.xml", "ut_compact_ref1.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ConstructSystem();
    Elem* root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    // Compact chromo
    root->CompactChromo();
    // Save compacted chromo and recreate the model
    iEnv->Root()->Chromos().Save("ut_compact_ref1_res.xml_");
    delete iEnv;

    iEnv = new Env("Env", "ut_compact_ref1_res.xml_", "ut_compact_ref1_res.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to re-create compacted system", iEnv != 0);
    iEnv->ConstructSystem();
     // Check if mut is not squeezed
    root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root after compacting", root != 0);
    ChromoNode rnode = root->Chromos().Root();
    ChromoNode::Reverse_Iterator it = rnode.Rbegin(); 
    ChromoNode mut = *it;
    TBool is_cont_ref = mut.Type() == ENt_Cont && mut.Attr(ENa_MutNode) == "Edge1/P2";
    CPPUNIT_ASSERT_MESSAGE("Fail to keep cont ref mut unsqueezed", is_cont_ref);


    delete iEnv;
}

// Compactization of chromo. Content change
void Ut_mut::test_CompactCont()
{
    printf("\n === Test of compacting of chromo: Content change\n");
    iEnv = new Env("Env", "ut_compact_cont.xml", "ut_compact_cont.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ConstructSystem();
    Elem* root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    // Compact chromo
    root->CompactChromo();
    // Save compacted chromo and recreate the model
    iEnv->Root()->Chromos().Save("ut_compact_cont_res.xml_");
    delete iEnv;

    iEnv = new Env("Env", "ut_compact_cont_res.xml_", "ut_compact_cont_res.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to re-create compacted system", iEnv != 0);
    iEnv->ConstructSystem();
     // Check if mut is not squeezed
    root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root after compacting", root != 0);
    Elem* e2 = root->GetNode("./E2");
    Elem* p1 = root->GetNode("./E2/P1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get P1", p1 != NULL);
    string cp1;
    p1->GetCont(cp1);
    CPPUNIT_ASSERT_MESSAGE("Incorrect content of P1", cp1 == "P1_new_content");
    ChromoNode rnode = e2->Chromos().Root();
    ChromoNode::Reverse_Iterator it = rnode.Rbegin(); 
    ChromoNode mut = *it;
    TBool is_cont_ok = mut.Type() == ENt_Cont && mut.Attr(ENa_MutVal) == "P1_new_content";
    CPPUNIT_ASSERT_MESSAGE("Incorrect cont mutation", is_cont_ok);
    it++;
    CPPUNIT_ASSERT_MESSAGE("Init cont mutation is not squeezed", it == rnode.Rend());


    delete iEnv;
}
