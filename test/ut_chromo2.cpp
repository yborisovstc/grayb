#include <env.h>
#include <stdlib.h>

#include "chromox.h"
#include "chromo2.h"

#include <cppunit/extensions/HelperMacros.h>

/*
 * This test is for chromo2
 */


class Ut_chromo2 : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(Ut_chromo2);
    //CPPUNIT_TEST(test_Chr1);
    CPPUNIT_TEST(test_Cre1);
    CPPUNIT_TEST(test_Combo);
    CPPUNIT_TEST(test_Seg);
   // CPPUNIT_TEST(test_Ns);
//    CPPUNIT_TEST(test_Conv);
    CPPUNIT_TEST_SUITE_END();
public:
    virtual void setUp();
    virtual void tearDown();
private:
    void test_Chr1();
    void test_Cre1();
    void test_Combo();
    void test_Seg();
    void test_Ns();
    void test_Conv();
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
    chr.Root().Dump();
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

    const string specn("ut_chr2_cre1");
    string ext = "chs";
    string spec = specn + string(".") + ext;
    string log = specn + "_" + ext + ".log";
    iEnv = new Env(spec, log);
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ImpsMgr()->ResetImportsPaths();
    iEnv->ConstructSystem();
    MUnit* root = iEnv->Root();
    MElem* eroot = root->GetObj(eroot);
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != NULL && eroot != NULL);
    eroot->Chromos().Save(specn + "_saved" + ".log");

    MUnit* e1_2_1 = root->GetNode("./e1/e1_2/e1_2_1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get e1_2_1", e1_2_1 != NULL);

    delete iEnv;
}

void Ut_chromo2::test_Combo()
{
    printf("\n === Test of extended chromo: chromo combo nodes\n");

    iEnv = new Env("ut_chr2_combo.chs", "ut_chr2_combo_chs.log");
    //iEnv = new Env("ut_chr2_combo.xml", "ut_chr2_combo_xml.log");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ImpsMgr()->ResetImportsPaths();
    iEnv->ConstructSystem();
    MUnit* root = iEnv->Root();
    MElem* eroot = root->GetObj(eroot);
    eroot->Chromos().Save("ut_chr2_combo_saved.chs");

    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != NULL && eroot != NULL);
    MUnit* s4 = root->GetNode("/root/e2/e3/s4");
    CPPUNIT_ASSERT_MESSAGE("Fail to get s4", s4 != NULL);

    delete iEnv;
}


void Ut_chromo2::test_Seg()
{
    printf("\n === Test of extended chromo: segment\n");

    for (int ct = 0; ct < 1; ct++) {
	const string specn("ut_seg");
	string ext = ct == 0 ? "xml" : "chs";
	string spec = specn + string(".") + ext;
	string log = specn + "_" + ext + ".log";
	iEnv = new Env(spec, log);
	CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
	iEnv->ImpsMgr()->ResetImportsPaths();
	iEnv->ImpsMgr()->AddImportsPaths("../modules");
	iEnv->ConstructSystem();
	MUnit* root = iEnv->Root();
	MElem* eroot = root->GetObj(eroot);
	CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != NULL && eroot != NULL);
	// Save chromo
	string saved_name = specn + "_saved." + ext;
	eroot->Chromos().Save(saved_name);

	MUnit* e4 = root->GetNode("./elem3/elem4");
	CPPUNIT_ASSERT_MESSAGE("Fail to get e4", e4 != NULL);

	MUnit* u1_1_1_2 = root->GetNode("./elem1/unit1_1/unit1_1_1/unit1_1_1_2");
	CPPUNIT_ASSERT_MESSAGE("Fail to get u1_1_1_2", u1_1_1_2 != NULL);

	MUnit* e1_1_1 = root->GetNode("./elem1/elem1_1/elem1_1_1");
	CPPUNIT_ASSERT_MESSAGE("Fail to get e1_1_1", e1_1_1 != NULL);

	MUnit* e1_1_2 = root->GetNode("./elem1/elem1_1/elem1_1_2");
	CPPUNIT_ASSERT_MESSAGE("Fail to get e1_1_2", e1_1_2 != NULL);

	delete iEnv;
    }
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

void Ut_chromo2::test_Conv()
{
    printf("\n === Test of Chr1 conversion to Chr2\n");
    ChromoX chr;
    chr.SetFromFile("ut_chr2_conv.xml");
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
    CPPUNIT_ASSERT_MESSAGE("Wront type of root comp1", cmp1t == ENt_Note);
    int cn = croot.Count();
    CPPUNIT_ASSERT_MESSAGE("Wrong root node comps number", cn == 8);

    Chromo2 chr2;
    chr2.Convert(chr);
    chr2.Root().Dump();
    chr2.Save("ut_chr2_conv.chs");

}
