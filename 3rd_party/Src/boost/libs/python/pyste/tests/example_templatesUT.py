import unittest
from templates import *

class TemplatesTest(unittest.TestCase):

    def testIt(self):  
        fp = FPoint()
        fp.i = 3.0
        fp.j = 4
        ip = IPoint()
        ip.x = 10
        ip.y = 3.0

        self.assertEqual(fp.i, 3.0)
        self.assertEqual(fp.j, 4)
        self.assertEqual(ip.x, 10)
        self.assertEqual(ip.y, 3.0)
        self.assertEqual(type(fp.i), float)
        self.assertEqual(type(fp.j), int)
        self.assertEqual(type(ip.x), int)
        self.assertEqual(type(ip.y), float)
                
                

if __name__ == '__main__':
    unittest.main() 
