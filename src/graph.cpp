#include "graph.h"

set<TNodeType> Graph::iCompsTypes;
bool Graph::iInit = false;

Graph::Graph(const string &aName): Elem(aName) 
{
}

void Graph::Init()
{
    if (!iInit) {
	iCompsTypes.insert(ENt_Node);
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

const set<TNodeType>& Graph::CompsTypes()
{
    return iCompsTypes;
}

