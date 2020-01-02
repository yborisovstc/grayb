IncapsComps : Elem
{
  $ # " Base elements for Incapsulated system layer. Capsula is incapsulated system. Its internal structure
     is hidded from outside world, the only connection points are the way of relation to the system. 
     ";
  Incaps : Syst
  {
    MACompsObserver : IncapsAgent;
    $ # " Element that acts as place for all extenders of the system.
	 IncapsSystem allows the connection outside only for those EPs that are in capsula ";
    Capsule : ACapsule;
  }
}
