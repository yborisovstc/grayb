
#include "capsule.h"
#include "edge.h"
#include "prov.h"

Capsule::Capsule(const string& aName, Elem* aMan, MEnv* aEnv): Syst(aName, aMan, aEnv)
{
    iEType = Type();
    // Adding membrane
    Elem* membr = Provider()->CreateNode("Syst", "Membr", this, iEnv);
    __ASSERT(membr != NULL);
    TBool res = AppendComp(membr);
    __ASSERT(res);
}

void *Capsule::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else {
	res = Syst::DoGetObj(aName);
    }
    return res;
}

void Capsule::OnCompChanged(Elem& aComp)
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
	    }
	    else {
		Logger()->WriteFormat("ERR: Syst [%s] connecting [%s - %s] - cannot find [%s]", Name().c_str(), pt1u.c_str(), pt2u.c_str(), 
		    (pt1 == NULL)? pt1u.c_str(): pt2u.c_str());
	    }
	}
    }
}

