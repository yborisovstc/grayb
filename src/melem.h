#ifndef __GRAYB_MELEM_H
#define __GRAYB_MELEM_H

#include "plat.h"
#include "guri.h"
#include "mmuta.h"
#include "miface.h"
#include "ifu.h"
#include "ifu.h"
#include <set>


class ChromoNode;
class Base;
class MUnit;
class MElem;
class Rank;
class TICacheRCtx;


// Interface of parent in native hierarchy
// TODO to migrate to MElem using instead of MUnit
class MParent
{
    public:
	static const char* Type() { return "MParent";};
    public: 
	virtual void OnChildDeleting(MUnit* aChild) = 0;
	virtual TBool OnChildRenamed(MUnit* aComp, const string& aOldName) = 0;
	virtual TBool AppendChild(MUnit* aChild) = 0;
	virtual void RemoveChild(MUnit* aChild) = 0;
};

class MChild
{
    public:
	static const char* Type() { return "MChild";};
    public:
	virtual MUnit* GetParent() = 0;
	virtual const MUnit* GetParent() const = 0;
	virtual void OnParentDeleting(MUnit* aParent) = 0;
	virtual void SetParent(const string& aParent) = 0;
	virtual void SetParent(MUnit* aParent) = 0;
};


/** @brief Composite interface of mutable elemntary agent
 *
 * TODO [YB] Do we need MComp also ?
 * */
class MElem : public MIface,  public MMutable, public MParent, public MChild
{
    friend class Elem;
    friend class ImportsMgr;
    public:
    // Predefined content categories
    static const string KCont_Categories;
    static const string KCont_Ctg_Readonly;
    static const string KCont_Ctg_Debug;
    public:
    static const char* Type() { return "MElem";};
    public:
    /** @brief Getter of unit interface
     * This is temoprarily solution to simplify MElem methods migration to useng pure MElem args
     */
    virtual MUnit* GetUnit() = 0;
    virtual void GetCRoot(TMut& aMut) const = 0;
    virtual MUnit* GetInhRoot() const = 0;
    // TODO [YB] To support returning result. Ref uc_013_dsc_01 for use-case details.
    virtual void Mutate(TBool aRunTimeOnly = EFalse, TBool aCheckSafety = EFalse, TBool aTrialMode = ETrue, const MUnit* aCtx = NULL) = 0;
    virtual void Mutate(const ChromoNode& aMutsRoot, TBool aRunTimeOnly = EFalse, TBool aCheckSafety = EFalse, TBool aTrialMode = ETrue, const MUnit* aCtx = NULL) = 0;
    virtual TInt GetCompLrank(const MUnit* aComp) const = 0;
    virtual MUnit* GetAowner() = 0;
    virtual const MUnit* GetAowner() const = 0;
    virtual MUnit* GetCompAowner(const MUnit* aComp) = 0;
    virtual const MUnit* GetCompAowner(const MUnit* aComp) const = 0;
    virtual MUnit* GetAcompOwning(MUnit* aComp) = 0;
    virtual MUnit* GetUpperAowner() = 0;
    virtual TBool IsAownerOf(const MUnit* aElem) const = 0;
    virtual MUnit* GetAttachedMgr() = 0;
    virtual const MUnit* GetAttachedMgr() const = 0;
    virtual TBool IsCompAttached(const MUnit* aComp) const = 0;
    virtual TBool IsInheritedComp(const MUnit* aNode) const = 0;
    virtual TBool HasInherDeps(const MUnit* aScope) const = 0;
    virtual void OnNodeMutated(const MUnit* aNode, const TMut& aMut, const MUnit* aCtx = NULL) = 0;
    virtual void OnParentMutated(MUnit* aParent, const TMut& aMut) = 0;
    // From MIface
    virtual MIface* MElem_Call(const string& aSpec, string& aRes) { return NULL;}
    virtual string MElem_Mid() const = 0;
    MIface* Call(const string& aSpec, string& aRes) override {return MElem_Call(aSpec, aRes);}
    string Mid() const override { return MElem_Mid();}
    // Mutating
    virtual void ChangeAttr(const ChromoNode& aSpec, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode = EFalse, const MUnit* aCtx = NULL) = 0;
    // Debugging
    virtual TBool IsHeirOf(const string& aParent) const = 0;
    virtual void SaveChromo(const char* aPath) const = 0;
    virtual void DumpChilds() const = 0;
    protected:
    class EIfu: public Ifu {
	public:
	    EIfu();
    };

    static EIfu mIfu;
};

#endif
