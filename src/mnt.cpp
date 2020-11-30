
#include "mnt.h"


// Content: env variable kept mounted model path
const string KCntEnvVar = "EnvVar";

string AMntp::PEType()
{
    return Elem::PEType() + GUri::KParentSep + Type();
}

AMntp::AMntp(const string& aName, MUnit* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv)
{
    SetCrAttr(PEType(), aName);
    InsertContent(KCntEnvVar);
}

AMntp::~AMntp() 
{
    if (mEnv) {
	delete mEnv;
	mEnv = NULL;
    }
}

TEhr AMntp::ProcessCompChanged(const MUnit* aComp, const string& aContName)
{
    TEhr res = EEHR_Ignored;
    if (aComp == this && aContName == KCntEnvVar) {
	string envvid = GetContent(KCntEnvVar);
	string mpath;
	bool eres = iEnv->GetEVar(envvid, mpath);
	if (eres) {
	    Logger()->Write(EInfo, this, "Model path from evar: %s", mpath.c_str());
	    TBool cres = CreateModel(mpath);
	    if (cres) {
		cres = AppendComp(mEnv->Root(), ETrue);
		if (!cres) {
		    Logger()->Write(EErr, this, "Cannot attach the model [%s]", mpath.c_str());
		}
	    } else {
		res = EEHR_Denied;
	    }
	} else {
	    Logger()->Write(EErr, this, "Cannot find env variable [%s]", envvid.c_str());
	}
    } else {
	res = Elem::ProcessCompChanged(aComp, aContName);
    }
    return res;
}

/*
   TBool AMntp::ImportModel(const string& aModelPath)
   {
   TBool res = ETrue;
    MChromo* chromo = mHost.Provider()->CreateChromo();
    TBool res1 = chromo->Set(modpath);
    ChromoNode sel = chromo->Root().GetNodeByMhUri(moduri);
    delete chromo;
    return res;
}
*/

TBool AMntp::CreateModel(const string& aSpecPath)
{
    TBool res = ETrue;
    if (mEnv != nullptr) {
	delete mEnv;
	mEnv = nullptr;
    }
    // Create model
    mEnv = new Env(aSpecPath, aSpecPath + ".log");
    assert(mEnv != nullptr);
    mEnv->ConstructSystem();
    if (!mEnv->Root()) {
	Logger()->Write(EErr, this, "Cannot create model [%s]", aSpecPath.c_str());
	res = EFalse;
    }
    return res;
}



