SysComps : Elem
{
  ConnPoint : ConnPointBase
  {
    Provided : Prop;
    Required : Prop;
  }
  ConnPointInp : ConnPointBaseInp
  {
    Provided : Prop;
    Required : Prop;
  }
  ConnPointOut : ConnPointBaseOut
  {
    Provided : Prop;
    Required : Prop;
  }
  ConnPointInpMc : ConnPointMc
  {
    Direction = Inp;
  }
  ConnPointOutMc : ConnPointMc
  {
    Direction = Out;
  }
  $ # " Element that acts as feedthrough for capsula of system - it extend
	 the connection from outside into the system. The extention is implemented via embedding inernal CP,
	 that is the point to connect from within the owning system.
	 Extender is fitted by simple CP as default. In case of enhanced extender is required this 
	 CP needs to be replaced by the enhanced conn point. 
	 ";
  ExtenderBase : Vert
  {
    $ # "node id=\"Int\" ";
    MAExteder : ExtenderAgent;
  }
  Extender : Vert
  {
    Int : ./../ConnPoint;
    MAExteder : ExtenderAgent;
  }
  ExtenderInpBase : Vert
  {
    $ # "node id=\"Int\"";
    MAExteder : ExtenderAgentInp;
  }
  ExtenderInp : Vert
  {
    Int : ./../ConnPoint;
    MAExteder : ExtenderAgentInp;
  }
  ExtenderOutBase : Vert
  {
    $ # "node id=\"Int\"";
    MAExteder : ExtenderAgentOut;
  }
  ExtenderOut : Vert
  {
    Int : ./../ConnPoint;
    MAExteder : ExtenderAgentOut;
  }
  ExtenderMc : Vert
  {
    About = "Base extender. Need to add internal connectable named Int";
    MAExteder : AExtender;
  }
  ExtenderInpMc : ./ExtenderMc
  {
    Direction = Inp;
  }
  ExtenderOutMc : ./ExtenderMc
  {
    Direction = Out;
  }
  ExtenderInpMcm : AExtd
  {
    Direction = Inp;
  }
  ExtenderOutMcm : AExtd
  {
    Direction = Out;
  }
  CpExtenderMc : Vert
  {
    About = "Extender of simple connpoint.";
    Int : ConnPointMc;
    MAExteder : AExtender;
  }
  CpExtenderInpMc : ./CpExtenderMc
  {
    Direction = Inp;
  }
  CpExtenderOutMc : ./CpExtenderMc
  {
    Direction = Out;
  }
  $ # " Socket ";
  Socket : Vert
  {
    Sock : ASocket;
  }
  $ # " Input Socket ";
  SocketInp : Vert
  {
    Sock : ASocketInp;
  }
  $ # " Output Socket ";
  SocketOut : Vert
  {
    Sock : ASocketOut;
  }
  SocketMc : Vert
  {
    Sock : ASocketMc;
  }
  SocketInpMc : ./SocketMc
  {
    Direction = Inp;
  }
  SocketOutMc : ./SocketMc
  {
    Direction = Out;
  }
}
