#include "prop.h"
#include <stdexcept> 

MProp::EIfu MProp::mIfu;

// Ifu static initialisation
MProp::EIfu::EIfu()
{
    RegMethod("Value", 0);
}



string Prop::PEType()
{
    return Elem::PEType() + GUri::KParentSep + Type();
}

Prop::Prop(const string& aName, MElem* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv)
{
    SetParent(Type());
}

Prop::Prop(MElem* aMan, MEnv* aEnv): Elem(Type(), aMan, aEnv)
{
    SetParent(Elem::PEType());
}

void *Prop::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    } else if (strcmp(aName, MProp::Type()) == 0) {
	res = (MProp*) this;
    } else {
	res = Elem::DoGetObj(aName);
    }
    return res;
}

const string& Prop::Value() const
{
    return iValue;
}

TBool Prop::ChangeCont(const string& aVal, TBool aRtOnly, const string& aName)
{
    TBool res = ETrue;
    //if (aVal != iValue) {
    if (true) {
	iValue = aVal;
	if (aRtOnly) {
	    iMan->OnContentChanged(*this);
	} else {
	    iMan->OnCompChanged(*this);
	}
    }
    return res;
}

string Prop::GetContent(const string& aName) const
{
    __ASSERT(aName.empty());
    return iValue;
}

MIface* Prop::MProp_Call(const string& aSpec, string& aRes)
{
    MIface* res = NULL;
    string name, sig;
    vector<string> args;
    Ifu::ParseIcSpec(aSpec, name, sig, args);
    TBool name_ok = MProp::mIfu.CheckMname(name);
    if (!name_ok) 
	throw (runtime_error("Wrong method name"));
    TBool args_ok = MProp::mIfu.CheckMpars(name, args.size());
    if (!args_ok) 
	throw (runtime_error("Wrong arguments number"));
    if (name == "Value") {
	aRes = Value();
    } else {
	throw (runtime_error("Unhandled method: " + name));
    }
    return  NULL;
}

string Prop::MProp_Mid() const
{
    return GetUri(iEnv->Root(), ETrue);
}


string Description::PEType()
{
    return Prop::PEType() + GUri::KParentSep + Type();
}

Description::Description(const string& aName, MElem* aMan, MEnv* aEnv): Prop(aName, aMan, aEnv)
{
    SetParent(Type());
}

Description::Description(MElem* aMan, MEnv* aEnv): Prop(Type(), aMan, aEnv)
{
    SetParent(Prop::PEType());
}

void *Description::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    } else if (strcmp(aName, MProp::Type()) == 0) {
	res = (MProp*) this;
    } else {
	res = Prop::DoGetObj(aName);
    }
    return res;
}


