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

MIface *Prop::DoGetObj(const char *aName)
{
    MIface* res = NULL;
    if (strcmp(aName, MProp::Type()) == 0) {
	res = dynamic_cast<MProp*>(this);
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
    if (aName.empty()) {
	iValue = aVal;
	iMan->OnCompChanged(*this, aName, aRtOnly);
	iEnv->Observer()->OnCompChanged(*this, aName, aRtOnly);
    } else {
	res = Elem::ChangeCont(aVal, aRtOnly, aName);
    }
    return res;
}

string Prop::GetContent(const string& aName, TBool aFull) const
{
    if (aName.empty()) {
	return iValue;
    } else {
	return Elem::GetContent(aName, aFull);
    }
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


