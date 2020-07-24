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
        Adapter : /*/Modules/AdpComps/UnitAdp
        {
            AdpAgent < AgentUri = ./../../Target;
        }
    }
}
