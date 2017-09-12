import unittest
from polymorphism_ext import *

class PolymorphTest(unittest.TestCase):

   def testReturnCpp(self):

      # Python Created Object With Same Id As
      # Cpp Created B Object 
      # b = B(872)  

      #  Get Reference To Cpp Created B Object
      a = getBCppObj()

      # Python Created B Object and Cpp B Object
      # Should have same result by calling f()
      self.failUnlessEqual ('B::f()', a.f())
      self.failUnlessEqual ('B::f()', call_f(a))
      self.failUnlessEqual ('A::f()', call_f(A()))

   def test_references(self):
      # B is not exposed to Python
      a = getBCppObj()
      self.failUnlessEqual(type(a), A)

      # C is exposed to Python
      c = getCCppObj()
      self.failUnlessEqual(type(c), C)
      
   def test_factory(self):
      self.failUnlessEqual(type(factory(0)), A)
      self.failUnlessEqual(type(factory(1)), A)
      self.failUnlessEqual(type(factory(2)), C)

   def testReturnPy(self):

      class D(A):
         def f(self):
            return 'D.f'

      d = D()
      
      self.failUnlessEqual ('D.f', d.f())
      self.failUnlessEqual ('D.f', call_f(d))

if __name__ == "__main__":
   
   # remove the option which upsets unittest
   import sys
   sys.argv = [ x for x in sys.argv if x != '--broken-auto-ptr' ]
   
   unittest.main()
