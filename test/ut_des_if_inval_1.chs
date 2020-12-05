testroot : Elem
{
    About = "Unit test of handling of iface invalidation (connect)";
    Modules : AImports
    {
        $ # " Module containing components of des layer ";
        $ + /DesComps/Des;
        $ + /DesComps/StcOutExt;
        $ + /DesComps/StcInpExt;
    }
    test : ./Modules/DesComps/Des
    {
        Syst_1 : ./../Modules/DesComps/Des
        {
            Comp_1 : AStatec;
	    Comp_1 < Debug.Update = y;
            Comp_1 < Value = "SI 3";
            Out1 : /*/Modules/DesComps/StcOutExt;
            Out1/Int ~ Comp_1;
        }
        Syst_2 : ./../Modules/DesComps/Des
        {
            Comp_1 : AStatec;
	    Comp_1 < Debug.Update = y;
            Comp_1 < Value = "SI 0";
            Inp1 : /*/Modules/DesComps/StcInpExt;
            Inp1/Int ~ Comp_1/Inp;
        }
    }
}
