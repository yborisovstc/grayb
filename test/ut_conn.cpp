#include <env.h>
#include <stdlib.h>
#include <elem.h>
#include <mvert.h>
#include <mvertp.h>
#include <mdata.h>
#include <medge.h>
#include <msyst.h>
#include <prov.h>
#include <vertp.h>


#include <cppunit/extensions/HelperMacros.h>

class MTestIface1: public MIfaceStub {
    public:
	static const char* Type() { return "MTestIface1";}
	virtual string Iface1_Test() = 0;
	string Uid() const override { return Type();}
};

class MTestIface2: public MIfaceStub {
    public:
	static const char* Type() { return "MTestIface2";}
	virtual string Iface2_Test() = 0;
	string Uid() const override { return Type();}
};

/**
 * Test agent providing the ifaces
 */
class TstAgt: public Elem, public MAgent
{
    public:
	class TestIface1: public MTestIface1 {
	    public:
		TestIface1(TstAgt& aHost): mHost(aHost) {}
		virtual ~TestIface1() {}
		virtual string Iface1_Test() override {
		    string res = "Error";
		    return res;
		}
	    private: TstAgt& mHost;
	};
	class TestIface2: public MTestIface2 {
	    public:
		TestIface2(TstAgt& aHost): mHost(aHost) {}
		virtual ~TestIface2() {}
		virtual string Iface2_Test() override {
		    string res = "Error";
		    MElem* owner = mHost.GetMan();
		    if (owner) {
			MSyst* ms = owner->GetObj(ms);
			if (ms) {
			    string kk = mHost.ContentKey(Vertp::KContent_Connpoints, "Cp1");
			    MVertp::TPairsEr pairs = ms->GetPairsForCp(kk);
			    if (pairs.first != pairs.second) {
				MVertp* pair = pairs.first->second;
				MElem* epair = pair->GetObj(epair);
				MIface* ifr = epair->GetSIfi(MTestIface1::Type(), owner);
				if (ifr) {
				    MTestIface1* ti = dynamic_cast<MTestIface1*>(ifr);
				    __ASSERT(ti != NULL);
				    res = ti->Uid();
				}
			    }
			}
		    }
		    return res;

		}
	    private: TstAgt& mHost;
	};
    public:
	static const char* Type() { return "TstAgt";};
	static string PEType() { return Elem::PEType() + GUri::KParentSep + Type(); }
	TstAgt(const string& aName = string(), MElem* aMan = NULL, MEnv* aEnv = NULL): Elem(aName, aMan, aEnv) {
	    SetParent(Type()); mIface1 = new TestIface1(*this); mIface2 = new TestIface2(*this);}
	TstAgt(MElem* aMan = NULL, MEnv* aEnv = NULL): Elem(Type(), aMan, aEnv) {
	    SetParent(Elem::PEType()); mIface1 = new TestIface1(*this); mIface2 = new TestIface2(*this);}
	virtual ~TstAgt() { delete mIface1; delete mIface2;}
	virtual MIface* DoGetObj(const char *aName) {
	    if (strcmp(aName, MAgent::Type()) == 0) return  dynamic_cast<MAgent*>(this);
	    else if (strcmp(aName, MTestIface1::Type()) == 0) return mIface1;
	    else if (strcmp(aName, MTestIface2::Type()) == 0) return mIface2;
	    return Elem::DoGetObj(aName);
	}
	// From MAgent
	virtual MIface* MAgent_DoGetIface(const string& aName) override { MIface* res = NULL; if (aName == MElem::Type()) res = dynamic_cast<MElem*>(this); return res; }
    private:
	MTestIface1* mIface1 = nullptr;
	MTestIface2* mIface2 = nullptr;
};

/**
 * Agent provider for test agents
 */
class TstProv: public GProvider
{
    public:
	TstProv(const string& aName, MEnv* aEnv): GProvider(aName, aEnv) {}
	virtual ~TstProv() {}
	// From MProvider
	virtual Elem* CreateNode(const string& aType, const string& aName, MElem* aMan, MEnv* aEnv);
	virtual Elem* GetNode(const string& aUri);
	virtual void AppendNodesInfo(vector<string>& aInfo);
	virtual const string& ModulesPath() const;
	static string GetParentName(const string& aUri);
};

string TstProv::GetParentName(const string& aUri)
{
    string res;
    size_t pos = aUri.find_last_of(GUri::KParentSep);
    res = aUri.substr(pos + 1);
    return res;
}

Elem* TstProv::CreateNode(const string& aType, const string& aName, MElem* aMan, MEnv* aEnv)
{
    Elem* res = NULL;
    if (aType.compare(TstAgt::Type()) == 0) {
	res = new TstAgt(aName, aMan, aEnv);
    }
    if (res != NULL) {
	Elem* parent = GetNode(aType);
	if (parent != NULL) {
	    parent->AppendChild(res);
	}
    }

    return res;
}

Elem* TstProv::GetNode(const string& aUri)
{
    Elem* res = NULL;
    if (iReg.count(aUri) > 0) {
	res = iReg.at(aUri);
    } else {
	Elem* parent = NULL;
	if (aUri.compare(TstAgt::Type()) == 0) {
	    //parent = GetNode("Elem");
	    res = new TstAgt(NULL, iEnv);
	}
	if (res != NULL) {
	    if (parent == NULL) {
		string pname = GetParentName(res->PName());
		parent = iEnv->Provider()->GetNode(pname);
	    }
	    if (parent != NULL) {
		parent->AppendChild(res);
	    }
	    iReg.insert(TRegVal(aUri, res));
	}
    }
    return res;
}

void TstProv::AppendNodesInfo(vector<string>& aInfo) {
}

const string& TstProv::ModulesPath() const {
}



/**
 * This test is for checking the functionality of connections
 */
class Ut_conn : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(Ut_conn);
    CPPUNIT_TEST(test_Vertp);
    CPPUNIT_TEST(test_Systp);
    CPPUNIT_TEST(test_Systp2);
    CPPUNIT_TEST(test_SystpSock);
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
    void test_Systp2();
    void test_SystpSock();
    void test_Sock();
    void test_Sock2();
    void test_Reconn();
    void test_Conn2();
    private:
    Env* iEnv;
    TstProv* mProv;
};

CPPUNIT_TEST_SUITE_REGISTRATION( Ut_conn );
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
    MElem* root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    // Verify that v1 and v2 are connected
    MElem* ev1 = root->GetNode("./test/v1");
    MVertp* mv1 = ev1->GetObj(mv1);
    MElem* ev2 = root->GetNode("./test/v2");
    MVertp* mv2 = ev2->GetObj(mv2);
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
//  PIA. Simple conn point, compatibility checking, iface resolution from pairs.
void Ut_conn::test_Systp()
{
    printf("\n === Test#1 of Systp connection: Simple conn point, compatibility checking, iface resolution from pairs\n");

    iEnv = new Env("ut_conn_systp_1.xml", "ut_conn_systp_1.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ConstructSystem();
    MElem* root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    // Verify that s1 and s2 are connected
    MElem* es1 = root->GetNode("./test/s1");
    MVertp* mv1 = es1->GetObj(mv1);
    MElem* es2 = root->GetNode("./test/s2");
    MVertp* mv2 = es2->GetObj(mv2);
    int pnum1 = mv1->PairsCount();
    int pnum2 = mv2->PairsCount();
    bool v2ispair = mv1->IsPair(mv2);
    CPPUNIT_ASSERT_MESSAGE("v1 and v2 aren't connected", pnum1 == 1 && pnum2 && v2ispair);
    // Test iface resolution
    MIface* iface = es1->GetSIfi("MVertp", TICacheRCtx(es2));
    CPPUNIT_ASSERT_MESSAGE("Incorrectly resolver MVertp iface", iface != nullptr && iface->Uid() == "./test/s1%MVertp");
    // Disconnect one point of edge e2
    ChromoNode mutn = root->AppendMutation(ENt_Cont);
    mutn.SetAttr(ENa_Targ, "./test");
    mutn.SetAttr(ENa_Id, "Edges.E1.P1");
    mutn.SetAttr(ENa_MutVal, "");
    root->Mutate();
    // Verify that v1 and v2 are disconnected
    es1 = root->GetNode("./test/s1");
    mv1 = es1->GetObj(mv1);
    pnum1 = mv1->PairsCount();
    CPPUNIT_ASSERT_MESSAGE("Wrong number of v1 pairs after e2 disconnection", pnum1 == 0);
    // Trying connect to incompatible connpoint
    mutn = root->AppendMutation(ENt_Cont);
    mutn.SetAttr(ENa_Targ, "./test");
    mutn.SetAttr(ENa_Id, "Edges.E2");
    mutn.SetAttr(ENa_MutVal, "{P1:'/Root/test/s1~ConnPoints.Cp2' P2:'/Root/test/s2~ConnPoints.Cp1'}");
    root->Mutate();
    // Verify that v1 and v2 are disconnected
    es1 = root->GetNode("./test/s1");
    mv1 = es1->GetObj(mv1);
    pnum1 = mv1->PairsCount();
    CPPUNIT_ASSERT_MESSAGE("s1 connects s2 even the cps are incompatible", pnum1 == 0);
}


// Test of Systp connection
//  Extension
void Ut_conn::test_Systp2()
{
    printf("\n === Test#2 of Systp connection: Extensions\n");

    iEnv = new Env("ut_conn_systp_2.xml", "ut_conn_systp_2.txt");
    mProv = new TstProv("TstProv", iEnv);
    iEnv->AddProvider(mProv);
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ConstructSystem();
    MElem* root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    // Trying connect s2 to s1 ext
    ChromoNode mutn = root->AppendMutation(ENt_Cont);
    mutn.SetAttr(ENa_Targ, "./test");
    mutn.SetAttr(ENa_Id, "Edges.E2");
    mutn.SetAttr(ENa_MutVal, "{P1:'/Root/test/s1~ConnPoints.Ext1' P2:'/Root/test/s2~ConnPoints.Cp1'}");
    root->Mutate();
    MElem* es1 = root->GetNode("./test/s1");
    MVertp* mv1 = es1->GetObj(mv1);
    MElem* es2 = root->GetNode("./test/s2");
    MVertp* mv2 = es2->GetObj(mv2);
    int pnum1 = mv1->PairsCount();
    int pnum2 = mv2->PairsCount();
    bool v2ispair = mv1->IsPair(mv2);
    // s1 to have 2 pairs: s1_1 (internal), and s2
    // s2 to have only one pair s1
    CPPUNIT_ASSERT_MESSAGE("v1 and v2 aren't connected", pnum1 == 2 && pnum2 == 1 && v2ispair);
    // 
    // Verifying iface resolution via connection
    MElem* es2_1 = root->GetNode("./test/s2/s2_1");
    CPPUNIT_ASSERT_MESSAGE("Cannot get es2_1", es2_1 != NULL);
    MTestIface2* es2_1t = es2_1->GetObj(es2_1t);
    CPPUNIT_ASSERT_MESSAGE("Cannot get es2_1 MTestIface2", es2_1 != NULL);
    //string ck = Vertp::KContent_Connpoints;
    string t2res = es2_1t->Iface2_Test();
    CPPUNIT_ASSERT_MESSAGE("Iface MIface1 is not resolved", t2res == "MTestIface1");

    iEnv->RemoveProvider(mProv);
    delete mProv;
    delete iEnv;
}

// Test of Systp connection: Socket
void Ut_conn::test_SystpSock()
{
    printf("\n === Test#2 of Systp connection: Socket\n");

    iEnv = new Env("ut_conn_systp_sock.xml", "ut_conn_systp_sock.txt");
    mProv = new TstProv("TstProv", iEnv);
    iEnv->AddProvider(mProv);
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ConstructSystem();
    MElem* root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root != 0);
    // Trying connect s2 to s1 ext
    ChromoNode mutn = root->AppendMutation(ENt_Cont);
    mutn.SetAttr(ENa_Targ, "./test");
    mutn.SetAttr(ENa_Id, "Edges.E2");
    mutn.SetAttr(ENa_MutVal, "{P1:'/Root/test/s1~ConnPoints.S1' P2:'/Root/test/s2~ConnPoints.S1'}");
    root->Mutate();
    MElem* es1 = root->GetNode("./test/s1");
    MVertp* mv1 = es1->GetObj(mv1);
    MElem* es2 = root->GetNode("./test/s2");
    MVertp* mv2 = es2->GetObj(mv2);
    int pnum1 = mv1->PairsCount();
    int pnum2 = mv2->PairsCount();
    bool v2ispair = mv1->IsPair(mv2);
    // Verifying connection s1 - s2
    // s1 to have 3 pairs: s1_1, s1_2 (internal), and s2
    // s2 to have 3 pairs: s2_1, s2_2 (internal), and s1
    CPPUNIT_ASSERT_MESSAGE("v1 and v2 aren't connected", pnum1 == 3 && pnum2 == 3 && v2ispair);
    // 
    // Verifying iface resolution via connection
    MElem* es2_1_1 = root->GetNode("./test/s2/s2_1/s2_1_1");
    CPPUNIT_ASSERT_MESSAGE("Cannot get es2_1_1", es2_1_1 != NULL);
    MTestIface2* es2_1_1t = es2_1_1->GetObj(es2_1_1t);
    CPPUNIT_ASSERT_MESSAGE("Cannot get es2_1 MTestIface2", es2_1_1t != NULL);
    //string ck = Vertp::KContent_Connpoints;
    string t2res = es2_1_1t->Iface2_Test();
    CPPUNIT_ASSERT_MESSAGE("Iface MIface1 is not resolved", t2res == "MTestIface1");

    iEnv->RemoveProvider(mProv);
    delete mProv;
    delete iEnv;
}

void Ut_conn::test_Sock()
{
    printf("\n === Test of connecting of sockets\n");

    iEnv = new Env("ut_conn_sock.xml", "ut_conn_sock.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ImpsMgr()->ResetImportsPaths();
    iEnv->ImpsMgr()->AddImportsPaths("../modules");
    iEnv->ConstructSystem();
    MElem* root = iEnv->Root();
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
    MElem* root = iEnv->Root();
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
    MElem* root = iEnv->Root();
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
    MElem* root = iEnv->Root();
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
