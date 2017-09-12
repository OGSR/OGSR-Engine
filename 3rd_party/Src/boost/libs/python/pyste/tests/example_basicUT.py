import unittest
import os

class BasicExampleTest(unittest.TestCase):

    def testIt(self):
        from basic import C, call_f

        class D(C):
            def f(self, x=10):
                return x+1

        d = D()
        c = C()

        self.assertEqual(c.f(), 20)
        self.assertEqual(c.f(3), 6)
        self.assertEqual(d.f(), 11)
        self.assertEqual(d.f(3), 4)
        self.assertEqual(call_f(c), 20)
        self.assertEqual(call_f(c, 4), 8)
        self.assertEqual(call_f(d), 11)
        self.assertEqual(call_f(d, 3), 4)


if __name__ == '__main__':
    unittest.main()
