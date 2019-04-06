
#include "imports.h"
#include "mprov.h"

string AImports::PEType()
{
    return Elem::PEType() + GUri::KParentSep + Type();
}

AImports::AImports(const string& aName, MUnit* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv)
{
    SetCrAttr(PEType(), aName);
    //SetParent(Type());
}

AImports::AImports(MUnit* aMan, MEnv* aEnv): Elem(Type(), aMan, aEnv)
{
    __ASSERT(false);
    SetParent(Elem::PEType());
}

AImports::~AImports()
{
}

TBool AImports::Import(const string& aUri)
{
    TBool res = ETrue;
    GUri moduri(aUri);
    GUri::const_elem_iter it = moduri.Elems().begin(); 
    it++;
    GUri::TElem urie = *it;
    string modname = urie.name();
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
	    Logger()->Write(EErr, this, "Importing module: cannot find chromo node [%s]", aUri.c_str());
	}
    } else {
	Logger()->Write(EErr, this, "Importing [%s]: cannot find module [%s]", aUri.c_str(), modname.c_str());
    }

    return res;
}

