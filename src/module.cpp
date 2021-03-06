
#include "module.h"
#include "mprov.h"


string AMod::PEType()
{
    return Elem::PEType() + GUri::KParentSep + Type();
}

AMod::AMod(const string& aName, MUnit* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv), mSpec(NULL)
{
    SetParent(Type());
}

AMod::AMod(MUnit* aMan, MEnv* aEnv): Elem(Type(), aMan, aEnv), mSpec(NULL)
{
    SetParent(Elem::PEType());
}

AMod::~AMod()
{
    if (mSpec != NULL) {
	delete mSpec;
    }
}


MIface* AMod::DoGetObj(const char *aName)
{
    MIface* res = NULL;
    if (strcmp(aName, MMod::Type()) == 0) {
	res = (MIface*) (MMod*) this;
    } else {
	res = Elem::DoGetObj(aName);
    }
    return res;
}

MUnit* AMod::Import(const GUri& aUri)
{
    MUnit* res = NULL;
    //TBool res1 = iMut->Set(mCont);
    TBool res1 = AppendMutation(mCont);
    ChromoNode sel = iMut->Root().GetNodeByMhUri(aUri);
    if (sel.Handle() != THandle()) {
	iMut->ReduceToSelection(sel);
	TNs ns;
	MutCtx ctx(NULL, ns);
	Mutate(EFalse, ETrue, EFalse, ctx);
	res = GetNode(aUri);
    } else {
	Logger()->Write(EErr, this, "Importing to module: cannot find chromo node [%s]", aUri.toString().c_str());
    }
    return res;
}

TBool AMod::ChangeCont(const string& aVal, TBool aRtOnly, const string& aName)
{
    TBool res = ETrue;
    if (aVal != mCont) {
	mCont = aVal;
	//mSpec = Provider()->CreateChromo();
	//TBool res1 = mSpec->Set(mCont);
	if (aRtOnly) {
	    iMan->OnChanged(this);
	} else {
	    iMan->OnCompChanged(this);
	}
    }
    return res;
}

TBool AMod::GetCont(string& aValue, const string& aName) const
{
    aValue = mCont;
    return ETrue;
}

TInt AMod::GetContCount() const
{
    return 1;
}
