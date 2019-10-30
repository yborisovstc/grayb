
#include "chromo2.h"
#include "chromox.h"

const string C2MdlCtxNode::KT_Target = "<";
const string C2MdlCtxNode::KT_Node = "-";
const string C2MdlCtxNode::KT_Namespace = "@";

static const string KR_CREATE = ":";

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
}

TNodeType Chromo2Mdl::GetType(const string& aId)
{
}

THandle Chromo2Mdl::Root(const THandle& aHandle)
{
}

THandle Chromo2Mdl::Parent(const THandle& aHandle)
{
}

THandle Chromo2Mdl::Next(const THandle& aHandle, TNodeType aType)
{
}

THandle Chromo2Mdl::Prev(const THandle& aHandle, TNodeType aType)
{
}

THandle Chromo2Mdl::GetFirstChild(const THandle& aHandle, TNodeType aType)
{
}

THandle Chromo2Mdl::GetLastChild(const THandle& aHandle, TNodeType aType)
{
}

char* Chromo2Mdl::GetAttr(const THandle& aHandle, TNodeAttr aAttr) const
{
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
    THandle handle = mMdlX->SetFromFile(aFileName);
    if (handle != NULL && mMdlX->GetType(handle) == ENt_Node) {
	// Root, evolve the chromo
	HandleXNode(handle, mRoot);
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




Chromo2::Chromo2()
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
