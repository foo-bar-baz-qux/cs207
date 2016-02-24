from pytest import raises, fixture
from binsearch import binary_search
import numpy as np

@fixture
def input_data():
    return list([1, 5, 10, 20, 35])

def test_weird_data():
    with raises(TypeError):
        binary_search([np.nan(), None, 0.1, 0.5], 0.5)

def test_empty():
    assert binary_search([], 1) == -1

def test_oneelem():
    assert binary_search([9], 9) == 0

def test_twoelem_first():
    assert binary_search([10, 50], 10) == 0

def test_twoelem_second():
    assert binary_search([10, 50], 50) == 1

def test_negative():
    assert binary_search([-10.5, -5.2, 10, 20.3, 30.9], -5.2), 1

def test_multiple_res():
    res = binary_search([1, 10, 10, 10, 50], 10)
    assert (res >=1 and res <= 3)

def test_lt_range(input_data):
    assert binary_search(input_data, -10) == -1

def test_gt_range(input_data):
    assert binary_search(input_data, 10000) == -1

def test_not_found_inrange(input_data):
    assert binary_search(input_data, 11) == -1

def test_unsorted():
    assert binary_search([10, 5, 3, 5], 4) == -1

# def test_char():
#     with raises(TypeError):
#         myaverage(['a',3])
#
# def test_mymath():
#     assert mymedian([9,3, 6]) == 5
#
# def test_zero_median():
#     with raises(ValueError):
#         mymedian([])
#
# def test_char_median():
#     with raises(TypeError):
#         mymedian(['a', 3])
