import sys
sys.path.append('..')  
import unittest
from policies import *

class PoliciesTest(unittest.TestCase):
    
    def testReturnInternal(self):
        'tests the code from a simple internal_reference'
        
        x = return_internal_reference(1)
        self.assertEqual(x.Code(), 'return_internal_reference< 1 >')
        x = return_internal_reference(3)
        self.assertEqual(x.Code(), 'return_internal_reference< 3 >') 


    def testCustodian(self):
        'tests the code from a simple custodian_and_ward'
        
        x = with_custodian_and_ward(1,2)
        self.assertEqual(x.Code(), 'with_custodian_and_ward< 1, 2 >')
        x = with_custodian_and_ward(3,4)
        self.assertEqual(x.Code(), 'with_custodian_and_ward< 3, 4 >') 


    def testReturnPolicies(self):        
        'tests all the return_value_policies'
        
        ret = 'return_value_policy< %s >'
        x = return_value_policy(reference_existing_object)
        self.assertEqual(x.Code(), ret % 'reference_existing_object')
        x = return_value_policy(copy_const_reference)
        self.assertEqual(x.Code(), ret % 'copy_const_reference') 
        x = return_value_policy(copy_non_const_reference)
        self.assertEqual(x.Code(), ret % 'copy_non_const_reference') 
        x = return_value_policy(manage_new_object)
        self.assertEqual(x.Code(), ret % 'manage_new_object') 


    def testReturnWithCustodiam(self):
        'test the mix of return_internal with custodian'

        x = return_internal_reference(1, with_custodian_and_ward(3,2))
        self.assertEqual(
            x.Code(), 
            'return_internal_reference< 1, with_custodian_and_ward< 3, 2 > >')


    def testReturnPoliciesWithInternal(self):
        'test the mix of return_internal with return_policy'

        x = return_internal_reference(1, return_value_policy(manage_new_object))
        self.assertEqual(
            x.Code(), 
            'return_internal_reference< 1, return_value_policy< manage_new_object > >')

        
if __name__ == '__main__':
    unittest.main()
