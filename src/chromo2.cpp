
#include <iostream>
#include <fstream>
#include <sstream>

#include "chromo2.h"
#include "chromox.h"

/** @brief Chromo grammar terminals
 * */
const string C2MdlCtxNode::KT_Target = "<";
const string C2MdlCtxNode::KT_Node = "-";
const string C2MdlCtxNode::KT_Namespace = "@";

const char KT_MutSeparator = ';';
const char KT_ChromoStart = '{';
const char KT_ChromoEnd = '}';

/** @brief Mutation symbols */
const string KMS_Add = ":";

static const string KR_CREATE = ":";

/** @brief Chromo spec parsing errors
 * */

static const string KPE_UnexpChrEnd = "Unexpected end of chromo";
static const string KPE_WrongMutLexNum = "Wrong number mutation lexems";

using namespace std;

static const map<TNodeType, string> KNtToR = {
    {ENt_Node, KR_CREATE}
};

C2MdlCtxNode::C2MdlCtxNode(const string& aType, const string& aValue): mType(aType), mValue(aValue)
{
}



C2MdlNode::C2MdlNode()
{
}

C2MdlNode::C2MdlNode(const C2MdlNode* aOwner): mOwner(aOwner)
{
}
void C2MdlNode::AddContext(const string& aType, const string& aValue)
{
    mContext.push_back(C2MdlCtxNode(aType, aValue));
}

C2MdlNode Chromo2Mdl::CreateNodeMut(const THandle& aHandle, const C2MdlNode& aOwner, TNodeType aR, TNodeAttr aP, TNodeAttr aQ)
{
    C2MdlNode node(&aOwner);
    node.mMut.mR = KNtToR.at(aR);
    node.mMut.mP = mMdlX->GetAttr(aHandle, aP);
    node.mMut.mQ = mMdlX->GetAttr(aHandle, aQ);
    return node;
}

C2MdlNode Chromo2Mdl::CreateNodeChr(const THandle& aHandle, const C2MdlNode& aOwner)
{
    C2MdlNode node(&aOwner);
    THandle handle = mMdlX->GetFirstChild(aHandle);
    do {
	HandleXNode(handle, node);
	handle = mMdlX->Next(handle);
    } while (handle != NULL);
    return node;
}

Chromo2Mdl::Chromo2Mdl(): Base(), mMdlX(NULL)
{
    mMdlX = new ChromoMdlX(); 
}

Chromo2Mdl::~Chromo2Mdl()
{
    if (mMdlX != NULL) {
	delete mMdlX;	
    }
}

TNodeType Chromo2Mdl::GetType(const THandle& aHandle)
{
    TNodeType res = ENt_Unknown;
    C2MdlNode* node = aHandle.Data(node);
    string rel = node->mMut.mR;
    if (rel == KMS_Add) {
	res = ENt_Node;
    }
    return res;
}

THandle Chromo2Mdl::Root(const THandle& aHandle)
{
}

THandle Chromo2Mdl::Parent(const THandle& aHandle)
{
}

THandle Chromo2Mdl::Next(const THandle& aHandle, TNodeType aType)
{
    C2MdlNode* node = aHandle.Data(node);
}

THandle Chromo2Mdl::Prev(const THandle& aHandle, TNodeType aType)
{
}

THandle Chromo2Mdl::GetFirstChild(const THandle& aHandle, TNodeType aType)
{
    C2MdlNode* node = aHandle.Data(node);
    C2MdlNode *comp = NULL;
    if (node->mChromo.size() > 0) {
	comp = &(node->mChromo.at(0));
    }
    return comp;
}

THandle Chromo2Mdl::GetLastChild(const THandle& aHandle, TNodeType aType)
{
}

string Chromo2Mdl::GetAttr(const THandle& aHandle, TNodeAttr aAttr) const
{
    C2MdlNode* node = aHandle.Data(node);
    string res;
    string rel = node->mMut.mR;
    if (rel == KMS_Add) {
	if (aAttr == ENa_Id) {
	    res = node->mMut.mP;
	} else if (aAttr == ENa_Parent) {
	    res = node->mMut.mQ;
	}
    }
}

void  Chromo2Mdl::GetAttr(const THandle& aNode, TNodeAttr aType, TInt& aVal) const
{
}

TBool Chromo2Mdl::AttrExists(const THandle& aHandle, TNodeAttr aAttr) const 
{
}

THandle Chromo2Mdl::AddChild(const THandle& aParent, TNodeType aType)
{
}

THandle Chromo2Mdl::AddChild(const THandle& aParent, const THandle& aHandle, TBool aCopy, TBool aRecursively)
{
}

THandle Chromo2Mdl::AddNext(const THandle& aPrev, const THandle& aHandle, TBool aCopy)
{
}

THandle Chromo2Mdl::AddNext(const THandle& aPrev, TNodeType aNode)
{
}

THandle Chromo2Mdl::AddPrev(const THandle& aNext, const THandle& aHandle, TBool aCopy)
{
}

void Chromo2Mdl::RmChild(const THandle& aParent, const THandle& aChild, TBool aDeattachOnly)
{
}

void Chromo2Mdl::Rm(const THandle& aHandle)
{
}

void Chromo2Mdl::SetAttr(const THandle& aNode, TNodeAttr aType, const char* aVal)
{
}

void Chromo2Mdl::SetAttr(const THandle& aNode, TNodeAttr aType, TInt aVal)
{
}

void Chromo2Mdl::RmAttr(const THandle& aNode, TNodeAttr aType)
{
}

void Chromo2Mdl::Dump(const THandle& aNode, MLogRec* aLogRec)
{
}

TBool Chromo2Mdl::ToString(const THandle& aNode, string& aString) const
{
}

void Chromo2Mdl::Save(const string& aFileName) const
{
}

THandle Chromo2Mdl::Find(const THandle& aHandle, const string& aUri)
{
}

TInt Chromo2Mdl::GetOrder(const THandle& aHandle, TBool aTree) const
{
}

void Chromo2Mdl::DeOrder(const THandle& aHandle)
{
}

TInt Chromo2Mdl::GetLineId(const THandle& aHandle) const
{
}



int Chromo2Mdl::GetAttrInt(void *aHandle, const char *aName)
{
}

THandle Chromo2Mdl::SetFromFile(const string& aFileName)
{
    /*
    THandle handle = mMdlX->SetFromFile(aFileName);
    if (handle != NULL && mMdlX->GetType(handle) == ENt_Node) {
	// Root, evolve the chromo
	HandleXNode(handle, mRoot);
    }
    */
    ifstream is(aFileName, std::ifstream::in);
    if ( (is.rdstate() & ifstream::failbit ) == 0 ) {
	is.seekg(0, is.beg);
	streampos beg = is.tellg();
	is.seekg(0, is.end);
	streampos end = is.tellg();
	ParseChromo(is, beg, end, mRoot);
	DumpMnode(mRoot, 0);
    }
    return &mRoot;
}

THandle Chromo2Mdl::Set(const string& aUri)
{
}

THandle Chromo2Mdl::SetFromSpec(const string& aSpec)
{
}

THandle Chromo2Mdl::Set(const THandle& aHandle)
{
}

THandle Chromo2Mdl::Init(TNodeType aRootType)
{
}

void Chromo2Mdl::Reset()
{
}

void Chromo2Mdl::HandleXNodeCtx(C2MdlNode& aMdlNode, const THandle& aHandle)
{
    if (mMdlX->AttrExists(aHandle, ENa_Targ)) {
	string targ = mMdlX->GetAttr(aHandle, ENa_Targ);
	aMdlNode.AddContext(C2MdlCtxNode::KT_Target, targ);
    }
    if (mMdlX->AttrExists(aHandle, ENa_MutNode)) {
	string mnode = mMdlX->GetAttr(aHandle, ENa_MutNode);
	aMdlNode.AddContext(C2MdlCtxNode::KT_Node, mnode);
    }
}

void Chromo2Mdl::HandleXNode(const THandle& aHandle, C2MdlNode& aOwner)
{
    TNodeType xntype = mMdlX->GetType(aHandle);
    // Mutation
    if (xntype == ENt_Node) {
	C2MdlNode node = CreateNodeMut(aHandle, aOwner, ENt_Node, ENa_Id, ENa_Parent);
	HandleXNodeCtx(node, aHandle);
	aOwner.mChromo.push_back(node);
	THandle child_h = mMdlX->GetFirstChild(aHandle);
	if (child_h != NULL) {
	    // Chromo node
	    C2MdlNode chromo_node = CreateNodeChr(aHandle, aOwner);
	    // Add node name as target context for chromo
	    string name = mMdlX->GetAttr(aHandle, ENa_Id);
	    chromo_node.AddContext(C2MdlCtxNode::KT_Target, name);
	    aOwner.mChromo.push_back(chromo_node);
	}
    }
}

void Chromo2Mdl::ParseChromo(std::istream& aIs, streampos aStart, streampos aEnd, C2MdlNode& aMnode)
{
#if 0
    string lexeme;
    char c = aIs.get();
    while (aIs.good()) {
	cout << c;
	if (c == ' ' || c == '\n') {
	    mLex.push_back(lexeme);
	    lexeme.clear();
	} else {
	    lexeme.push_back(c);
	}
	c = aIs.get();
    };
#endif

#if 0
    // Variant #2

    string cnode;
    char c = aIs.get();
    int cscount = 0;
    while (aIs.good()) {
	if (c == KT_MutSeparator) {
	    ParseMut(cnode);
	    cnode.erase();
	} else if (c == KT_ChromoStart) {
	    cscount++;
	    c = aIs.get();
	    while (aIs.good()) {
		if (c == KT_ChromoStart) {
		    cscount++;
		    cnode.push_back(c);
		} else if (c == KT_ChromoEnd && --cscount == 0) {
		    ParseChromo(cnode);
		    cnode.erase();
		    break;
		} else {
		    cnode.push_back(c);
		}
		c = aIs.get();
	    }
	} else if (c == KT_ChromoEnd && --cscount == 0) {
	    ParseChromo(cnode);
	    cnode.erase();
	} else {
	    cnode.push_back(c);
	}
	c = aIs.get();
    };
#endif

    DumpIsFrag(aIs, aStart, aEnd);

    int cscount = 0;
    streampos beg = aStart;
    streampos end = aEnd;
    streampos nbeg = aStart;
    aIs.seekg(aStart, aIs.beg);
    streampos pos = aIs.tellg();
    while (pos != aEnd) {
	char c = aIs.get();
	if (c == KT_MutSeparator) {
	    streampos cur = aIs.tellg();
	    aIs.seekg(-1, aIs.cur);
	    streampos mend = aIs.tellg();
	    ParseCnodeMut(aIs, nbeg, mend, aMnode);
	    aIs.seekg(cur, aIs.beg);
	    nbeg = aIs.tellg();
	} else if (c == KT_ChromoStart) {
	    cscount++;
	    while (pos != aEnd) {
		c = aIs.get();
		if (c == KT_ChromoStart) {
		    cscount++;
		} else if (c == KT_ChromoEnd) {
		    cscount--;
		    if (cscount == 0) {
			end = aIs.tellg();
			ParseCnodeChromo(aIs, nbeg, end, aMnode);
			nbeg = end;
			break;
		    } else if (cscount < 0) {
			mErr.Set(pos, KPE_UnexpChrEnd);
			break;
		    }
		}
		pos = aIs.tellg();
	    }
	}
	pos = aIs.tellg();
    }
}

void Chromo2Mdl::ParseCnodeMut(istream& aIs, streampos aBeg, streampos aEnd, C2MdlNode& aMnode)
{
    cout << "Parsing mutation:" << endl;
    DumpIsFrag(aIs, aBeg, aEnd);

    // Get lexems
    vector<string> lexs;   //!< Lexems
    string lexeme;
    aIs.seekg(aBeg, aIs.beg);
    streampos pos = aIs.tellg();
    while (pos != aEnd) {
	char c = aIs.get();
	if (c == ' ' || c == '\n') {
	    lexs.push_back(lexeme);
	    lexeme.clear();
	} else {
	    lexeme.push_back(c);
	}
	pos = aIs.tellg();
    }
    lexs.push_back(lexeme);

    if (lexs.size() >= 3) {
	string mss = lexs.at(lexs.size() - 2);
	// Adding nodel node of chromo type
	C2MdlNode mnode;  
	if (mss == KMS_Add) {
	    mnode.mMut.mR = mss;
	    mnode.mMut.mP = lexs.at(lexs.size() - 3);
	    mnode.mMut.mQ = lexs.at(lexs.size() - 1);
	}
	aMnode.mChromo.push_back(mnode);
    } else {
	mErr.Set(pos, KPE_WrongMutLexNum);
    }
}

void Chromo2Mdl::ParseCnodeChromo(istream& aIs, streampos aStart, streampos aEnd, C2MdlNode& aMnode)
{
    cout << "Parsing chromo:" << endl;
    DumpIsFrag(aIs, aStart, aEnd);

    // Adding nodel node of chromo type
    C2MdlNode mnode;  
    aIs.seekg(aStart, aIs.beg);
    streampos pos = aIs.tellg();
    while (pos != aEnd) {
	char c = aIs.get();
	if (c == KT_ChromoStart) {
	    pos = aIs.tellg();
	    break;
	}
	pos = aIs.tellg();
    }
    __ASSERT(pos != aEnd);
    aIs.seekg(aEnd, aIs.beg);
    aIs.seekg(-1, aIs.cur);
    streampos cend = aIs.tellg();
    ParseChromo(aIs, pos, cend, mnode);
    aMnode.mChromo.push_back(mnode);
}

void Chromo2Mdl::DumpIsFrag(istream& aIs, streampos aStart, streampos aEnd)
{
    streampos curpos = aIs.tellg();
    streampos pos = aStart;
    aIs.seekg(pos, aIs.beg);
    while (pos != aEnd) {
	char c = aIs.get();
	cout << c;
	pos = aIs.tellg();
    };
    cout << endl;
    aIs.seekg(curpos, aIs.beg);
}

static void Offset(int aLevel)
{
    for (int i = 0; i < aLevel; i++)  cout << "  ";
}

void Chromo2Mdl::DumpMnode(const C2MdlNode& aNode, int aLevel) const
{
    if (!aNode.mMut.mR.empty()) {
	Offset(aLevel); cout << aNode.mMut.mP << " " << aNode.mMut.mR << " " << aNode.mMut.mQ << ";" << endl;
    }
    int cnum = aNode.mChromo.size();
    if (cnum > 0) {
	Offset(aLevel); cout << "{" << endl;
	for (vector<C2MdlNode>::const_iterator it = aNode.mChromo.begin(); it != aNode.mChromo.end(); it++) {
	    const C2MdlNode& node = *it;
	    DumpMnode(node, aLevel + 1);
	}
	Offset(aLevel); cout << "}" << endl;
    }
}



Chromo2::Chromo2(): mRootNode(mMdl, THandle())
{
}

Chromo2::Chromo2(const Chromo2& aSrc)
{
    Set(aSrc.Root());
}

void Chromo2::SetFromFile(const string& aFileName)
{
    THandle root = mMdl.SetFromFile(aFileName);
    mRootNode = ChromoNode(mMdl, root);
}

TBool Chromo2::Set(const string& aUri)
{
    TBool res = EFalse;
    THandle root = mMdl.Set(aUri);
    if (root != NULL) {
	mRootNode = ChromoNode(mMdl, root);
	res = ETrue;
    }
    return res;
}

TBool Chromo2::SetFromSpec(const string& aSpec)
{
    TBool res = EFalse;
    THandle root = mMdl.SetFromSpec(aSpec);
    if (root != NULL) {
	mRootNode = ChromoNode(mMdl, root);
	res = ETrue;
    }
    return res;

}

void Chromo2::Set(const ChromoNode& aRoot)
{
    THandle root = mMdl.Set(aRoot.Handle());
    mRootNode = ChromoNode(mMdl, root);
}

Chromo2::~Chromo2()
{
}

ChromoNode& Chromo2::Root()
{
    return mRootNode;
}

const ChromoNode& Chromo2::Root() const
{
    return mRootNode;
}

void Chromo2::Reset()
{
    mMdl.Reset();
}

void Chromo2::Init(TNodeType aRootType)
{
    THandle root = mMdl.Init(aRootType);
    mRootNode = ChromoNode(mMdl, root);
}

void Chromo2::Save(const string& aFileName) const
{
    // We cannot simply save the doc (mMdl.iDoc) because it will save not only root node but
    // also adjacent nodes. So we need to create new model and add to doc only one separated root
    Chromo2Mdl mdl;
    mdl.Set(mRootNode.Handle());
    mdl.Save(aFileName);
}

ChromoNode Chromo2::CreateNode(const THandle& aHandle)
{
    return ChromoNode(mMdl, aHandle);
}

void Chromo2::ReduceToSelection(const ChromoNode& aSelNode)
{
    ChromoNode sel = aSelNode;
    ChromoNode prnt = *sel.Parent();
    while (prnt != ChromoNode()) {
	prnt.ReduceToSelection(sel);
	sel = prnt;
	prnt = *sel.Parent();
    }
}

TBool Chromo2::GetSpec(string& aSpec)
{
    return mMdl.ToString(mRootNode.Handle(), aSpec);
}

bool Chromo2::IsError() const
{
    return !mMdl.Error().mText.empty();
}

const CError& Chromo2::Error() const
{
    return mMdl.Error();
}
