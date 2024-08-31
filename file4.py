import unittest

def inc(x):
	return x + 10

class TestRearrange(unittest.TestCase):
	def test_basic(self):
		testcase = 11
		expected = 20
		self.assertEqual(inc(testcase), expected)

# Run the tests
unittest.main()
