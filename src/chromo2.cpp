
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

void C2MdlNode::AddContext(const string& aType, const string& aValue)
{
    mContext.push_back(C2MdlCtxNode(aType, aValue));
}

C2MdlNode Chromo2Mdl::CreateNodeMut(const void* aHandle, TNodeType aR, TNodeAttr aP, TNodeAttr aQ)
{
    C2MdlNode node;
    node.mMut.mR = KNtToR.at(aR);
    node.mMut.mP = mMdlX->GetAttr(aHandle, aP);
    node.mMut.mQ = mMdlX->GetAttr(aHandle, aQ);
    return node;
}

C2MdlNode Chromo2Mdl::CreateNodeChr(const void* aHandle)
{
    C2MdlNode node;
    void* handle = mMdlX->GetFirstChild(aHandle);
    do {
	C2MdlNode child_node = CreateNodeMut(handle, ENt_Node, ENa_Id, ENa_Parent);
	node.mChromo.push_back(child_node);
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

TNodeType Chromo2Mdl::GetType(const void* aHandle)
{
}

TNodeType Chromo2Mdl::GetType(const string& aId)
{
}

void* Chromo2Mdl::Root(const void* aHandle)
{
}

void* Chromo2Mdl::Parent(const void* aHandle)
{
}

void* Chromo2Mdl::Next(const void* aHandle, TNodeType aType)
{
}

void* Chromo2Mdl::NextText(const void* aHandle)
{
}

void* Chromo2Mdl::Prev(const void* aHandle, TNodeType aType)
{
}

void* Chromo2Mdl::GetFirstChild(const void* aHandle, TNodeType aType)
{
}

void* Chromo2Mdl::GetLastChild(const void* aHandle, TNodeType aType)
{
}

char* Chromo2Mdl::GetAttr(const void* aHandle, TNodeAttr aAttr) const
{
}

void  Chromo2Mdl::GetAttr(const void* aNode, TNodeAttr aType, TInt& aVal) const
{
}

TBool Chromo2Mdl::AttrExists(const void* aHandle, TNodeAttr aAttr) const 
{
}

void* Chromo2Mdl::AddChild(void* aParent, TNodeType aType)
{
}

void* Chromo2Mdl::AddChild(void* aParent, const void* aHandle, TBool aCopy, TBool aRecursively)
{
}

void* Chromo2Mdl::AddChildDef(void* aParent, const void* aHandle, TBool aCopy)
{
}

void* Chromo2Mdl::AddNext(const void* aPrev, const void* aHandle, TBool aCopy)
{
}

void* Chromo2Mdl::AddNext(const void* aPrev, TNodeType aNode)
{
}

void* Chromo2Mdl::AddPrev(const void* aNext, const void* aHandle, TBool aCopy)
{
}

void Chromo2Mdl::RmChild(void* aParent, void* aChild, TBool aDeattachOnly)
{
}

void Chromo2Mdl::Rm(void* aHandle)
{
}

void Chromo2Mdl::MoveNextTo(void* aHandle, void* aDest)
{
}

void Chromo2Mdl::MovePrevTo(void* aHandle, void* aDest)
{
}

void Chromo2Mdl::MoveToEnd(void* aHandle)
{
}

void Chromo2Mdl::SetAttr(void* aNode, TNodeAttr aType, const char* aVal)
{
}

void Chromo2Mdl::SetAttr(void* aNode, TNodeAttr aType, TInt aVal)
{
}

void Chromo2Mdl::RmAttr(void* aNode, TNodeAttr aType)
{
}

void Chromo2Mdl::Dump(void* aNode, MLogRec* aLogRec)
{
}

TBool Chromo2Mdl::ToString(void* aNode, string& aString) const
{
}

void Chromo2Mdl::Save(const string& aFileName) const
{
}

void* Chromo2Mdl::Find(const void* aHandle, const string& aUri)
{
}

TInt Chromo2Mdl::GetOrder(void* aHandle, TBool aTree) const
{
}

void Chromo2Mdl::SetOrder(void* aHandle, TInt aOrder, TBool aTree)
{
}

void Chromo2Mdl::DeOrder(void* aHandle)
{
}

TInt Chromo2Mdl::GetLineId(void* aHandle) const
{
}



int Chromo2Mdl::GetAttrInt(void *aHandle, const char *aName)
{
}

void* Chromo2Mdl::SetFromFile(const string& aFileName)
{
    void* handle = mMdlX->SetFromFile(aFileName);
    if (handle != NULL && mMdlX->GetType(handle) == ENt_Node) {
	// Root, evolve the chromo
	HandleXNode(handle);
    }
}

void* Chromo2Mdl::Set(const string& aUri)
{
}

void* Chromo2Mdl::SetFromSpec(const string& aSpec)
{
}

void* Chromo2Mdl::Set(const void* aHandle)
{
}

void* Chromo2Mdl::Init(TNodeType aRootType)
{
}

void Chromo2Mdl::Reset()
{
}

void Chromo2Mdl::HandleXNodeCtx(C2MdlNode& aMdlNode, const void* aHandle)
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

void Chromo2Mdl::HandleXNode(const void* aHandle)
{
    TNodeType xntype = mMdlX->GetType(aHandle);
    // Mutation
    if (xntype == ENt_Node) {
	C2MdlNode node = CreateNodeMut(aHandle, ENt_Node, ENa_Id, ENa_Parent);
	HandleXNodeCtx(node, aHandle);
	mNodes.push_back(node);
	void* child_h = mMdlX->GetFirstChild(aHandle);
	if (child_h != NULL) {
	    // Chromo node
	    C2MdlNode chromo_node = CreateNodeChr(aHandle);
	    // Add node name as target context for chromo
	    string name = mMdlX->GetAttr(aHandle, ENa_Id);
	    chromo_node.AddContext(C2MdlCtxNode::KT_Target, name);
	    mNodes.push_back(chromo_node);
	}
    }
}




Chromo2::Chromo2(): mRootNode(reinterpret_cast<MChromoMdl&>(mMdl), NULL)
{
}

Chromo2::Chromo2(const Chromo2& aSrc)
{
    Set(aSrc.Root());
}

void Chromo2::SetFromFile(const string& aFileName)
{
    void *root = mMdl.SetFromFile(aFileName);
    mRootNode = ChromoNode(mMdl, root);
}

TBool Chromo2::Set(const string& aUri)
{
    TBool res = EFalse;
    void *root = mMdl.Set(aUri);
    if (root != NULL) {
	mRootNode = ChromoNode(mMdl, root);
	res = ETrue;
    }
    return res;
}

TBool Chromo2::SetFromSpec(const string& aSpec)
{
    TBool res = EFalse;
    void *root = mMdl.SetFromSpec(aSpec);
    if (root != NULL) {
	mRootNode = ChromoNode(mMdl, root);
	res = ETrue;
    }
    return res;

}

void Chromo2::Set(const ChromoNode& aRoot)
{
    void *root = mMdl.Set(aRoot.Handle());
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
    void *root = mMdl.Init(aRootType);
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

ChromoNode Chromo2::CreateNode(void* aHandle)
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
