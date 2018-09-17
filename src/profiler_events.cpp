
#include "profiler_events.h"

MProfiler::TEventId PEvents::KEidUndef = MProfiler::TEvent::KUndefEid;

/** Profile events register */
const TProfilerEvents PEvents::Events = {
    {Env_Start_Constr, {"Env start constr", KEidUndef}},
    {Env_Root_Created, {"Env Root Created", KEidUndef}},
    {Env_Root_Created_From_Start, {"Env Root Created From Start", Env_Start_Constr}},
    {Env_End_Constr, {"Env End Constr", KEidUndef}},
    {Elem_Constr_Start, {"Elem constructor start", KEidUndef}},
    {Elem_Constr_End, {"Elem constructor end", KEidUndef}},
    {Elem_Constr_Dur, {"Elem constructor duration", Elem_Constr_Start}}
};
