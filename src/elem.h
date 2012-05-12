#ifndef __GRAYB_ELEM_H
#define __GRAYB_ELEM_H


#include "melem.h"
#include "menv.h"
#include "mlog.h"
#include "mmuta.h"
#include "base.h"
#include "chromo.h"

class Chromo;
class MProvider;

// Element of native hier - mutable
class Elem: public Base, public MMutable, public MCompsObserver
{
    public:
	// name, parent
	typedef pair<string, string> TCkey;
	typedef pair<TCkey, Elem*> TCelem;
	typedef multimap<TCkey, Elem*> TMElem;
    public:
	static const char* Type() { return "Elem";};
	Elem(const string &aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	Elem* GetNode(const GUri& aUri);
	GUri GetUri(const Elem* aElem) const;
	virtual ~Elem();
	void SetEType(const string& aEType);
	void SetParent(const string& aParent);
	void SetMan(Elem* aMan);
	void SetMutation(const ChromoNode& aMuta);
	void Mutate(TBool aRunTimeOnly = EFalse);
	string PName() const;
	const vector<Elem*>& Comps() const;
    public:
	// aInitCont - the uri of initial context of element, this is requires to element "understand" that it is
	// in new context now and corrected uris related to initial context
	Elem* CreateHeir(const string& aName, Elem* aMan/*, const GUri& aInitCont*/);
	const MChromo& Chromos() { return *iChromo;};
	MChromo& Mutation() { return *iMut;};
	// Gets the comp with given type and owning given element
	Elem* GetCompOwning(const string& aParent, Elem* aElem);
	Elem* GetCompOwning(Elem* aElem);
	TBool IsComp(Elem* aElem);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue);
	// From MElem
	virtual const string& EType() const;
	virtual const set<string>& CompsTypes();
	virtual Elem* GetMan();
	virtual Elem* GetNode(const string& aUri);
	virtual Elem* GetNode(const GUri& aUri, GUri::const_elem_iter& aPathBase);
	virtual Elem* GetNodeLoc(const GUri::TElem& aElem);
	virtual TBool ChangeCont(const string& aVal); 
	virtual TBool AddNode(const ChromoNode& aSpec);
	const vector<Elem*>& Comps();
	// From MCompsObserver
	virtual void OnCompDeleting(Elem& aComp);
	virtual void OnCompAdding(Elem& aComp);
	virtual void OnCompChanged(Elem& aComp);
	// From MMutable
	virtual void DoMutation(const ChromoNode& aCromo, TBool aRunTime);
    protected:
	Elem* AddElem(const ChromoNode& aSpec);
	static void Init();
	inline MLogRec* Logger();
	inline MProvider* Provider() const;
	TBool AppendComp(Elem* aComp);
	Elem* GetComp(const string& aParent, const string& aName);
	TBool DoMutChangeCont(const ChromoNode& aSpec);
	TBool MergeMutation(const ChromoNode& aSpec);
	TBool MergeMutMove(const ChromoNode& aSpec);
	Elem* GetComp(TInt aInd);
	virtual void DoOnCompChanged(Elem& aComp);
    protected:
	// Element type - parent's name
	string iEType;
	// Environment
	MEnv* iEnv;
	// Managed (higher) element of hier
	Elem* iMan;
	// Chromo
	Chromo* iChromo;
	// Mutation
	Chromo* iMut;
	// Components types
	static set<string> iCompsTypes;
	// Sign of inited
	static bool iInit;
	// Components, owninig container
	vector<Elem*> iComps;
	// Components map, not owning
	multimap<TCkey, Elem*> iMComps;
};

inline MLogRec* Elem::Logger() {return iEnv ? iEnv->Logger(): NULL; }

inline MProvider* Elem::Provider() const {return iEnv ? iEnv->Provider(): NULL; }


// Runtime agent - implements interfaces that can be used by the man

class Agent: public Elem
{
    public:
    static const char* Type() { return "Agent";};
    Agent(const string &aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
    // From Base
    virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue);
};

#endif
