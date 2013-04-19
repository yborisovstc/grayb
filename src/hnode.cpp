#include "mutelem.h"
#include "mprov.h"
#include "chromo.h"


set<string> MutElem::iCompsTypes;
bool MutElem::iInit = false;

void MutElem::Init()
{
    if (!iInit) {
	iCompsTypes.insert("MutElem");
	iInit = true;
    }
}

MutElem::MutElem(const string &aName, Elem* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv), iMut(NULL), iChromo(NULL)
{
    iMut = Provider()->CreateChromo();
    iMut->Init(ENt_Node);
    iChromo = Provider()->CreateChromo();
    iChromo->Init(ENt_Node);
    ChromoNode croot = iChromo->Root();
    croot.SetAttr(ENa_Id, iName);
}

void* MutElem::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    return (strcmp(aName, Type()) == 0) ? this : NULL;
}

void MutElem::SetMutation(const ChromoNode& aMuta)
{
    iMut->Set(aMuta);
}

void MutElem::Mutate(TBool aRunTimeOnly)
{
    ChromoNode& root = iMut->Root();
    DoMutation(root, aRunTimeOnly);
    // Clear mutation
    for (ChromoNode::Iterator mit = root.Begin(); mit != root.End();)
    {
	ChromoNode node = *mit;
	mit++; // It is required because removing node by iterator breakes iterator itself
	root.RmChild(node);
    }
}


void MutElem::DoMutation(const ChromoNode& aMutSpec, TBool aRunTime)
{
    const ChromoNode& mroot = aMutSpec;
    ChromoNode& chrroot = iChromo->Root();
    for (ChromoNode::Const_Iterator rit = mroot.Begin(); rit != mroot.End(); rit++)
    {
	TBool res = EFalse;
	ChromoNode rno = (*rit);
	TNodeType rnotype = rno.Type();
	if (rnotype == ENt_Node) {
	    res = AddNode(rno);
	    if (!res) {
		string pname = rno.Attr(ENa_Parent);
		Logger()->WriteFormat("ERROR: Node [%s] - adding node of type [%s] failed", Name().c_str(), pname.c_str());
	    }
	}
	else if (rnotype == ENt_Add) {
	    MutAddNode(rno);
	}
	/*
	   else if (rnotype == ENt_Rm) {
	   string snode = rno.Attr(ENa_MutNode);
	   DesUri unode(snode);
	   RmNode(unode);
	   }
	   else if (rnotype == ENt_Change) {
	   ChangeAttr_v2(rno);
	   }
	   else if (rnotype == ENt_MutChangeCont) {
	   ChangeCont_v2(rno);
	   }
	   else if (rnotype == ENt_MutMove) 
	   {
	   MoveElem_v1(rno);
	   }
	   */
	else {
	    Logger()->WriteFormat("ERROR: Mutating node [%s] - unknown mutation type [%d]", Name().c_str(), rnotype);
	}
    }
}


void MutElem::MutAddNode(const ChromoNode& aSpec)
{
    TBool res = ETrue;
    string snode = aSpec.Attr(ENa_MutNode);
    GUri unode(snode);
    MElem* node = GetNode(unode);
    if (node != NULL) {
	for (ChromoNode::Const_Iterator mit = aSpec.Begin(); mit != aSpec.End() && res; mit++)
	{
	    const ChromoNode& mno = (*mit);
	    res = node->AddNode(mno);
	}
	if (!res) {
	    Logger()->WriteFormat("ERROR: Adding node into [%s] - failure", snode.c_str());
	}
    }
    else {
	Logger()->WriteFormat("ERROR: Adding node: cannot find [%s]", snode.c_str());
    }
}

const set<string>& MutElem::CompsTypes()
{
    return iCompsTypes;
}

