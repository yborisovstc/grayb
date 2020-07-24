testroot : Elem
{
    Modules : AImports
    {
        $ # " Module containing components of des layer ";
        $ + /DesComps/Des;
        $ + /AdpComps/UnitAdp;
    }
    test : ./Modules/DesComps/Des
    {
        About = "MUnit adapter test: observation of compoments number";
        Log.Creation = y;
        Target : Unit;
        Target < Cmp_0 : Unit;
        Target < Cmp_1 : Unit;
        Controller : /*/Modules/DesComps/Des
        {
            About = "Controller using adapter for access to target";
            Adapter : /*/Modules/AdpComps/UnitAdp
            {
                AdpAgent < AgentUri = ./../../../Target;
                CompUid < Debug.Update = y;
            }
            $ # "Comps count index incrementor";
            CompIdxCount : AStatec;
            CompIdxCount < Value = "SI 0";
            CompIdxCount < Debug.Update = y;
            Incr : ATrcAddVar;
            Const_1 : AStatec;
            Const_1 < Value = "SI 1";
            CompIdxCount/Inp ~ Incr;
            Incr/Inp ~ CompIdxCount;
            Incr/Inp ~ Const_1;
            Adapter/InpCompIdx ~ CompIdxCount;
        }
    }
}
