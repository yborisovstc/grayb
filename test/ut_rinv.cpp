#include <stdlib.h>
#include <elem.h>
#include <mvert.h>
#include <mdata.h>
#include <env.h>
#include <typeinfo>

#include <cppunit/extensions/HelperMacros.h>


/*
 * This test is for checking the functionality of remote invocation
 * To run test suite separatelly: ./ut-grayb-lib Ut_Rinv
 */

class Ut_Rinv : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(Ut_Rinv);
    CPPUNIT_TEST(test_Rinv1);
    CPPUNIT_TEST_SUITE_END();
public:
    virtual void setUp();
    virtual void tearDown();
private:
    void test_Rinv1();
private:
    Env* iEnv;
};

//CPPUNIT_TEST_SUITE_REGISTRATION( Ut_Rinv );
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(Ut_Rinv, "Ut_Rinv");

void Ut_Rinv::setUp()
{
}

void Ut_Rinv::tearDown()
{
//    delete iEnv;
    CPPUNIT_ASSERT_EQUAL_MESSAGE("tearDown", 0, 0);
}

void Ut_Rinv::test_Rinv1()
{
    printf("\n === Test of remote invocation: MEnv\n");
    iEnv = new Env("ut_rinv1.xml", "ut_rinv1.txt");
    iEnv->ChMgr()->SetEnableFixErrors(ETrue);
    iEnv->ChMgr()->SetEnableReposMuts(ETrue);
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ImpsMgr()->ResetImportsPaths();
    iEnv->ImpsMgr()->AddImportsPaths("../modules");
    iEnv->ConstructSystem();
    MEnv* menv = iEnv->GetObj(menv);
    const std::type_info& ti_menv = typeid(menv);
    string ti_menv_name = ti_menv.name();
    CPPUNIT_ASSERT_MESSAGE("Fail to get menv", menv != 0);
    MIface* menvi = dynamic_cast<MIface*>(menv);
    CPPUNIT_ASSERT_MESSAGE("Fail to get menvi", menvi != 0);
    string spec = "Root,s";
    string rres;
    MIface* root = menvi->Call(spec, rres);
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    root->Call("Name", rres);
    CPPUNIT_ASSERT_MESSAGE("Fail to get root name", rres == "test");
    MIface* v1 = NULL;
    // Get node
    try {
	v1 = root->Call("GetNode,1,./v1", rres);
	CPPUNIT_ASSERT_MESSAGE("Fail to get v1", v1 != NULL);
    } catch (exception& e) {
	CPPUNIT_ASSERT_MESSAGE(string("Exception on getting v1: ") + e.what(), false);
    }
    // Mutate node
    try {
	v1->Call("Mutate,1,<node><node id=\"v1_1\" parent=\"Elem\"></node></node>,false,true,true", rres);
    } catch (exception& e) {
	CPPUNIT_ASSERT_MESSAGE(string("Exception on mutating v1: ") + e.what(), false);
    }
    // Verifying mutation
    MIface* v1_1 = NULL;
    try {
	v1_1 = root->Call("GetNode,1,./v1/v1_1", rres);
	CPPUNIT_ASSERT_MESSAGE("Cannot get v1_1", v1_1 != NULL);
	v1_1->Call("Name", rres);
	printf("Verifying mutation -- v1_1: %s\n", rres.c_str());
    } catch (exception& e) {
	CPPUNIT_ASSERT_MESSAGE(string("Exception on getting v1_1: ") + e.what(), false);
    }
    // Checking GetCont
    MIface* prop1 = NULL;
    try {
	prop1 = root->Call("GetNode,1,./Prop1", rres);
	CPPUNIT_ASSERT_MESSAGE("Fail to get Prop1", prop1 != NULL);
	prop1->Call("GetCont,1,", rres);
	CPPUNIT_ASSERT_MESSAGE("Wrong Prop1 content", rres == "Prop1_content");
    } catch (exception& e) {
	CPPUNIT_ASSERT_MESSAGE(string("Exception on checking GetCont: ") + e.what(), false);
    }

    delete iEnv;
}
