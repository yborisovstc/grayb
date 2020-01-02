testroot : Elem
{
  Modules : AImports
  {
    $ + /SysComps/ConnPoint;
    $ + /SysComps/Extender;
    $ + /IncapsComps/Incaps;
  }
  test : /testroot/Modules/IncapsComps/Incaps
  {
    Ics1 : /testroot/Modules/IncapsComps/Incaps
    {
      cp_int : /testroot/Modules/SysComps/ConnPoint
      {
        ./Provided - $ = Role3;
        ./Required - $ = Role4;
      }
      ./Capsule - ep : /testroot/Modules/SysComps/Extender
      {
        ./Int/Provided - $ = Role4;
        ./Int/Required - $ = Role3;
      }
    }
    Ics2 : /testroot/Modules/IncapsComps/Incaps
    {
      ./Capsule - ep : /testroot/Modules/SysComps/Extender
      {
        ./Int/Provided - $ = Role3;
        ./Int/Required - $ = Role4;
      }
    }
    $ # " Trying to connect extention points from capsules - right way to connect ";
    E1 : Edge
    {
      ./P1 - $ = ./../../Ics1/Capsule/ep;
      ./P2 - $ = ./../../Ics2/Capsule/ep;
    }
    $ # " Trying to connect internal conn points - that should be refused by incapsulated system ";
    E2 : Edge
    {
      ./P1 - $ = ./../../Ics1/cp_int;
      ./P2 - $ = ./../../Ics2/Capsule/ep;
    }
  }
}
