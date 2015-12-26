#ifndef __GRAYB_MMUTA_H
#define __GRAYB_MMUTA_H

#include "plat.h"
#include "mchromo.h"

class ChromoNode;
class MElem;

// Ref to mutation: [model node, mut handle]
typedef pair<MElem*, void*> TMutRef;
// Deps Mutations on RT model node
typedef pair<TMutRef, TNodeAttr> TMDep;
// Deps muts on run-time agent node
typedef vector<TMDep> TMDeps;

// Mutable element of graph
class MMutable
{
    public:
	virtual void DoMutation(const ChromoNode& aCromo, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode = EFalse) = 0;
	virtual TBool DoMutChangeCont(const ChromoNode& aSpec, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode = EFalse) = 0;
	virtual MElem* CreateHeir(const string& aName, MElem* aMan) = 0;
	virtual MElem* AddElem(const ChromoNode& aSpec, TBool aRunTime = EFalse, TBool aTrialMode = EFalse) = 0;
	virtual TBool RmNode(const ChromoNode& aSpec, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode = EFalse) = 0;
	virtual const MChromo& Chromos() const = 0;
	virtual MChromo& Chromos() = 0;
	virtual MChromo& Mutation() = 0;
	virtual void SetMutation(const ChromoNode& aMuta) = 0;
	virtual TBool AppendMutation(const string& aFileName) = 0;
	virtual ChromoNode AppendMutation(const ChromoNode& aMuta) = 0;
	virtual TMDeps& GetMDeps() = 0;
	virtual void AddMDep(MElem* aNode, const ChromoNode& aMut, TNodeAttr aAttr) = 0;
	virtual void AddCMDep(const ChromoNode& aMut, TNodeAttr aAttr, MElem* aNode) = 0;
	virtual void RemoveMDep(const TMDep& aDep, const MElem* aContext = NULL) = 0;
	virtual TBool RmCMDep(const ChromoNode& aMut, TNodeAttr aAttr, const MElem* aContext = NULL) = 0;
	virtual TBool IsChromoAttached() const = 0;
	virtual void GetDep(TMDep& aDep, TNodeAttr aAttr, TBool aLocalOnly = EFalse, TBool aAnyType = EFalse) const = 0;
	virtual TMDep GetMajorDep() = 0;
	virtual void GetMajorDep(TMDep& aDep, TBool aUp = EFalse, TBool aDown = ETrue) = 0;
	virtual TMDep GetMajorDep(TNodeType aMut, MChromo::TDepsLevel aLevel) = 0;
	virtual void GetMajorDep(TMDep& aDep, TNodeType aMut, MChromo::TDPath aDpath, MChromo::TDepsLevel aLevel, TBool aUp = ETrue, TBool aDown = ETrue) = 0;
	virtual ChromoNode GetLocalForwardCCDep(MElem* aOwner, const ChromoNode& aMut) const = 0;
	virtual void GetImplicitDep(TMDep& aDep, MElem* aObj, MElem* aRef) = 0;
	virtual TBool IsRefSafe(MElem* aRef, TNodeAttr aReftype, MElem* aObj = NULL, TMDep* aDep = NULL) = 0;
	// Resolve owned mutation unsafety via changing mutation position
	virtual TBool ResolveMutUnsafety(MElem* aMutated, const TMDep& aDep) = 0;
	virtual TBool ResolveMutsUnsafety() = 0;
	virtual TBool CompactChromo() = 0;
	virtual TBool CompactChromo(const ChromoNode& aNode) = 0;
	// Transformations
	virtual TBool HasParentModifs() const = 0;
	virtual void CopyModifsFromParent() = 0;
	virtual TBool HasModifs(const MElem* aOwner) const = 0;
	virtual void CopyParentModifsToComp(MElem* aComp) = 0;
	virtual TBool RebaseUriToIntNode(const GUri& aUri, const MElem* aComp, GUri& aResult) = 0;
};

#endif
