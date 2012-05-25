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
    CPPUNIT_TEST_SUITE_END();
public:
    virtual void setUp();
    virtual void tearDown();
private:
    void test_Add();
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
    printf("\n === Test of mutation of type adding\n");

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
    // Do mutation
    ChromoNode madd = root->Mutation().Root().AddChild(ENt_Node);
    madd.SetAttr(ENa_Id, "new_elem1");
    madd.SetAttr(ENa_Parent, "elem5");
    root->Mutate();
    // Check the element added
    Elem* eadded = root->GetNode("elem5:new_elem1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get elem added", eadded != 0);
    // Delete element
    ChromoNode mdel = root->Mutation().Root().AddChild(ENt_Rm);
    mdel.SetAttr(ENa_MutNode, "new_elem1");
    root->Mutate();
    Elem* erem = root->GetNode("elem5:new_elem1");
    CPPUNIT_ASSERT_MESSAGE("Fail to remove elem", erem == 0);
 
    delete iEnv;
}

