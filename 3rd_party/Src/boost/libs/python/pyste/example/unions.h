namespace unions {

class UnionTest
{
public:
   union    // unions are not supported for now
   {
      int i;
      short s1;
      short s2;
   } mBad;

   int mGood;
};

}
