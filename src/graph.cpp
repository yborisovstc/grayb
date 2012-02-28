#include "graph.h"

set<string> Graph::iCompsTypes;
bool Graph::iInit = false;

Graph::Graph(const string &aName, Elem* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv) 
{
}

void Graph::Init()
{
    if (!iInit) {
	iCompsTypes.insert("Vert");
	iCompsTypes.insert("Edge");
	iInit = true;
    }
}

void* Graph::DoGetObj(const char *aName)
{
    return (strcmp(aName, Type()) == 0) ? this : NULL;
}

TBool Graph::AddNode(const ChromoNode& aSpec)
{
}

TBool Graph::DoMutation(const ChromoNode& aCromo)
{
}

const set<string>& Graph::CompsTypes()
{
    return iCompsTypes;
}

