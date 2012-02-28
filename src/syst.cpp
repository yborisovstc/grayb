
#include "syst.h"
#include "edge.h"
#include "prov.h"

Syst::Syst(const string& aName, Elem* aMan, MEnv* aEnv): Vert(aName, aMan, aEnv)
{
    iEType = Type();
}

void *Syst::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else {
	res = Vert::DoGetObj(aName);
    }
    return res;
}

Elem* Syst::Clone(const string& aName, Elem* aMan, MEnv* aEnv) const
{
    return new Syst(aName, aMan, aEnv);
}

void Syst::OnCompChanged(Elem& aComp)
{
    if (aComp.EType() == "Edge") {
	// Reconnect the edge
	Edge* edge = aComp.GetObj(edge);	
	__ASSERT(edge != NULL);
	edge->Disconnect();
	const string& pt1u = edge->Point1u();
	const string& pt2u = edge->Point2u();
	if (!pt1u.empty() && !pt2u.empty()) {
	    Elem* pt1 = GetNode(pt1u);
	    Elem* pt2 = GetNode(pt2u);
	    if (pt1 != NULL && pt2 != NULL) {
		MVert* pt1v = pt1->GetObj(pt1v);
		MVert* pt2v = pt2->GetObj(pt2v);
		if (pt1v != NULL && pt2v != NULL) {
		    edge->SetPoints(pt1v, pt2v);
		    TBool res = edge->Connect();
		    if (res) {
			Logger()->WriteFormat("Syst [%s] connected [%s - %s]", Name().c_str(), pt1u.c_str(), pt2u.c_str());
		    }
		    else {
			Logger()->WriteFormat("ERR: Syst [%s] connected [%s - %s] failed", Name().c_str(), pt1u.c_str(), pt2u.c_str());
		    }
		}
		else {
		    Logger()->WriteFormat("ERR: Syst [%s] connecting [%s - %s] - ends aren't vertexes", Name().c_str(), pt1u.c_str(), pt2u.c_str());
		}
	    }
	    else {
		Logger()->WriteFormat("ERR: Syst [%s] connecting [%s - %s] - cannot find [%s]", Name().c_str(), pt1u.c_str(), pt2u.c_str(), 
		    (pt1 == NULL)? pt1u.c_str(): pt2u.c_str());
	    }
	}
    }
}

