#ifndef __GRAYB_GURI_H
#define __GRAYB_GURI_H

#include <utility>
#include <vector>
#include <map>
#include <string>

using namespace std;

enum TNodeType
{
    ENt_Unknown = 0,
    ENt_Graph = 1,
    ENt_Vert = 2,
    ENt_Edge = 3,   	// Connection
    ENt_Log = 4,  	// Logging specification
    ENt_Add = 5,      	// Mutation - add
    ENt_Move = 7, 	// Mutation - move node
    ENt_Rm = 8,   	// Mutation - removal
    ENt_Change = 9, 	// Change node attribute
};

enum TNodeAttr
{
    ENa_Unknown = 0,
    ENa_Id = 1,
    ENa_Parent = 2,
    ENa_MutNode = 10,
    ENa_MutAttr = 11,
    ENa_MutVal = 12,
};

// Graph URI, dedicated only for locating graph element within native graph hier, ref md sec_refac_primhier
class GUri
{
    public:
	enum TQueryOpr {
	    EQop_Unknown,
	    EQop_And
	};
    public:
	// Element type: type (parent), name
	typedef pair<TNodeType, string> TElem;
	typedef pair<TNodeAttr, string> TQueryCnd;
	typedef pair<TQueryOpr, TQueryCnd> TQueryElem;
	typedef vector<TElem>::const_iterator const_elem_iter;
    public:
	GUri(const string& aGUri);
	GUri();
	const vector<TElem>& Elems() const {return iElems;};
	string GetUri(vector<TElem>::const_iterator aStart) const;
	string GetUri() const { return GetUri(iElems.begin());};
	TNodeType GetType() const;
	string GetName() const;
	void AppendElem(TNodeType aType, const string& aName);
	void PrependElem(TNodeType aType, const string& aName);
	void AppendQueryElem(TQueryOpr aOpr, TNodeAttr aAttr, const string& aValue);
	static const string& NodeAttrName(TNodeAttr aAttr);
	static const string& NodeTypeName(TNodeType aType);
	static TNodeAttr NodeAttr(const string& aAttrName);
	static TNodeType NodeType(const string& aTypeName);
    protected:
	static void Construct();
    private:
	void Parse();
    private:
	static map<string, TNodeType> iEbNameToNType;
	string iUri;
	string iScheme;
	vector<TElem> iElems;
	vector<TQueryElem> iQueryElems;
	static TBool iInit;
};

#endif
