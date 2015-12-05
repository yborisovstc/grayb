
#include "imports.h"
#include "mprov.h"


string AImports::PEType()
{
    return Elem::PEType() + GUri::KParentSep + Type();
}

AImports::AImports(const string& aName, Elem* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv)
{
    SetEType(Type(), Elem::PEType());
    SetParent(Type());
}

AImports::AImports(Elem* aMan, MEnv* aEnv): Elem(Type(), aMan, aEnv)
{
    SetEType(Elem::PEType());
    SetParent(Elem::PEType());
}

AImports::~AImports()
{
}


void* AImports::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    } else if (strcmp(aName, MImports::Type()) == 0) {
	res = (MImports*) this;
    } else {
	res = Elem::DoGetObj(aName);
    }
    return res;
}

/*
TBool AImports::Import(const string& aUri)
{
    TBool res = ETrue;
    GUri moduri(aUri);
    if (!moduri.GetBase().empty()) {
	// Explicit chromo uri
	// Get the whole external chromo
	TBool res1 = AppendMutation(moduri.GetBase());
	// Rebasing uri to mut root, and get the target node
	GUri selr("."); 
	GUri::const_elem_iter it = moduri.Elems().begin(); 
	it++;
	selr.AppendTail(moduri, it);
	ChromoNode sel = iMut->Root().GetNode(selr);
	if (sel.Handle() != NULL) {
	    // Reduce chromo to target node, mutate and check
	    iMut->ReduceToSelection(sel);
	    Mutate();
	    res = GetNode(aUri);
	} else {
	    Logger()->Write(MLogRec::EErr, this, "Importing to module: cannot find chromo node [%s]", aUri.c_str());
	}
    }

    return res;
}
*/

TBool AImports::Import(const string& aUri)
{
    TBool res = ETrue;
    GUri moduri(aUri);
    GUri::const_elem_iter it = moduri.Elems().begin(); 
    it++;
    GUri::TElem urie = *it;
    string modname = urie.second.second;
    string modpath = iEnv->ImpsMgr()->GetModulePath(modname);
    if (!modpath.empty()) {
	// Explicit chromo uri
	// Get the whole external chromo
	TBool res1 = AppendMutation(modpath);
	// Rebasing uri to mut root, and get the target node
	GUri selr("."); 
	GUri::const_elem_iter it = moduri.Elems().begin(); 
	it++;
	selr.AppendTail(moduri, it);
	ChromoNode sel = iMut->Root().GetNodeByMhUri(selr);
	if (sel.Handle() != NULL) {
	    // Reduce chromo to target node, mutate and check
	    iMut->ReduceToSelection(sel);
	    Mutate(EFalse, ETrue, EFalse);
	    res = GetNode(aUri);
	} else {
	    Logger()->Write(MLogRec::EErr, this, "Importing module: cannot find chromo node [%s]", aUri.c_str());
	}
    } else {
	Logger()->Write(MLogRec::EErr, this, "Importing [%s]: cannot find module [%s]", aUri.c_str(), modname.c_str());
    }

    return res;
}

