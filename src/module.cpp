
#include "module.h"
#include "mprov.h"


string AMod::PEType()
{
    return Elem::PEType() + GUri::KParentSep + Type();
}

AMod::AMod(const string& aName, MElem* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv), mSpec(NULL)
{
    SetParent(Type());
}

AMod::AMod(MElem* aMan, MEnv* aEnv): Elem(Type(), aMan, aEnv), mSpec(NULL)
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

MElem* AMod::Import(const GUri& aUri)
{
    MElem* res = NULL;
    //TBool res1 = iMut->Set(mCont);
    TBool res1 = AppendMutation(mCont);
    ChromoNode sel = iMut->Root().GetNodeByMhUri(aUri);
    if (sel.Handle() != NULL) {
	iMut->ReduceToSelection(sel);
	Mutate(EFalse, ETrue, EFalse);
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
	    iMan->OnChanged(*this);
	} else {
	    iMan->OnCompChanged(*this);
	}
    }
    return res;
}

TBool AMod::GetCont(string& aValue, const string& aName) const
{
    aValue = mCont;
}

TInt AMod::GetContCount() const
{
    return 1;
}
