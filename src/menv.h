#ifndef __GRAYB_MENV_H
#define __GRAYB_MENV_H

class MProvider;
class MLogRec;

// Executive environment interface
class MEnv
{
    public:
	virtual MProvider *Provider() const = 0;
	virtual MLogRec *Logger() = 0;
};


#endif
