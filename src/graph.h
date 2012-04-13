#ifndef __GRAYB_GRAPH_H
#define __GRAYB_GRAPH_H

#include "mgraph.h"
#include "elem.h"

// Graph base
class Graph: public Elem, public MGraph
{
    public:
	static const char* Type() { return "Graph";};
	Graph(const string &aName, Elem* aMan, MEnv* aEnv);
    public:
	static void Init();
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue);
	// From Elem
	virtual TBool AddNode(const ChromoNode& aSpec);
	virtual TBool DoMutation(const ChromoNode& aCromo);
	virtual const set<string>& CompsTypes();
    protected:
	// Components types
	static set<string> iCompsTypes;
	// Sign of inited
	static bool iInit;
};

#endif
