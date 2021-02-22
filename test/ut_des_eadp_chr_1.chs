testroot : Elem
{
    About = "Variant using DMut data";
    Modules : AImports
    {
        $ # " Module containing components of des layer ";
        $ + /DesComps/Des;
        $ + /AdpComps/UnitAdp;
        $ + /AdpComps/MelemAdp;
    }
    test : ./Modules/DesComps/Des
    {
        About = "MElem adapter test: mutation via chromo data";
        Log.Creation = y;
        Target : Elem;
        Target < Cmp_0 : Unit;
        Target < Cmp_1 : Unit;
        Controller : /*/Modules/DesComps/Des
        {
            About = "Controller using adapter for access to target";
            UnitAdp : /*/Modules/AdpComps/UnitAdp
            {
                AdpAgent < AgentUri = ./../../../Target;
                CompsCount < Debug.Update = y;
            }
            ElemAdp : /*/Modules/AdpComps/MelemAdp
            {
                AdpAgent < AgentUri = ./../../../Target;
            }
            $ # "Mutation";
            Mut : AStatec;
            Mut < Debug.Update = y;
            Mut < Value = "CHR2 : Unit { Comp : Unit; }";
            ElemAdp/InpMut ~ Mut;
        }
    }
}
