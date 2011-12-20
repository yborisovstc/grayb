#include "elem.h"
#include "chromo.h"

const set<TNodeType> Elem::iCompsTypes;

Elem::Elem(const string &aName): Base(aName) 
{
}

void *Elem::DoGetObj(const char *aName)
{
    return (strcmp(aName, Type()) == 0) ? this : NULL;
}

TBool Elem::AddNode(const ChromoNode& aSpec)
{
    TBool res = ETrue;
    TNodeType ntype = aSpec.Type();
    if (CompsTypes().find(ntype) != iCompsTypes.end()) {
    }
    return res;
}


TBool CAE_EBase::AddNode(const CAE_ChromoNode& aSpec)
{
    TBool res = ETrue;
    TNodeType ntype = aSpec.Type();
    if (ntype == ENt_Logspec) {
	AddLogSpec(aSpec);
    }
    else {
	res = CAE_NBase::AddNode(aSpec);
    }
    return res;
}

MElem* Elem::GetNode(const GUri& aUri)
{
}

MElem* Elem::GetNode(const GUri& aUri, GUri::const_elem_iter aPathBase)
{
}

const set<TNodeType>& Elem::CompsTypes()
{
    return iCompsTypes;
}

