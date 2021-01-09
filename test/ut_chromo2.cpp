#include <env.h>
#include <stdlib.h>

#include "chromox.h"
#include "chromo2.h"
#include "mprov.h"
#include "mvert.h"

#include <cppunit/extensions/HelperMacros.h>

/*
 * This test is for chromo2
 */


class Ut_chromo2 : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(Ut_chromo2);
    //CPPUNIT_TEST(test_Chr1);
    ////CPPUNIT_TEST(test_Cre1);
    ////CPPUNIT_TEST(test_Combo);
    ////CPPUNIT_TEST(test_Seg);
    ////CPPUNIT_TEST(test_Ns);
    //CPPUNIT_TEST(test_Conv);
    //CPPUNIT_TEST(test_Tranf1);
    //CPPUNIT_TEST(test_Tranf2);
    //CPPUNIT_TEST(test_Dmc1);
    //CPPUNIT_TEST(test_Dmc2);
    //CPPUNIT_TEST(test_Dmc3);
    CPPUNIT_TEST(test_Dmc4);
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
    void test_Tranf1();
    void test_Tranf2();
    void test_Dmc1();
    void test_Dmc2();
    void test_Dmc3();
    void test_Dmc4();
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

    for (int ct = 1; ct < 2; ct++) {
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

	delete iEnv;
    }
}

void Ut_chromo2::test_Ns()
{
    printf("\n === Test of extended chromo: namespace\n");
    const string specn("ut_chr2_ns");
    string ext = "chs";
    string spec = specn + string(".") + ext;
    string log = specn + "_" + ext + ".log";
    iEnv = new Env(spec, log);
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ImpsMgr()->ResetImportsPaths();
    iEnv->ImpsMgr()->AddImportsPaths("../modules");
    iEnv->ConstructSystem();
    MUnit* root = iEnv->Root();
    MElem* eroot = root->GetObj(eroot);
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    // Save chromo
    string saved_name = specn + "_saved." + ext;
    eroot->Chromos().Save(saved_name);
    //       MUnit* e4 = root->GetNode("./elem3/elem4");
 //      CPPUNIT_ASSERT_MESSAGE("Fail to get e4", e4 != 0);
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

void Ut_chromo2::test_Tranf1()
{
    printf("\n === Test of chromo transforming to tree-like form\n");

    string specn = "ut_chr2_tf1";
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
    // Save
    eroot->Chromos().Save(specn + "_saved." + ext);
    // Check the model created
    MUnit* s1_2_cp2 = root->GetNode("./s1/s1_2/cp2");
    CPPUNIT_ASSERT_MESSAGE("Fail to get ./s1/s1_2/cp2", s1_2_cp2);
    MUnit* s1_2_u1_1  = root->GetNode("./s1/s1_2/s1_2_u1/s1_2_u1_1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get s1_2_u1_1  ", s1_2_u1_1);
    // Dump s1 chromo
    MUnit* s1 = root->GetNode("./s1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get ./s1", s1);
    cout << "Dump s1 chromo: " << endl;
    MElem* s1e = s1->GetObj(s1e);
    CPPUNIT_ASSERT_MESSAGE("Fail to get ./s1 MElem", s1e);
    s1e->Chromos().Root().Dump();
    // Transform
    MProvider* prov = iEnv->Provider();
    MChromo* tchr = prov->CreateChromo("chs");
    tchr->TransformLn(eroot->Chromos());
    tchr->Save(specn + "_transf." + ext);

    delete iEnv;


    // Check model creation from saved chromo
    cout << "Creating model from saved chromoe" << endl;
    specn = "ut_chr2_tf1_saved";
    ext = "chs";
    spec = specn + string(".") + ext;
    log = specn + "_" + ext + ".log";
    iEnv = new Env(spec, log);
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ImpsMgr()->ResetImportsPaths();
    iEnv->ConstructSystem();
    root = iEnv->Root();
    eroot = root->GetObj(eroot);
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != NULL && eroot != NULL);
    // Check the model created
    s1_2_cp2 = root->GetNode("./s1/s1_2/cp2");
    CPPUNIT_ASSERT_MESSAGE("Fail to get ./s1/s1_2/cp2", s1_2_cp2);
    s1_2_u1_1  = root->GetNode("./s1/s1_2/s1_2_u1/s1_2_u1_1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get s1_2_u1_1  ", s1_2_u1_1);

    delete iEnv;
 
    // Check model creation from transformed chromo
    cout << "Creating model from transromed chromoe" << endl;
    specn = "ut_chr2_tf1_transf";
    ext = "chs";
    spec = specn + string(".") + ext;
    log = specn + "_" + ext + ".log";
    iEnv = new Env(spec, log);
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ImpsMgr()->ResetImportsPaths();
    iEnv->ConstructSystem();
    root = iEnv->Root();
    eroot = root->GetObj(eroot);
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != NULL && eroot != NULL);
    // Check the model created
    s1_2_cp2 = root->GetNode("./s1/s1_2/cp2");
    CPPUNIT_ASSERT_MESSAGE("Fail to get ./s1/s1_2/cp2", s1_2_cp2);
    s1_2_u1_1  = root->GetNode("./s1/s1_2/s1_2_u1/s1_2_u1_1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get s1_2_u1_1  ", s1_2_u1_1);

    delete iEnv;

}

void Ut_chromo2::test_Tranf2()
{
    printf("\n === Test of chromo transforming: melding to parent targ\n");

    string specn = "ut_chr2_tf2";
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
    // Save
    eroot->Chromos().Save(specn + "_saved." + ext);
    // Check the model created
    MUnit* s1_1_u1  = root->GetNode("./s1/s1_1/s1_1_u1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get s1_1_u1", s1_1_u1);
    // Transform
    MProvider* prov = iEnv->Provider();
    MChromo* tchr = prov->CreateChromo("chs");
    tchr->TransformLn(eroot->Chromos());
    tchr->Save(specn + "_transf." + ext);

    delete iEnv;
}

void Ut_chromo2::test_Dmc1()
{
    printf("\n === Test of DMC chromo: base chromo\n");

    string specn = "ut_chr2_dmc_1";
    string ext = "chs";
    string spec = specn + string(".") + ext;
    string log = specn + "_" + ext + ".log";
    iEnv = new Env(spec, log);
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ImpsMgr()->ResetImportsPaths();
    iEnv->ConstructSystem();
    MUnit* root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root);
    MElem* eroot = root->GetObj(eroot);
    CPPUNIT_ASSERT_MESSAGE("Fail to get eroot", eroot);
    // Save
    eroot->Chromos().Save(specn + "_saved." + ext);
    // Check the model created
    MUnit* s1_v1u  = root->GetNode("./s1/v1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get s1/v1 unit", s1_v1u);
    MUnit* s1_v2u  = root->GetNode("./s1/v2");
    CPPUNIT_ASSERT_MESSAGE("Fail to get s1/v2 unit", s1_v2u);
    // Dump root chromo
    cout << "Dump root chromo: " << endl;
    eroot->Chromos().Root().Dump();
    // Check connection
    MVert* s1_v1 = s1_v1u->GetObj(s1_v1);
    CPPUNIT_ASSERT_MESSAGE("Fail to get s1/v1 MVert", s1_v1);
    MVert* s1_v2 = s1_v2u->GetObj(s1_v2);
    CPPUNIT_ASSERT_MESSAGE("Fail to get s1/v2 MVert", s1_v2);
    CPPUNIT_ASSERT_MESSAGE("Fail to get v1 - v2 connected", s1_v2->IsPair(s1_v1));



    delete iEnv;
}

void Ut_chromo2::test_Dmc2()
{
    printf("\n === Test of DMC chromo: anonymous dep\n");

    string specn = "ut_chr2_dmc_2";
    string ext = "chs";
    string spec = specn + string(".") + ext;
    string log = specn + "_" + ext + ".log";
    iEnv = new Env(spec, log);
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ImpsMgr()->ResetImportsPaths();
    iEnv->ConstructSystem();
    MUnit* root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root);
    MElem* eroot = root->GetObj(eroot);
    CPPUNIT_ASSERT_MESSAGE("Fail to get eroot", eroot);
    // Save
    eroot->Chromos().Save(specn + "_saved." + ext);
    // Check the model created
    MUnit* s1_v1u  = root->GetNode("./s1/v1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get s1/v1 unit", s1_v1u);
    // Dump root chromo
    cout << "Dump root chromo: " << endl;
    eroot->Chromos().Root().Dump();
    // Check connection
    MVert* s1_v1 = s1_v1u->GetObj(s1_v1);
    CPPUNIT_ASSERT_MESSAGE("Fail to get s1/v1 MVert", s1_v1);
    CPPUNIT_ASSERT_MESSAGE("Fail to get v1 - v2 connected", s1_v1->PairsCount() == 1);

    delete iEnv;
}

void Ut_chromo2::test_Dmc3()
{
    printf("\n === Test of DMC chromo: name space seg\n");

    string specn = "ut_chr2_dmc_3";
    string ext = "chs";
    string spec = specn + string(".") + ext;
    string log = specn + "_" + ext + ".log";
    iEnv = new Env(spec, log);
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ImpsMgr()->ResetImportsPaths();
    iEnv->ConstructSystem();
    MUnit* root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root);
    MElem* eroot = root->GetObj(eroot);
    CPPUNIT_ASSERT_MESSAGE("Fail to get eroot", eroot);
    // Dump root chromo
    cout << "Dump root chromo: " << endl;
    eroot->Chromos().Root().Dump();
    // Save
    eroot->Chromos().Save(specn + "_saved." + ext);
    // Check the model created
    MUnit* s1_v1u  = root->GetNode("./s1/v1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get s1/v1 unit", s1_v1u);

    delete iEnv;
}

void Ut_chromo2::test_Dmc4()
{
    printf("\n === Test of DMC chromo: DES tree\n");

    string specn = "ut_chr2_dmc_4";
    string ext = "chs";
    string spec = specn + string(".") + ext;
    string log = specn + "_" + ext + ".log";
    iEnv = new Env(spec, log);
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ImpsMgr()->ResetImportsPaths();
    iEnv->ConstructSystem();
    MUnit* root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root);
    MElem* eroot = root->GetObj(eroot);
    CPPUNIT_ASSERT_MESSAGE("Fail to get eroot", eroot);
    // Dump root chromo
    cout << "Dump root chromo: " << endl;
    eroot->Chromos().Root().Dump();
    // Save
    eroot->Chromos().Save(specn + "_saved." + ext);
    // Check the model created
    MUnit* s1_state  = root->GetNode("./s1/State");
    CPPUNIT_ASSERT_MESSAGE("Fail to get s1/State", s1_state);
    MUnit* s1_state_inp  = s1_state->GetNode("./Inp");
    CPPUNIT_ASSERT_MESSAGE("Fail to get s1/State/Inp", s1_state_inp);
    MVert* s1_state_inpv = s1_state_inp->GetObj(s1_state_inpv);
    CPPUNIT_ASSERT_MESSAGE("Fail to get s1/State/Inp vector", s1_state_inpv);
    CPPUNIT_ASSERT_MESSAGE("s1/State/Inp isn't connected", s1_state_inpv->PairsCount() == 1);

    delete iEnv;
}


