#ifndef __GRAYB_HNODE_H
#define __GRAYB_HNODE_H

#include "melem.h"
#include "menv.h"
#include "mlog.h"
#include "mmuta.h"
#include "mgraph.h"
#include "base.h"
#include "chromo.h"

class MProvider;

// Node of native hier
class HNode: public Base, public MElem 
{
    public:
	// name, parent
	typedef pair<string, string> TCkey;
	typedef pair<TCkey, Elem*> TCelem;
    public:
	static const char* Type() { return "Elem";};
	Elem(const string &aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	Elem* GetNode(const GUri& aUri);
	virtual Elem* GetNode(const GUri& aUri, GUri::const_elem_iter& aPathBase);
	void SetEType(const string& aEType);
	void SetMutation(const ChromoNode& aMuta);
	void Mutate(TBool aRunTimeOnly = EFalse);
    public:
	// From Base
	virtual void *DoGetObj(const char *aName);
	// From MElem
	virtual const string& EType() const;
	virtual const set<string>& CompsTypes();
    protected:
	Elem* AddElem(const ChromoNode& aSpec);
	static void Init();
	inline MLogRec* Logger();
	inline MProvider* Provider() const;
	TBool AppendComp(Elem* aComp);
	Elem* GetComp(const string& aParent, const string& aName);
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
	// Components
	map<TCkey, Elem*> iComps;
};

inline MLogRec* Elem::Logger() {return iEnv ? iEnv->Logger(): NULL; }

inline MProvider* Elem::Provider() const {return iEnv ? iEnv->Provider(): NULL; }


#endif
