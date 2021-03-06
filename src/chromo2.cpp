
#include <iostream>
#include <fstream>
#include <sstream>

#include "chromo2.h"
#include "chromox.h"

/** @brief Chromo grammar terminals
 * */
const string KT_Target = "<";
const string KT_Node = "-";
const string KT_Namespace = "@";
const string KT_ContextSmb = KT_Target + KT_Namespace;

const char KT_MutSeparator = ';';
const char KT_ChromoStart = '{';
const char KT_ChromoEnd = '}';
const char KT_TextDelim = '"';
const char KT_Escape = '\\';

/** @brief Mutation symbols */
const string KMS_Add = ":";
const string KMS_Cont = "=";
const string KMS_Note = "#";
const string KMS_Rename = "|";
const string KMS_Remove = "!";
const string KMS_Import = "+";
const string KMS_Conn = "~";
const string KMS_Disconn = "!~";

static const vector<string> KDmcMutops = {KMS_Conn};

/** @brief Separators **/
const string KSep = " \t\n\r";
/** @brief Standard control symbols **/
const string K_Ctrls = {KT_TextDelim};

/** @brief Default chromo indentation */
const TInt K_Indent = 4;



/** @brief Default P part */
static const string KT_Default = "$";

static const string KR_CREATE = ":";

/** @brief Chromo spec parsing errors
 * */

static const string KPE_ErrOnReadingSpec = "Error on reading chromo";
static const string KPE_UnexpChrEnd = "Unexpected end of chromo";
static const string KPE_WrongMutLexNum = "Wrong number of mutation lexems";
static const string KPE_WrongCtxType = "Wrong context type";
static const string KPE_MissingEndOfGroup = "Missin end of group";
static const string KPE_UnknownMutation = "Unknown mutation";
static const string KPE_WrongCombMutType = "Wrong type of combined mutation";
static const string KPE_TextNotClosed = "Text block is not closed";
static const string KPE_WrongDepMut = "Wrong depenent mutation, shall be 'node'";
static const string KPE_MissingSegContext = "Missing context of the segment";
static const string KPE_WrongNumOfCtxLex = "Wrong number of context lexemd";
static const string KPE_SmallLexNumInDmcMut = "Too small number of lex in DMC mut";

/** @brief Segment offset when node output */
const int KA_OutOffset = 3;

using namespace std;

/** @brief Mapping for context: attr to ctx type
 * */
const map<TNodeAttr, string> KM_CtxAttToType = {
    {ENa_Targ, KT_Target},
    {ENa_MutNode, KT_Node},
    {ENa_NS, KT_Namespace}
};

/** @brief Mapping for mut: node type to mut rel
 * */
const map<TNodeType, string> KM_MutTypeToRel = {
    {ENt_Node, KMS_Add},
    {ENt_Note, KMS_Note},
    {ENt_Change, KMS_Rename},
    {ENt_Rm, KMS_Remove},
    {ENt_Import, KMS_Import},
    {ENt_Cont, KMS_Cont},
    {ENt_Conn, KMS_Conn},
    {ENt_Disconn, KMS_Disconn}
};

string GetMutRel(TNodeType aType)
{
    return KM_MutTypeToRel.count(aType) == 0 ? string() : KM_MutTypeToRel.at(aType);
}

string GetCtxRel(TNodeAttr aAttr)
{
    return KM_CtxAttToType.count(aAttr) == 0 ? string() : KM_CtxAttToType.at(aAttr);
}

bool IsMutSmb(const string& aSmb)
{
    bool res = false;
    for (auto it : KM_MutTypeToRel) {
	if (it.second == aSmb) {
	    res = true; break;
	}
    }
    return res;
}

/** @brief Checks if given symbol is separator
 * */
bool IsSep(char aSmb)
{
    return  (KSep.find(aSmb) != string::npos);
}

/** @brief Checks if given symbol belongs to the set
 * */
bool IsSymbOf(char aSmb, const string& aSet)
{
    return  (aSet.find(aSmb) != string::npos);
}





static const map<TNodeType, string> KNtToR = {
    {ENt_Node, KR_CREATE}
};

/*
C2MdlCtxNode::C2MdlCtxNode(const string& aType, const string& aValue): mType(aType), mValue(aValue)
{
}
*/



C2MdlNode::C2MdlNode(): mOwner(NULL), mQnode(NULL), mChromoPos(-1)
{
}

C2MdlNode::C2MdlNode(C2MdlNode* aOwner): mOwner(aOwner), mQnode(NULL), mChromoPos(-1)
{
}

C2MdlNode::C2MdlNode(const C2MdlNode& aSrc): mOwner(aSrc.mOwner), mContext(aSrc.mContext), mMut(aSrc.mMut), mChromo(aSrc.mChromo),
    mQnode(NULL), mChromoPos(aSrc.mChromoPos)
{
}

C2MdlNode::C2MdlNode(const C2MdlNode& aSrc, C2MdlNode* aOwner): mOwner(aOwner), mContext(aSrc.mContext), mMut(aSrc.mMut),
    mQnode(NULL), mChromoPos(aSrc.mChromoPos)
{
    if (aSrc.mQnode) {
	mQnode = new C2MdlNode(aSrc.mQnode, this);
    }
    for (TC2MdlNodesCiter it = aSrc.mChromo.begin(); it != aSrc.mChromo.end(); it++) {
	const C2MdlNode& cn = *it;
	mChromo.push_back(C2MdlNode(cn, this));
    }
}

C2MdlNode::~C2MdlNode()
{
    /*
    if (mQnode) {
	delete mQnode;
	mQnode = NULL;
    }
    */
}

void C2MdlNode::CloneFrom(const C2MdlNode& aSrc, bool aChromo)
{
    mContext = aSrc.mContext;
    mMut = aSrc.mMut;
    mChromoPos = aSrc.mChromoPos;
    if (aChromo) {
	for (TC2MdlNodesCiter it = aSrc.mChromo.begin(); it != aSrc.mChromo.end(); it++) {
	    const C2MdlNode& cn = *it;
	    mChromo.push_back(C2MdlNode(cn, this));
	}
    }
}

void C2MdlNode::BindTree(C2MdlNode* aOwner)
{
    mOwner = aOwner;
    for (TC2MdlNodesIter it = mChromo.begin(); it != mChromo.end(); it++) {
	C2MdlNode& cn = *it;
	cn.BindTree(this);
    }
}

void C2MdlNode::AddContext(const string& aType, const string& aValue)
{
    mContext.insert(TC2MdlCtxElem(aType, aValue));
}

void C2MdlNode::RmContext(TNodeAttr aAttr)
{
    __ASSERT(KM_CtxAttToType.count(aAttr) > 0);
    string crel = KM_CtxAttToType.at(aAttr);
    __ASSERT(mContext.count(crel) > 0);
    mContext.erase(crel);
}

C2MdlNode* C2MdlNode::GetNextComp(C2MdlNode* aComp)
{
    C2MdlNode* res = NULL;
    for (TC2MdlNodesIter it = mChromo.begin(); it != mChromo.end(); it++) {
	C2MdlNode* node = &(*it);
	if (node == aComp) {
	    res = (++it == mChromo.end()) ? NULL: &(*it);
	    break;
	}
    }
    return res;
}

C2MdlNode* C2MdlNode::GetPrevComp(C2MdlNode* aComp)
{
    C2MdlNode* res = NULL;
    for (TC2MdlNodesRIter it = mChromo.rbegin(); it != mChromo.rend(); it++) {
	C2MdlNode* node = &(*it);
	if (node == aComp) {
	    res = (++it == mChromo.rend()) ? NULL: &(*it);
	    break;
	}
    }
    return res;
}

bool C2MdlNode::ExistsContextByAttr(TNodeAttr aAttr)
{
    bool res = false;
    if (KM_CtxAttToType.count(aAttr) > 0) {
	string rel = KM_CtxAttToType.at(aAttr);
	res = mContext.count(rel) > 0;
    }
    return res;
}

void C2MdlNode::AddQnode(const C2MdlNode& aNode)
{
    mQnode = new C2MdlNode(aNode);
}


// **** Chromo2Mdl *****


Chromo2Mdl::Chromo2Mdl(): Base()
{
}

Chromo2Mdl::~Chromo2Mdl()
{
}

streampos SeekChar(istream& aIs, streampos aBeg, streampos aEnd, char aChar)
{
    aIs.seekg(aBeg, aIs.beg);
    streampos pos = aIs.tellg();
    while (pos != aEnd) {
	char c = aIs.get();
	if (c == aChar) {
	    pos = aIs.tellg();
	    break;
	}
	pos = aIs.tellg();
    }
    return pos;
}

/** @brief Finds control symbol */
streampos SeekCtrl(istream& aIs, streampos aBeg, streampos aEnd, char aChar)
{
    aIs.seekg(aBeg, aIs.beg);
    streampos pos = aIs.tellg();
    bool text = false;
    while (pos != aEnd) {
	char c = aIs.get();
	if (c == KT_TextDelim) {
	    text = !text;
	}
	if (!text && c == aChar) {
	    pos = aIs.tellg();
	    break;
	}
	pos = aIs.tellg();
    }
    return pos;
}

string GenerateName(const string& aParent, int aPos)
{
    return string("_") + aParent + to_string(aPos);
}

bool IsLexCtx(const string& aLex)
{
    return aLex == KT_Namespace || aLex == KT_Target || aLex == KT_Node;
}

TNodeType Chromo2Mdl::GetType(const THandle& aHandle) {
    TNodeType res = ENt_Unknown;
    C2MdlNode* node = aHandle.Data(node);
    string rel = node->mMut.mR;
    if (rel.empty()) {
	__ASSERT(!node->mChromo.empty());
	res = ENt_Seg;
    } else  if (rel == KMS_Add) {
	res = ENt_Node;
    } else if (rel == KMS_Rename) {
	// TODO To replace change mut with renaming mut
	res = ENt_Change;
    } else if (rel == KMS_Remove) {
	res = ENt_Rm;
    } else  if (rel == KMS_Note) {
	res = ENt_Note;
    } else  if (rel == KMS_Import) {
	res = ENt_Import;
    } else  if (rel == KMS_Cont) {
	res = ENt_Cont;
    } else  if (rel == KMS_Conn) {
	res = ENt_Conn;
    } else  if (rel == KMS_Disconn) {
	res = ENt_Disconn;
    }
    return res;
}

THandle Chromo2Mdl::Root(const THandle& aHandle)
{
    return &mRoot;
}

THandle Chromo2Mdl::Parent(const THandle& aHandle)
{
    C2MdlNode* node = aHandle.Data(node);
    return node->mOwner;
}

THandle Chromo2Mdl::Next(const THandle& aHandle, TNodeType aType)
{
    C2MdlNode* node = aHandle.Data(node);
    __ASSERT(node->mOwner != NULL);
    C2MdlNode* next = node->mOwner->GetNextComp(node);
    return next;
}

THandle Chromo2Mdl::Prev(const THandle& aHandle, TNodeType aType)
{
    C2MdlNode* node = aHandle.Data(node);
    C2MdlNode* prev = node->mOwner->GetPrevComp(node);
    return prev;
}

THandle Chromo2Mdl::GetFirstChild(const THandle& aHandle, TNodeType aType)
{
    C2MdlNode* node = aHandle.Data(node);
    C2MdlNode *comp = NULL;
    if (node->mChromo.size() > 0) {
	comp = &(node->mChromo.front());
    }
    return comp;
}

THandle Chromo2Mdl::GetLastChild(const THandle& aHandle, TNodeType aType)
{
    C2MdlNode* node = aHandle.Data(node);
    C2MdlNode *comp = NULL;
    if (node->mChromo.size() > 0) {
	comp = &(node->mChromo.back());
    }
    return comp;
}

string Chromo2Mdl::GetContextByAttr(const C2MdlNode& aNode, TNodeAttr aAttr)
{
    string res;
    if (KM_CtxAttToType.count(aAttr) > 0) {
	string rel = KM_CtxAttToType.at(aAttr);
	if (aNode.mContext.count(rel) > 0) {
	    res = aNode.mContext.at(rel);
	}
    }
    return res;
}

string Chromo2Mdl::GetAttr(const THandle& aHandle, TNodeAttr aAttr) const
{
    C2MdlNode* node = aHandle.Data(node);
    string res;
    string rel = node->mMut.mR;
    if (aAttr == ENa_Id) {
	__ASSERT(rel == KMS_Add || rel == KMS_Import || rel == KMS_Cont);
	if (rel == KMS_Add) {
	    res = node->mMut.mP;
	} else if (rel == KMS_Cont) {
	    res = node->mMut.mP;
	    if (res == KT_Default) {
		res.clear();
	    }
	} else if (rel == KMS_Import) {
	    res = node->mMut.mQ;
	}
    } else if (aAttr == ENa_Parent && rel == KMS_Add) {
	res = node->mMut.mQ;
    } else if (aAttr == ENa_Targ || aAttr == ENa_NS) {
	res = GetContextByAttr(*node, aAttr);
    } else if (aAttr == ENa_MutAttr) {
	res = "id";
    } else if (aAttr == ENa_MutNode) {
	__ASSERT(rel == KMS_Remove || rel == KMS_Add || rel == KMS_Cont || rel == KMS_Conn || rel == KMS_Disconn || rel.empty());
	if (rel == KMS_Add || rel == KMS_Cont || rel.empty()) {
	    res = GetContextByAttr(*node, aAttr);
	} else if (rel == KMS_Conn || rel == KMS_Disconn) {
	    res = node->mMut.mP;
	} else {
	    res = node->mMut.mP;
	}
    } else if (aAttr == ENa_P) {
	__ASSERT(rel == KMS_Conn || rel == KMS_Disconn);
	if (rel == KMS_Conn || rel == KMS_Disconn) {
	    res = node->mMut.mP;
	}
    } else if (aAttr == ENa_Q) {
	__ASSERT(rel == KMS_Conn || rel == KMS_Disconn);
	if (rel == KMS_Conn || rel == KMS_Disconn) {
	    res = node->mMut.mQ;
	}
    } else if (aAttr == ENa_MutVal) {
	__ASSERT(rel == KMS_Cont || rel == KMS_Note || rel == KMS_Rename);
	res = node->mMut.mQ;
    }
    return res;
}

TBool Chromo2Mdl::AttrExists(const THandle& aHandle, TNodeAttr aAttr) const 
{
    TBool res = false;
    C2MdlNode* node = aHandle.Data(node);
    string rel = node->mMut.mR;
    if (aAttr == ENa_Id) {
	res = (rel == KMS_Add) || rel == KMS_Cont || (rel == KMS_Import);
    } else if (aAttr == ENa_Parent) {
	res = (rel == KMS_Add);
    } else if (aAttr == ENa_Targ || aAttr == ENa_NS) {
	res = node->ExistsContextByAttr(aAttr);
    } else if (aAttr == ENa_MutVal) {
	res = (rel == KMS_Cont || rel == KMS_Note || rel == KMS_Rename);
    } else if (aAttr == ENa_MutAttr) {
	res = (rel == KMS_Rename);
    } else if (aAttr == ENa_MutNode) {
	if (rel == KMS_Remove) {
	    res = !node->mMut.mP.empty();
	} else {
	    res = ((rel == KMS_Add) || (rel == KMS_Cont) || rel.empty()) && node->ExistsContextByAttr(ENa_MutNode);
	}
    } else if (aAttr == ENa_P) {
	    res = ((rel == KMS_Conn || rel == KMS_Disconn) && !node->mMut.mP.empty());
    } else if (aAttr == ENa_Q) {
	    res = ((rel == KMS_Conn || rel == KMS_Disconn) && !node->mMut.mQ.empty());
    } else if (aAttr == ENa_Ref) {
	// Use MutVal instead
    }
    return res;
}

THandle Chromo2Mdl::AddChild(const THandle& aParent, TNodeType aType)
{
    C2MdlNode* parent = aParent.Data(parent);
    C2MdlNode node;
    node.mMut.mR = GetMutRel(aType);
    node.mOwner = parent;
    parent->mChromo.push_back(node);
    C2MdlNode& res = parent->mChromo.back();
    return &res;
}

THandle Chromo2Mdl::AddChild(const THandle& aParent, const THandle& aHandle, TBool aCopy, TBool aRecursively)
{
    C2MdlNode* parent = aParent.Data(parent);
    C2MdlNode* child = aHandle.Data(child);
    C2MdlNode node;
    node.CloneFrom(*child, true);
    node.mOwner = parent;
    parent->mChromo.push_back(node);
    parent->BindTree(parent->mOwner);
    C2MdlNode& res = parent->mChromo.back();
    return &res;
}

THandle Chromo2Mdl::AddNext(const THandle& aPrev, const THandle& aHandle, TBool aCopy)
{
    __ASSERT(false);
}

THandle Chromo2Mdl::AddNext(const THandle& aPrev, TNodeType aNode)
{
    __ASSERT(false);
}

THandle Chromo2Mdl::AddPrev(const THandle& aNext, const THandle& aHandle, TBool aCopy)
{
    __ASSERT(false);
}

void Chromo2Mdl::RmChild(const THandle& aParent, const THandle& aChild, TBool aDeattachOnly)
{
    bool removed = false;
    C2MdlNode* parent = aParent.Data(parent);
    C2MdlNode* child = aChild.Data(child);
    for (TC2MdlNodesIter it = parent->mChromo.begin(); it != parent->mChromo.end(); it++) {
	C2MdlNode& cn = *it;
	if (&cn == child) {
	    parent->mChromo.erase(it);
	    child->mOwner = NULL;
	    removed = true;
	    break;
	}
    }
    __ASSERT(removed);
}

void Chromo2Mdl::Rm(const THandle& aHandle)
{
    __ASSERT(false);
}

void Chromo2Mdl::SetAttr(const THandle& aHandle, TNodeAttr aType, const string& aVal)
{
    C2MdlNode* node = aHandle.Data(node);
    string rel = node->mMut.mR;
    if (aType == ENa_Id) {
	__ASSERT (rel == KMS_Add || rel == KMS_Cont || rel == KMS_Import);
	if (rel == KMS_Add || rel == KMS_Cont) {
	    node->mMut.mP = aVal;
	} else if (rel == KMS_Import) {
	    node->mMut.mQ = aVal;
	    node->mMut.mP = KT_Default;
	} else if (rel == KMS_Cont) {
	    node->mMut.mP = aVal.empty() ? KT_Default : aVal;
	}
    } else if (aType == ENa_Parent) {
	__ASSERT (rel.empty() || rel == KMS_Add);
	node->mMut.mQ = aVal;
	if (rel.empty()) {
	    node->mMut.mR = KMS_Add;
	}
    } else if (aType == ENa_MutAttr) {
	__ASSERT (rel == KMS_Cont || (rel == KMS_Rename));
	// Attr for rename is not used, omit
    } else if (aType == ENa_MutVal) {
	__ASSERT (rel == KMS_Cont || rel == KMS_Note || rel == KMS_Rename);
	if (rel == KMS_Note || rel == KMS_Rename) {
	    node->mMut.mQ = aVal;
	    node->mMut.mP = KT_Default;
	} else {
	    node->mMut.mQ = aVal;
	}
    } else if (aType == ENa_Targ) {
	string ctxrel = GetCtxRel(aType);
	__ASSERT (!ctxrel.empty());
	node->mContext[ctxrel] = aVal;
    } else if (aType == ENa_MutNode) {
	__ASSERT (rel == KMS_Remove || rel == KMS_Add || rel == KMS_Cont || rel == KMS_Conn || rel == KMS_Disconn || rel.empty());
	if (rel == KMS_Add || rel == KMS_Cont || rel.empty()) {
	    node->AddContext(KT_Node, aVal);
	} else if (rel == KMS_Conn || rel == KMS_Disconn) {
	    node->mMut.mP = aVal;
	} else {
	    node->mMut.mP = aVal;
	}
    } else if (aType == ENa_P) {
	__ASSERT (rel == KMS_Conn || rel == KMS_Disconn);
	if (rel == KMS_Conn || rel == KMS_Disconn) {
	    node->mMut.mP = aVal;
	}
    } else if (aType == ENa_Q) {
	__ASSERT (rel == KMS_Conn || rel == KMS_Disconn);
	if (rel == KMS_Conn || rel == KMS_Disconn) {
	    node->mMut.mQ = aVal;
	}
    } else if (aType == ENa_Ref) {
	__ASSERT(rel == KMS_Cont);
	node->mMut.mQ = aVal;
	if (node->mMut.mP.empty()) {
	    node->mMut.mP = KT_Default;
	}
    } else if (aType == ENa_NS) {
	string ctxrel = GetCtxRel(aType);
	__ASSERT (!ctxrel.empty());
	node->mContext[ctxrel] = aVal;
    }
}

void Chromo2Mdl::SetAttr(const THandle& aNode, TNodeAttr aType, TInt aVal)
{
}

void Chromo2Mdl::RmAttr(const THandle& aHandle, TNodeAttr aType)
{
    C2MdlNode* node = aHandle.Data(node);
    string rel = node->mMut.mR;
    if (aType == ENa_MutNode) {
	__ASSERT (rel == KMS_Remove || rel == KMS_Add || rel == KMS_Cont || rel == KMS_Conn || rel == KMS_Disconn || rel.empty());
	if (rel == KMS_Add || rel == KMS_Cont || rel.empty()) {
	    node->RmContext(aType);
	} else if (rel == KMS_Conn || rel == KMS_Disconn) {
	    node->mMut.mP.erase();
	} else {
	    node->mMut.mP.erase();
	}
    } else if (KM_CtxAttToType.count(aType) > 0) {
	string crel = KM_CtxAttToType.at(aType);
	node->mContext.erase(crel);
    }
}

void Chromo2Mdl::Dump(const THandle& aHandle)
{
    C2MdlNode* node = aHandle.Data(node);
    DumpMnode(*node, 0);
}

void Chromo2Mdl::DumpBackTree(const THandle& aHandle)
{
    C2MdlNode* node = aHandle.Data(node);
    while (node != NULL) {
	cout << node << "  Id: " << node->mMut.mP << ", Owner: " << node->mOwner << endl;;
	node = node->mOwner;
    }
}


void Chromo2Mdl::DumpToLog(const THandle& aNode, MLogRec* aLogRec)
{
}

TBool Chromo2Mdl::ToString(const THandle& aNode, string& aString) const
{
    C2MdlNode* node = aNode.Data(node);
    ostringstream ss;
    OutputNode(*node, ss, 0, 0);
    aString = ss.str();
    return true;
}

void Chromo2Mdl::Save(const string& aFileName, TInt aIndent) const
{
    ofstream os(aFileName, ofstream::out);
    OutputNode(mRoot, os, 0, aIndent == 0 ? K_Indent : aIndent);
    os.flush();
    os.close();
}

THandle Chromo2Mdl::Find(const THandle& aHandle, const string& aUri)
{
    __ASSERT(false);
}

TInt Chromo2Mdl::GetOrder(const THandle& aHandle, TBool aTree) const
{
    return 0;
}

void Chromo2Mdl::DeOrder(const THandle& aHandle)
{
    __ASSERT(false);
}

TInt Chromo2Mdl::GetLineId(const THandle& aHandle) const
{
    C2MdlNode* node = aHandle.Data(node);
    return node->mChromoPos;
}



int Chromo2Mdl::GetAttrInt(void *aHandle, const char *aName)
{
    __ASSERT(false);
}

THandle Chromo2Mdl::SetFromFile(const string& aFileName)
{
    ifstream is(aFileName, std::ifstream::in);
    if ( (is.rdstate() & ifstream::failbit ) == 0 ) {
	is.seekg(0, is.beg);
	streampos beg = is.tellg();
	is.seekg(0, is.end);
	streampos end = is.tellg();
	//ParseChromo(is, beg, end, mRoot);
	ParseCnodeChromo(is, beg, end, mRoot, true, false);
	//ParseCnodeMut(is, beg, end, mRoot, true, false);
	mRoot.BindTree(NULL);
	//cout << "SetFromFile: parsed chromo : " << endl;
	//DumpMnode(mRoot, 0); //!!
    } else {
	// Error reading the file
	mErr.Set(0, KPE_ErrOnReadingSpec);
    }
    return &mRoot;
}

THandle Chromo2Mdl::Set(const string& aUri)
{
    string path;
    ChromoUtils::GetPath(aUri, path);
    return SetFromFile(path);
}

THandle Chromo2Mdl::SetFromSpec(const string& aSpec)
{
    istringstream is(aSpec);
    is.seekg(0, is.beg);
    streampos beg = is.tellg();
    is.seekg(0, is.end);
    streampos end = is.tellg();
    ParseCnodeChromo(is, beg, end, mRoot, true, false);
    //ParseCnodeMut(is, beg, end, mRoot, true, false);
    mRoot.BindTree(NULL);
    //DumpMnode(mRoot, 0);
    return &mRoot;
}

THandle Chromo2Mdl::Set(const THandle& aHandle)
{
    C2MdlNode* node = aHandle.Data(node);
    mRoot.CloneFrom(*node, true);
    mRoot.BindTree(NULL);
    return &mRoot;
}

THandle Chromo2Mdl::Init(TNodeType aRootType)
{
    mRoot.mMut.mR = GetMutRel(aRootType);
    return &mRoot;
}

void Chromo2Mdl::Reset()
{
    __ASSERT(false);
}

static void DumpIs(istream& aIs, streampos aStart, streampos aEnd)
{
    streampos curpos = aIs.tellg();
    aIs.seekg(aStart, aIs.beg);
    streampos pos = aIs.tellg();
    while (pos != aEnd) {
	char c = aIs.get();
	cout << c;
	pos = aIs.tellg();
    }
    cout << endl;
    aIs.seekg(curpos, aIs.beg);
}

bool PassThroughText(istream& aIs, streampos aEnd)
{
    bool closed = false;
    streampos pos = aIs.tellg();
    while (pos != aEnd) {
	char c = aIs.get();
	if (c == KT_TextDelim) {
	    closed = true; break;
	}
	pos = aIs.tellg();
    }
    return closed;
}

void Chromo2Mdl::ParseChromo(istream& aIs, streampos aStart, streampos aEnd, C2MdlNode& aMnode)
{
    //DumpIsFrag(aIs, aStart, aEnd);

    int cscount = 0;
    streampos beg = aStart;
    streampos end = aEnd;
    streampos nbeg = aStart;
    aIs.seekg(aStart, aIs.beg);
    streampos pos = aIs.tellg();
    while (pos != aEnd) {
	char c = aIs.get();
	if (c == KT_TextDelim) {
	    if (!PassThroughText(aIs, aEnd)) {
		mErr.Set(pos, KPE_TextNotClosed);
		break;
	    }
	} else if (c == KT_MutSeparator) {
	    streampos cur = aIs.tellg();
	    aIs.seekg(-1, aIs.cur);
	    streampos mend = aIs.tellg();
	    ParseCnodeMut(aIs, nbeg, mend, aMnode, false, false);
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
			// Checking if it is the mut with dependency
			pos = aIs.tellg();
			streampos pos2 = pos;
			bool dmc = false;
			while (pos2 != aEnd) {
			    c = aIs.get();
			    if (!IsSep(c)) {
				if (c == KT_MutSeparator) {
				    dmc = true;
				    pos = aIs.tellg();
				} else {
				    aIs.seekg(pos, aIs.beg);
				}
				break;
			    }
			    pos2 = aIs.tellg();
			}
			end = aIs.tellg();
			if (dmc) {
			    ParseCnodeMut(aIs, nbeg, end, aMnode, false, true);
			} else {
			    ParseCnodeChromo(aIs, nbeg, end, aMnode, false, false);
			}
			//ParseCnodeMut(aIs, nbeg, end, aMnode, false, dmc);
			nbeg = end;
			// Restore position for chromo parsing can change it
			aIs.seekg(end, aIs.beg);
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

bool IsGroupNested(char aGroupMark)
{
    return false;
}

bool IsTextDelimiter(char aChar)
{
    return (aChar == KT_TextDelim);
}

bool GetText(istream& aIs, streampos aEnd, string& aLex, char aDelimiter)
{
    bool res = false;
    streampos pos = aIs.tellg();
    char prev_c = '\x00';
    while (pos != aEnd) {
	char c = aIs.get();
	if (IsTextDelimiter(c) && (prev_c != KT_Escape)) {
	    res = true;
	    break;
	} else {
	    aLex.push_back(c);
	}
	prev_c = c;
    }
    return res;
}

void Chromo2Mdl::GetLexs(istream& aIs, streampos aBeg, streampos aEnd, vector<string>& aLexs)
{
    bool res = true;
    string lexeme;
    aIs.seekg(aBeg, aIs.beg);
    streampos pos = aIs.tellg();
    while (pos != aEnd) {
	char c = aIs.get();
	if ((c == ' ' || c == '\n' || c == '\r' || c == '\t')) {
	    if (!lexeme.empty()) {
		aLexs.push_back(lexeme);
		lexeme.clear();
	    }
	} else if (IsTextDelimiter(c)) {
	    res = GetText(aIs, aEnd, lexeme, KT_TextDelim);
	    pos = aIs.tellg();
	    if (res) {
		aLexs.push_back(lexeme);
		lexeme.clear();
	    } else {
		mErr.Set(pos, KPE_MissingEndOfGroup);
		break;
	    }
	} else {
	    lexeme.push_back(c);
	}
	pos = aIs.tellg();
    }
    if (!lexeme.empty()) {
	aLexs.push_back(lexeme);
    }
}

void Chromo2Mdl::GetLexsPos(istream& aIs, streampos aBeg, streampos aEnd, TLexPos& aLexs)
{
    bool res = true;
    string lexeme;
    aIs.seekg(aBeg, aIs.beg);
    streampos pos = aIs.tellg();
    while (pos != aEnd) {
	char c = aIs.get();
	if ((c == ' ' || c == '\n' || c == '\r' || c == '\t')) {
	    if (!lexeme.empty()) {
		// Set pos before the lexem start
		aLexs.push_back(TLexPosElem(lexeme, pos - (streampos) lexeme.size()));
		lexeme.clear();
	    }
	} else if (IsTextDelimiter(c)) {
	    pos = aIs.tellg();
	    res = GetText(aIs, aEnd, lexeme, KT_TextDelim);
	    if (res) {
		aLexs.push_back(TLexPosElem(lexeme, pos));
		lexeme.clear();
	    } else {
		mErr.Set(pos, KPE_MissingEndOfGroup);
		break;
	    }
	} else {
	    lexeme.push_back(c);
	}
	pos = aIs.tellg();
    }
    if (!lexeme.empty()) {
	aLexs.push_back(TLexPosElem(lexeme, pos));
    }
}


void Chromo2Mdl::ParseContext(vector<string>& aLexs, streampos aPos, C2MdlNode& aMnode)
{
    for (int i = 0; (2*i+1) < aLexs.size(); i++) {
	string val = aLexs.at(2*i);
	string rel = aLexs.at(2*i + 1);
	if (rel == KT_Target || rel == KT_Namespace || rel == KT_Node) {
	    aMnode.mContext.insert(TC2MdlCtxElem(rel, val));
	} else {
	    mErr.Set(aPos, KPE_WrongCtxType);
	}
    }
}

void Chromo2Mdl::ParseContext2(const TLex& aLexs, streampos aPos, C2MdlNode& aMnode)
{
    int ls = aLexs.size(); // Lexems number
    int lidx = aLexs.size() - 1; // Lexems index
    if (ls >= 2) {
	while (lidx > 0 && !mErr.IsSet()) {
	    const string& lex = aLexs.at(lidx);
	    if (IsLexCtx(lex)) {
		if (lidx >= 2) {
		    const string& depmut = aLexs.at(lidx - 2);
		    if (depmut == KMS_Add) {
			// There is assosiated dep mutation, evolve it
			string dmp = (lidx >= 3) ? aLexs.at(lidx - 3) : string(); // P operand of depmut
			const string& q = aLexs.at(lidx - 1);
			const string& r = aLexs.at(lidx - 2);
			string p;
			int shift = 3;
			if (dmp.empty()) {
			    // P operand is omitted, anonymous node. Generate P operand
			    p = string("_") + q + to_string((int) aPos);
			    shift = 2;
			} else {
			    p = dmp;
			}
			// Store dependent mutation in upper chromo
			C2MdlNode mnode;
			mnode.mOwner = aMnode.mOwner;
			mnode.mMut.mQ = q;
			mnode.mMut.mR = r;
			mnode.mMut.mP = p;
			mnode.mChromoPos = aPos;
			aMnode.mOwner->mChromo.push_back(mnode);
			aMnode.mContext.insert(TC2MdlCtxElem(lex, p)); // Context
			aMnode.mOwner->mQnode = &(*aMnode.mOwner->mChromo.rbegin());
			lidx -= shift; // Move to next context item
		    } else {
			mErr.Set(aPos, KPE_WrongCtxType);
		    }
		} else {
		    // Just regular context, set it to the current node
		    const string& val = aLexs.at(lidx - 1);
		    aMnode.mContext.insert(TC2MdlCtxElem(lex, val));
		    lidx -= 2; // Move to next context item
		}
	    } else {
		mErr.Set(aPos, KPE_WrongCtxType);
	    }
	}
    } else {
	mErr.Set(aPos, KPE_WrongNumOfCtxLex);
    }
}

bool Chromo2Mdl::IsLexDmcMutop(const string& aLex) const
{
    bool res = false;
    for (auto lex : KDmcMutops) {
	if (aLex == lex) {
	    res = true; break;
	}
    }
    return res;
}

void Chromo2Mdl::ParseCnodeMut(istream& aIs, streampos aBeg, streampos aEnd, C2MdlNode& aMnode, bool aRoot, bool aDepNode)
{
    //cout << "Parsing mutation:" << endl;
    //DumpIsFrag(aIs, aBeg, aEnd);
    //
    C2MdlNode omnode;
    C2MdlNode& mnode = aRoot ? aMnode : omnode;
    if (!aRoot) {
	mnode.mOwner = &aMnode;
    }

    aIs.seekg(aBeg, aIs.beg);
    //streampos pos = aIs.tellg();

    // Find the possible chromo
    streampos pos = SeekCtrl(aIs, aBeg, aEnd, KT_ChromoStart);
    bool includesChromo = (pos != aEnd);
    /*
    if (includesChromo) {
	aIs.seekg(aEnd, aIs.beg);
	aIs.seekg(-1, aIs.cur);
	streampos cend = aIs.tellg();
	ParseChromo(aIs, pos, cend, mnode);
    }
    */

    // Get lexems
    TLexPos lexs;   // Lexems
    aIs.seekg(pos, aIs.beg);
    if (includesChromo) {
	aIs.seekg(-1, aIs.cur);
    }
    streampos lexend = aIs.tellg();
    GetLexsPos(aIs, aBeg, lexend, lexs);

    if (lexs.size() >= 1) {
	const string& lastl = lexs.back().first;
	if (IsLexCtx(lastl)) {
	    lexs.pop_back();
	} 
    }

    if (lexs.size() >= 3 || aDepNode && (lexs.size() >= 2)) {
	bool dmc = false;
	auto lit = lexs.rbegin();
	const string& l2 = (lit + 1)->first;
	string l3;
	if (lexs.size() >= 3) {
	    l3 = (lit + 2)->first;
	    if (lexs.size() >= 4) {
		const string& l4 = (lit + 3)->first;
		dmc = l2 == KMS_Add && (IsLexDmcMutop(l4) || IsLexDmcMutop(l3));
	    }
	}
	if (dmc) {
	    // DMC mut
	    int msidx = IsLexDmcMutop(l3) ? 2 : 3; 
	    //C2MdlNode mnode;
	    mnode.mMut.mR = (lit + msidx)->first;
	    mnode.mMut.mP = (lit + msidx + 1)->first;
	    int deppos = (lit + msidx - 1)->second;
	    if (includesChromo) {
		ParseCnodeChromo(aIs, deppos, aEnd, aMnode, false, true);
	    } else {
		ParseCnodeMut(aIs, deppos, aEnd, aMnode, false, true);
	    }
	    //ParseCnodeMut(aIs, deppos, aEnd, aMnode, false, true);
	    if (!mErr.IsSet()) {
		const C2MdlNode& dep = *aMnode.mQnode;
		if (!dep.mMut.mP.empty()) {
		    mnode.mMut.mQ = dep.mMut.mP;
		} else if (dep.mContext.count(KT_Target) > 0) {
		    mnode.mMut.mQ = dep.mContext.at(KT_Target);
		} else if (dep.mContext.count(KT_Namespace) > 0) {
		    mnode.mMut.mQ = dep.mContext.at(KT_Namespace);
		}
		mnode.mChromoPos = pos;
		aMnode.mChromo.push_back(mnode);
	    }
	} else if (includesChromo) {
	    // Mut included chromo but not parsed it, error
	    mErr.Set(pos, KPE_SmallLexNumInDmcMut);
	} else {
	    // Simple mutation
	    string q = lexs.back().first; lexs.pop_back();
	    string r = lexs.back().first; lexs.pop_back();
	    string p;
	    if (lexs.empty()) {
		if (aDepNode) {
		    if (r == KMS_Add) {
			// Apply generated name
			p = string("_") + q + to_string((int) pos);
		    } else {
			mErr.Set(pos, KPE_WrongDepMut);
		    }
		} else {
		    mErr.Set(pos, KPE_WrongMutLexNum);
		}
	    } else {
		p = lexs.back().first; lexs.pop_back();
	    }
	    if (!mErr.IsSet() && IsMutSmb(r)) {
		// Adding nodel node of chromo type
		//C2MdlNode mnode;
		//mnode.mOwner = &aMnode;
		mnode.mMut.mR = r;
		mnode.mMut.mP = p;
		mnode.mMut.mQ = q;
		if (!lexs.empty()) {
		    // Parse context
		    if ((lexs.size() >= 2) && (lexs.size() % 2) == 0) {
			TLex lx;
			for (auto lp : lexs) {
			    lx.push_back(lp.first);
			}
			ParseContext(lx, aBeg, mnode);
		    } else {
			mErr.Set(pos, KPE_WrongMutLexNum);
		    }
		}
		mnode.mChromoPos = pos;
		if (!mErr.IsSet()) {
		    if (!aRoot) {
			aMnode.mChromo.push_back(mnode);
			if (aDepNode) {
			    aMnode.mQnode = &(*aMnode.mChromo.rbegin());
			}
		    }
		}
	    } else if (!mErr.IsSet()) {
		mErr.Set(pos, KPE_UnknownMutation);
	    } else {
		TLexPos lexs2;
		GetLexsPos(aIs, aBeg, lexend, lexs2);
	    }
	}
    } else {
	mErr.Set(pos, KPE_WrongMutLexNum);
    }
}

void Chromo2Mdl::ParseCnodeChromo(istream& aIs, streampos aStart, streampos aEnd, C2MdlNode& aMnode, bool aRoot, bool aDepNode)
{
    //cout << "Parsing chromo:" << endl;
    //DumpIsFrag(aIs, aStart, aEnd);

    // Adding nodel node of chromo type
    C2MdlNode omnode;
    C2MdlNode& mnode = aRoot ? aMnode : omnode;
    if (!aRoot) {
	mnode.mOwner = &aMnode;
    }
    streampos pos = SeekChar(aIs, aStart, aEnd, KT_ChromoStart);
    __ASSERT(pos != aEnd);
    aIs.seekg(aEnd, aIs.beg);
    aIs.seekg(-1, aIs.cur);
    streampos cend = aIs.tellg();
    ParseChromo(aIs, pos, cend, mnode);

    // Parse leading part
    vector<string> lexs;   //!< Lexems
    aIs.seekg(pos, aIs.beg);
    aIs.seekg(-1, aIs.cur);
    streampos lexend = aIs.tellg();
    //DumpIs(aIs, aStart, lexend);
    //cout << aEnd << endl;
    GetLexs(aIs, aStart, lexend, lexs);
    int ls = lexs.size();
    if (ls >= 1) {
	auto lb = lexs.rbegin();
	string l1 = *lb;
	if (IsLexCtx(l1)) {
	    ParseContext2(lexs, aStart, mnode);
	} else if (ls >= 2) {
	    auto l2 = *(lb + 1);
	    if (l2 == KMS_Add) {
		mnode.mMut.mQ = l1; lexs.pop_back();
		mnode.mMut.mR = l2; lexs.pop_back();
		if (ls >= 3) {
		    // Explicitly named node
		    auto l3 = *(lb + 2);
		    mnode.mMut.mP = l3; lexs.pop_back();
		    if (!lexs.empty()) {
			ParseContext(lexs, aStart, mnode);
		    }
		} else {
		    // Anonymous node
		    mnode.mMut.mP = GenerateName(l1, pos);
		}
	    } else {
		mErr.Set(pos, KPE_WrongCombMutType);
	    }
	}
	/*
       	else if (ls >= 3 && ((ls - 3) % 2) == 0) {
	    mnode.mMut.mQ = lexs.back(); lexs.pop_back();
	    mnode.mMut.mR = lexs.back(); lexs.pop_back();
	    mnode.mMut.mP = lexs.back(); lexs.pop_back();
	    if (mnode.mMut.mR == KMS_Add) {
		if (!lexs.empty()) {
		    ParseContext(lexs, aStart, mnode);
		}
	    } else {
		mErr.Set(pos, KPE_WrongCombMutType);
	    }
	} else {
	    mErr.Set(pos, KPE_WrongMutLexNum);
	}
	*/
    } else {
	mErr.Set(pos, KPE_MissingSegContext);
    }
    if (!mErr.IsSet()) {
	if (!aRoot) {
	    mnode.mChromoPos = pos;
	    aMnode.mChromo.push_back(mnode);
	    if (aDepNode) {
		aMnode.mQnode = &(*aMnode.mChromo.rbegin());
	    }
	}
    }
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

void Offset(int aLevel, int aIndent, ostream& aOs)
{
    for (int i = 0; i < aLevel*aIndent; i++)  aOs << " ";
}

/** @brief Checks if lexeme contains separators
 * */
bool ContainsSep(const string& aLex)
{
    bool res = false;
    for (auto c : aLex) {
	if (KSep.find(c) != string::npos) {
	    res = true; break;
	}
    }
    return res;
}

/** @brief Groups lexeme
 * */
string GroupLexeme(const string& aLex)
{
    string res;
    bool sep = ContainsSep(aLex);
    if (sep) {
	res = KT_TextDelim;
    }
    res += aLex;
    if (sep) {
	res += KT_TextDelim;
    }
    return res;
}

void Chromo2Mdl::OutputNode(const C2MdlNode& aNode, ostream& aOs, int aLevel, int aIndent)
{
    bool cnt = false;
    if (!aNode.mContext.empty()) {
	for (TC2MdlCtxCiter it = aNode.mContext.begin(); it != aNode.mContext.end(); it++) {
	    Offset(aLevel, aIndent, aOs); aOs << it->second << " " << it->first << " ";
	    cnt = true;
	}
    }

    bool mut = false;
    int cnum = aNode.mChromo.size();
    if (!aNode.mMut.mR.empty()) {
	if (!cnt) Offset(aLevel, aIndent, aOs); aOs << aNode.mMut.mP << " " << aNode.mMut.mR << " " << GroupLexeme(aNode.mMut.mQ);
	if (cnum == 0) {
	    aOs << ";";
	}
	aOs << endl;
	mut = true;
    }

    if (cnum > 0) {
	if (!cnt || mut) Offset(aLevel, aIndent, aOs); aOs << "{" << endl;
	for (TC2MdlNodesCiter it = aNode.mChromo.begin(); it != aNode.mChromo.end(); it++) {
	    const C2MdlNode& node = *it;
	    OutputNode(node, aOs, aLevel + 1, aIndent);
	}
	Offset(aLevel, aIndent, aOs); aOs << "}" << endl;
    }
}

void Chromo2Mdl::DumpMnode(const C2MdlNode& aNode, int aLevel) const
{
    bool cnt = false;
    if (aNode.mOwner == NULL) {
	bool treeok = CheckTree(aNode);
	cout << "Tree integrity: " << (treeok ? "OK" : "NOK") << endl;
    }
    OutputNode(aNode, cout, aLevel, K_Indent);
}

bool Chromo2Mdl::CheckTree(const C2MdlNode& aNode) const
{
    bool res = true;
    for (TC2MdlNodesCiter it = aNode.mChromo.begin(); it != aNode.mChromo.end() && res; it++) {
	const C2MdlNode& comp = *it;
	res = comp.mOwner == &aNode;
	res = res && CheckTree(comp);
    }
    return res;
}

bool Chromo2Mdl::IsError() const
{
    return mErr.mText.empty();
}

void Chromo2Mdl::TransfTl(const THandle& aHandle, const THandle& aSrc)
{
    C2MdlNode* node = aHandle.Data(node);
    
}

TBool Chromo2Mdl::operator==(const Chromo2Mdl& b)
{
    TBool res = !mErr.IsSet() && !b.mErr.IsSet();
    return res;
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
    C2MdlNode* nroot = root.Data(nroot);
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
    if (!mRootNode.IsNil()) {
	// Clear old mutation // TODO to re-design
	ChromoNode::Iterator mit = mRootNode.Begin();
	while (mit != mRootNode.End()) {
	    ChromoNode node = *mit;
	    mRootNode.RmChild(node);
	    mit = mRootNode.Begin();
	}
    }
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

Chromo2& Chromo2::operator=(const Chromo2& aSrc)
{
    Set(aSrc.Root());
    return *this;
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

void Chromo2::Save(const string& aFileName, TInt aIndent) const
{
    // We cannot simply save the doc (mMdl.iDoc) because it will save not only root node but
    // also adjacent nodes. So we need to create new model and add to doc only one separated root
    Chromo2Mdl mdl;
    mdl.Set(mRootNode.Handle());
    mdl.Save(aFileName, aIndent);
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

void Chromo2::Convert(const MChromo& aSrc)
{
    ChromoNode sroot = aSrc.Root();
    Init(sroot.Type());
    ConvertNode(mRootNode, sroot);
}

/** @brief Escape control symbols */
string EscapeCtrls(const string& aData, const string& aCtrls)
{
    string res;
    for (auto c : aData) {
	if (aCtrls.find(c) != string::npos || c == KT_Escape) {
	    res.push_back(KT_Escape);
	}
	res.push_back(c);
    }
    return res;
}

void ConvertAttr(ChromoNode& aDst, const ChromoNode& aSrc, TNodeAttr aAttr)
{
    if (aSrc.AttrExists(aAttr)) {
	string escSrc = EscapeCtrls(aSrc.Attr(aAttr), K_Ctrls);
	aDst.SetAttr(aAttr, escSrc);
    }
}

void Chromo2::ConvertNode(ChromoNode& aDst, const ChromoNode& aSrc)
{
    ConvertAttr(aDst, aSrc, ENa_Id);
    ConvertAttr(aDst, aSrc, ENa_Parent);
    ConvertAttr(aDst, aSrc, ENa_Targ);
    ConvertAttr(aDst, aSrc, ENa_MutNode);
    ConvertAttr(aDst, aSrc, ENa_MutVal);
    ConvertAttr(aDst, aSrc, ENa_Ref);
    ConvertAttr(aDst, aSrc, ENa_NS);
    ConvertAttr(aDst, aSrc, ENa_P);
    ConvertAttr(aDst, aSrc, ENa_Q);
    if (aSrc.Type() == ENt_Cont && (!aSrc.AttrExists(ENa_Id) || aSrc.Attr(ENa_Id).empty())) {
	// Default content
	aDst.SetAttr(ENa_Id, KT_Default);
    }
    for (ChromoNode::Const_Iterator it = aSrc.Begin(); it != aSrc.End(); it++) {
	const ChromoNode& scomp = *it;
	ChromoNode dcomp = aDst.AddChild(scomp.Type());
	ConvertNode(dcomp, scomp);
    }
}

void Chromo2::TransformLn(const MChromo& aSrc)
{
    ChromoNode sroot = aSrc.Root();
    Init(sroot.Type());
    //mRootNode.TransfTl(sroot);
    TransfTlNode(mRootNode, sroot, true);
}

bool IsTarg(const ChromoNode& aNode, const ChromoNode& aTarg, ChromoNode& aBase)
{
    bool res = false;
    if (aTarg.Type() == ENt_Node) {
	/*
	string tname = aTarg.Attr(ENa_Id);
	string ttarg = aTarg.Attr(ENa_Targ);
	string ntarg = aNode.Attr(ENa_Targ);
	GUri ttargUri(ttarg);
	GUri nuri(ntarg);
	ttargUri.AppendElem("", tname);
	res = nuri == ttargUri;
	*/
	string ntarg = aNode.Attr(ENa_Targ);
	GUri nuri(ntarg);
	GUri turi;
	aTarg.GetUri(turi, aBase);
	res = (nuri == turi);
    }
    return res;
}

/** @brief Checks if adjacent node is same segment (has same targ) as given node
 * @parem  aNode  given node
 * @parem  aAdj   adjacent node
 * @parem  aBase   owner of given node
 *
 * */
bool IsSeg(const ChromoNode& aNode, const ChromoNode& aAdj, ChromoNode& aBase)
{
    bool res = false;
    if (aNode.AttrExists(ENa_Targ)) {
	string nnode = aNode.Attr(ENa_Targ);
	GUri nuri(nnode);
	GUri adjUri;
	aAdj.GetTarget(adjUri, aBase);
	res = (nuri == adjUri);
    }
    return res;
}

/** @brief Checks if adjacent node is owning segment (owns targ) as given node
 * @parem  aNode  given node
 * @parem  aAdj   adjacent node
 * @parem  aBase   owner of given node
 *
 * */
bool IsOwningSeg(const ChromoNode& aNode, const ChromoNode& aAdj, ChromoNode& aBase, GUri& aTail)
{
    bool res = false;
    if (aNode.AttrExists(ENa_Targ)) {
	string nnode = aNode.Attr(ENa_Targ);
	GUri nuri(nnode);
	GUri adjUri;
	aAdj.GetTarget(adjUri, aBase);
	res = (!adjUri.Elems().empty() && adjUri <= nuri);
	if (res) {
	    nuri.Tail(adjUri, aTail);
	}
    }
    return res;
}



/** @brief Gets adjacent node mutation corresponding to the given node target
 * @param  aNode  given node
 * @param  aTarg  adjacent node - candidate for the corresponding node
 * @param  aBase  base node, owner of aNode
 * */
ChromoNode getAdjNode(const ChromoNode& aNode, ChromoNode& aTarg, ChromoNode& aBase)
{
    ChromoNode res(aTarg.Mdl(), THandle());
    if (IsTarg(aNode, aTarg, aBase)) {
	res = aTarg;
    } else if (aTarg.Count() > 0) {
	ChromoNode targ = *(aTarg.Rbegin());
	res = getAdjNode(aNode, targ, aBase);
    }
    return res;
}

/** @brief Gets the chromo adjacent segment */
ChromoNode getAdjSeg(const ChromoNode& aNode, ChromoNode& aTarg, ChromoNode& aBase)
{
    ChromoNode res(aTarg.Mdl(), THandle());
    if (IsSeg(aNode, aTarg, aBase)) {
	res = aTarg;
    } else if (aTarg.Count() > 0) {
	ChromoNode targ = *(aTarg.Rbegin());
	res = getAdjSeg(aNode, targ, aBase);
    }
    return res;
}

/** @brief Gets the chromo adjacent owning segment */
ChromoNode getAdjOwnSeg(const ChromoNode& aNode, ChromoNode& aTarg, ChromoNode& aBase, GUri& aTail)
{
    ChromoNode res(aTarg.Mdl(), THandle());
    if (IsOwningSeg(aNode, aTarg, aBase, aTail)) {
	res = aTarg;
    } else if (aTarg.Count() > 0) {
	ChromoNode targ = *(aTarg.Rbegin());
	ChromoNode cres = getAdjOwnSeg(aNode, targ, aBase, aTail);
	if (!cres.IsNil()) {
	    res = cres;
	}
    }
    return res;
}


// TODO To optimize. Seems the cases  with getAdjSeg and getAdjOwnSeg can be combined
void Chromo2::TransfTlNode(ChromoNode& aDst, const ChromoNode& aSrc, bool aTarg)
{
    ConvertAttr(aDst, aSrc, ENa_Id);
    ConvertAttr(aDst, aSrc, ENa_Parent);
    if (aTarg) ConvertAttr(aDst, aSrc, ENa_Targ);
    ConvertAttr(aDst, aSrc, ENa_MutVal);
    ConvertAttr(aDst, aSrc, ENa_Ref);
    ConvertAttr(aDst, aSrc, ENa_NS);
    ConvertAttr(aDst, aSrc, ENa_P);
    ConvertAttr(aDst, aSrc, ENa_Q);
    auto it = aSrc.Begin();
    if (it != aSrc.End()) {
	auto scomp = *it;
	ChromoNode dcomp = aDst.AddChild(scomp.Type());
	TransfTlNode(dcomp, scomp, true);
	ChromoNode sprev = dcomp; // Previous comp
	it++;
	for (; it != aSrc.End(); it++) {
	    scomp = *it;
	    // Try to find corresponding node in adjacent mutation and add cur mut to it
	    ChromoNode targ = getAdjNode(scomp, sprev, aDst);
	    if (!targ.IsNil()) {
		dcomp = targ.AddChild(scomp.Type());
		TransfTlNode(dcomp, scomp, false);
	    } else {
		// Try to find corresponging segment in adjacent mutation and add cur mut to it
		targ = getAdjSeg(scomp, sprev, aDst);
		if (!targ.IsNil()) {
		    if (targ.Type() == ENt_Seg) {
			dcomp = targ.AddChild(scomp.Type());
			TransfTlNode(dcomp, scomp, false);
		    } else {
			// Target is just simple mut. Adding segment.
			ChromoNode targOwner = *targ.Parent(); 
			//targOwner.RmChild(targ, true);
			ChromoNode seg = targOwner.AddChild(ENt_Seg);
			seg.SetAttr(ENa_Targ, targ.Attr(ENa_Targ));
			ChromoNode newTarg = seg.AddChild(targ, true, true);
			newTarg.RmAttr(ENa_Targ);
			targOwner.RmChild(targ);
			dcomp = seg.AddChild(scomp.Type());
			TransfTlNode(dcomp, scomp, false);
			sprev = seg;
		    }
		} else {
		    GUri shiftUri;
		    targ = getAdjOwnSeg(scomp, sprev, aDst, shiftUri);
		    if (!targ.IsNil()) {
			if (targ.Type() == ENt_Seg) {
			    dcomp = targ.AddChild(scomp.Type());
			    TransfTlNode(dcomp, scomp, false);
			    dcomp.SetAttr(ENa_Targ, shiftUri);
			} else {
			    // Target is just simple mut. Adding segment.
			    ChromoNode targOwner = *targ.Parent(); 
			    ChromoNode seg = targOwner.AddChild(ENt_Seg);
			    seg.SetAttr(ENa_Targ, targ.Attr(ENa_Targ));
			    ChromoNode newTarg = seg.AddChild(targ, true, true);
			    newTarg.RmAttr(ENa_Targ);
			    targOwner.RmChild(targ);
			    dcomp = seg.AddChild(scomp.Type());
			    TransfTlNode(dcomp, scomp, false);
			    dcomp.SetAttr(ENa_Targ, shiftUri);
			    sprev = seg;
			}
		    } else {
			dcomp = aDst.AddChild(scomp.Type());
			TransfTlNode(dcomp, scomp, true);
			sprev = dcomp;
		    }
		}
	    }
	}
    }
}

TBool Chromo2::operator==(const Chromo2& b)
{
    return mMdl == b.mMdl;
}
