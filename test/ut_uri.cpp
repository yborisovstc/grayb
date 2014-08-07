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
    CPPUNIT_TEST(test_UriOpr);
    CPPUNIT_TEST(test_UriBase);
    CPPUNIT_TEST_SUITE_END();
public:
    virtual void setUp();
    virtual void tearDown();
private:
    void test_UriOpr();
    void test_UriBase();
private:
    Env* iEnv;
};

CPPUNIT_TEST_SUITE_REGISTRATION( Ut_uri );


void Ut_uri::setUp()
{
}

void Ut_uri::tearDown()
{
//    delete iEnv;
    CPPUNIT_ASSERT_EQUAL_MESSAGE("tearDown", 0, 0);
}

void Ut_uri::test_UriOpr()
{
    printf("\n === Test of uri operations\n");
    GUri uri_a("(Elem:)Node1/(Elem:)Node2");
    GUri uri_b("(Elem:)Node3");
    uri_a += uri_b;
    string uri_as = uri_a.GetUri();
    CPPUNIT_ASSERT_MESSAGE("Fail to uri +=", uri_as == "(Elem:)Node1/(Elem:)Node2/(Elem:)Node3");
}

void Ut_uri::test_UriBase()
{
    printf("\n === Test of base uri\n");

    iEnv = new Env("Env", "ut_uri_base.xml", "ut_uri_base.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ConstructSystem();
    Elem* root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    Elem* doutp = root->GetNode("(Incaps:)test/(DataSInt:)DataS_Int_1/(Elem:)Capsule/out");
    CPPUNIT_ASSERT_MESSAGE("Fail to get data out", doutp != 0);
    MDIntGet* doutpget = doutp->GetObj(doutpget);
    CPPUNIT_ASSERT_MESSAGE("Fail to get data out Get iface", doutpget != 0);
    CPPUNIT_ASSERT_MESSAGE("Fail to get value of data iface", doutpget->Value() == 34);
    MVert* mdoutpv = doutp->GetObj(mdoutpv);
    CPPUNIT_ASSERT_MESSAGE("Fail to get data out vertex", mdoutpv != 0);
    MVert* pair = *(mdoutpv->Pairs().begin());
    CPPUNIT_ASSERT_MESSAGE("Fail to get pair", pair != 0);
    Elem* efuninp = root->GetNode("(Incaps:)test/(FuncIncInt:)Incr/Capsule/inp");
    CPPUNIT_ASSERT_MESSAGE("Fail to get fun inp", efuninp != NULL);
    MVert* mpairt = efuninp->GetObj(mpairt);
    CPPUNIT_ASSERT_MESSAGE("Wrong pair", pair == mpairt);

    // Try to get node via incorrect  uri
    Elem* enode = root->GetNode("/Elem");
    CPPUNIT_ASSERT_MESSAGE("Fail to get node via incorrect  uri", enode == NULL);

    Elem* foutp = root->GetNode("test/Incr2/Capsule/out");
    CPPUNIT_ASSERT_MESSAGE("Fail to get func out", foutp != 0);
    MDIntGet* foutpget = foutp->GetObj(foutpget);
    CPPUNIT_ASSERT_MESSAGE("Fail to get func out Get iface", foutpget != 0);
    TInt fres = foutpget->Value();
    CPPUNIT_ASSERT_MESSAGE("Incorrect func result", fres == 36);

    Elem* resdata = root->GetNode("(Incaps:)test/ResData/(Elem:)Capsule/out");
    CPPUNIT_ASSERT_MESSAGE("Fail to get res data out", resdata != 0);
    MDIntGet* rdataget = resdata->GetObj(rdataget);
    CPPUNIT_ASSERT_MESSAGE("Fail to get rdata out Get iface", rdataget != 0);
    TInt rdataval = rdataget->Value();
    CPPUNIT_ASSERT_MESSAGE("Incorrect rdata value", rdataval == 36);
    Elem* resdataprop = root->GetNode("(Incaps:)test/(DataSInt:)ResData/(Prop:)Value");
    CPPUNIT_ASSERT_MESSAGE("Fail to get rdata value property", resdataprop != 0);
    MProp* rdmprop = resdataprop->GetObj(rdmprop);
    const string& rdval = rdmprop->Value();
    CPPUNIT_ASSERT_MESSAGE("Incorrect rdata prop value", rdval == "36");

    // Checking the result update on update of input
    // Mutate the input data first
    Elem* dinp = root->GetNode("(Incaps:)test/(DataSInt:)DataS_Int_1");
    ChromoNode nchange = dinp->Mutation().Root().AddChild(ENt_Cont);
    nchange.SetAttr(ENa_MutNode, "(Prop:)Value");
    nchange.SetAttr(ENa_MutVal, "57");
    dinp->Mutate();
    // Check the function output
    Elem* foutp1 = root->GetNode("test/Incr2/Capsule/out");
    CPPUNIT_ASSERT_MESSAGE("Fail to get func out when inp data changed", foutp1 != 0);
    MDIntGet* foutpget1 = foutp1->GetObj(foutpget1);
    CPPUNIT_ASSERT_MESSAGE("Fail to get func out Get iface when input data changed", foutpget1 != 0);
    TInt fres1 = foutpget1->Value();
    CPPUNIT_ASSERT_MESSAGE("Incorrect func result after input data change", fres1 == 59);
    // Check the output data
    Elem* resdataprop1 = root->GetNode("test/ResData/Value");
    CPPUNIT_ASSERT_MESSAGE("Fail to get result data value property when inp data changed", resdataprop1 != 0);
    MProp* rdmprop1 = resdataprop1->GetObj(rdmprop1);
    const string& rdval1 = rdmprop1->Value();
    CPPUNIT_ASSERT_MESSAGE("Incorrect result data prop value when inp data changed", rdval1 == "59");


    // Checking URI with asterisk in URI elem
   // Elem* resdata1 = root->GetNode("Incaps:test/DataSInt:ResData/Elem:Capsule/ConnPoint:out");
    Elem* resdata1 = root->GetNode("(Incaps:)test/(DataSInt:)ResData/(*:)*/out");
    CPPUNIT_ASSERT_MESSAGE("Fail to get res data out (asterisk URI)", resdata1 != 0);
    // Checking URI with "anywhere" pattern from root
    Elem* node0 = root->GetNode("/testroot/test/Incr2");
    CPPUNIT_ASSERT_MESSAGE("Fail to get node (/testroot/test/Incr2)", node0 != 0);
    Elem* node1 = root->GetNode("/**/Incr2");
    CPPUNIT_ASSERT_MESSAGE("Fail to get node (/**/Incr2)", node1 != 0 && node1->GetUri(NULL) == node0->GetUri(NULL));
    // Checking URI with beginning "anywhere" pattern
    Elem* node2 = root->GetNode("**/Incr2");
    CPPUNIT_ASSERT_MESSAGE("Fail to get node (**/Incr2)", node2 != 0 && node2->GetUri(NULL) == node0->GetUri(NULL));
    // Checking "anywhere" pattern for direct root comp
    Elem* elm1 = root->GetNode("**/SysComps");
    CPPUNIT_ASSERT_MESSAGE("Fail to get base elm1", elm1 != 0);
    // Checking creation from parent specified with URI with "anywhere" pattern
    Elem* cp1 = root->GetNode("/testroot/test/Cp1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get base CP1", cp1 != 0);
    Elem* cp2 = root->GetNode("/testroot/test/Cp2");
    CPPUNIT_ASSERT_MESSAGE("Fail to get CP2", cp2 != 0);
 
    // Checking getting uri basing on hier mgr
    GUri rduri;
    resdata1->GetUri(rduri, root);
    string rduriss = rduri.GetUri();
    CPPUNIT_ASSERT_MESSAGE("Fail to get URI from hier", rduriss == "(Incaps:)test/(DataSInt:)ResData/(Elem:)Capsule/(ConnPointOut:)out");
    // Checking getting absolute uri
    GUri rduria;
    resdata1->GetUri(rduria);
    string rduriass= rduria.GetUri();
    CPPUNIT_ASSERT_MESSAGE("Fail to get absolute URI", 
	    rduriass == "/(Elem:)testroot/(Incaps:)test/(DataSInt:)ResData/(Elem:)Capsule/(ConnPointOut:)out");
    // Checking of getting node by absolute uri
    Elem* nodeau = resdata1->GetNode(rduriass);
    CPPUNIT_ASSERT_MESSAGE("Fail to get node by absolute URI", nodeau == resdata1);
    // Checking creating module with absolute uri
    /* Temporarily avoided because of disabling modules deletion, so FuncComps is already in root
    ChromoNode madd = root->Mutation().Root().AddChild(ENt_Node);
    madd.SetAttr(ENa_Id, "new_elem1");
    madd.SetAttr(ENa_Parent, "file:../modules/func.xml#/(Elem:)FuncComps");
    root->Mutate();
    // Check the element added
    Elem* eadded = root->GetNode("(FuncComps:)new_elem1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get elem added", eadded != 0);
    */
 
    // Checking getting uri basing on inheritance relations
    Elem* node_ni_1 = root->GetNode("/testroot/IncapsComps/Incaps");
    Elem* node_ih_1 = root->GetNode(":Elem:Vert:Syst:Incaps");
    CPPUNIT_ASSERT_MESSAGE("Fail to get node by inh URI", node_ni_1 != NULL && node_ih_1 != NULL && node_ni_1 == node_ih_1);
    Elem* node_ih_2 = root->GetNode(":Elem:IncapsComps/Incaps");
    CPPUNIT_ASSERT_MESSAGE("Fail to get node by inh URI", node_ni_1 != NULL && node_ih_2 != NULL && node_ni_1 == node_ih_2);
    // With any pattern
    Elem* incapsmod_ni_1 = root->GetNode("/testroot/IncapsComps");
    Elem* incapsmod_ih_1 = root->GetNode(":Elem:IncapsComps");
    CPPUNIT_ASSERT_MESSAGE("Fail to get node by inh URI and -any- pattern", incapsmod_ni_1 != NULL && incapsmod_ih_1 != NULL && incapsmod_ni_1 == incapsmod_ih_1);
}


