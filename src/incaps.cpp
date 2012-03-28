
#include "incaps.h"
#include "edge.h"
#include "prov.h"
#include "mprop.h"

Incaps::Incaps(const string& aName, Elem* aMan, MEnv* aEnv): Syst(aName, aMan, aEnv)
{
    SetEType(Type());
    SetParent(Type());
    // Set mutation and mutate
    ChromoNode& mut = iMut->Root();
    ChromoNode ncaps = mut.AddChild(ENt_Node);
    ncaps.SetAttr(ENa_Id, "Capsule");
    ncaps.SetAttr(ENa_Parent, "Elem");
    Mutate();
}

void *Incaps::DoGetObj(const char *aName)
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

Elem* Incaps::Clone(const string& aName, Elem* aMan, MEnv* aEnv) const
{
    return new Incaps(aName, aMan, aEnv);
}

void Incaps::OnCompChanged(Elem& aComp)
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
		// Check if CPs belongs to capsule
		Elem* pt1man = pt1->GetMan();
		Elem* pt2man = pt2->GetMan(); 
		if (pt1man->Name() == "Capsule" && pt2man->Name() == "Capsule") {
		    MVert* pt1v = pt1->GetObj(pt1v);
		    MVert* pt2v = pt2->GetObj(pt2v);
		    if (pt1v != NULL && pt2v != NULL) {
			// Check roles conformance
			// Point#1 provided
			Elem* ept1prov = pt1->GetNode("Prop:Provided");
			MProp* ppt1prov = ept1prov->GetObj(ppt1prov);
			Elem* ept2req = pt2->GetNode("Prop:Required");
			MProp* ppt2req = ept2req->GetObj(ppt2req);
			// Point#2 provided
			Elem* ept2prov = pt2->GetNode("Prop:Provided");
			MProp* ppt2prov = ept2prov->GetObj(ppt2prov);
			Elem* ept1req = pt1->GetNode("Prop:Required");
			MProp* ppt1req = ept1req->GetObj(ppt1req);
			if (ppt1prov->Value() == ppt2req->Value() && ppt2prov->Value() == ppt1req->Value()) {
			    // Roles are compatible - connect
			    edge->SetPoints(pt1v, pt2v);
			    TBool res = edge->Connect();
			    if (res) {
				Logger()->WriteFormat("Incaps [%s] connected [%s - %s]", Name().c_str(), pt1u.c_str(), pt2u.c_str());
			    }
			    else {
				Logger()->WriteFormat("ERR: Incaps [%s] connecting [%s - %s] failed", Name().c_str(), pt1u.c_str(), pt2u.c_str());
			    }
			}
			else {
			    Logger()->WriteFormat("ERR: Incaps [%s] connecting [%s - %s] - incompatible roles", Name().c_str(), pt1u.c_str(), pt2u.c_str());
			}
		    }
		    else {
			Logger()->WriteFormat("ERR: Incaps [%s] connecting [%s - %s] - ends aren't vertexes", Name().c_str(), pt1u.c_str(), pt2u.c_str());
		    }
		} // pt1man->Name() == "Capsule"
		else {
		    Logger()->WriteFormat("ERR: Incaps [%s] connecting [%s - %s] - not capsule cp", Name().c_str(), pt1u.c_str(), pt2u.c_str());
		}
	    } // pt1 != NULL ...
	    else {
		Logger()->WriteFormat("ERR: Incaps [%s] connecting [%s - %s] - cannot find [%s]", Name().c_str(), pt1u.c_str(), pt2u.c_str(), 
			(pt1 == NULL)? pt1u.c_str(): pt2u.c_str());
	    }
	}
    }
}

