import unittest
from wrappertest import *

class WrapperTest(unittest.TestCase):

    def testIt(self):              
        self.assertEqual(Range(10), range(10))
        self.assertEqual(C().Mul(10), [x*10 for x in range(10)])

if __name__ == '__main__':
    unittest.main()  
