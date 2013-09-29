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
    CPPUNIT_TEST_SUITE_END();
public:
    virtual void setUp();
    virtual void tearDown();
private:
    void test_Add();
    void test_MutSyst();
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
    Elem* e2 = root->GetNode("Elem:elem1/Elem:elem2");
    CPPUNIT_ASSERT_MESSAGE("Fail to get e2", e2 != 0);
    Elem* e4 = root->GetNode("elem1:elem3/elem2:elem4");
    CPPUNIT_ASSERT_MESSAGE("Fail to get e4", e4 != 0);
    // Mutation of type "Adding node to current node"
    ChromoNode madd = root->Mutation().Root().AddChild(ENt_Node);
    madd.SetAttr(ENa_Id, "new_elem1");
    madd.SetAttr(ENa_Parent, "elem5");
    root->Mutate();
    // Check the element added
    Elem* eadded = root->GetNode("elem5:new_elem1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get elem added", eadded != 0);
    // Mutation of type "Delete node"
    ChromoNode mdel = root->Mutation().Root().AddChild(ENt_Rm);
    mdel.SetAttr(ENa_MutNode, "new_elem1");
    root->Mutate();
    Elem* erem = root->GetNode("elem5:new_elem1");
    CPPUNIT_ASSERT_MESSAGE("Fail to remove elem", erem == 0);
    // Check if we can add the node with the same name again
    ChromoNode madd1 = root->Mutation().Root().AddChild(ENt_Node);
    madd1.SetAttr(ENa_Id, "new_elem1");
    madd1.SetAttr(ENa_Parent, "elem5");
    root->Mutate();
    // Check the element added
    Elem* eadded1 = root->GetNode("elem5:new_elem1");
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
    Elem* cp1 = root->GetNode("ConnPoint:cp1");
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
    Elem* erem = root->GetNode("Edge:edge1");
    CPPUNIT_ASSERT_MESSAGE("Fail to remove edge", erem == 0);
    // Adding the edge again
    ChromoNode madd = root->Mutation().Root().AddChild(ENt_Node);
    madd.SetAttr(ENa_Id, "edge1");
    madd.SetAttr(ENa_Parent, "Edge");
    ChromoNode cnt1 = madd.AddChild(ENt_Cont);
    cnt1.SetAttr(ENa_MutNode, "Prop:P1");
    cnt1.SetAttr(ENa_MutVal, "Syst:Syst1/ConnPoint:cp");
    ChromoNode cnt2 = madd.AddChild(ENt_Cont);
    cnt2.SetAttr(ENa_MutNode, "Prop:P2");
    cnt2.SetAttr(ENa_MutVal, "ConnPoint:cp2");
    root->Mutate();
    // Check the element added
    Elem* eadded = root->GetNode("edge1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get edge added after deleting", eadded != 0);
    Elem* cp_1 = root->GetNode("Syst:Syst1/ConnPoint:cp");
    CPPUNIT_ASSERT_MESSAGE("Fail to get Syst:Syst1/ConnPoint:cp", cp_1 != 0);
    MVert* mcp_1 = cp_1->GetObj(mcp_1);
    CPPUNIT_ASSERT_MESSAGE("Fail to get Syst:Syst1/ConnPoint:cp MVert", mcp_1 != 0);
    MVert* pair_1 = *(mcp_1->Pairs().begin());
    CPPUNIT_ASSERT_MESSAGE("Fail to get pair of Syst:Syst1/ConnPoint:cp", pair_1 != 0);
    const string pname_1 = pair_1->EBase()->Name();
    CPPUNIT_ASSERT_MESSAGE("Wrong name of pair of Syst:Syst1/ConnPoint:cp", pname_1 == "cp2");
 
    delete iEnv;
}
