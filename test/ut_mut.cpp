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
    CPPUNIT_TEST(test_MutDepsRm);
    CPPUNIT_TEST(test_MutDepsRm2);
    CPPUNIT_TEST(test_MutRmParent);
    CPPUNIT_TEST_SUITE_END();
public:
    virtual void setUp();
    virtual void tearDown();
private:
    void test_Add();
    void test_MutSyst();
    void test_Move();
    void test_MutDepsRm();
    void test_MutDepsRm2();
    void test_MutRmParent();
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
    Elem* e2 = root->GetNode("elem1/elem2");
    CPPUNIT_ASSERT_MESSAGE("Fail to get e2", e2 != 0);
    Elem* e4 = root->GetNode("elem3/elem4");
    CPPUNIT_ASSERT_MESSAGE("Fail to get e4", e4 != 0);
    // Mutation of type "Adding node to current node"
    ChromoNode madd = root->Mutation().Root().AddChild(ENt_Node);
    madd.SetAttr(ENa_Id, "new_elem1");
    madd.SetAttr(ENa_Parent, "elem5");
    root->Mutate();
    // Check the element added
    Elem* eadded = root->GetNode("(elem5:)new_elem1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get elem added", eadded != 0);
    // Mutation of type "Delete node"
    ChromoNode mdel = root->Mutation().Root().AddChild(ENt_Rm);
    mdel.SetAttr(ENa_MutNode, "new_elem1");
    root->Mutate();
    Elem* erem = root->GetNode("(elem5:)new_elem1");
    CPPUNIT_ASSERT_MESSAGE("Fail to remove elem", erem == 0);
    // Check if we can add the node with the same name again
    ChromoNode madd1 = root->Mutation().Root().AddChild(ENt_Node);
    madd1.SetAttr(ENa_Id, "new_elem1");
    madd1.SetAttr(ENa_Parent, "elem5");
    root->Mutate();
    // Check the element added
    Elem* eadded1 = root->GetNode("new_elem1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get elem added", eadded1 != 0);
     // Mutation of type "Rename node"
    ChromoNode mren = root->Mutation().Root().AddChild(ENt_Change);
    mren.SetAttr(ENa_MutNode, "elem5");
    mren.SetAttr(ENa_MutAttr, GUriBase::NodeAttrName(ENa_Id));
    mren.SetAttr(ENa_MutVal, "elem5_renamed");
    root->Mutate();
    Elem* eren = root->GetNode("elem5_renamed");
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
    Elem* cp1 = root->GetNode("cp1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get cp1", cp1 != 0);
    MVert* mcp1 = cp1->GetObj(mcp1);
    CPPUNIT_ASSERT_MESSAGE("Fail to get mcp1", mcp1 != 0);
    MVert* pair = *(mcp1->Pairs().begin());
    CPPUNIT_ASSERT_MESSAGE("Fail to get pair", pair != 0);
    const string pname = pair->EBase()->Name();
    CPPUNIT_ASSERT_MESSAGE("Wrong pair's name", pname == "cp2");
    // Delete edge
    ChromoNode mdel = root->Mutation().Root().AddChild(ENt_Rm);
    mdel.SetAttr(ENa_MutNode, "edge1");
    root->Mutate();
    Elem* erem = root->GetNode("edge1");
    CPPUNIT_ASSERT_MESSAGE("Fail to remove edge", erem == 0);
    // Adding the edge again
    ChromoNode madd = root->Mutation().Root().AddChild(ENt_Node);
    madd.SetAttr(ENa_Id, "edge1");
    madd.SetAttr(ENa_Parent, "Edge");
    ChromoNode cnt1 = madd.AddChild(ENt_Cont);
    cnt1.SetAttr(ENa_MutNode, "P1");
    cnt1.SetAttr(ENa_MutVal, "Syst1/cp");
    ChromoNode cnt2 = madd.AddChild(ENt_Cont);
    cnt2.SetAttr(ENa_MutNode, "P2");
    cnt2.SetAttr(ENa_MutVal, "cp2");
    root->Mutate();
    // Check the element added
    Elem* eadded = root->GetNode("edge1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get edge added after deleting", eadded != 0);
    Elem* cp_1 = root->GetNode("Syst1/cp");
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
    Elem* e2 = root->GetNode("elem1/elem2");
    CPPUNIT_ASSERT_MESSAGE("Fail to get e2", e2 != 0);
    Elem* e4 = root->GetNode("(elem1:)elem3/(elem2:)elem4");
    CPPUNIT_ASSERT_MESSAGE("Fail to get e4", e4 != 0);

    // Moving remote node
    ChromoNode mmovermt = root->Mutation().Root().AddChild(ENt_Move);
    mmovermt.SetAttr(ENa_Id, "file:../modules/syst.xml#");
    mmovermt.SetAttr(ENa_MutNode, "/test");
    root->Mutate();
    Elem* eext = root->GetNode("/test/SysComps/Extender");
    CPPUNIT_ASSERT_MESSAGE("Fail get extender from moved remote module", eext != NULL);

    // Mutation of type "Move node"
    ChromoNode mmove = root->Mutation().Root().AddChild(ENt_Move);
    mmove.SetAttr(ENa_Id, "elem5");
    mmove.SetAttr(ENa_MutNode, "elem3");
    root->Mutate();
    Elem* emoved = root->GetNode("elem5");
    Elem* ebase = root->GetNode("elem3");
    CPPUNIT_ASSERT_MESSAGE("Fail to rename node", emoved != NULL && ebase != NULL);
    Elem* enext = NULL;
    for (vector<Elem*>::iterator it = root->Comps().begin(); it != root->Comps().end(); it++) {
	if (*it == emoved) {
	    it++; enext = *it; break;
	}
    }
    CPPUNIT_ASSERT_MESSAGE("Fail to rename node", enext != NULL && enext == ebase);

 
    delete iEnv;
}

// Preventing of mutation braking model consistency
// Mutation - rm, dependency - child
void Ut_mut::test_MutDepsRm()
{
    printf("\n === Test of mutation consistency\n");

    iEnv = new Env("Env", "ut_mut_dep_1.xml", "ut_mut_dep_1.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ConstructSystem();
    Elem* root = iEnv->Root();
     // Check creation first
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    Elem* e2 = root->GetNode("elem1/elem2");
    CPPUNIT_ASSERT_MESSAGE("Fail to get elem2", e2 != 0);
    Elem* e3 = root->GetNode("(elem2:)elem3");
    CPPUNIT_ASSERT_MESSAGE("Fail to get elem3", e3 != 0);
    // Check major child
    Rank rmc;
    Elem* mc = e2->GetMajorChild(rmc);
    CPPUNIT_ASSERT_MESSAGE("Fail to get elem2 major child", mc == e3);
    // Get major dep
    Elem::TDep mdep = e2->GetMajorDep();
    CPPUNIT_ASSERT_MESSAGE("Fail to get elem2 major dep", mdep.first == e3 && mdep.second == -1);
    // Try to remove elem2 from elem1 - unsafe mutation
    Elem* e1 = root->GetNode("elem1");
    ChromoNode mut = e1->Mutation().Root().AddChild(ENt_Rm);
    mut.SetAttr(ENa_MutNode, "elem2");
    e1->Mutate();
    // Check that the mutation is refused
    e2 = root->GetNode("elem1/elem2");
    CPPUNIT_ASSERT_MESSAGE("Mutation -rm- of elem2 is not refused", e2 != NULL);
    // Try to remove elem2 from root - safe mutation
    mut = root->Mutation().Root().AddChild(ENt_Rm);
    mut.SetAttr(ENa_MutNode, "elem1/elem2");
    root->Mutate();
    // Check that the mutation is not refused
    e2 = root->GetNode("elem1/elem2");
    CPPUNIT_ASSERT_MESSAGE("Root mutation -rm- of elem2 is refused", e2 == NULL);
}

// Preventing of mutation braking model consistency
// Mutation - rm, dependency - object of change
void Ut_mut::test_MutDepsRm2()
{
    printf("\n === Test of mutation consistency, mut -rm-, dep - object \n");

    iEnv = new Env("Env", "ut_mut_dep_2.xml", "ut_mut_dep_2.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ConstructSystem();
    Elem* root = iEnv->Root();
     // Check creation first
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    Elem* e2 = root->GetNode("elem1/elem2");
    CPPUNIT_ASSERT_MESSAGE("Fail to get elem2", e2 != 0);
    // Get major dep
    Elem::TDep mdep = e2->GetMajorDep();
    CPPUNIT_ASSERT_MESSAGE("Fail to get elem2 major dep", mdep.first == root && mdep.second == 1);
    // Try to remove elem2 from elem1 - unsafe mutation
    Elem* e1 = root->GetNode("elem1");
    ChromoNode mut = e1->Mutation().Root().AddChild(ENt_Rm);
    mut.SetAttr(ENa_MutNode, "elem2");
    e1->Mutate();
    // Check that the mutation is refused
    e2 = root->GetNode("elem1/elem2");
    CPPUNIT_ASSERT_MESSAGE("Mutation -rm- of elem2 is not refused", e2 != NULL);
    // Try to remove elem2 from root - safe mutation
    mut = root->Mutation().Root().AddChild(ENt_Rm);
    mut.SetAttr(ENa_MutNode, "elem1/elem2");
    root->Mutate();
    // Check that the mutation is not refused
    e2 = root->GetNode("elem1/elem2");
    CPPUNIT_ASSERT_MESSAGE("Root mutation -rm- of elem2 is refused", e2 == NULL);
}

// Handling of parents removing, ref uc_029
void Ut_mut::test_MutRmParent()
{
    printf("\n === Test of mutation consistency\n");

    iEnv = new Env("Env", "ut_mutadd_1.xml", "ut_mut_rm_parent.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ConstructSystem();
    Elem* root = iEnv->Root();
     // Check creation first
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    Elem* e2 = root->GetNode("elem1/elem2");
    CPPUNIT_ASSERT_MESSAGE("Fail to get elem2", e2 != 0);
    // Remove elem2, which is parent of elem5 
    ChromoNode mut = root->Mutation().Root().AddChild(ENt_Rm);
    mut.SetAttr(ENa_MutNode, "elem1/elem2");
    root->Mutate();
    // Create child from elem5
    ChromoNode madd = root->Mutation().Root().AddChild(ENt_Node);
    madd.SetAttr(ENa_Id, "elem6");
    madd.SetAttr(ENa_Parent, "elem5");
    root->Mutate();
    // Check that the mutation is performed
    Elem* e6 = root->GetNode("elem6");
    CPPUNIT_ASSERT_MESSAGE("Creating elem6 as child of elem5 failed", e6 != NULL);
    // Check that the elem6 contains mutation from deleted parent elem2
    Elem* e2_1 = root->GetNode("elem6/elem2_1");
    CPPUNIT_ASSERT_MESSAGE("elem6 containing deleted parent comp elem2_1 failed", e2_1 != NULL);
}
