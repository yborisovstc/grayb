#include "mutelem.h"
#include "chromo.h"

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
	ChromoNode rno = (*rit);
	TNodeType rnotype = rno.Type();
	if (rnotype == ENt_Add) {
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
	    Logger()->WriteFormat("ERROR: Mutating object [%s] - unknown mutation type [%d]", Name().c_str(), rnotype);
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
