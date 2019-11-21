#include <env.h>
#include <stdlib.h>

#include "chromo2.h"

#include <cppunit/extensions/HelperMacros.h>

/*
 * This test is for chromo2
 */


class Ut_chromo2 : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(Ut_chromo2);
//    CPPUNIT_TEST(test_Chr1);
    CPPUNIT_TEST(test_Cre1);
   // CPPUNIT_TEST(test_Seg);
   // CPPUNIT_TEST(test_Ns);
    CPPUNIT_TEST_SUITE_END();
public:
    virtual void setUp();
    virtual void tearDown();
private:
    void test_Chr1();
    void test_Cre1();
    void test_Seg();
    void test_Ns();
private:
    Env* iEnv;
};

CPPUNIT_TEST_SUITE_REGISTRATION( Ut_chromo2 );
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(Ut_chromo2, "Ut_chromo2");


void Ut_chromo2::setUp()
{
}

void Ut_chromo2::tearDown()
{
    CPPUNIT_ASSERT_EQUAL_MESSAGE("tearDown", 0, 0);
}

void Ut_chromo2::test_Chr1()
{
    printf("\n === Test of Chromo2 creation\n");
    Chromo2 chr;
    chr.SetFromFile("ut_chr2_1.chs");
    if (chr.IsError()) {
	cout << "Pos: " << chr.Error().mPos << " -- " << chr.Error().mText << endl;
    }
    CPPUNIT_ASSERT_MESSAGE("Chromo parsing error", !chr.IsError());
    ChromoNode croot = chr.Root();
    CPPUNIT_ASSERT_MESSAGE("Chromo root is empty", croot != ChromoNode());
    ChromoNode::Iterator beg = croot.Begin();
    ChromoNode cmp1 = *beg;
    CPPUNIT_ASSERT_MESSAGE("Chromo root comp1 is empty", cmp1 != ChromoNode());
    TNodeType cmp1t = cmp1.Type();
    CPPUNIT_ASSERT_MESSAGE("Wront type of root comp1", cmp1t == ENt_Node);
    int cn = croot.Count();
    CPPUNIT_ASSERT_MESSAGE("Wrong root node comps number", cn == 2);
    
}

void Ut_chromo2::test_Cre1()
{
    printf("\n === Test of extended chromo: creating simple model\n");

    iEnv = new Env("ut_chr2_cre1.chs", "ut_chr2_cre1.log");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ImpsMgr()->ResetImportsPaths();
    iEnv->ConstructSystem();
    MUnit* root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != NULL);
    MUnit* e1_2_1 = root->GetNode("./e1/e1_2/e1_2_1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get e1_2_1", e1_2_1 != NULL);

    delete iEnv;
}

void Ut_chromo2::test_Seg()
{
    printf("\n === Test of extended chromo: segment\n");


    MutCtx mc((const MUnit*) NULL);

    iEnv = new Env("ut_seg.xml", "ut_seg.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ImpsMgr()->ResetImportsPaths();
    iEnv->ImpsMgr()->AddImportsPaths("../modules");
    iEnv->ConstructSystem();
    MUnit* root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != NULL);
    MUnit* e4 = root->GetNode("./elem3/elem4");
    CPPUNIT_ASSERT_MESSAGE("Fail to get e4", e4 != NULL);
    MElem* eroot = root->GetObj(eroot);
    CPPUNIT_ASSERT_MESSAGE("Fail to get eroot", eroot != NULL);
    eroot->Chromos().Save("ut_seg_saved.xml_");
    //Chromo2 chr;
    //chr.SetFromFile("ut_seg.xml");
    //ChromoNode croot = chr.Root();
}

void Ut_chromo2::test_Ns()
{
    printf("\n === Test of extended chromo: namespace\n");
    iEnv = new Env("ut_ns.xml", "ut_ns.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ImpsMgr()->ResetImportsPaths();
    iEnv->ImpsMgr()->AddImportsPaths("../modules");
    iEnv->ConstructSystem();
    MUnit* root = iEnv->Root();
    /*
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    MUnit* e4 = root->GetNode("./elem3/elem4");
    CPPUNIT_ASSERT_MESSAGE("Fail to get e4", e4 != 0);
    */
}

