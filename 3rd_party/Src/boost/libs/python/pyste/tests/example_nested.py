import unittest
from nested import *

class NestedTest(unittest.TestCase):

    def testIt(self):              
        self.assertEqual(Root.staticXValue, 10)
        self.assertEqual(Root.Y.staticYValue, 20)
        z = Root.Y.Z()
        z.valueZ = 3
        self.assertEqual(z.valueZ, 3)
        

if __name__ == '__main__':
    unittest.main()
