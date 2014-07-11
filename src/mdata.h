#ifndef __GRAYB_MDATA_H
#define __GRAYB_MDATA_H

#include "plat.h"
#include <set>
#include <vector>
#include <string>

using namespace std;

class Base;

// Data interfaces
//
class MUpdatable
{
    public:
	static const char* Type() { return "MUpdatable";};
	virtual TBool Update() = 0;
};

// Confirmable data. For instance the double buffered data where the first part
// of data (intermediate result) can become final via confirmation
class MConfirmable
{
    public:
	static const char* Type() { return "MConfirmable";};
	virtual TBool Confirm() = 0;
};


class MDInt
{
    public:
	static const char* Type() { return "MDInt";};
	virtual TInt Data() const = 0;
	virtual void Set(TInt aData) = 0;
//	virtual void Update() = 0;
};

class MDFloat
{
    public:
	static const char* Type() { return "MDFloat";};
	virtual float Data() const = 0;
	virtual void Set(float aData) = 0;
};

class MDIntGet
{
    public:
	static const char* Type() { return "MDIntGet";};
	virtual TInt Value() = 0;
};

class MDFloatGet
{
    public:
	static const char* Type() { return "MDFloatGet";};
	virtual float Value() = 0;
};

class MDBoolGet
{
    public:
	static const char* Type() { return "MDBoolGet";};
	virtual TBool Value() = 0;
};

class MDIntSet
{
    public:
	static const char* Type() { return "MDIntSet";};
	virtual void SetValue(TInt aData) = 0;
};

class MDFloatSet
{
    public:
	static const char* Type() { return "MDFloatSet";};
	virtual void SetValue(float aData) = 0;
};

// Vector Int getting iface
class MVIntGet
{
    public:
	static const char* Type() { return "MVIntGet";};
	virtual vector<TInt> Value() = 0;
};



class MDataObserver
{
    public:
	static const char* Type() { return "MDataObserver";};
	virtual void OnDataChanged() = 0;
};

class MDVar
{
    public:
	static const char* Type() { return "MDVar";};
};

class Elem;
class MDVarGet
{
    public:
	static const char* Type() { return "MDVarGet";};
	virtual Elem* VarGetBase() = 0;
	// Get iface id. Is used for type negotiation from root to leafs
	virtual string VarGetIfid() const = 0;
};

class MDVarSet
{
    public:
	static const char* Type() { return "MDVarSet";};
	virtual Elem* VarSetBase() = 0;
};

// Scalar data
template <class T> class MDataGet
{
    public:
	static const char* Type();
	static const char* TypeSig();
	virtual void DataGet(T& aData) = 0;
};


// Vector
//typedef std::vector<float> VFloat;
template<class T> struct Vect: public std::vector<T> { };
//typedef std::vector<class T> Vect;
//template<class T> struct Vect { typedef std::vector<T> Type; };

template <class T> class MVectGet
{
    public:
	static const char* Type();
	static const char* TypeSig();
	virtual void VectGet(Vect<T>& aData) = 0;
};

// Daigonal matrix
template<class T> struct Mtrd: public vector<T> { };

template <class T> class MMtrdGet
{
    public:
	static const char* Type();
	static const char* TypeSig();
	virtual void MtrdGet(Mtrd<T>& aData) = 0;
};

// Matrix
struct MtrBase 
{ 
    public: 
	// Type of matrix
	enum TMtrType {
	    EMt_Unknown,
	    EMt_Regular,
	    EMt_Diagonal,
	    EMt_Utriang, // Upper-triangular
	    EMt_Btriang, // Bottom-triangular
	    EMt_Vector, // Column vector
	    EMt_String, // Line vector
	};
	// Dimension
	typedef pair<int, int> TMtrDim;
    public:
	TMtrType mType;
	TMtrDim mDim;
};

template<class T> struct Mtr: public MtrBase
{ 
    public:
	vector<T> mData;
	void ToString(string& aString) const;
};


template <class T> class MMtrGet
{
    public:
	static const char* Type();
	static const char* TypeSig();
	virtual TBool MtrGet(Mtr<T>& aData) = 0;
};



#endif
