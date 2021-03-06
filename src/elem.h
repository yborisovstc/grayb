#ifndef __GRAYB_ELEM_H
#define __GRAYB_ELEM_H


#include "melem.h"
#include "menv.h"
#include "mlog.h"
#include "mmuta.h"
#include "base.h"
#include "chromo.h"
#include "profiler.h"
#include "ifu.h"
#include "unit.h"
#include <time.h>

#define UNICONTENT

class Chromo;
class MProvider;

// Standard element of native hier
class Elem: public Unit, public MElem
{
    public:
	typedef string TNEKey; //!< Key type of name-to-elem map
	typedef pair<TNEKey, MUnit*> TNEVal; //!< Name-to-elem map entry
	typedef multimap<string, MUnit*> TNEReg; //!< Name-to-elem multimap

    public:
	// Formatter
	class Fmt
	{
	    public:
		// Separator of content inputs info
		static string mSepContInp;
		// Separator of parts of content name
		static string mSepContName;
	};

    public:
	static const char* Type() { return "Elem";};
	virtual const string& Name() const { return iName;}
	// Predefined extended type, for core elements only
	static string PEType();
	Elem(const string &aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	virtual ~Elem();
	virtual void SetMutation(const ChromoNode& aMuta);
	virtual void SetMutation(const string& aMutSpec);
	virtual ChromoNode AppendMutation(const ChromoNode& aMuta);
	virtual ChromoNode AppendMutation(TNodeType aType);
	virtual TBool AppendMutation(const string& aFileName);
	virtual void AppendMutation(const TMut& aMut);
	string PName() const;
    public:
	virtual MUnit* CreateHeir(const string& aName, MUnit* aMan, MUnit* aContext);
	virtual string GetChromoSpec() const;
	virtual const MChromo& Chromos() const { return *iChromo;};
	virtual MChromo& Chromos() { return *iChromo;};
	virtual MUnit* GetAowner();
	virtual const MUnit* GetAowner() const;
	virtual MUnit* GetAcompOwning(MUnit* aComp);
	virtual MUnit* GetUpperAowner();
	virtual MUnit* GetInhRoot() const;
	virtual MUnit* GetCompAowner(const MUnit* aComp);
	virtual const MUnit* GetCompAowner(const MUnit* aComp) const;
	virtual TBool IsAownerOf(const MUnit* aElem) const;
	virtual TBool IsCompAttached(const MUnit* aComp) const;
	virtual MUnit* GetAttachedMgr();
	virtual const MUnit* GetAttachedMgr() const;
	// Checks if elements chromo is attached. Ref UC_019 for details
	virtual TBool IsChromoAttached() const;
	TBool IsMutAttached(const ChromoNode& aMut) const;
	virtual TBool IsInheritedComp(const MUnit* aNode) const;
	// Checks if the node is originated via phenotypical modification
	TBool IsDirectInheritedComp(const MUnit* aNode) const;
	TBool IsCompOfInheritedComp(const MUnit* aNode) const;
	// From MIfProv Iface provider
	MIface* getLocalIface(const string& aName, const TICacheRCtx& aCtx) override;
	TInt IfRangeSize(const TIfRange& aRange) const;
	// From Base
	virtual MIface *DoGetObj(const char *aName);
	// From MElem
	virtual string EType(TBool aShort = ETrue) const;
	virtual MUnit* GetUnit() override;
	virtual void GetCRoot(TMut& aMut) const;
	virtual TInt GetCompLrank(const MUnit* aComp) const;
	virtual void Mutate(TBool aRunTimeOnly /*EFalse*/, TBool aCheckSafety /*EFalse*/, TBool aTrialMode /*ETrue*/, const MutCtx& aCtx);
	virtual void Mutate(const ChromoNode& aMutsRoot, TBool aRunTimeOnly /*EFalse*/, TBool aCheckSafety /*EFalse*/, TBool aTrialMode /*ETrue*/, const MutCtx& aCtx);
	virtual void ChangeAttr(const ChromoNode& aSpec, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode /*EFalse*/, const MutCtx& aCtx);
	virtual TBool ChangeAttr(TNodeAttr aAttr, const string& aVal);
	// Nodes
	virtual TBool RmNode(const ChromoNode& aSpec, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode /*EFalse*/, const MutCtx& aCtx);
	virtual TBool MoveNode(const ChromoNode& aSpec, TBool aRunTime, TBool aTrialMode /* EFalse*/, const MutCtx& aCtx);
	virtual TBool ImportNode(const ChromoNode& aSpec, TBool aRunTime, TBool aTrialMode = EFalse);
	// From MChild
	virtual MUnit* GetParent();
	virtual const MUnit* GetParent() const;
	virtual void OnParentDeleting(MUnit* aParent);
	virtual void SetParent(const string& aParent);
	virtual void SetParent(MUnit* aParent);
	// From MParent
	virtual void OnChildDeleting(MUnit* aChild);
	virtual TBool OnChildRenamed(MUnit* aChild, const string& aOldName);
	virtual TBool AppendChild(MUnit* aChild);
	virtual void RemoveChild(MUnit* aChild);
	// From MMutable
	virtual void DoMutation(const ChromoNode& aCromo, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode, const MutCtx& aCtx);
	virtual TBool DoMutChangeCont(const ChromoNode& aSpec, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode, const MutCtx& aCtx);
	virtual MUnit* AddElem(const ChromoNode& aSpec, TBool aRunTime, TBool aTrialMode, const MutCtx& aCtx);
	virtual void OnNodeMutated(const MUnit* aNode, const TMut& aMut, const MutCtx& aCtx);
	virtual void OnParentMutated(MUnit* aParent, const TMut& aMut);
	// Chromo
	virtual TBool CompactChromo() override { return EFalse;}
	virtual TBool CompactChromo(const ChromoNode& aNode) override { return EFalse;}
	void UndoCompactChromo() {}
	// From MIface
	virtual MIface* MElem_Call(const string& aSpec, string& aRes) override;
	virtual string MElem_Mid() const override { return Unit::Mid();}
	// Local
	/** @brief Perform specific mutation
	 * */
	virtual void DoSpecificMut(const ChromoNode& aSpec, TBool aRunTime, TBool aTrialMode, const MutCtx& aCtx);
    protected:
	/** @brief Gets nearest mutable owner */
	static MUnit* GetMowner(MUnit* aNode);
	/** @brief Helper. Sets cromo root attributes: parent, name */
	void SetCrAttr(const string& aEType, const string& aName);
	TBool RegisterChild(MUnit* aChild);
	void NotifyNodeMutated(const ChromoNode& aMut, const MutCtx& aCtx);
	void NotifyParentMutated(const TMut& aMut);
	// aName is required because the comp can be renamed already. This is the case of
	// comp renaming: comp is renamed first, then the renaming is handled
	TBool UnregisterChild(MUnit* aChild, const string& aName = string());
	TBool HasChilds() const;
	virtual TBool HasInherDeps(const MUnit* aScope) const;
	/** @brief Updates name spaces from the current chromo node context
	 * @param[out] aNs   Name space
	 * @param[in] aCnode Chromo node
	 * */
	void UpdateNs(TNs& aNs, const ChromoNode& aCnode);
	// Debugging
	virtual void SaveChromo(const char* aPath) const;
	virtual void DumpChilds() const;
	virtual void DumpCnode(const ChromoNode& aNode) const;
	void DumpCtx(const MutCtx& aCtx) const;
    public:
    protected:
	// Chromo
	MChromo* iChromo;
	// Mutation
	MChromo* iMut;
	// Children register
	TNEReg iChilds;
	// Parent
	// TODO Should it be MElem?
	MUnit* iParent;

	static TBool EN_MUT_LIM;
	static const string KCont_About;
	/*
	   protected:
	   class EIfu: public Ifu {
	   public:
	   EIfu();
	   };
	// Interface methods utility
	static EIfu mIfu;
	*/
};


#endif
