import unittest
from _abstract_test import *

class AbstractTest(unittest.TestCase):

    def testIt(self):
        class C(A):
            def f(self):
                return 'C::f'

        a = A()
        b = B()
        c = C()
        self.assertRaises(RuntimeError, a.f)
        self.assertEqual(b.f(), 'B::f')
        self.assertEqual(call(b), 'B::f')
        self.assertEqual(c.f(), 'C::f')
        self.assertEqual(call(c), 'C::f') 


if __name__ == '__main__':
    unittest.main()
