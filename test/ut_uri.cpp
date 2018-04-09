#include <env.h>
#include <stdlib.h>
#include <elem.h>
#include <mvert.h>
#include <mdata.h>
#include <mprop.h>

#include <cppunit/extensions/HelperMacros.h>

/*
 * This test is for checking the functionality of URI
 */


class Ut_uri : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(Ut_uri);
    CPPUNIT_TEST(test_UriParsing);
    CPPUNIT_TEST(test_UriOpr);
    CPPUNIT_TEST(test_UriBase);
    CPPUNIT_TEST(test_UriRebase);
    //CPPUNIT_TEST(test_UriChromo_1);
    CPPUNIT_TEST_SUITE_END();
public:
    virtual void setUp();
    virtual void tearDown();
private:
    void test_UriParsing();
    void test_UriOpr();
    void test_UriBase();
    void test_UriRebase();
    void test_UriChromo_1();
private:
    Env* iEnv;
};

CPPUNIT_TEST_SUITE_REGISTRATION( Ut_uri );

const string KModulesPath = "../modules/";

void Ut_uri::setUp()
{
}

void Ut_uri::tearDown()
{
//    delete iEnv;
    CPPUNIT_ASSERT_EQUAL_MESSAGE("tearDown", 0, 0);
}

void Ut_uri::test_UriParsing()
{
    printf("\n === Test of uri parsing\n");
    GUri uri_a("sock://localhost:12345/env_1#/root/root_comp");
    CPPUNIT_ASSERT_MESSAGE("Fail to get scheme", uri_a.Scheme() == "sock");
    CPPUNIT_ASSERT_MESSAGE("Fail to get authority", uri_a.Authority() == "localhost:12345");
    CPPUNIT_ASSERT_MESSAGE("Fail to get path", uri_a.Path() == "env_1");
}

void Ut_uri::test_UriOpr()
{
    printf("\n === Test of uri operations\n");
    GUri uri_a("./(Elem:)Node1/(Elem:)Node2");
    string uri_bis = "./(Elem:)Node3";
    GUri uri_b(uri_bis);
    string uri_bs = uri_b.toString();
    CPPUNIT_ASSERT_MESSAGE("Fail to uri_b convert to string", uri_bs == uri_bis);
    uri_a += uri_b;
    string uri_as = uri_a.toString();
    CPPUNIT_ASSERT_MESSAGE("Fail to uri +=", uri_as == "./(Elem:)Node1/(Elem:)Node2/(Elem:)Node3");
}

void Ut_uri::test_UriBase()
{
    printf("\n === Test of base uri\n");

    iEnv = new Env("ut_uri_base.xml", "ut_uri_base.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ImpsMgr()->ResetImportsPaths();
    iEnv->ImpsMgr()->AddImportsPaths(KModulesPath);
    iEnv->ConstructSystem();
    Elem* root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);

    // Try to get node via incorrect  uri
    MElem* enode = root->GetNode("/Elem");
    CPPUNIT_ASSERT_MESSAGE("Fail to get node via incorrect  uri", enode == NULL);

    // Getting inheritance root
    enode = root->GetNode(":Elem");
    CPPUNIT_ASSERT_MESSAGE("Fail to get inher root", enode != NULL);

    // Getting native agent
    enode = root->GetNode("Elem");
    CPPUNIT_ASSERT_MESSAGE("Fail to get native agent", enode != NULL);

    // Generating native agent uri
    string euri = enode->GetUri();
    MElem* enode1 = root->GetNode(euri);
    CPPUNIT_ASSERT_MESSAGE("Fail to generate URI of native agent", enode1 == enode);
    
    MElem* cnode = root->GetNode(".");
    CPPUNIT_ASSERT_MESSAGE("Fail to get current node", cnode != NULL);

    MElem* enode2 = root->GetNode("./test/Incr");
    CPPUNIT_ASSERT_MESSAGE("Fail to get Incr", enode2 != NULL);
    MElem* owner = enode2->GetNode("./..");
    CPPUNIT_ASSERT_MESSAGE("Fail to get current owner", owner != NULL);
    // Generating native agent relative uri
    string eruri = enode->GetUri(enode2);
    enode1 = enode2->GetNode(eruri);
    CPPUNIT_ASSERT_MESSAGE("Fail to generate URI of native agent", enode1 == enode);

    MElem* doutp = root->GetNode("./(Incaps:)test/(DataSInt:)DataS_Int_1/Capsule/out");
    CPPUNIT_ASSERT_MESSAGE("Fail to get data out", doutp != 0);
    MDIntGet* doutpget = (MDIntGet*) doutp->GetSIfi(MDIntGet::Type(), NULL);
    CPPUNIT_ASSERT_MESSAGE("Fail to get data out Get iface", doutpget != 0);
    CPPUNIT_ASSERT_MESSAGE("Fail to get value of data iface", doutpget->Value() == 34);
    MVert* mdoutpv = doutp->GetObj(mdoutpv);
    CPPUNIT_ASSERT_MESSAGE("Fail to get data out vertex", mdoutpv != 0);
    MVert* pair = mdoutpv->GetPair(0);
    CPPUNIT_ASSERT_MESSAGE("Fail to get pair", pair != 0);
    MElem* efuninp = root->GetNode("./(Incaps:)test/(FuncIncInt:)Incr/Capsule/inp");
    CPPUNIT_ASSERT_MESSAGE("Fail to get fun inp", efuninp != NULL);
    MVert* mpairt = efuninp->GetObj(mpairt);
    CPPUNIT_ASSERT_MESSAGE("Wrong pair", pair == mpairt);

    MElem* foutp = root->GetNode("./test/Incr2/Capsule/out");
    CPPUNIT_ASSERT_MESSAGE("Fail to get func out", foutp != 0);
    MDIntGet* foutpget = (MDIntGet*) foutp->GetSIfi(MDIntGet::Type(), NULL);
    CPPUNIT_ASSERT_MESSAGE("Fail to get func out Get iface", foutpget != 0);
    TInt fres = foutpget->Value();
    CPPUNIT_ASSERT_MESSAGE("Incorrect func result", fres == 36);

    MElem* resdata = root->GetNode("./(Incaps:)test/ResData/Capsule/out");
    CPPUNIT_ASSERT_MESSAGE("Fail to get res data out", resdata != 0);
    MDIntGet* rdataget = (MDIntGet*) resdata->GetSIfi(MDIntGet::Type(), NULL);
    CPPUNIT_ASSERT_MESSAGE("Fail to get rdata out Get iface", rdataget != 0);
    TInt rdataval = rdataget->Value();
    CPPUNIT_ASSERT_MESSAGE("Incorrect rdata value", rdataval == 36);
    MElem* resdataprop = root->GetNode("./(Incaps:)test/(DataSInt:)ResData/(Prop:)Value");
    CPPUNIT_ASSERT_MESSAGE("Fail to get rdata value property", resdataprop != 0);
    MProp* rdmprop = resdataprop->GetObj(rdmprop);
    const string& rdval = rdmprop->Value();
    CPPUNIT_ASSERT_MESSAGE("Incorrect rdata prop value", rdval == "36");

    // Checking the result update on update of input
    // Mutate the input data first
    MElem* dinp = root->GetNode("./(Incaps:)test/(DataSInt:)DataS_Int_1");
    ChromoNode nchange = dinp->AppendMutation(ENt_Cont);
    nchange.SetAttr(ENa_MutNode, "./(Prop:)Value");
    nchange.SetAttr(ENa_MutVal, "57");
    dinp->Mutate();
    // Check the function output
    MElem* foutp1 = root->GetNode("./test/Incr2/Capsule/out");
    CPPUNIT_ASSERT_MESSAGE("Fail to get func out when inp data changed", foutp1 != 0);
    MDIntGet* foutpget1 = (MDIntGet*) foutp1->GetSIfi(MDIntGet::Type(), NULL);
    CPPUNIT_ASSERT_MESSAGE("Fail to get func out Get iface when input data changed", foutpget1 != 0);
    TInt fres1 = foutpget1->Value();
    CPPUNIT_ASSERT_MESSAGE("Incorrect func result after input data change", fres1 == 59);
    // Check the output data
    MElem* resdataprop1 = root->GetNode("./test/ResData/Value");
    CPPUNIT_ASSERT_MESSAGE("Fail to get result data value property when inp data changed", resdataprop1 != 0);
    MProp* rdmprop1 = resdataprop1->GetObj(rdmprop1);
    const string& rdval1 = rdmprop1->Value();
    CPPUNIT_ASSERT_MESSAGE("Incorrect result data prop value when inp data changed", rdval1 == "59");


    MElem* resdata1 = root->GetNode("./(Incaps:)test/ResData/Capsule/out");
    // Checking getting uri basing on hier mgr
    GUri rduri;
    resdata1->GetUri(rduri, root);
    string rduriss = rduri.toString();
    CPPUNIT_ASSERT_MESSAGE("Fail to get URI from hier", rduriss == "./(Incaps:)test/(DataSInt:)ResData/(ACapsule:)Capsule/(ConnPointOut:)out");
    // Checking getting absolute uri
    GUri rduria;
    resdata1->GetUri(rduria);
    string rduriass= rduria.toString();
    CPPUNIT_ASSERT_MESSAGE("Fail to get absolute URI", 
	    rduriass == "/(Elem:)testroot/(Incaps:)test/(DataSInt:)ResData/(ACapsule:)Capsule/(ConnPointOut:)out");
    // Checking of getting node by absolute uri
    MElem* nodeau = resdata1->GetNode(rduriass);
    CPPUNIT_ASSERT_MESSAGE("Fail to get node by absolute URI", nodeau == resdata1);
    // Checking creating module with absolute uri
    /* Temporarily avoided because of disabling modules deletion, so FuncComps is already in root
    ChromoNode madd = root->AppendMutation(ENt_Node);
    madd.SetAttr(ENa_Id, "new_elem1");
    madd.SetAttr(ENa_Parent, "file:../modules/func.xml#/(Elem:)FuncComps");
    root->Mutate();
    // Check the element added
    Elem* eadded = root->GetNode("(FuncComps:)new_elem1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get elem added", eadded != 0);
    */
}

// Test of operating with chromo uris
void Ut_uri::test_UriChromo_1()
{
    printf("\n === Test of chromo uri\n");

    iEnv = new Env("ut_uri_chromo_1.xml", "ut_uri_chromo_1.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ConstructSystem();
    Elem* root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != NULL);

    // Check if chromo GetNode works ok in case of node with destination
    MElem* e_2_1 = root->GetNode("/testroot/E_2/E_1_1/E_2_1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get e_2_1", e_2_1 != NULL);
    MElem* e_1 = root->GetNode("/testroot/E_1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get e_1", e_1 != NULL);
    ChromoNode n_2_1 = e_2_1->Chromos().Root();
    ChromoNode n_1 = e_1->Chromos().Root();
    GUri uri1("./../../../E_1");
    ChromoNode nres = n_2_1.GetNode(uri1);
    CPPUNIT_ASSERT_MESSAGE("Fail to get node from node with destination", nres == n_1);

}

void Ut_uri::test_UriRebase()
{
    printf("\n === Test of rebase uri\n");

    iEnv = new Env("ut_uri_base.xml", "ut_uri_base.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ImpsMgr()->ResetImportsPaths();
    iEnv->ImpsMgr()->AddImportsPaths(KModulesPath);
    iEnv->ConstructSystem();
    Elem* root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);

    GUri uri("/testroot/Modules/DataComps/(DataS:)DataSInt");
    MElem* base = root->GetNode("/testroot/Modules");
    CPPUNIT_ASSERT_MESSAGE("Fail to get base", base != NULL);
    GUri ures;
    root->RebaseUri(uri, base, ures);
    GUri uritest("./DataComps/(DataS:)DataSInt");
    CPPUNIT_ASSERT_MESSAGE("Fail to rebase uri#1", ures == uritest);
}
