#include <env.h>
#include <stdlib.h>
#include <elem.h>
#include <mvert.h>
#include <mdata.h>

#include <cppunit/extensions/HelperMacros.h>

/*
 * This test is for checking the functionality of connections
 */


class Ut_conn : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(Ut_conn);
    CPPUNIT_TEST(test_Sock);
    CPPUNIT_TEST(test_Sock2);
    CPPUNIT_TEST_SUITE_END();
public:
    virtual void setUp();
    virtual void tearDown();
private:
    void test_Sock();
    void test_Sock2();
private:
    Env* iEnv;
};

CPPUNIT_TEST_SUITE_REGISTRATION( Ut_conn );


void Ut_conn::setUp()
{
}

void Ut_conn::tearDown()
{
//    delete iEnv;
    CPPUNIT_ASSERT_EQUAL_MESSAGE("tearDown", 0, 0);
}

void Ut_conn::test_Sock()
{
    printf("\n === Test of connecting of sockets\n");

    iEnv = new Env("Env", "ut_conn_sock.xml", "ut_conn_sock.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ConstructSystem();
    Elem* root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    Elem* doutp = root->GetNode("test/L1/Cp1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get L1 Cp1", doutp != 0);
    //MDIntGet* doutpget = doutp->GetObj(doutpget);
    MDIntGet* doutpget = (MDIntGet*) doutp->GetSIfi(MDIntGet::Type());
    CPPUNIT_ASSERT_MESSAGE("Fail to get data out Get iface", doutpget != 0);
    CPPUNIT_ASSERT_MESSAGE("Fail to get value of data iface", doutpget->Value() == 3);

    doutp = root->GetNode("test/L1/Cp2");
    CPPUNIT_ASSERT_MESSAGE("Fail to get L1 Cp2", doutp != 0);
    doutpget = (MDIntGet*) doutp->GetSIfi(MDIntGet::Type());
    CPPUNIT_ASSERT_MESSAGE("Fail to get data out Get iface for Cp2", doutpget != 0);
    CPPUNIT_ASSERT_MESSAGE("Fail to get value of data iface for Cp2", doutpget->Value() == 1);

    doutp = root->GetNode("test/L1/Cp3");
    CPPUNIT_ASSERT_MESSAGE("Fail to get L1 Cp3", doutp != 0);
    doutpget = (MDIntGet*) doutp->GetSIfi(MDIntGet::Type());
    CPPUNIT_ASSERT_MESSAGE("Fail to get data out Get iface for Cp3", doutpget != 0);
    CPPUNIT_ASSERT_MESSAGE("Fail to get value of data iface for Cp3", doutpget->Value() == 20);

    delete iEnv;
}


void Ut_conn::test_Sock2()
{
    printf("\n === Test of extending of sockets\n");

    iEnv = new Env("Env", "ut_conn_sock2.xml", "ut_conn_sock2.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ConstructSystem();
    Elem* root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);

    Elem* doutp = root->GetNode("test/L1/Cp1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get L1 Cp1", doutp != 0);
    MDIntGet* doutpget = (MDIntGet*) doutp->GetSIfi(MDIntGet::Type());
    CPPUNIT_ASSERT_MESSAGE("Fail to get data out Get iface for Cp1", doutpget != 0);
    CPPUNIT_ASSERT_MESSAGE("Fail to get value of data iface for Cp1", doutpget->Value() == 3);

    doutp = root->GetNode("test/L1/Cp2");
    CPPUNIT_ASSERT_MESSAGE("Fail to get L1 Cp2", doutp != 0);
    doutpget = (MDIntGet*) doutp->GetSIfi(MDIntGet::Type());
    CPPUNIT_ASSERT_MESSAGE("Fail to get data out Get iface for Cp2", doutpget != 0);
    CPPUNIT_ASSERT_MESSAGE("Fail to get value of data iface for Cp2", doutpget->Value() == 1);

    doutp = root->GetNode("test/L1/Cp3");
    CPPUNIT_ASSERT_MESSAGE("Fail to get L1 Cp3", doutp != 0);
    doutpget = (MDIntGet*) doutp->GetSIfi(MDIntGet::Type());
    CPPUNIT_ASSERT_MESSAGE("Fail to get data out Get iface for Cp3", doutpget != 0);
    CPPUNIT_ASSERT_MESSAGE("Fail to get value of data iface for Cp3", doutpget->Value() == 20);

    doutp = root->GetNode("test/L1/Cp1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get L1 Cp1", doutp != 0);
    doutpget = (MDIntGet*) doutp->GetSIfi(MDIntGet::Type());
    CPPUNIT_ASSERT_MESSAGE("Fail to get data out Get iface for Cp1 from cache", doutpget != 0);
    CPPUNIT_ASSERT_MESSAGE("Fail to get value of data iface for Cp1 from cache", doutpget->Value() == 3);



    delete iEnv;
}
