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
            }
            $ # Components_count;
            CompCount : AStatec;
            CompCount < Value = "SI 0";
            CompCount < Debug.Update = y;
            CompCount/Inp ~ Adapter/CompsCount;
            $ # Components;
            CompNames : AStatec;
            CompNames < Value = "VS";
            CompNames < Debug.Update = y;
            CompNames/Inp ~ Adapter/CompNames;
        }
    }
}
