test : Syst
{
  _ #  Module containing components of system layer ;
  sysmod : file:../modules/syst.xml#SysComps;
  Syst1 : Syst;
  ./Syst1 < cp : ./../sysmod/ConnPoint;
  ./Syst1/cp/Provided <  = Role1;
  ./Syst1/cp/Required <  = Role2;
  ./Syst1 < ep : ./../sysmod/Extender;
  ./Syst1/ep/Int/Provided <  = Role3;
  ./Syst1/ep/Int/Required <  = Role4;
  cp1 : ./sysmod/ConnPoint;
  ./cp1/Provided <  = Role1;
  ./cp1/Required <  = Role2;
  cp2 : ./sysmod/ConnPoint;
  ./cp2/Provided <  = Role2;
  ./cp2/Required <  = Role1;
  cp3 : ./sysmod/ConnPoint;
  ./cp3/Provided <  = Role4;
  ./cp3/Required <  = Role3;
  E1 : Edge;
  ./E1/P1 <  = ;
  ./E1/P2 <  = ;
  edge1 : Edge;
  ./edge1/P1 <  = ;
  ./edge1/P2 <  = ;
  E2 : Edge;
  ./E2/P1 <  = ;
  ./E2/P2 <  = ;
}
