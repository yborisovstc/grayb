
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

