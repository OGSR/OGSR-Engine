import sys
sys.path.append('../src')  
import unittest
import os.path
from glob import glob

if __name__ == '__main__':
    loader = unittest.defaultTestLoader
    tests = []
    for name in glob('*UT.py'):
        module = __import__(os.path.splitext(name)[0])
        tests.append(loader.loadTestsFromModule(module))
    runner = unittest.TextTestRunner()
    result = runner.run(unittest.TestSuite(tests))
    sys.exit(not result.wasSuccessful())    
