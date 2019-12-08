Root : Elem
{
   #  PIA. Simple conn point, compatibility checking, iface resolution from pairs.  ;
  test : Systp;
  ./test < s1 : Systp;
  ./test/s1 < ConnPoints.Cp1 = {ConnPoint:{Provided:'MVertp' Required:'Iface2'}};
  ./test/s1 < ConnPoints.Cp2 = {ConnPoint:{Provided:'Iface3' Required:'Iface2'}};
  ./test < s2 : Systp;
  ./test/s2 < ConnPoints.Cp1 = {ConnPoint:{Provided:'Iface2' Required:'MVertp'}};
  ./test < Edges.E1 = {P1:'/Root/test/s1~ConnPoints.Cp1' P2:'/Root/test/s2~ConnPoints.Cp1'};
}
