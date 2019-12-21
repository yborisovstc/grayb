test : Syst
{
  Modules : AImports
  {
    $ + /SysComps/ConnPoint;
    $ + /SysComps/Extender;
  }
  Syst1 : Syst
  {
    Logspec : Elem
    {
      Creation : Elem;
    }
    cp : /test/Modules/SysComps/ConnPoint
    {
      ./Provided -  = Role1;
      ./Required -  = Role2;
    }
    ep : /test/Modules/SysComps/Extender
    {
      ./Int/Provided -  = Role4;
      ./Int/Required -  = Role3;
    }
    scp1 : ConnPointMc
    {
      Provided = Role1;
      Required = Role2;
    }
  }
  ./Syst1 - Syst1_1 : Syst;
  ./Syst1/Syst1_1 - Syst1_1_1 : Syst;
  cp1 : /test/Modules/SysComps/ConnPoint
  {
    ./Provided -  = Role1;
    ./Required -  = Role2;
  }
  cp2 : /test/Modules/SysComps/ConnPoint
  {
    ./Provided -  = Role2;
    ./Required -  = Role1;
  }
  cp3 : /test/Modules/SysComps/ConnPoint
  {
    ./Provided -  = Role4;
    ./Required -  = Role3;
  }
  E1 : Aedge
  {
    P1 = ./../cp1;
    P2 = ./../cp2;
  }
  E2 : Edge
  {
    ./P1 - $ = ./../../Syst1/cp;
    ./P2 - $ = ./../../cp2;
  }
  E3 : Aedge
  {
    P1 = ./../Syst1/ep;
    P2 = ./../cp3;
  }
  E2mc : Aedge
  {
    P1 = ./../Syst1/scp1;
    P2 = ./../cp2;
  }
}
