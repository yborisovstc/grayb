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
        About = "Mounting agent test: mounting of simple model";
        Log.Creation = y;

        $ # "Model mounting";
        ModelMnt : AMntp;
        ModelMnt < EnvVar = Model;
    }
}
