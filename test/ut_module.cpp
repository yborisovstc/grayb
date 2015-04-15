#include <env.h>
#include <stdlib.h>
#include <elem.h>
#include <mvert.h>
#include <mdata.h>

#include <cppunit/extensions/HelperMacros.h>

/*
 * This test is for checking the modularity support
 */


class Ut_mod : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(Ut_mod);
    CPPUNIT_TEST(test_ModInt1);
    CPPUNIT_TEST_SUITE_END();
public:
    virtual void setUp();
    virtual void tearDown();
private:
    void test_ModInt1();
private:
    Env* iEnv;
};

CPPUNIT_TEST_SUITE_REGISTRATION( Ut_mod );

void Ut_mod::setUp()
{
}

void Ut_mod::tearDown()
{
    CPPUNIT_ASSERT_EQUAL_MESSAGE("tearDown", 0, 0);
}


// Suppotring invariance with respect to mutations position
void Ut_mod::test_ModInt1()
{
    printf("\n === Test of importing internal node from module \n");

    iEnv = new Env("Env", "ut_mod_int_1.xml", "ut_mod_int_1.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    // Enabling mutation repositioning in order to resolve unsafety
    iEnv->ChMgr()->SetEnableReposMuts(ETrue);
    iEnv->ConstructSystem();
    Elem* root = iEnv->Root();
    // Check creation first
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != NULL);
    // Try to add node with parent from module
    ChromoNode mut = root->Mutation().Root().AddChild(ENt_Node);
    mut.SetAttr(ENa_Parent, "/test/Modules/ut_mod_int_1m/Mod_comp_1");
    mut.SetAttr(ENa_Id, "v3");
    root->Mutate();
    Elem* mcomp = root->GetNode("/test/Modules/ut_mod_int_1m/Mod_comp_1");
    CPPUNIT_ASSERT_MESSAGE("Modules comp not imported", mcomp != NULL);

    // Save upated chromo
    iEnv->Root()->Chromos().Save("ut_mod_int_1_res.xml_");
    delete iEnv;
}


