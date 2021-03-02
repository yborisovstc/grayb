#include "rdatauri.h"



const char* DGuri::TypeSig() { return  "URI";};

TBool DGuri::IsSrepFit(const string& aString)
{
    return DtBase::IsSrepFit(aString, TypeSig());
}

TBool DGuri::IsDataFit(const DGuri& aData)
{
    TBool res = aData.mValid && aData.GetTypeSig() == TypeSig();
    return res;
}

TBool DGuri::DataFromString(istringstream& aStream, TBool& aRes)
{
    TBool changed = ETrue;
    GUri uri(aStream.str());
    aRes = !uri.IsErr();
    if (aRes) {
	if (mData != uri) {
	    mData = uri;
	}
    } else {
	changed = EFalse;
    }
    return changed;
}

void DGuri::DataToString(stringstream& aStream) const
{
    aStream << mData.operator string();
}




