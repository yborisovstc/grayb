
#include "rmutdata.h"

const char* DMut::TypeSig() { return  "MUT";};

TBool DMut::IsSrepFit(const string& aString)
{
    return DtBase::IsSrepFit(aString, TypeSig());
}

TBool DMut::IsDataFit(const DMut& aData)
{
    TBool res = aData.mValid && aData.GetTypeSig() == TypeSig();
    return res;
}

TBool DMut::DataFromString(istringstream& aStream, TBool& aRes)
{
    TBool changed = ETrue;
    TMut mut(aStream.str());
    aRes = mut.IsValid();
    if (aRes) {
    // changed = mData != mut;
	mData = mut;
    } else {
	changed = EFalse;
    }
    return changed;
}

void DMut::DataToString(stringstream& aStream) const
{
    aStream << mData.ToString();
}



const char* DChr2::TypeSig() { return  "CHR2";};

TBool DChr2::IsSrepFit(const string& aString)
{
    return DtBase::IsSrepFit(aString, TypeSig());
}

TBool DChr2::IsDataFit(const DChr2& aData)
{
    TBool res = aData.mValid && aData.GetTypeSig() == TypeSig();
    return res;
}

TBool DChr2::DataFromString(istringstream& aStream, TBool& aRes)
{
    TBool changed = ETrue;
    mData.SetFromSpec(aStream.str());
    aRes = !mData.IsError();
    if (aRes) {
    } else {
	changed = EFalse;
    }
    return changed;
}

void DChr2::DataToString(stringstream& aStream) const
{
    string spec;
    DChr2* self = const_cast<DChr2*>(this);
    self->mData.GetSpec(spec);
    aStream << spec;
}

