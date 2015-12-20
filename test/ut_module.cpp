#include <env.h>
#include <stdlib.h>
#include <elem.h>
#include <mvert.h>
#include <mdata.h>
#include <mmod.h>

#include <cppunit/extensions/HelperMacros.h>

/*
 * This test is for checking the modularity support
 */


class Ut_mod : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(Ut_mod);
//    CPPUNIT_TEST(test_ImpsMgr1);
    CPPUNIT_TEST(test_ImpsMgr2);
    CPPUNIT_TEST(test_ImpsMgrOpt);
//    CPPUNIT_TEST(test_ModInt1);
    CPPUNIT_TEST_SUITE_END();
public:
    virtual void setUp();
    virtual void tearDown();
private:
    void test_ImpsMgr1();
    void test_ImpsMgr2();
    void test_ImpsMgrOpt();
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


// Enironments imports manager
void Ut_mod::test_ImpsMgr1()
{
    printf("\n === Test of imports manager APIs\n");

    iEnv = new Env("ut_mod_impmgr_1.xml", "ut_mod_impmgr_1.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ImpsMgr()->AddImportsPaths("modules");
    // Enabling mutation repositioning in order to resolve unsafety
    iEnv->ChMgr()->SetEnableReposMuts(ETrue);
    iEnv->ConstructSystem();
    Elem* root = iEnv->Root();
    // Check creation first
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != NULL);
    vector<string> modnames;
    iEnv->ImpsMgr()->GetModulesNames(modnames);
    // Verify the module found in modules
    TBool found = EFalse;
    for (vector<string>::const_iterator it = modnames.begin(); it < modnames.end() && !found; it++) {
	found = (*it == "Mod_root");
    }
    CPPUNIT_ASSERT_MESSAGE("Module ut_mod_int_1m isn't found", found);
    delete iEnv;
}

// Enironments imports manager: importing modules node
void Ut_mod::test_ImpsMgr2()
{
    printf("\n === Test of imports manager: importing modules node\n");

    iEnv = new Env("ut_mod_impmgr_2.xml", "ut_mod_impmgr_2.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ImpsMgr()->AddImportsPaths("modules");
    iEnv->ImpsMgr()->AddImportsPaths("modules2");
    // Enabling mutation repositioning in order to resolve unsafety
    iEnv->ChMgr()->SetEnableReposMuts(ETrue);
    iEnv->ConstructSystem();
    Elem* root = iEnv->Root();
    // Check creation first
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != NULL);
    // Verifying importing Mod2_comp_2
    MElem* mod2_c2 = root->GetNode("/test/Modules/Mod2_root_3/Mod2_comp_2");
    CPPUNIT_ASSERT_MESSAGE("Mod2_comp_2 not imported", mod2_c2 != NULL);
    // Verifying importing Mod_comp_2
    MElem* mod_c2 = root->GetNode("/test/Modules/Mod_root_3/Mod_comp_2");
    CPPUNIT_ASSERT_MESSAGE("Mod_comp_2 not imported", mod_c2 != NULL);
    // Verifying creation from imported node
    MElem* v4 = root->GetNode("./v4");
    CPPUNIT_ASSERT_MESSAGE("v4 is not created", v4 != NULL);
    MElem* v5 = root->GetNode("./v5");
    CPPUNIT_ASSERT_MESSAGE("v5 is not created", v5 != NULL);
    // Verify if node is imported with path containing nodes dest, ref ds_mod_prb_uri_chromo
    MElem* mod3_c4_c1_c1 = root->GetNode("./Modules/Mod_root_3/Mod3_c4/Mod3_comp_int_c1/Mod3_c4_c1/Mod3_c4_c1_c1");
    CPPUNIT_ASSERT_MESSAGE("mod3_c4_c1_c1 is not created", mod3_c4_c1_c1 != NULL);
    MElem* v6 = root->GetNode("./v6");
    CPPUNIT_ASSERT_MESSAGE("v6 is not created", v6 != NULL);
    delete iEnv;
}

#if 0
// Test of importing internal node from module
void Ut_mod::test_ModInt1()
{
    printf("\n === Test of importing internal node from module \n");

    iEnv = new Env("ut_mod_int_1.xml", "ut_mod_int_1.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    // Enabling mutation repositioning in order to resolve unsafety
    iEnv->ChMgr()->SetEnableReposMuts(ETrue);
    iEnv->ConstructSystem();
    Elem* root = iEnv->Root();
    // Check creation first
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != NULL);
    // Verifying importing Mod2_comp_2
    Elem* mod2_c2 = root->GetNode(":Elem:AImports/Mod2_root/Mod2_comp_2");
    CPPUNIT_ASSERT_MESSAGE("Mod2_comp_2 not imported", mod2_c2 != NULL);
    // Verifying importing Mod_comp_2
    Elem* mod_c2 = root->GetNode(":Elem:AImports/Mod_root/Mod_comp_2");
    CPPUNIT_ASSERT_MESSAGE("Mod_comp_2 not imported", mod_c2 != NULL);
    // Verifying creation from imported node
    Elem* v4 = root->GetNode("./v4");
    CPPUNIT_ASSERT_MESSAGE("v4 is not created", v4 != NULL);
    
    /*
    Elem* mode = root->GetNode("./Modules/ut_mod_int_1m");
    CPPUNIT_ASSERT_MESSAGE("Module isn't created", mode != NULL);
    MMod* moda = mode->GetObj(moda);
    GUri impuri("./Mod_root/Mod_comp_1/Mod_comp_1_1");
    moda->Import(impuri);
    CPPUNIT_ASSERT_MESSAGE("Module agent isn't created", moda != NULL);
    Elem* impe = mode->GetNode("./Mod_root/Mod_comp_1/Mod_comp_1_1");
    CPPUNIT_ASSERT_MESSAGE("Import is failed", impe != NULL);
    // Try to import node to from module
    // Try to add node with parent from module
    ChromoNode mut = root->Mutation().Root().AddChild(ENt_Node);
    mut.SetAttr(ENa_Parent, "/test/Modules/ut_mod_int_1m/Mod_root/Mod_comp_1");
    mut.SetAttr(ENa_Id, "v3");
    root->Mutate();
    Elem* mcomp = root->GetNode("/test/Modules/ut_mod_int_1m/Mod_root/Mod_comp_1");
    CPPUNIT_ASSERT_MESSAGE("Modules comp not imported", mcomp != NULL);
    */

    // Save upated chromo
    iEnv->Root()->Chromos().Save("ut_mod_int_1_res.xml_");
    delete iEnv;
}
#endif

// Importing internal node from module
// Using dedicated agent for imports, not active now
void Ut_mod::test_ModInt1()
{
    printf("\n === Test of importing internal node from module \n");

    iEnv = new Env("ut_mod_int_1.xml", "ut_mod_int_1.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ImpsMgr()->AddImportsPaths("modules");
    iEnv->ImpsMgr()->AddImportsPaths("modules2");
    // Enabling mutation repositioning in order to resolve unsafety
    iEnv->ChMgr()->SetEnableReposMuts(ETrue);
    iEnv->ConstructSystem();
    Elem* root = iEnv->Root();
    // Check creation first
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != NULL);
    // Verifying importing Mod2_comp_2
    MElem* mod2_c2 = root->GetNode(":Elem:AImports/Mod2_root/Mod2_comp_2");
    CPPUNIT_ASSERT_MESSAGE("Mod2_comp_2 not imported", mod2_c2 != NULL);
    // Verifying importing Mod_comp_2
    MElem* mod_c2 = root->GetNode(":Elem:AImports/Mod_root/Mod_comp_2");
    CPPUNIT_ASSERT_MESSAGE("Mod_comp_2 not imported", mod_c2 != NULL);
    // Verifying creation from imported node
    MElem* v4 = root->GetNode("./v4");
    CPPUNIT_ASSERT_MESSAGE("v4 is not created", v4 != NULL);
    
    // Save upated chromo
    iEnv->Root()->Chromos().Save("ut_mod_int_1_res.xml_");
    delete iEnv;
}

// Enironments imports manager + chromo soft optimization
// Verifying optimization when there are de-attached chromos
// ref. ds_mut_sqeezing_so_prnc_att
void Ut_mod::test_ImpsMgrOpt()
{
    printf("\n === Test of imports manager: importing + chromo opt\n");

    iEnv = new Env("ut_mod_impmgr_opt.xml", "ut_mod_impmgr_opt.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ImpsMgr()->AddImportsPaths("modules");
    iEnv->ImpsMgr()->AddImportsPaths("modules2");
    // Enabling mutation repositioning in order to resolve unsafety
    iEnv->ChMgr()->SetEnableReposMuts(ETrue);
    iEnv->ConstructSystem();
    Elem* root = iEnv->Root();
    // Check creation first
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != NULL);
    // Verifying importing Mod5_comp_2
    MElem* mod2_c2 = root->GetNode("/test/Modules/Mod5_root/Mod5_comp_2");
    CPPUNIT_ASSERT_MESSAGE("Mod5_comp_2 not imported", mod2_c2 != NULL);
    // Verifying importing Mod_comp_2
    MElem* mod_c2 = root->GetNode("/test/Modules/Mod4_root/Mod_comp_2");
    CPPUNIT_ASSERT_MESSAGE("Mod_comp_2 not imported", mod_c2 != NULL);
    // Verifying creation from imported node
    MElem* v4 = root->GetNode("./v4");
    CPPUNIT_ASSERT_MESSAGE("v4 is not created", v4 != NULL);
    // Verifying v4/Mod5_comp_2_1/Mod5_comp_2_1_1 is resolved (no mutliple choice)
    MElem* v4c = v4->GetNode("./Mod5_comp_2_1/Mod5_comp_2_1_1");
    CPPUNIT_ASSERT_MESSAGE("v4/Mod5_comp_2_1/Mod5_comp_2_1_1 is not acessible", v4c != NULL);
    delete iEnv;
}

