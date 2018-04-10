#ifndef __GRAYB_MMUTA_H
#define __GRAYB_MMUTA_H

#include "plat.h"
#include "mchromo.h"
#include <memory>

    class ChromoNode;
    class MElem;
    class TMut;

    // Mutable element of graph
    class MMutable
    {
	public:
	    virtual void DoMutation(const ChromoNode& aCromo, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode = EFalse, const MElem* aCtx = NULL) = 0;
	    virtual TBool DoMutChangeCont(const ChromoNode& aSpec, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode = EFalse, const MElem* aCtx = NULL) = 0;
	    virtual MElem* CreateHeir(const string& aName, MElem* aMan, MElem* aContext) = 0;
	    virtual MElem* AddElem(const ChromoNode& aSpec, TBool aRunTime = EFalse, TBool aTrialMode = EFalse, const MElem* aCtx = NULL) = 0;
	    virtual TBool RmNode(const ChromoNode& aSpec, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode = EFalse, const MElem* aCtx = NULL) = 0;
	    virtual unique_ptr<MChromo> GetFullChromo() const = 0;
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
