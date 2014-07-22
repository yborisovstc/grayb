#ifndef __GRAYB_MENV_H
#define __GRAYB_MENV_H

class MProvider;
class MLogRec;
class Elem;
class MChromo;

// Chromo manager
class MChromoMgr
{
    public:
	virtual int GetOrder(const MChromo& aChromo) const = 0;
};

// Executive environment interface
class MEnv
{
    public:
	virtual MProvider *Provider() const = 0;
	virtual MLogRec *Logger() = 0;
	virtual Elem* Root() = 0;
	virtual MChromoMgr* ChMgr() = 0;
};


#endif
