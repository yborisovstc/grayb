root : Elem
{
    $ # "Test";
    s1 : Syst
    {
        State : AStatec;
        State/Inp ~ : ATrcAddVar @ {
             Inp ~ : AStatec;
        };
    }
}
