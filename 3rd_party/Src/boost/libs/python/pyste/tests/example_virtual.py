import unittest
from virtual import *

class VirtualTest(unittest.TestCase):

    def testIt(self):              
        
        class D(C):
            def f_abs(self):
                return 3
            
        class E(C):
            def f(self):
                return 10
            def name(self):
                return 'E'

        d = D()
        e = E()

        self.assertEqual(d.f(), 3)
        self.assertEqual(call_f(d), 3)
        self.assertEqual(e.f(), 10)
        self.assertEqual(call_f(e), 10)
        self.assertEqual(d.get_name(), 'C')
        self.assertEqual(e.get_name(), 'E')


        
if __name__ == '__main__':
    unittest.main()  
