#ifndef __GRAYB_PROFILER_EVENTS_H
#define __GRAYB_PROFILER_EVENTS_H

#include "profiler.h"

class PEvents
{
    public:
	enum {
	    Env_Start_Constr = 0,
	    Env_Root_Created,
	    Env_Root_Created_From_Start,
	    Env_End_Constr,
	    Elem_Constr_Start,
	    Elem_Constr_End,
	    Elem_Constr_Dur,
	};
	static MProfiler::TEventId KEidUndef;
	static const TProfilerEvents Events;
};

#endif


