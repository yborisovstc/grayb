testroot : Elem
{
    Modules : AImports
    {
        $ # " Module containing components of des layer ";
        $ + /DesComps/Des;
    }
    test : ./Modules/DesComps/Des
    {
        About = "Simple test of Vector state and transitions";
        Log.Creation = y;
        State : AStatec;
        ./State - Value = "VS Test";
        ./State - Debug.Update = y;
        Result : AStatec;
        ./Result - Value = "SS Nil";
        ./Result - Debug.Update = y;
        Idx : AStatec;
        ./Idx - Debug.Update = y;
        At : ATrcAtVar;
        Result/Inp ~ At;
        At/Inp ~ State;
        At/Idx ~ Idx;
    }
}
