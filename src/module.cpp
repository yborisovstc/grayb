
#include "module.h"


string AMod::PEType()
{
    return Elem::PEType() + GUri::KParentSep + Type();
}

AMod::AMod(const string& aName, Elem* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv)
{
    SetEType(Type(), Elem::PEType());
    SetParent(Type());
}

AMod::AMod(Elem* aMan, MEnv* aEnv): Elem(Type(), aMan, aEnv)
{
    SetEType(Elem::PEType());
    SetParent(Elem::PEType());
}

AMod::~AMod()
{
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
