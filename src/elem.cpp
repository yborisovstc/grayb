#include "elem.h"
#include "chromo.h"
#include "mprov.h"
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <sys/time.h>
#include <stdexcept> 
#include <iostream> 
#include "mmod.h"
#include "ifu.h"
#include "profiler_events.h"

TBool Elem::EN_MUT_LIM = ETrue;

const string Elem::KCont_About = "About";
const string MElem::KCont_Categories = "Categories";
const string MElem::KCont_Ctg_Readonly = "Readonly";
const string MElem::KCont_Ctg_Debug = "Debug";

string Elem::Fmt::mSepContInp = ";";
string Elem::Fmt::mSepContName = "~";
// TODO [YB] Alternative method via static Ifu derived is used, remaining for info only
//string IfuPars = "Name,0,GetNode,1,Mutate,4";
//Ifu Elem::mIfu(IfuPars);


MElem::EIfu MElem::mIfu;

// Ifu static initialisation
MElem::EIfu::EIfu()
{
    RegMethod("Name", 0);
    RegMethod("GetMan", 0);
    RegMethod("GetCtx", 0);
    RegMethod("GetAttachedMgr", 0);
    RegMethod("GetAowner", 0);
    RegMethod("GetCompAowner", 1);
    RegMethod("GetRoot", 0);
    RegMethod("GetNode", 2);
    RegMethod("GetNode#2", 3);
    RegMethod("GetContent", 2);
    RegMethod("Mutate", 4);
    RegMethod("Mutate#2", 4);
    RegMethod("IsProvided", 0);
    RegMethod("GetParent", 0);
    RegMethod("GetChromoSpec", 0);
    RegMethod("EType", 1);
    RegMethod("DoGetObj", 1);
    RegMethod("GetSIfi", 2);
    RegMethod("SetObserver", 1);
    RegMethod("OnCompAdding", 1);
    RegMethod("OnCompChanged", 2);
    RegMethod("OnCompDeleting", 2);
    RegMethod("OnChanged", 1);
    RegMethod("CompsCount", 0);
    RegMethod("GetComp", 1);
    RegMethod("GetComp#2", 2);
    RegMethod("AppendMutation", 1);
    RegMethod("AppendMutation#2", 1);
    RegMethod("IsRemoved", 0);
    RegMethod("IsHeirOf", 1);
    RegMethod("IsComp", 1);
    RegMethod("OnNodeMutated", 3);
    RegMethod("IsCompAttached", 1);
    RegMethod("GetCompLrank", 1);
    RegMethod("GetCRoot", 0);
    RegMethod("IsChromoAttached", 0);
    RegMethod("RemoveChild", 1);
    RegMethod("AppendChild", 1);
    RegMethod("AppendComp", 1);
    RegMethod("RemoveComp", 1);
    RegMethod("OnChildDeleting", 1);
    RegMethod("ContentExists", 1);
    RegMethod("ContValueExists", 1);
    RegMethod("GetContCount", 1);
    RegMethod("GetContComp", 2);
    RegMethod("SetParent", 1);
    RegMethod("IsContOfCategory", 2);
}


string Elem::PEType()
{
    return string() + GUri::KParentSep + Elem::Type();
}

// Element

/** Constructor creating inherited agent: name is defined, parent is Type() */
Elem::Elem(const string &aName, MUnit* aMan, MEnv* aEnv): Unit(aName, aMan, aEnv), iParent(NULL) 
{
    Pdur(PEvents::Dur_Elem_Constr_Start, this);
    //Pdstat(PEvents::DurStat_Elem_Constr, true);
    Pdstat(PEvents::DurStat_Elem_Constr_Chromo, true);
    iMut = Provider()->CreateChromo();
    iMut->Init(ENt_Node);
    iChromo = Provider()->CreateChromo();
    iChromo->Init(ENt_Node);
    SetCrAttr(PEType(), aName);
    /*
       ChromoNode croot = iChromo->Root();
       string parent;
       if (aName.empty()) {
       iName = Type();
       } else {
       parent = Type();
       }
       croot.SetAttr(ENa_Id, iName);
       croot.SetAttr(ENa_Parent, parent);
       */
    Pdstat(PEvents::DurStat_Elem_Constr_Chromo, false);
    InsertContent(KCont_About);
    Pdstat(PEvents::DurStat_Elem_Constr, false);
    //Pdur(PEvents::Dur_Elem_Constr, this);
    //InsertContent(KCont_Categories);
    //InsertContent(KCont_Ctg_Readonly);
}

void Elem::SetCrAttr(const string& aEType, const string& aName)
{
    ChromoNode croot = iChromo->Root();
    string ptype;
    if (aName.empty()) {
	// Native base agent, its name is cpp class type
	iName = GetType(aEType);
	ptype = GetPType(aEType);
    } else {
	// Inherited native agent, its name is given, type is class extended type
	iName = aName;
	// Using short type for parent to be compatible with current version
	// Needs to consider to use full type
	// ptype = aEType;
	ptype = GetType(aEType);
    }
    croot.SetAttr(ENa_Id, iName);
    croot.SetAttr(ENa_Parent, ptype);
}

Elem::~Elem() 
{
    // Notify the parent of child deleting
    MElem* eparent = iParent == NULL ? NULL : iParent->GetObj(eparent);
    if (eparent != NULL) {
	eparent->OnChildDeleting(this);
    }
    // Disconnect from the childs
    for (TNEReg::iterator it = iChilds.begin(); it != iChilds.end(); it++) {
	MUnit* child = it->second;
	MElem* echild = child->GetObj(echild);
	echild->OnParentDeleting(this);
	//child->SetParent(NULL);
    }
    iChilds.clear();
    if (iMut != NULL) {
	delete iMut;
	iMut = NULL;
    }
    if (iChromo != NULL) {
	// Remove chromo only if it is deattached from parent's chromo. Ref UC_016. Normally the parent does
	// not deattach the childs chromo even if the child is deleted. This allows to keep node creation 
	// mutation in parents chromo even if the child is deleted then. 
	ChromoNode::Iterator pint = iChromo->Root().Parent();
	if (pint == iChromo->Root().End()) {
	    delete iChromo;
	}
	iChromo = NULL;
    }
}

string Elem::PName() const
{
    ChromoNode croot = iChromo->Root();
    return croot.Attr(ENa_Parent);
}

MUnit* Elem::GetUnit()
{
    return dynamic_cast<MUnit*>(this);
}

void Elem::SetParent(const string& aParent)
{
    ChromoNode croot = iChromo->Root();
    croot.SetAttr(ENa_Parent, aParent);
}

void Elem::GetCRoot(TMut& aMut) const
{
    aMut = iChromo->Root();
}

MIface *Elem::DoGetObj(const char *aName)
{
    MIface* res = NULL;
    if (strcmp(aName, MElem::Type()) == 0) {
	res = dynamic_cast<MElem*>(this);
    } else {
	res = Unit::DoGetObj(aName);
    }
    return res;
}

MIface* Elem::getLocalIface(const string& aName, const TICacheRCtx& aCtx)
{
    MIface* res = (MIface*) DoGetObj(aName.c_str());
    if (res != NULL) {
	InsertIfCache(aName, aCtx, this, res);
    }
    return res;
}

string Elem::EType(TBool aShort) const
{
    if (iParent == NULL) {
	return string();
    }
    if (aShort) {
	return iParent->Name();
    }
    else {
	return iParent->EType(aShort) + GUri::KParentSep + iParent->Name();
    }
}

void Elem::SetMutation(const ChromoNode& aMuta)
{
    iMut->Set(aMuta);
}

void Elem::SetMutation(const string& aMutSpec)
{
    iMut->SetFromSpec(aMutSpec);
}

ChromoNode Elem::AppendMutation(const ChromoNode& aMuta)
{
    return iMut->Root().AddChild(aMuta);
}

ChromoNode Elem::AppendMutation(TNodeType aType)
{
    return iMut->Root().AddChild(aType);
}

TBool Elem::AppendMutation(const string& aFileName)
{
    TBool res = EFalse;
    MChromo *spec = Provider()->CreateChromo();
    res = spec->Set(aFileName);
    if (res) {
	iMut->Root().AddChild(spec->Root(), ETrue);
	res = ETrue;
    }
    delete spec;
    return res;
}

void Elem::Mutate(TBool aRunTimeOnly, TBool aCheckSafety, TBool aTrialMode, const MutCtx& aCtx)
{
    ChromoNode& root = iMut->Root();
    MutCtx mctx(aCtx.mUnit == NULL ? this : aCtx.mUnit, aCtx.mNs);
    DoMutation(root, aRunTimeOnly, aCheckSafety, aTrialMode, mctx);
    // Clear mutation
    ChromoNode::Iterator mit = root.Begin();
    while (mit != root.End()) {
	ChromoNode node = *mit;
	root.RmChild(node);
	mit = root.Begin();
    }
}

void Elem::Mutate(const ChromoNode& aMutsRoot, TBool aRunTimeOnly, TBool aCheckSafety, TBool aTrialMode, const MutCtx& aCtx)
{
    MutCtx mctx(aCtx.mUnit == NULL ? this : aCtx.mUnit, aCtx.mNs);
    DoMutation(aMutsRoot, aRunTimeOnly, aCheckSafety, aTrialMode, mctx);
}

void Elem::DoSpecificMut(const ChromoNode& aSpec, TBool aRunTime, TBool aTrialMode, const MutCtx& aCtx)
{
    TNodeType rnotype = aSpec.Type();
    Logger()->Write(EErr, this, "Mutating - unknown mutation type [%d]", rnotype);
}

void Elem::DoMutation(const ChromoNode& aMutSpec, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode, const MutCtx& aCtx)
{
    bool res = true;
    const ChromoNode& mroot = aMutSpec;
    if (mroot.Begin() == mroot.End()) return;
    TInt tord = 0;
    TInt lim = 0;
    TInt order = 0;
    TBool isattached = EFalse;
    if (EN_MUT_LIM && this == GetRoot()) {
	tord = iEnv->ChMgr()->GetSpecMaxOrder();
	lim = iEnv->ChMgr()->GetLim();
	isattached = IsChromoAttached();
    }
    // TODO No need to update root ns. Root mutation is not deleted. To analyze if this is correct
    TNs root_ns = aCtx.mNs;
    //UpdateNs(root_ns, sroot);

    MUnit* btarg = this; // Base target
    string sbtarg;
    if (mroot.AttrExists(ENa_Targ)) {
	sbtarg = mroot.Attr(ENa_Targ);
	btarg = GetNodeByName(sbtarg, root_ns);
	if (btarg == NULL) {
	    Logger()->Write(EErr, this, "Cannot find base target node [%s]", sbtarg.c_str());
	    res = false;
	}
    }
    for (ChromoNode::Const_Iterator rit = mroot.Begin(); rit != mroot.End() && res; rit++, order++)
    {
	Pdstat(PEvents::DurStat_TransfOsm, true);
	ChromoNode rno = (*rit);
	// Omit inactive mutations
	if (iEnv->ChMgr()->EnableOptimization() && rno.AttrExists(ENa_Inactive)) {
	    if (!aRunTime) {
		iChromo->Root().AddChild(rno);
	    }
	    continue;
	}
	TNs ns = aCtx.mNs;
	UpdateNs(ns, rno);
	Logger()->SetContextMutId(rno.LineId());
	if (EN_MUT_LIM && this == GetRoot()) {
	    // Avoiding mutations above limit. Taking into account only attached chromos.
	    if (EN_MUT_LIM && isattached && tord > 0 && order > tord - lim) {
		if (!aRunTime && !aTrialMode) {
		    iChromo->Root().AddChild(rno);
		}
		continue;
	    }
	}
	// Get target node by analysis of mut-target and mut-node, ref ds_chr2_cctx_tn_umt
	MUnit* ftarg = btarg;
	MElem* eftarg = this; // Mutable target
	MUnit* aoftarg = NULL; // Attached owner of target
	bool exs_targ = rno.AttrExists(ENa_Targ);
	bool exs_mnode = rno.AttrExists(ENa_MutNode);
	string starg, smnode;
	if (rno.AttrExists(ENa_Id) && rno.Name() == "unit1_1_1") {
	    Logger()->Write(EInfo, this, "BP");
	}
	if (exs_targ) {
	    starg = rno.Attr(ENa_Targ);
	    ftarg = btarg->GetNodeByName(starg, ns);
	    if (ftarg == NULL) {
		Logger()->Write(EErr, this, "Cannot find target node [%s]", starg.c_str());
		continue;
	    }
	}
	if (exs_mnode) {
	    // Transform DHC mutation to OSM mutation
	    // Transform ENa_Targ: enlarge to ENa_MutNode
	    smnode = rno.Attr(ENa_MutNode);
	    ftarg = ftarg->GetNodeByName(smnode, ns);
	    if (ftarg == NULL) {
		Logger()->Write(EErr, this, "Cannot find mut node [%s]", smnode.c_str());
		continue;
	    }
	}
	Pdstat(PEvents::DurStat_TransfOsm, false);
	if (ftarg != this) {
	    // Targeted mutation
	    eftarg = ftarg->GetObj(eftarg);
	    aoftarg = ftarg;
	    if (eftarg == NULL) {
		// Target is not mutable, redirect to mutable owner
		// TODO Should the mut be redirected to attached owner but not just mutable?
		aoftarg = GetMowner(ftarg);
		eftarg = aoftarg->GetObj(eftarg);
	    }
	    if (eftarg != NULL) {
		if (ftarg != aoftarg) {
		    string newTargUri = ftarg->GetUri(aoftarg, false);
		    rno.SetAttr(ENa_Targ, newTargUri);
		} else {
		    rno.RmAttr(ENa_Targ);
		}
		rno.RmAttr(ENa_NS);
		rno.RmAttr(ENa_MutNode);
		if (!rno.AttrExists(ENa_Targ)) {
		    // Correcting target if the mut is component related
		    TNodeType mtype = rno.Type();
		    if (mtype == ENt_Change || mtype == ENt_Rm) {
			// Mutation is for component only, find the comp mutable owner
			aoftarg = GetMowner(ftarg);
			eftarg = aoftarg->GetObj(eftarg);
			if (ftarg != aoftarg) {
			    string newTargUri = ftarg->GetUri(aoftarg, false);
			    rno.SetAttr(ENa_Targ, newTargUri);
			}
		    }
		}
		/* TODO this is wrong impl of ds_umt_rtnsu_rbs, to remove
		// Rebase refs to final target, ref ds_umt_rtnsu_rbs
		if (aoftarg != ftarg) {
		    GUri fpuri = ftarg->GetUri(aoftarg, true);
		    if (rno.AttrExists(ENa_Parent)) {
			string prnturi = rno.Attr(ENa_Parent);
			fpuri.Append(prnturi);
			rno.SetAttr(ENa_Parent, spuri);
		    }
		}
		*/
	    } else {
		string ftarg_uri = ftarg->GetUri(NULL, false);
		Logger()->Write(EErr, this, "Cannot find mutable target for [%s]", ftarg_uri.c_str());
	    }
	} else {
	    // Local mutation
	    rno.RmAttr(ENa_Targ);
	}
	if (eftarg != this) {
	    // Redirect the mut to target: no run-time to keep the mut in internal nodes
	    // Propagate till target owning comp if run-time to keep hidden all muts from parent
	    ChromoNode madd = eftarg->AppendMutation(rno);
	    MutCtx mctx(aRunTime ? GetCompOwning(ftarg) : aCtx.mUnit, ns);
	    eftarg->Mutate(EFalse, aCheckSafety, aTrialMode, mctx);
	    //ftarg->Mutate(aRunTime, aCheckSafety, aTrialMode, aRunTime ? GetCompOwning(ftarg) : aCtx);
	} else {
	    // Local mutation
	    TNodeType rnotype = rno.Type();
	    if (rnotype == ENt_Node) {
		AddElem(rno, aRunTime, aTrialMode, aCtx);
	    }
	    else if (rnotype == ENt_Seg) {
		Mutate(rno, aRunTime, aCheckSafety, aTrialMode, aCtx);
	    }
	    else if (rnotype == ENt_Change) {
		Pdstat(PEvents::DurStat_MutChange, true);
		ChangeAttr(rno, aRunTime, aCheckSafety, aTrialMode, aCtx);
		Pdstat(PEvents::DurStat_MutChange, false);
	    }
	    else if (rnotype == ENt_Cont) {
		Pdstat(PEvents::DurStat_MutCont, true);
		DoMutChangeCont(rno, aRunTime, aCheckSafety, aTrialMode, aCtx);
		Pdstat(PEvents::DurStat_MutCont, false);
	    }
	    else if (rnotype == ENt_Move) {
		MoveNode(rno, aRunTime, aTrialMode, aCtx);
	    }
	    else if (rnotype == ENt_Import) {
		ImportNode(rno, aRunTime, aTrialMode);
	    }
	    else if (rnotype == ENt_Rm) {
		RmNode(rno, aRunTime, aCheckSafety, aTrialMode, aCtx);
	    }
	    else if (rnotype == ENt_Note) {
		// Comment, just accept
		iChromo->Root().AddChild(rno);
		NotifyNodeMutated(rno, aCtx);
	    }
	    else {
		DoSpecificMut(rno, aRunTime, aTrialMode, aCtx);
	    }
	    Logger()->SetContextMutId();
	}
    }
}

void Elem::ChangeAttr(const ChromoNode& aSpec, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode, const MutCtx& aCtx)
{
    __ASSERT(!aSpec.AttrExists(ENa_Comp));
    __ASSERT(!aSpec.AttrExists(ENa_MutNode));
    TBool epheno = iEnv->ChMgr()->EnablePhenoModif();
    string mattrs = aSpec.Attr(ENa_MutAttr);
    string mval = aSpec.Attr(ENa_MutVal);
    string snode;
    MUnit* node = this;
    if (aSpec.AttrExists(ENa_Targ)) {
	snode = aSpec.Attr(ENa_Targ);
	node = GetNode(snode);
    }
    MElem* enode = node != NULL ? node->GetObj(enode) : NULL;
    TBool mutadded = EFalse;
    if (enode != NULL) {
	if (node != this && (epheno || enode->GetAowner() == this  || IsDirectInheritedComp(node))) {
	    TBool res = node->ChangeAttr(TMut::NodeAttr(mattrs), mval);
	    if (!res) {
		Logger()->Write(EErr, this, "Changing node [%s] - failure", snode.c_str());
	    } else {
		// Adding dependency to object of change
		if (!aRunTime) {
		    ChromoNode chn = iChromo->Root().AddChild(aSpec);
		    NotifyNodeMutated(aSpec, aCtx);
		    mutadded = ETrue;
		}
	    }
	} else {
	    Logger()->Write(EErr, this, "Changing node [%s]  - attempt of phenotypic modification - disabled", snode.c_str());
	}
    } else {
	Logger()->Write(EErr, this, "Changing node [%s] - cannot find node or node isn't component", snode.c_str());
    }
    // Append mutation to chromo anytype, ref uc_043
    if (!aRunTime && !mutadded && !aTrialMode) {
	iChromo->Root().AddChild(aSpec);
	NotifyNodeMutated(aSpec, aCtx);
    }
}

TBool Elem::ChangeAttr(TNodeAttr aAttr, const string& aVal)
{
    TBool res = EFalse;
    if (aAttr == ENa_Id) {
	string sOldName(Name());
	res = Unit::ChangeAttr(aAttr, aVal);
	if (res) {
	    MElem* eparent = (iParent == NULL) ? NULL : iParent->GetObj(eparent);
	    if (eparent != NULL) {
		res = eparent->OnChildRenamed(this, sOldName);
	    }
	    if (!res) {
		// Rollback
		iName = sOldName;
	    }
	}
    }
    return res;
}

TBool Elem::DoMutChangeCont(const ChromoNode& aSpec, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode, const MutCtx& aCtx)
{
    //__ASSERT(!aSpec.AttrExists(ENa_MutNode));
    __ASSERT(!aSpec.AttrExists(ENa_Comp));
    TBool res = ETrue;
    TBool epheno = iEnv->ChMgr()->EnablePhenoModif();
    TBool efix = iEnv->ChMgr()->EnableFixErrors();
    TBool erepos = iEnv->ChMgr()->EnableReposMuts();
    string snode = aSpec.Attr(ENa_Targ);
    string cname = aSpec.Attr(ENa_Id);
    TBool refex = aSpec.AttrExists(ENa_Ref);
    string mval = aSpec.Attr(refex ? ENa_Ref :ENa_MutVal);
    MUnit* node = NULL;
    MElem* enode = NULL;
    MUnit* rnode = NULL;
    TBool mutadded = EFalse;
    TNs ns = aCtx.mNs;
    if (snode.length() == 0) {
	node = this; 
    }
    else {
	GUri unode(snode);
	node = GetNodeByName(unode, ns);
	enode = node != NULL ? node->GetObj(enode) : NULL;
    }
    if (node != NULL && (node == this || IsComp(node))) {
	// Only direct inherited comp is available for modif, ref. uc_044_disc_3
	if (epheno || node == this || (!aCheckSafety || IsDirectInheritedComp(node))) {
	    if (refex) {
		// For -ref- attr the value is the ref relative to mutated node context
		rnode = node->GetNode(mval);
		//mval = rnode->GetRUri(node);
		if (rnode == NULL) {
		    rnode = node->GetNode(mval);
		    Logger()->Write(EErr, this, aSpec,
			    "Changing content of node [%s] to ref [%s] - cannot find ref", snode.c_str(), mval.c_str());
		    res = EFalse;
		}
	    }
	    if (res) {
		// Ref ds_mut_osm_linchr_notif. With OSM (Original Sequence of Mutation) approach aRunTime can be set false
		// even if the mutation is parents mutation (inherited mutation) in order to keep all nodes chromo.
		// So we need to re-calculate run-time property: only if the mut is not to be included in root chromo
		// Mark change as persistent (not run-time) only if the change affects model chromo
		// TODO [YB] To cleanup, to remove run-time arg from notifications
		TBool persist = ETrue;
		// TODO actually the node content will be changed even if returned false. This is design issue.
		// We need to not perform actual change is owner doesn't accept the changhe
		res = node->ChangeCont(mval, !persist, cname);
		if (res) {
		    if (!aRunTime) {
			ChromoNode chn = iChromo->Root().AddChild(aSpec);
			NotifyNodeMutated(aSpec, aCtx);
			mutadded = ETrue;
		    }
		} else {
		    Logger()->Write(EErr, this, aSpec, "Changing node [%s] - failure", snode.c_str());
		    node->ChangeCont(mval, aRunTime, cname);
		}
	    }
	}
	else  {
	    Logger()->Write(EErr, this, "Changing content of node [%s]  - attempt of phenotypic modification - disabled", 
		    snode.c_str());
	}
    }
    else {
	Logger()->Write(EErr, this, "Changing node [%s] - cannot find node or node isn't comp", snode.c_str());
    }
    // Append mutation to chromo anytype, ref uc_043
    // TODO To update trial logic - to refuse adding mutation if trial and mutation failed
    if (!aRunTime && !mutadded && !aTrialMode) {
	ChromoNode mut = iChromo->Root().AddChild(aSpec);
	NotifyNodeMutated(aSpec, aCtx);
	TInt mutid = mut.LineId();
    }
    return res;
}

MUnit* Elem::AddElem(const ChromoNode& aNode, TBool aRunTime, TBool aTrialMode, const MutCtx& aCtx)
{
    //__ASSERT(!aNode.AttrExists(ENa_MutNode));
    __ASSERT(!aNode.AttrExists(ENa_Comp));
    string snode = aNode.Attr(ENa_Targ);
    string sparent = aNode.Attr(ENa_Parent);
    string sname = aNode.Name();
    TNs ns = aCtx.mNs;
    UpdateNs(ns, aNode);
    TBool epheno = iEnv->ChMgr()->EnablePhenoModif();
    TBool erepos = iEnv->ChMgr()->EnableReposMuts();
    TBool ecsaf = iEnv->ChMgr()->EnableCheckSafety();
    TBool mutadded = EFalse;
    TBool res = EFalse;
    //Log(TLog(EInfo, this) + "Adding element [" + sname + "]");

    __ASSERT(!sname.empty());
    /*
       if (IsLogeventCreOn()) {
       Logger()->Write(EInfo, this, "Start adding node [%s:%s]", sparent.c_str(), sname.c_str());
       }
       */
    //Logger()->Write(EInfo, this, "Start adding node [%s:%s]", sparent.c_str(), sname.c_str());
    MUnit* uelem = NULL;
    MUnit* node = snode.empty() ? this: GetNodeByName(snode, ns);
    if (node != NULL) {
	// Obtain parent first
	MUnit *parent = NULL;
	// Check if the parent is specified
	if (!sparent.empty()) {
	    // Check the parent scheme
	    GUri prnturi(sparent);
	    TBool ext_parent = ETrue;
	    if (prnturi.Scheme().empty()) {
		// Local parent
		// Resolving parent ref basing on target, ref ds_umt_rtnsu_rbs
		parent = node->GetNodeByName(prnturi, ns);
		if (parent == NULL) {
		    // Probably external node not imported yet - ask env for resolving uri
		    GUri pruri(prnturi);
		    MImportMgr* impmgr = iEnv->ImpsMgr();
		    parent = impmgr->OnUriNotResolved(node, pruri);
		}
		ext_parent = EFalse;
	    }
	    else {
		// TODO [YB] To add seaching the module - it will allow to specify just file of spec wo full uri
		MChromo *spec = Provider()->CreateChromo();
		res = spec->Set(sparent);
		if (res) {
		    const ChromoNode& root = spec->Root();
		    parent = AddElem(root, false, false, aCtx);
		    delete spec;
		}
	    }
	    if (parent != NULL) {
		if (epheno || node == this || !ecsaf || IsDirectInheritedComp(node)) {
		    // Create heir from the parent
		    uelem = parent->CreateHeir(sname, node, NULL);
		    // Remove external parent from system
		    // [YB] DON'T remove parent, otherwise the inheritance chain will be broken
		    if (ext_parent) {
			// delete parent;
		    }
		    MElem* elem = uelem == NULL ? NULL : uelem->GetObj(elem);
		    if (elem != NULL) {
			// TODO [YB] Seems to be just temporal solution. To consider using context instead.
			// Make heir based on the parent: re-parent the heir (currently it's of grandparent's parent) and clean the chromo
			ChromoNode hroot = elem->Chromos().Root();
			hroot.SetAttr(ENa_Parent, sparent);
			if (!snode.empty()) {
			    hroot.SetAttr(ENa_MutNode, snode);
			}
			if (!aNode.Attr(ENa_NS).empty()) {
			    hroot.SetAttr(ENa_NS, aNode.Attr(ENa_NS));
			}
			// Heir has been created, now we can establish solid two-ways relations, ref. ds_daa_hunv
			//
			// Mark change as persistent (not run-time) only if the change affects model chromo, ref ds_mut_osm_linchr_notif.
			/*
			   TBool persist = ETrue;
			   elem->SetMan(NULL);
			   res = node->AppendComp(elem, !persist);
			   */
			elem->SetParent(NULL);
			MElem* eparent = parent == NULL ? NULL : parent->GetObj(eparent);
			res = eparent == NULL ? EFalse : eparent->AppendChild(uelem);
			if (res) {
			    if (!aRunTime) {
				// Copy just top node, not recursivelly, ref ds_daa_chrc_va
				ChromoNode ech = elem->Chromos().Root();
				ChromoNode chn = iChromo->Root().AddChild(ech, ETrue, EFalse);
				NotifyNodeMutated(ech, aCtx);
				mutadded = ETrue;
			    } else {
				mutadded = ETrue;
			    }
			    // Mutate object ignoring run-time option. This is required to keep nodes chromo even for inherited nodes
			    // To avoid this inherited nodes chromo being attached we just don't attach inherited nodes chromo
			    elem->SetMutation(aNode);
			    TNs ns(aCtx.mNs);
			    ns.push_back(uelem);
			    MutCtx ctx(aCtx.mUnit, ns);
			    elem->Mutate(EFalse, ecsaf, aTrialMode, aRunTime ? MutCtx(uelem, ns) : ctx);
			}
			else {
			    Log(TLog(EErr, this) + "Adding node [" + uelem->EType() + ":" + uelem->Name() + "] failed");
			    delete elem;
			    elem = NULL;
			}
		    }
		    else {
			// Node that doesn't support mutation. Generate error if there is mutation of the node
			if (aNode.Count() > 0) {
			    Logger()->Write(EErr, this, "Attempt to mutate node [%s] that doesn't support mutation", sname.c_str());
			}
		    }
		}
		else  {
		    TBool isi = IsInheritedComp(node);
		    Logger()->Write(EErr, this, "Creating elem [%s] in node [%s] - attempt of phenotypic modification - disabled", 
			    sname.c_str(), snode.c_str());
		}
	    }
	    else {
		Logger()->Write(EErr, this, "Creating [%s] - parent [%s] not found", sname.c_str(), sparent.c_str());
	    }
	}
    }
    else  {
	Logger()->Write(EErr, this, "Creating elem [%s] in node [%s] - cannot find node", sname.c_str(), snode.c_str());
    }
    if (!aRunTime && !mutadded && !aTrialMode) {
	ChromoNode chn = iChromo->Root().AddChild(aNode, ETrue, EFalse);
	NotifyNodeMutated(chn, aCtx);
    }
    return uelem;
}

// Reimplemented to keep system consistency. New version is based on run-time model only
// Ref uc_031, ds_rn_prnt
MUnit* Elem::CreateHeir(const string& aName, MUnit* aMan, MUnit* aContext)
{
    Pdstat(PEvents::DurStat_CreateHeir, true);
    MUnit* uheir = NULL;
    if (IsProvided()) {
	Pdstat(PEvents::DurStat_CreateNativeAgt, true);
	// TODO Needs to use GetNode instead of CreateNode?
	uheir = Provider()->CreateNode(Name(), aName, aMan, iEnv);
	MElem* heir = uheir == NULL ? NULL : uheir->GetObj(heir);
	Pdstat(PEvents::DurStat_CreateNativeAgt, false);
	// Persistently attach heir to final owner, but also set context for mutation, ref ds_daa_itn_sfo
	aMan->AppendComp(uheir);
	uheir->SetCtx(aContext);
	// TODO To move AppendComp to CreateNode: initially set two-ways ownning relation ?
	// Using "light" one-way relation on creation phase, ref. ds_daa_hunv
	MUnit* hprnt = heir->GetParent();
	MElem* ehprnt = hprnt->GetObj(ehprnt);
	ehprnt->RemoveChild(uheir);
	heir->SetParent(hprnt);
    } else {
	__ASSERT(iParent != NULL);
	uheir = iParent->CreateHeir(aName, aMan, iMan);
	MElem* heir = uheir == NULL ? NULL : uheir->GetObj(heir);
	// Mutate bare child with original parent chromo, mutate run-time only to have clean heir's chromo
	ChromoNode croot = iChromo->Root();
	heir->SetMutation(croot);
	// Mutate run-time only - !! DON'T UPDATE CHROMO, ref UC_019
	TNs ns; MutCtx mctx(uheir, ns);
	heir->Mutate(ETrue, EFalse, EFalse, mctx);
	// Mutated with parent's own chromo - so panent's name is the type now. Set also the parent, but it will be updated further
	heir->SetParent(Name());
	// Relocate heir to hier from which the request of creating heir came
	uheir->SetCtx(NULL);
	uheir->SetCtx(aContext);
	// Using full comp-owner relation, ref ds_di_cnfr_susl
	//iMan->RemoveComp(heir);
	//aMan->AppendComp(heir);
	// Using "light" one-way relation on creation phase, ref. ds_daa_hunv
	heir->SetParent(NULL);
	heir->SetParent(this);
    }
    Pdstat(PEvents::DurStat_CreateHeir, false);
    return uheir;
}

TBool Elem::AppendChild(MUnit* aChild)
{
    TBool res = RegisterChild(aChild);
    if (res) {
	MElem* echild = aChild->GetObj(echild);
	echild->SetParent(this);
    }
    return res;
}

void Elem::RemoveChild(MUnit* aChild)
{
    UnregisterChild(aChild);
    MElem* echild = aChild->GetObj(echild);
    echild->SetParent(NULL);
}

TBool Elem::RegisterChild(MUnit* aChild)
{
    TBool res = ETrue;
    for (TNEReg::iterator it = iChilds.begin(); it != iChilds.end(); it++) {
	if (it->second == aChild) {
	    __ASSERT(EFalse);
	}
    }
    iChilds.insert(TNEVal(aChild->Name(), aChild));
    return res;
}

TBool Elem::UnregisterChild(MUnit* aChild, const string& aName)
{
    TBool res = EFalse;
    const string& name = aName.empty() ? aChild->Name() : aName;
    assert (iChilds.count(name) > 0); 
    pair<TNEReg::iterator, TNEReg::iterator> range = iChilds.equal_range(name);
    TBool found = EFalse;
    for (TNEReg::iterator it = range.first; it != range.second && !found; it++) {
	if (it->second == aChild) {
	    iChilds.erase(it);
	    found = ETrue;
	}
    }
    __ASSERT(found);
    res = ETrue;
    return res;
}

MUnit* Elem::GetUpperAowner() {
    MUnit* cand = GetAowner();
    MUnit* res = NULL;
    while (cand != NULL) {
	res = cand;
	MElem* ecand = cand->GetObj(ecand);
	cand = ecand->GetAowner();
    }
    return res;
}

TBool Elem::IsAownerOf(const MUnit* aElem) const
{
    MUnit* uelem = const_cast<MUnit*>(aElem);
    MElem* elem = uelem->GetObj(elem);
    MUnit* aowner = elem->GetAowner();
    while (aowner != NULL && aowner != this) {
	MElem* eaowner = aowner->GetObj(eaowner);
	aowner = eaowner->GetAowner();	
    }
    return aowner == this;
}

TInt Elem::GetCompLrank(const MUnit* aComp) const
{
    TInt res = -1;
    const ChromoNode& croot = Chromos().Root();
    //const ChromoNode& comproot = aComp->Chromos().Root();
    TMut comproot;
    MElem* ecomp = const_cast<MUnit*>(aComp)->GetObj(ecomp);
    ecomp->GetCRoot(comproot);
    //string name = comproot.Name();
    string name = comproot.Attr(ENa_Id);
    //TBool ia = comproot.IsActive();
    TBool found = EFalse;
    for (ChromoNode::Const_Iterator it = croot.Begin(); it != croot.End() && !found; it++, res++) {
	ChromoNode mut = *it;
	//found = (mut.Type() == ENt_Node) && mut.IsActive() == ia && (mut.Name() == name);
	found = (mut.Type() == ENt_Node) && mut.IsActive() && (mut.Name() == name);
	if (found) {
	    const MUnit* targ = this;
	    if (mut.AttrExists(ENa_Targ)) {
		targ = const_cast<Elem*>(this)->GetNode(mut.Attr(ENa_Targ), ETrue);
	    }
	    const MUnit* cowner = aComp->GetMan();
	    if (cowner != targ) {
		found = EFalse;
	    }
	}
    }
    if (!found)  {
	res = -1;
    }
    return res;
}

MUnit* Elem::GetCompAowner(const MUnit* aComp)
{
    MUnit* res = NULL;
    TInt lr = GetCompLrank(aComp);
    if (lr == -1) {
	if (iMan != NULL) {
	    MElem* eowner = iMan->GetObj(eowner);
	    res = eowner->GetCompAowner(aComp);
	} else {
	    res = NULL;
	}
    } else {
	res = this;
    }
    return res;
}

const MUnit* Elem::GetCompAowner(const MUnit* aComp) const
{
    const MUnit* res = NULL;
    TInt lr = GetCompLrank(aComp);
    if (lr == -1) {
	if (iMan != NULL) {
	    MElem* eowner = iMan->GetObj(eowner);
	    res = eowner->GetCompAowner(aComp);
	} else {
	    res = NULL;
	}
    } else {
	res = this;
    }
    return res;
}

MUnit* Elem::GetInhRoot() const
{
    Elem* ethis = const_cast<Elem*>(this);
    MUnit* res = ethis;
    if (iParent != NULL) {
	MElem* eparent = iParent->GetObj(eparent);
	res = eparent->GetInhRoot();
    }
    return res;

}

TBool Elem::RmNode(const ChromoNode& aSpec, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode, const MutCtx& aCtx)
{
    TBool res = EFalse;
    TBool epheno = iEnv->ChMgr()->EnablePhenoModif();
    string snode;
    MUnit* node = this;
    if (aSpec.AttrExists(ENa_Targ)) {
	snode = aSpec.Attr(ENa_Targ);
	node = GetNode(snode);
    }
    TBool mutadded = EFalse;
    if (node != NULL) {
	if (true) {
	    // Check dependent mutations
	    res = ETrue;
	    // Just mark node as removed but not remove actually, ref ds_mut_rm_appr
	    // Refuse removing if here are heirs, needs to unparent them first, ref ds_mut_rm_deps
	    MElem* enode = node->GetObj(enode);
	    if (!enode->HasInherDeps(node)) {
		// Refuse removing if there are other critical deps (e.g. -ref-)
		// TODO [YB] To add checking refs
		if (ETrue) {
		    node->SetRemoved(aRunTime);
		} else {
		    Logger()->Write(EErr, this,
			    "Removing node [%s], refused, there is ref [%s] to the node, release first",
			    snode.c_str(), "some ref");
		}
	    } else {
		Logger()->Write(EErr, this,
			"Removing node [%s], refused, there are heirs of the node, unparent first", snode.c_str());
		enode->HasInherDeps(node);
	    }
	    if (IsLogeventCreOn()) {
		Logger()->Write(EInfo, this, "Removed elem [%s]", snode.c_str());
	    }
	} else  {
	    Logger()->Write(EErr, this, "Removing node [%s] - attempt of phenotypic modification - disabled", snode.c_str());
	    IsCompOfInheritedComp(node);
	}
    } else {
	Logger()->Write(EErr, this, "Removing node [%s] - not found or isn't component", snode.c_str());
    }
    // Append mutation to chromo anytype, ref uc_043
    if (!aRunTime && !mutadded && !aTrialMode) {
	iChromo->Root().AddChild(aSpec);
	NotifyNodeMutated(aSpec, aCtx);
    }
    return res;
}

TBool Elem::MoveNode(const ChromoNode& aSpec, TBool aRunTime, TBool aTrialMode, const MutCtx& aCtx)
{
    TBool res = EFalse;
    string srcs = aSpec.Attr(ENa_Id);
    TBool dest_spec = aSpec.AttrExists(ENa_MutNode);
    string dests = dest_spec ? aSpec.Attr(ENa_MutNode): string();
    MUnit* dnode = dest_spec ? GetNode(dests) : this;
    TBool ecsaf = iEnv->ChMgr()->EnableCheckSafety();
    TBool mutadded = EFalse;
    if (dnode != NULL) {
	MElem* ednode = dnode->GetObj(ednode);
	GUri srcsuri(srcs);
	if (srcsuri.Scheme().empty()) {
	    // Local node
	    MUnit* snode = GetNode(srcs);
	    if (snode != NULL) {
		MUnit* sowner = snode->GetMan();
		if (sowner != NULL && dnode == sowner) {
		    // Attempt of moving node to the current contect 
		    Logger()->Write(EErr, this, "Moving node [%s] to it's current owner - disabled", snode->GetUri().c_str());
		}
		else if (dnode == snode) {
		    // Attempt of moving node to itself
		    Logger()->Write(EErr, this, "Moving node [%s] into itself - disabled", snode->GetUri().c_str());
		}
		else if (dnode != this) {
		    Logger()->Write(EErr, this, "Moving to another destination");
		}
		else {
		    // Inter-nodes movement
		    // TODO [YB] To consider if this movement does make sense. It is not movement of chunk of chromo, but 
		    // run-time only.
		    // It is rather complicated to re-create snode in new context because all snode
		    // mutation are related to original snode context, so use trick, ref "ds_mv_local"
		    // Create heir of source node in destination context
		    MElem* esnode = snode->GetObj(esnode);
		    MUnit* heir = snode->CreateHeir(snode->Name(), dnode, NULL);
		    // Re-adopt back to source parent
		    MElem* eheir = heir->GetObj(eheir);
		    eheir->SetParent(NULL);
		    MUnit* snodeparent = esnode->GetParent();
		    MElem* esnodeparent = snodeparent->GetObj(esnodeparent);
		    esnodeparent->AppendChild(heir);
		    //res = dnode->AppendComp(heir);
		    res = ETrue;
		    if (res) {
			// Remove source node
			// If node has children then just mark it as removed but not remove actually
			if (esnode->HasInherDeps(snode)) {
			    //TODO [YB] Consider to handle correctly
			    snode->SetRemoved(aRunTime);
			}
			else {
			    delete snode;
			}
		    }
		    else {
			Logger()->Write(EErr, this, "Moving node [%s] failed", snode->GetUri().c_str());
			delete heir;
		    }
		}
	    }
	    else {
		Logger()->Write(EErr, this, "Moving node [%s] - not found", srcs.c_str());
	    }
	}
	else {
	    // Remote node
	    MChromo *spec = Provider()->CreateChromo();
	    TBool res1 = spec->Set(srcs);
	    if (res1) {
		const ChromoNode& root = spec->Root();
		MUnit* nnode = NULL;
		if (dnode != NULL) {
		    nnode= ednode->AddElem(root, ETrue, EFalse, aCtx);
		}
		else {
		    nnode = AddElem(root, ETrue, false, aCtx);
		}
		delete spec;
		res = nnode != NULL;
	    }
	    else {
		Logger()->Write(EErr, this, "Moving [%s] to [%s]: source node not found", srcs.c_str(), dests.c_str());
	    }
	}
	if (!aRunTime && res) {
	    // Adding dependency to object of change
	    ChromoNode chn = iChromo->Root().AddChild(aSpec);
	    TNs ns; MutCtx mctx(NULL, ns);
	    NotifyNodeMutated(aSpec, mctx);
	    mutadded = ETrue;
	}
    }
    else {
	Logger()->Write(EErr, this, "Moving to node [%s] - not found", dests.c_str());
    }
    // Append mutation to chromo anytype, ref uc_043
    if (!aRunTime && !mutadded && !aTrialMode) {
	iChromo->Root().AddChild(aSpec);
	TNs ns; MutCtx mctx(NULL, ns);
	NotifyNodeMutated(aSpec, mctx);
    }
    return res;
}

// TODO [YB] To implement with usage of URI but not just string
// TODO [YB] The problem is more serious: we need to check the full type
// (all parents chain) to detect inheritance.
TBool Elem::IsHeirOf(const string& aParent) const
{
    TBool res = EFalse;
    string et = EType(EFalse);
    int pos = et.find(aParent);
    if (pos == 0) {
	res = (aParent.size() == et.size() || et.at(aParent.size()) == GUri::KParentSep);
    }
    return res;
    //return pos == 0 && (aParent.size() == et.size() || et.at(aParent.size()) == GUri::KParentSep);
}

// Checks if elements chromo is attached. Ref UC_019 for details
// Using model based calculation, ref ds_daa_chrc_va
TBool Elem::IsChromoAttached() const
{
    const MUnit* atm = GetAttachedMgr();
    return atm == this;
}

TBool Elem::IsMutAttached(const ChromoNode& aMut) const
{
    return IsChromoAttached();
}

MUnit* Elem::GetAttachedMgr() 
{
    MUnit* res = GetRoot();
    if (this != res) {
	MUnit* comp = res->GetCompOwning(this);
	MElem* eres = res->GetObj(eres);
	while (res != this && eres->GetCompLrank(comp) > -1) {
	    res = comp;
	    eres = res->GetObj(eres);
	    comp = res->GetCompOwning(this);
	}
    }
    return res;
}

const MUnit* Elem::GetAttachedMgr() const
{
    const MUnit* res = GetRoot();
    if (this != res) {
	const MUnit* comp = res->GetCompOwning(this);
	MElem* eres = const_cast<MUnit*>(res)->GetObj(eres);
	while (res != this && eres->GetCompLrank(comp) > -1) {
	    res = comp;
	    eres = const_cast<MUnit*>(res)->GetObj(eres);
	    comp = res->GetCompOwning(this);
	}
    }
    return res;
}

MUnit* Elem::GetAcompOwning(MUnit* aComp)
{
    MUnit* res = NULL;
    MUnit* cand = aComp;
    while (res == NULL && cand != NULL) {
	if (IsAownerOf(cand)) {
	    res = cand;
	} else {
	    cand = cand->GetMan();
	}
    }
    return res;
}

// Get the node that attaches this node. Don't confuse with the nearest attached
// owner, for that ref to GetAttachedMgr
MUnit* Elem::GetAowner()
{
    MUnit* res = NULL;
    MElem* eowner = iMan == NULL ? NULL : iMan->GetObj(eowner);
    if (eowner != NULL) {
	res = eowner->GetCompAowner(this);
    }
    return res;
}

const MUnit* Elem::GetAowner() const
{
    const MUnit* res = NULL;
    MElem* eowner = iMan->GetObj(eowner);
    if (eowner != NULL) {
	res = eowner->GetCompAowner(this);
    }
    return res;
}

// TODO Roughtly implemeted, re-implement
MUnit* Elem::GetMowner(MUnit* aNode)
{
    __ASSERT(aNode != NULL);
    MElem* eowner = NULL;
    MUnit* owner = aNode->GetMan();
    while (owner != NULL) {
	eowner = owner->GetObj(eowner);
	if (eowner != NULL) {
	    break;
	}
	owner = owner->GetMan();
    }
    return owner;
}

TBool Elem::IsInheritedComp(const MUnit* aNode) const
{
    MElem* enode = const_cast<MUnit*>(aNode)->GetObj(enode);
    MUnit* uaowner = enode->GetUpperAowner();
    TBool res = uaowner == NULL || IsComp(uaowner) && !uaowner->IsComp(this);
    return res;
}

TBool Elem::IsDirectInheritedComp(const MUnit* aNode) const
{
    MElem* enode = const_cast<MUnit*>(aNode)->GetObj(enode);
    MUnit* uaowner = enode->GetUpperAowner();
    MUnit* acompo = ((MElem*) this)->GetAcompOwning(const_cast<MUnit*>(aNode));
    TBool isinher = uaowner == NULL || IsComp(uaowner) && !uaowner->IsComp(this);
    TBool res = isinher && acompo == NULL;
    return res;
}

TBool Elem::IsCompOfInheritedComp(const MUnit* aNode) const
{
    TBool res = IsInheritedComp(aNode);
    if (!res) {
	const MUnit* owner = aNode->GetMan();
	while (owner != this && !res) {
	    res = IsInheritedComp(owner);
	    owner = owner->GetMan();
	}
    }
    return res;
}

MUnit* Elem::GetParent()
{
    return iParent;
}

const MUnit* Elem::GetParent() const
{
    return iParent;
}

void Elem::SetParent(MUnit* aParent)
{
    __ASSERT(aParent != NULL && iParent == NULL || aParent == NULL && iParent != NULL);
    iParent = aParent;
}

void Elem::OnChildDeleting(MUnit* aChild)
{
    // Unregister the child
    UnregisterChild(aChild);
}

TBool Elem::OnChildRenamed(MUnit* aChild, const string& aOldName)
{
    TBool res = EFalse;
    // Unregister the child with its old name
    res = UnregisterChild(aChild, aOldName);
    if (res) {
	// Register the comp againg with its current name
	res = RegisterChild(aChild);
    }
    return res;
}

// Handles parent deleting, ref uc_029
void Elem::OnParentDeleting(MUnit* aParent)
{
    iParent = NULL;
    /* TODO [YB] this solution contradicts to principles of incrementing creations
     * to redesign
    // Only local parent deletion is handled for now
    __ASSERT(aParent == iParent);
    // Copy parents choromo and reparent to grandparent
    ChromoNode phroot = iParent->Chromos().Root();
    ChromoNode hroot = iChromo->Root();
    ChromoNode fchild = *(hroot.Begin());
    for (ChromoNode::Iterator it = phroot.Begin(); it != phroot.End(); it++) {
    ChromoNode cn = *it;
    hroot.AddPrev(fchild, cn);
    }
    hroot.SetAttr(ENa_Parent, phroot.Attr(ENa_Parent));
    Elem* gparent = iParent->GetParent();
    gparent->RegisterChild(this);
    iParent = gparent;
    */
}

ChromoNode Elem::GetChNode(const GUri& aUri) const
{
}

TBool Elem::HasChilds() const
{
    return !iChilds.empty();
}

TBool Elem::HasInherDeps(const MUnit* aScope) const
{
    TBool res = EFalse;
    for (TNEReg::const_iterator it = iChilds.begin(); it != iChilds.end() && !res; it++) {
	MUnit* comp = it->second;
	res = !comp->IsRemoved() && !aScope->IsComp(comp);
    }
    for (TNEReg::const_iterator it = iMComps.begin(); it != iMComps.end() && !res; it++) {
	MUnit* comp = it->second;
	MElem* ecomp = comp->GetObj(ecomp);
	res = !comp->IsRemoved() && ecomp->HasInherDeps(aScope);
    }
    return res;
}

// This variant is for using env imports manager
TBool Elem::ImportNode(const ChromoNode& aSpec, TBool aRunTime, TBool aTrialMode)
{
    MImportMgr* impmgr = iEnv->ImpsMgr();
    __ASSERT(impmgr != NULL);
    TBool res = EFalse;
    TBool mutadded = EFalse;
    string srcs = aSpec.Attr(ENa_Id);
    res = impmgr->Import(srcs);
    if (!aRunTime && (res || !res && !aTrialMode)) {
	iChromo->Root().AddChild(aSpec);
	TNs ns; MutCtx mctx(NULL, ns);
	NotifyNodeMutated(aSpec, mctx);
    }
    return ETrue;
}

MIface* Elem::MElem_Call(const string& aSpec, string& aRes)
{
    MIface* res = NULL;
    string name, sig;
    vector<string> args;
    Ifu::ParseIcSpec(aSpec, name, sig, args);
    TBool name_ok = MElem::mIfu.CheckMname(name);
    if (!name_ok) 
	throw (runtime_error("Wrong method name"));
    TBool args_ok = MElem::mIfu.CheckMpars(name, args.size());
    if (!args_ok) 
	throw (runtime_error("Wrong arguments number"));
    if (name == "Name") {
	aRes = Name();
    } else if (name == "GetMan") {
	res = GetMan();
    } else if (name == "GetCtx") {
	res = GetCtx();
    } else if (name == "GetRoot") {
	res = GetRoot();
    } else if (name == "GetParent") {
	res = GetParent();
    } else if (name == "IsProvided") {
	TBool rr = IsProvided();
	//aRes = Ifu::FromBool(rr);
	aRes = Ifu::Pack(rr);
    } else if (name == "GetNode") {
	TBool inclrm = Ifu::ToBool(args.at(1));
	res = GetNode(args.at(0), inclrm);
    } else if (name == "GetNode#2") {
	GUri uri(args.at(0));
	//TBool anywhere = Ifu::ToBool(args.at(1));
	TBool anywhere = Ifu::Unpack(args.at(1), anywhere);
	TBool inclrm = Ifu::ToBool(args.at(2));
	GUri::const_elem_iter it = uri.Begin();
	res = GetNode(uri, it, anywhere, inclrm);
    } else if (name == "ContentExists") {
	TBool rr = ContentExists(args.at(0));
	aRes = Ifu::Pack(rr);
    } else if (name == "ContValueExists") {
	TBool rr = ContValueExists(args.at(0));
	aRes = Ifu::Pack(rr);
    } else if (name == "GetContCount") {
	TInt rr = GetContCount(args.at(0));
	aRes = Ifu::FromInt(rr);
    } else if (name == "GetContent") {
	TBool full = Ifu::ToBool(args.at(1));
	aRes = GetContent(args.at(0), full);
    } else if (name == "GetContComp") {
	TInt ind = Ifu::ToInt(args.at(1));
	aRes = GetContComp(args.at(0), ind);
    } else if (name == "Mutate") {
	TBool rtonly, checksafety, trialmode;
	rtonly = Ifu::ToBool(args.at(1));
	checksafety = Ifu::ToBool(args.at(2));
	trialmode = Ifu::ToBool(args.at(3));
	SetMutation(args.at(0));
	TNs ns; MutCtx mctx(NULL, ns);
	Mutate(rtonly, checksafety, trialmode, mctx);
    } else if (name == "Mutate#2") {
	TBool rtonly, checksafety, trialmode;
	rtonly = Ifu::ToBool(args.at(0));
	checksafety = Ifu::ToBool(args.at(1));
	trialmode = Ifu::ToBool(args.at(2));
	MUnit* uctx = GetNode(args.at(3));
	TNs ns; MutCtx mctx(uctx, ns);
	Mutate(rtonly, checksafety, trialmode, mctx);
    } else if (name == "GetChromoSpec") {
	aRes = GetChromoSpec();
    } else if (name == "EType") {
	aRes = EType(Ifu::ToBool(args.at(0)));
    } else if (name == "AppendChild") {
	MUnit* child = GetNode(args.at(0));
	if (child == NULL) {
	    MUnit* nn = GetNode(args.at(0));
	    throw (runtime_error("Cannot get node " + args.at(0)));
	}
	TBool rr = AppendChild(child);
	aRes = Ifu::Pack(rr);
    } else if (name == "RemoveChild") {
	MUnit* child = GetNode(args.at(0));
	RemoveChild(child);
    } else if (name == "SetParent") {
	MUnit* parent = GetNode(args.at(0));
	SetParent(parent);
    } else if (name == "DoGetObj") {
	res = DoGetObj(args.at(0).c_str());
    } else if (name == "OnChildDeleting") {
	MUnit* child = GetNode(args.at(0));
	OnChildDeleting(child);
    } else if (name == "OnParentDeleting") {
	MUnit* prnt = GetNode(args.at(0));
	OnParentDeleting(prnt);
    } else if (name == "AppendMutation") {
	TMut mut(args.at(0));
	AppendMutation(mut);
    } else if (name == "AppendMutation#2") {
	MChromo* mut = iEnv->Provider()->CreateChromo();
	mut->SetFromSpec(args.at(0));
	AppendMutation(mut->Root());
	delete mut;
    } else if (name == "IsHeirOf") {
	TBool rr = IsHeirOf(args.at(0));
	aRes = Ifu::FromBool(rr);
    } else if (name == "OnNodeMutated") {
	MUnit* node = GetNode(args.at(0));
	TMut mut(args.at(1));
	MUnit* uctx = GetNode(args.at(2));
	TNs ns; MutCtx mctx(uctx, ns);
	OnNodeMutated(node, mut, mctx);
    } else if (name == "IsCompAttached") {
	MUnit* node = GetNode(args.at(0));
	TBool isatt = IsCompAttached(node);
	aRes = Ifu::FromBool(isatt);
    } else if (name == "GetCompLrank") {
	MUnit* node = GetNode(args.at(0));
	TInt rank = GetCompLrank(node);
	aRes = Ifu::FromInt(rank);
    } else if (name == "GetCRoot") {
	TMut croot;
	GetCRoot(croot);
	aRes = croot;
    } else if (name == "IsChromoAttached") {
	TBool isatt = IsChromoAttached();
	aRes = Ifu::FromBool(isatt);
    } else if (name == "GetAowner") {
	res = GetAowner();
    } else if (name == "GetCompAowner") {
	MUnit* comp = GetNode(args.at(0));
	if (comp == NULL) {
	    throw (runtime_error("Cannot get node " + args.at(0)));
	}
	res = GetCompAowner(comp);
    } else {
	throw (runtime_error("Unhandled method: " + name));
    }
    return res;
}

TInt Elem::IfRangeSize(const TIfRange& aRange) const
{
    TInt res = 0;
    for (TIfIter it = aRange.first; it != aRange.second; it++, res++);
    return res;
}

void Elem::DumpChilds() const
{
    for (TNEReg::const_iterator it = iChilds.begin(); it != iChilds.end(); it++) {
	MUnit* child = it->second;
	cout << "ptr: " << (void*) child << ", name: " << child->Name() << endl;
    }
}

void Elem::NotifyNodeMutated(const ChromoNode& aMut, const MutCtx& aCtx)
{
    Pdstat(PEvents::DurStat_NotifNodeMutated, true);
    if (this != GetRoot()) {
	MUnit* owner = GetMowner(this);
	MElem* eowner = owner == NULL ? NULL : owner->GetObj(eowner);
	if (aCtx.mUnit != NULL && (aCtx.mUnit == owner || aCtx.mUnit->IsComp(owner)) || eowner->IsCompAttached(this)) {
	    eowner->OnNodeMutated(this, aMut, aCtx);
	    iEnv->Observer()->OnCompMutated(this);
	}
	if (HasChilds()) {
	    NotifyParentMutated(aMut);
	}
    } 
    Pdstat(PEvents::DurStat_NotifNodeMutated, false);
}

void Elem::OnNodeMutated(const MUnit* aNode, const TMut& aMut, const MutCtx& aCtx)
{
    MUnit* root = GetRoot();
    // Copy mutation to the chromo in case of root or first non-attached owner in he owners chain
    // ref ds_mut_osm_linchr_comdp_mp
    //if (this == root || !IsCompAttached(GetCompOwning(aNode))) {
    // Accept propagation from attached comp only
    if (true) {
	ChromoNode anode = iChromo->Root().AddChild(aMut);
	GUri nuri = aNode->GetUri(this, ETrue);
	if (anode.AttrExists(ENa_Targ)) {
	    string starg = anode.Attr(ENa_Targ);
	    nuri.Append(starg);
	}
	anode.SetAttr(ENa_Targ, nuri);
    }
    if (this != root) {
	// Propagate till upper node of attaching chain, ref ds_mut_osm_linchr_lce
	// i.e to the owner if owner is attaching owner or is under mutated node attaching owner
	MUnit* owner = GetMowner(this);
	MElem* eowner = owner == NULL ? NULL : owner->GetObj(eowner);
	if (aCtx.mUnit != NULL && (aCtx.mUnit == owner || aCtx.mUnit->IsComp(owner)) || eowner->IsCompAttached(this)) {
	    eowner->OnNodeMutated(aNode, aMut, aCtx);
	}
	if (HasChilds()) {
	    string nuri = aNode->GetUri(this, ETrue);
	    TMut mut(aMut);
	    mut.SetAttr(ENa_Targ, nuri);
	    NotifyParentMutated(mut);
	}
    }
}

void Elem::NotifyParentMutated(const TMut& aMut)
{
    if (this != GetRoot()) {
	for (TNEReg::iterator it = iChilds.begin(); it != iChilds.end(); it++) {
	    MUnit* child = it->second;
	    MElem* echild = child == NULL ? NULL : child->GetObj(echild);
	    echild->OnParentMutated(this, aMut);
	}
    }
}

void Elem::OnParentMutated(MUnit* aParent, const TMut& aMut)
{
    TBool rres = ETrue;
    // Rebase and apply parent's mutation
    ChromoNode rno = iMut->Root().AddChild(aMut);
    if (rno.AttrExists(ENa_Ref)) {
	MUnit* ref = aParent->GetNode(rno.Attr(ENa_Ref));
	if (ref != NULL) {
	    string ruri = ref->GetRUri(this);
	    rno.SetAttr(ENa_Ref, ruri);
	} else {
	    Logger()->Write(EErr, this, "Cannot find ref [%s]", rno.Attr(ENa_Ref).c_str());
	    rres = EFalse;
	}
    }
    if (rno.AttrExists(ENa_Parent)) {
	MUnit* prn = aParent->GetNode(rno.Attr(ENa_Parent));
	if (prn != NULL) {
	    string puri = prn->GetRUri(this);
	    rno.SetAttr(ENa_Parent, puri);
	} else {
	    Logger()->Write(EErr, this, "Cannot find parent [%s]", rno.Attr(ENa_Parent).c_str());
	    rres = EFalse;
	}
    }
    if (rres) {
	TNs ns; MutCtx mctx(NULL, ns);
	Mutate(ETrue, EFalse, ETrue, mctx);
    }
}

TBool Elem::IsCompAttached(const MUnit* aComp) const
{
    return (GetCompLrank(aComp) != -1);
}

void Elem::SaveChromo(const char* aPath) const
{
    iChromo->Save(aPath);
}

string Elem::GetChromoSpec() const
{
    string res;
    iChromo->GetSpec(res);
    return res;
}

void Elem::AppendMutation(const TMut& aMut)
{
    ChromoNode mut = iMut->Root().AddChild(aMut);
}

/*
   void Elem::GetNs(TNs& aNs, const ChromoNode& aCnode)
   {
   if (aCnode.AttrExists(ENa_NS)) {
   string ns = aCnode.Attr(ENa_NS);
   MUnit* nsu = GetNode(ns);
   if (nsu == NULL) {
   Log(TLog(EErr, this) + "Cannot find namespace [" + ns + "]");
   } else {
   aNs.push_back(nsu);
   }
   }
   ChromoNode owner = *(aCnode.Parent());
   if (owner != ChromoNode()) {
   GetNs(aNs, owner);
   }
   }
   */

void Elem::UpdateNs(TNs& aNs, const ChromoNode& aCnode)
{
    if (aCnode.AttrExists(ENa_NS)) {
	string ns = aCnode.Attr(ENa_NS);
	MUnit* nsu = GetNode(ns);
	if (nsu == NULL) {
	    Log(TLog(EErr, this) + "Cannot find namespace [" + ns + "]");
	} else {
	    aNs.push_back(nsu);
	}
    }
}

void Elem::DumpCnode(const ChromoNode& aNode) const
{
    aNode.DumpToLog(iEnv->Logger());
}

void Elem::DumpCtx(const MutCtx& aCtx) const
{
    cout << "Unit: " << aCtx.mUnit << "   " << (aCtx.mUnit != NULL ? aCtx.mUnit->GetUri(NULL, true) : "") << endl;
    cout << "Namespaces:" << endl;
    for (MUnit* ns : aCtx.mNs) {
	cout << ns << "   " << ns->GetUri(NULL, true) << endl;
    }
}

