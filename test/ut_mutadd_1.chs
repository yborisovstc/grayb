test : Elem
{
  elem1 : Elem;
  ./elem1 < elem2 : Elem;
  ./elem1 < unit2 : Unit;
  ./unit2 -   ./elem1 < unit3 : Unit;
  ./elem1/elem2 < elem2_1 : Elem;
  elem3 : ./elem1;
  ./elem3 < elem4 : ./../elem1/elem2;
  ./elem3/elem4 < elem4_1 : Elem;
  elem5 : ./elem1/elem2;
  elem4_1i : ./elem3/elem4/elem4_1;
}
