#ifndef __GRAYB_MMUTA_H
#define __GRAYB_MMUTA_H

#include "plat.h"
#include "mchromo.h"
#include <memory>

class ChromoNode;
class MUnit;
class MElem;
class TMut;

/** @brief Name spaces
 * */
using TNs = vector<MUnit*>;

/** @brief Mutation context
 * */
class MutCtx
{
    public:
	MutCtx(const MutCtx& aSrc): mUnit(aSrc.mUnit), mNs(aSrc.mNs) {}
	MutCtx(const MUnit* aUnit, const TNs& aNs): mUnit(aUnit), mNs(aNs) {}
	MutCtx(const MUnit* aUnit): mUnit(aUnit), mNs(TNs()) {}
	//MutCtx(): MutCtx(NULL) {}
	MutCtx(): mUnit(NULL), mNs(TNs()) {}
    public:
	const MUnit* mUnit; //!< Unit
	const TNs& mNs;     //!< Name spaces
};


/** @brief Interface of Mutable instance
 * */
class MMutable
{
    public:
	virtual void DoMutation(const ChromoNode& aCromo, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode /*EFalse*/, const MutCtx& aCtx) = 0;
	// TODO [YN] Why DoMutChangeCont is included but other mutations like ChangeAttr are not?
	virtual TBool DoMutChangeCont(const ChromoNode& aSpec, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode /*EFalse*/, const MutCtx& aCtx) = 0;
	virtual MUnit* AddElem(const ChromoNode& aSpec, TBool aRunTime /*EFalse*/, TBool aTrialMode /*EFalse*/, const MutCtx& aCtx) = 0;
	virtual TBool RmNode(const ChromoNode& aSpec, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode /*EFalse*/, const MutCtx& aCtx) = 0;
	virtual string GetChromoSpec() const = 0;
	virtual const MChromo& Chromos() const = 0;
	virtual MChromo& Chromos() = 0;
	virtual void SetMutation(const ChromoNode& aMuta) = 0;
	virtual TBool AppendMutation(const string& aFileName) = 0;
	virtual ChromoNode AppendMutation(const ChromoNode& aMuta) = 0;
	virtual ChromoNode AppendMutation(TNodeType aType) = 0;
	virtual void AppendMutation(const TMut& aMut) = 0;
	virtual TBool IsChromoAttached() const = 0;
	virtual TBool CompactChromo() = 0;
	virtual TBool CompactChromo(const ChromoNode& aNode) = 0;
};

#endif
