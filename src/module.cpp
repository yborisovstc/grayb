
#include "module.h"
#include "mprov.h"


string AMod::PEType()
{
    return Elem::PEType() + GUri::KParentSep + Type();
}

AMod::AMod(const string& aName, Elem* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv), mSpec(NULL)
{
    SetEType(Type(), Elem::PEType());
    SetParent(Type());
}

AMod::AMod(Elem* aMan, MEnv* aEnv): Elem(Type(), aMan, aEnv), mSpec(NULL)
{
    SetEType(Elem::PEType());
    SetParent(Elem::PEType());
}

AMod::~AMod()
{
    if (mSpec != NULL) {
	delete mSpec;
    }
}


void* AMod::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    } else if (strcmp(aName, MMod::Type()) == 0) {
	res = (MMod*) this;
    } else {
	res = Elem::DoGetObj(aName, aIncUpHier);
    }
    return res;
}

Elem* AMod::Import(const GUri& aUri)
{
    Elem* res = NULL;
    //TBool res1 = iMut->Set(mCont);
    TBool res1 = AppendMutation(mCont);
    ChromoNode sel = iMut->Root().GetNodeByMhUri(aUri);
    if (sel.Handle() != NULL) {
	iMut->ReduceToSelection(sel);
	Mutate(EFalse, ETrue, EFalse);
	res = GetNode(aUri);
    } else {
	Logger()->Write(MLogRec::EErr, this, "Importing to module: cannot find chromo node [%s]", aUri.GetUri().c_str());
    }
    return res;
}

TBool AMod::IsContChangeable(const string& aName) const
{
    return ETrue;
}

TBool AMod::ChangeCont(const string& aVal, TBool aRtOnly, const string& aName)
{
    TBool res = ETrue;
    if (aVal != mCont) {
	mCont = aVal;
	//mSpec = Provider()->CreateChromo();
	//TBool res1 = mSpec->Set(mCont);
	if (aRtOnly) {
	    iMan->OnContentChanged(*this);
	} else {
	    iMan->OnCompChanged(*this);
	}
    }
    return res;
}

void AMod::GetCont(string& aCont, const string& aName)
{
    aCont = mCont;
}

TInt AMod::GetContCount() const
{
    return 1;
}
