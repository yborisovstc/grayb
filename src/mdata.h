#ifndef __GRAYB_MDATA_H
#define __GRAYB_MDATA_H

#include "plat.h"
#include <set>
#include <vector>
#include <string>
#include "rdata.h"
#include "miface.h"

using namespace std;

class Base;

// Data interfaces
//
class MUpdatable: public MIface
{
    public:
	static const char* Type() { return "MUpdatable";};
	virtual TBool Update() = 0;
	// From MIface
	virtual MIface* MUpdatable_Call(const string& aSpec, string& aRes) { return NULL;}
	virtual string MUpdatable_Mid() const {return "?";}
	MIface* Call(const string& aSpec, string& aRes) override {return MUpdatable_Call(aSpec, aRes);}
	string Mid() const override { return MUpdatable_Mid();}
};

// Confirmable data. For instance the double buffered data where the first part
// of data (intermediate result) can become final via confirmation
class MConfirmable: public MIfaceStub
{
    public:
	static const char* Type() { return "MConfirmable";};
	virtual TBool Confirm() = 0;
};


class MDInt: public MIface
{
    public:
	static const char* Type() { return "MDInt";};
	virtual TInt Data() const = 0;
	virtual void Set(TInt aData) = 0;
	// From MIface
	virtual MIface* MDInt_Call(const string& aSpec, string& aRes) { return NULL;}
	virtual string MDInt_Mid() const {return "?";}
	MIface* Call(const string& aSpec, string& aRes) override {return MDInt_Call(aSpec, aRes);}
	string Mid() const override { return MDInt_Mid();}
};

class MDFloat: public MIfaceStub
{
    public:
	static const char* Type() { return "MDFloat";};
	virtual float Data() const = 0;
	virtual void Set(float aData) = 0;
};

class MDIntGet: public MIface
{
    public:
	static const char* Type() { return "MDIntGet";};
	virtual TInt Value() = 0;
	virtual MIface* MDIntGet_Call(const string& aSpec, string& aRes) = 0;
	virtual string MDIntGet_Mid() const = 0;
	MIface* Call(const string& aSpec, string& aRes) override {return MDIntGet_Call(aSpec, aRes);}
	string Mid() const override { return MDIntGet_Mid();}
};

class MDFloatGet: public MIfaceStub
{
    public:
	static const char* Type() { return "MDFloatGet";};
	virtual float Value() = 0;
};

class MDBoolGet: public MIfaceStub
{
    public:
	static const char* Type() { return "MDBoolGet";};
	virtual TBool Value() = 0;
};

class MDIntSet: public MIface
{
    public:
	static const char* Type() { return "MDIntSet";};
	virtual void SetValue(TInt aData) = 0;
	// From MIface
	virtual MIface* MDIntSet_Call(const string& aSpec, string& aRes) { return NULL;}
	virtual string MDIntSet_Mid() const {return "?";}
	MIface* Call(const string& aSpec, string& aRes) override {return MDIntSet_Call(aSpec, aRes);}
	string Mid() const override { return MDIntSet_Mid();}
};

class MDFloatSet: public MIfaceStub 
{
    public:
	static const char* Type() { return "MDFloatSet";};
	virtual void SetValue(float aData) = 0;
};

// Vector Int getting iface
class MVIntGet: public MIfaceStub
{
    public:
	static const char* Type() { return "MVIntGet";};
	virtual vector<TInt> Value() = 0;
};



class MDataObserver: public MIface
{
    public:
	static const char* Type() { return "MDataObserver";};
	virtual void OnDataChanged() = 0;
	// From MIface
	virtual MIface* MDataObserver_Call(const string& aSpec, string& aRes) { return NULL;}
	virtual string MDataObserver_Mid() const {return "?";}
	MIface* Call(const string& aSpec, string& aRes) override {return MDataObserver_Call(aSpec, aRes);}
	string Mid() const override { return MDataObserver_Mid();}
};

class MDVar: public MIface
{
    public:
	static const char* Type() { return "MDVar";};
	// From MIface
	virtual MIface* MDVar_Call(const string& aSpec, string& aRes) { return NULL;}
	virtual string MDVar_Mid() const {return "?";}
	MIface* Call(const string& aSpec, string& aRes) override {return MDVar_Call(aSpec, aRes);}
	string Mid() const override { return MDVar_Mid();}
};

class Elem;
class MDVarGet: public MIface
{
    public:
	static const char* Type() { return "MDVarGet";};
	template <class T> T* GetDObj(T* aInst) { return aInst = static_cast<T*>(DoGetDObj(aInst->Type()));};
	void* GetDObj(const char *aType) { return DoGetDObj(aType); };
	// TODO YB To return MIface?
	virtual void *DoGetDObj(const char *aName) { return NULL;};
	// Get iface id. Is used for type negotiation from root to leafs
	virtual string VarGetIfid() = 0;
	// From MIface
	virtual string Uid() const override { return Mid() + "%" + Type();};
	virtual MIface* MDVarGet_Call(const string& aSpec, string& aRes) { return NULL;}
	virtual string MDVarGet_Mid() const {return "?";}
	MIface* Call(const string& aSpec, string& aRes) override {return MDVarGet_Call(aSpec, aRes);}
	string Mid() const override { return MDVarGet_Mid();}
};

class MDVarSet: public MIface
{
    public:
	static const char* Type() { return "MDVarSet";};
	virtual MIface *DoGetSDObj(const char *aName) { return NULL;};
	// Get iface id. Is used for type negotiation from root to leafs
	virtual string VarGetSIfid() = 0;
	// From MIface
	virtual MIface* MDVarSet_Call(const string& aSpec, string& aRes) { return NULL;}
	virtual string MDVarSet_Mid() const {return "?";}
	MIface* Call(const string& aSpec, string& aRes) override {return MDVarSet_Call(aSpec, aRes);}
	string Mid() const override { return MDVarSet_Mid();}
};

// Simple scalar data
template <class T> class MDataGet: public MIfaceStub
{
    public:
	static const char* Type();
	static const char* TypeSig();
	virtual void DataGet(T& aData) = 0;
};

// Generic data

class MDtGetBase: public MIfaceStub
{
    public:
	static const char* TypeBase() { return "MDtGet";};
	static void GetType(const string& aDtTypeSig, string& aRes) { aRes = string(TypeBase()) + "_" + aDtTypeSig;};
};

template <class T> class MDtGet: public MDtGetBase
{
    public:
	static const string mType;
	static const char* Type() { return mType.c_str();};
	virtual void DtGet(T& aData) = 0;
};

template<class T> const string MDtGet<T>::mType = string(TypeBase()) + "_" + T::TypeSig();

class MDtSetBase: public MIfaceStub
{
    public:
	static const char* TypeBase() { return "MDtSet";};
	static void GetType(const string& aDtTypeSig, string& aRes) { aRes = string(TypeBase()) + "_" + aDtTypeSig;};
};

/** @brief Generic data setter interface
 * */
template <class T> class MDtSet: public MDtSetBase
{
    public:
	static const string mType;
	static const char* Type() { return mType.c_str();};
	virtual void DtSet(const T& aData) = 0;
};

template<class T> const string MDtSet<T>::mType = string(TypeBase()) + "_" + T::TypeSig();

// TODO [YB] Obsolete, replaced ty MMtrGet, to remove
// Vector
template<class T> struct Vect: public std::vector<T> { };

template <class T> class MVectGet: public MIfaceStub
{
    public:
	static const char* Type();
	static const char* TypeSig();
	virtual void VectGet(Vect<T>& aData) = 0;
};

// TODO [YB] Obsolete, replaced ty MMtrGet, to remove
// Daigonal matrix
template<class T> struct Mtrd: public vector<T> { };

template <class T> class MMtrdGet: public MIfaceStub
{
    public:
	static const char* Type();
	static const char* TypeSig();
	virtual void MtrdGet(Mtrd<T>& aData) = 0;
};

// Matrix
template <class T> class MMtrGet: public MIfaceStub
{
    public:
	static const char* Type();
	static const char* TypeSig();
	virtual void MtrGet(Mtr<T>& aData) = 0;
};


/** @brief Interface of variant base data host
 * */
class MBdVarHost
{
    public:
	virtual MDVarGet* HGetInp(const Base* aRmt) = 0;
	virtual void HOnDataChanged(const Base* aRmt) = 0;
	virtual string GetUid() { return "?";}
	virtual string GetDvarUid(const MDVar* aComp) const { return "BdVar";}
};

template <typename T>
class MVectorGet : public MIface
{
    public:
	static const char* Type();
    public: 
	virtual int Size() const = 0;
	virtual TBool GetElem(int aInd, T& aElem) const = 0;
	virtual string MVectorGet_Mid() const = 0;
	virtual MIface* MVectorGet_Call(const string& aSpec, string& aRes) = 0;
	// From MIface
	MIface* Call(const string& aSpec, string& aRes) override {return MVectorGet_Call(aSpec, aRes);}
	string Mid() const override { return MVectorGet_Mid();}
};

#endif
