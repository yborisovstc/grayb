#include <env.h>
#include <stdlib.h>
#include <elem.h>
#include <mvert.h>
#include <mdata.h>
#include <medge.h>

#include <cppunit/extensions/HelperMacros.h>

/*
 * This test is for checking the functionality of connections
 */


class Ut_conn : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(Ut_conn);
    CPPUNIT_TEST(test_Vertp);
    CPPUNIT_TEST(test_Systp);
    CPPUNIT_TEST(test_Sock);
    CPPUNIT_TEST(test_Sock2);
    CPPUNIT_TEST(test_Reconn);
    CPPUNIT_TEST(test_Conn2);
    CPPUNIT_TEST_SUITE_END();
public:
    virtual void setUp();
    virtual void tearDown();
private:
    void test_Vertp();
    void test_Systp();
    void test_Sock();
    void test_Sock2();
    void test_Reconn();
    void test_Conn2();
private:
    Env* iEnv;
};

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(Ut_conn, "Ut_conn");


void Ut_conn::setUp()
{
}

void Ut_conn::tearDown()
{
//    delete iEnv;
    CPPUNIT_ASSERT_EQUAL_MESSAGE("tearDown", 0, 0);
}

// Test of Vertp connection
void Ut_conn::test_Vertp()
{
    printf("\n === Test of Vertp connection\n");

    iEnv = new Env("ut_conn_vertp_1.xml", "ut_conn_vertp_1.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ConstructSystem();
    Elem* root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    // Verify that v1 and v2 are connected
    MElem* ev1 = root->GetNode("./test/v1");
    MVert* mv1 = ev1->GetObj(mv1);
    MElem* ev2 = root->GetNode("./test/v2");
    MVert* mv2 = ev2->GetObj(mv2);
    int pnum1 = mv1->PairsCount();
    int pnum2 = mv2->PairsCount();
    bool v2ispair = mv1->IsPair(mv2);
    CPPUNIT_ASSERT_MESSAGE("v1 and v2 aren't connected", pnum1 == 1 && pnum2 && v2ispair);
   // Disconnect one point of edge e2
    ChromoNode mutn = root->AppendMutation(ENt_Cont);
    mutn.SetAttr(ENa_Targ, "./test");
    mutn.SetAttr(ENa_Id, "Edges.E1.P1");
    mutn.SetAttr(ENa_MutVal, "");
    root->Mutate();
    // Verify that v1 and v2 are disconnected
    ev1 = root->GetNode("./test/v1");
    mv1 = ev1->GetObj(mv1);
    pnum1 = mv1->PairsCount();
    CPPUNIT_ASSERT_MESSAGE("Wrong number of v1 pairs after e2 disconnection", pnum1 == 0);
}

// Test of Systp connection
void Ut_conn::test_Systp()
{
    printf("\n === Test of Systp connection\n");

    iEnv = new Env("ut_conn_systp_1.xml", "ut_conn_systp_1.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ConstructSystem();
    Elem* root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);

}

void Ut_conn::test_Sock()
{
    printf("\n === Test of connecting of sockets\n");

    iEnv = new Env("ut_conn_sock.xml", "ut_conn_sock.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ImpsMgr()->ResetImportsPaths();
    iEnv->ImpsMgr()->AddImportsPaths("../modules");
    iEnv->ConstructSystem();
    Elem* root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    MElem* doutp = root->GetNode("./test/L1/Cp1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get L1 Cp1", doutp != 0);
    //MDIntGet* doutpget = doutp->GetObj(doutpget);
    MDIntGet* doutpget = (MDIntGet*) doutp->GetSIfi(MDIntGet::Type());
    CPPUNIT_ASSERT_MESSAGE("Fail to get data out Get iface", doutpget != 0);
    CPPUNIT_ASSERT_MESSAGE("Fail to get value of data iface", doutpget->Value() == 3);

    doutp = root->GetNode("./test/L1/Cp2");
    CPPUNIT_ASSERT_MESSAGE("Fail to get L1 Cp2", doutp != 0);
    doutpget = (MDIntGet*) doutp->GetSIfi(MDIntGet::Type());
    CPPUNIT_ASSERT_MESSAGE("Fail to get data out Get iface for Cp2", doutpget != 0);
    CPPUNIT_ASSERT_MESSAGE("Fail to get value of data iface for Cp2", doutpget->Value() == 1);

    doutp = root->GetNode("./test/L1/Cp3");
    CPPUNIT_ASSERT_MESSAGE("Fail to get L1 Cp3", doutp != 0);
    doutpget = (MDIntGet*) doutp->GetSIfi(MDIntGet::Type());
    CPPUNIT_ASSERT_MESSAGE("Fail to get data out Get iface for Cp3", doutpget != 0);
    CPPUNIT_ASSERT_MESSAGE("Fail to get value of data iface for Cp3", doutpget->Value() == 20);

    delete iEnv;
}


void Ut_conn::test_Sock2()
{
    printf("\n === Test of extending of sockets\n");

    iEnv = new Env("ut_conn_sock2.xml", "ut_conn_sock2.txt");
    iEnv->ImpsMgr()->ResetImportsPaths();
    iEnv->ImpsMgr()->AddImportsPaths("../modules");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ConstructSystem();
    Elem* root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);

    MElem* doutp = root->GetNode("./test/L1/Cp1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get L1 Cp1", doutp != 0);
    MDIntGet* doutpget = (MDIntGet*) doutp->GetSIfi(MDIntGet::Type());
    CPPUNIT_ASSERT_MESSAGE("Fail to get data out Get iface for Cp1", doutpget != 0);
    CPPUNIT_ASSERT_MESSAGE("Fail to get value of data iface for Cp1", doutpget->Value() == 3);

    doutp = root->GetNode("./test/L1/Cp2");
    CPPUNIT_ASSERT_MESSAGE("Fail to get L1 Cp2", doutp != 0);
    doutpget = (MDIntGet*) doutp->GetSIfi(MDIntGet::Type());
    CPPUNIT_ASSERT_MESSAGE("Fail to get data out Get iface for Cp2", doutpget != 0);
    CPPUNIT_ASSERT_MESSAGE("Fail to get value of data iface for Cp2", doutpget->Value() == 1);

    doutp = root->GetNode("./test/L1/Cp3");
    CPPUNIT_ASSERT_MESSAGE("Fail to get L1 Cp3", doutp != 0);
    doutpget = (MDIntGet*) doutp->GetSIfi(MDIntGet::Type());
    CPPUNIT_ASSERT_MESSAGE("Fail to get data out Get iface for Cp3", doutpget != 0);
    CPPUNIT_ASSERT_MESSAGE("Fail to get value of data iface for Cp3", doutpget->Value() == 20);

    doutp = root->GetNode("./test/L1/Cp1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get L1 Cp1", doutp != 0);
    doutpget = (MDIntGet*) doutp->GetSIfi(MDIntGet::Type());
    CPPUNIT_ASSERT_MESSAGE("Fail to get data out Get iface for Cp1 from cache", doutpget != 0);
    CPPUNIT_ASSERT_MESSAGE("Fail to get value of data iface for Cp1 from cache", doutpget->Value() == 3);

    delete iEnv;
}


// Test of disconnecting and then connecting to another point. Ref UC_015
void Ut_conn::test_Reconn()
{
    printf("\n === Test of reconnecting edge\n");

    iEnv = new Env("ut_conn_1.xml", "ut_conn_1.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ConstructSystem();
    Elem* root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
 
    // Delete v1
    ChromoNode mutn = root->AppendMutation(ENt_Rm);
    mutn.SetAttr(ENa_MutNode, "./v1");
    root->Mutate();
    // Verify the connection pair is disconnected
    MElem* ev2 = root->GetNode("./v2");
    MVert* mv2 = ev2->GetObj(mv2);
    int pnum = mv2->PairsCount();
    CPPUNIT_ASSERT_MESSAGE("Wrong number of v2 pairs after disconnection", pnum == 0);
    // Verify edges point previously connected to v1 is disconnected
    MElem* ee1 = root->GetNode("./e1");
    MEdge* me1 = ee1->GetObj(me1);
    MVert* p1 = me1->Point1();
    CPPUNIT_ASSERT_MESSAGE("Edges Point1 is not disconnected", p1 == 0);
    // Delete v3
    mutn = root->AppendMutation(ENt_Rm);
    mutn.SetAttr(ENa_MutNode, "./v3");
    root->Mutate();
    // Verify the connection pair is disconnected
    MElem* ev5 = root->GetNode("./v5");
    MVert* mv5 = ev5->GetObj(mv5);
    int pnum5 = mv5->PairsCount();
    CPPUNIT_ASSERT_MESSAGE("Wrong number of v5 pairs after disconnection", pnum5 == 0);
    // Verify edges point previously connected to v1 is disconnected
    MElem* ee2 = root->GetNode("./e2");
    MEdge* me2 = ee2->GetObj(me2);
    MVert* p2_2 = me2->Point2();
    CPPUNIT_ASSERT_MESSAGE("Edges Point2 is not disconnected within e2", p2_2 == 0);
}

// Test of double connection and then disconnecting one of them. 
void Ut_conn::test_Conn2()
{
    printf("\n === Test of reconnecting edge\n");

    iEnv = new Env("ut_conn_2.xml", "ut_conn_2.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ConstructSystem();
    Elem* root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
 
   // Disconnect one point of edge e2
    MElem* e2 = root->GetNode("./e2");
    ChromoNode mutn = e2->AppendMutation(ENt_Cont);
    mutn.SetAttr(ENa_MutNode, "./P1");
    mutn.SetAttr(ENa_Ref, "");
    e2->Mutate();
    // Verify that v1 and v2 are still connected
    MElem* ev1 = root->GetNode("./v1");
    MVert* mv1 = ev1->GetObj(mv1);
    int pnum1 = mv1->PairsCount();
    CPPUNIT_ASSERT_MESSAGE("Wrong number of v1 pairs after e2 disconnection", pnum1 == 1);
}
