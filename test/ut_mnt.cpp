#include <env.h>
#include <stdlib.h>
#include <elem.h>
#include <mvert.h>
#include <mdata.h>
#include <mdes.h>

#include <cppunit/extensions/HelperMacros.h>

/*
 * This test is for checking the functionality of model mounting
 */


class Ut_mnt : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(Ut_mnt);
    CPPUNIT_TEST(test_Mnt_1);
    CPPUNIT_TEST_SUITE_END();
public:
    virtual void setUp();
    virtual void tearDown();
private:
    void test_Mnt_1();
private:
    Env* iEnv;
};

CPPUNIT_TEST_SUITE_REGISTRATION( Ut_mnt );
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(Ut_mnt, "Ut_mnt");


void Ut_mnt::setUp()
{
}

void Ut_mnt::tearDown()
{
//    delete iEnv;
    CPPUNIT_ASSERT_EQUAL_MESSAGE("tearDown", 0, 0);
}

/** @brief Mouting agent: mount simple model
 * */
void Ut_mnt::test_Mnt_1()
{
    printf("\n === Test of mounting agent: simple model\n");

    const string specn("ut_mnt_1");
    string ext = "chs";
    string spec = specn + string(".") + "chs";
    string log = specn + "_" + ext + ".log";
    iEnv = new Env(spec, log);
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ImpsMgr()->ResetImportsPaths();
    iEnv->ImpsMgr()->AddImportsPaths("../modules");
    iEnv->SetEVar("Model", "./ut_mnt_test_model_1.chs");
    iEnv->ConstructSystem();
    MUnit* root = iEnv->Root();
    MElem* eroot = root->GetObj(eroot);
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != NULL && eroot != NULL);

    // Verify mounted model
    MUnit* mnt = root->GetNode("./test/ModelMnt");
    CPPUNIT_ASSERT_MESSAGE("Fail to get mounting agent", mnt != NULL);
    MUnit* mroot = mnt->GetNode("./*");
    CPPUNIT_ASSERT_MESSAGE("Fail to get mounting agent root", mroot != NULL);
    MUnit* mroot2 = root->GetNode("/testroot/test/ModelMnt/test");
    CPPUNIT_ASSERT_MESSAGE("Wrong mounting agent root", mroot == mroot2);

    delete iEnv;
}

