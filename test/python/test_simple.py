from lib import simple
import numpy as np


def test_display():
    simple.display("Hello world!")


def test_return_same():
    assert simple.return_same(0) == 0
    assert simple.return_same(1) == 1


def test_sum():
    assert simple.sum_int(-1, 2) == 1
    assert np.isclose(simple.sum_float(-1., 2.), 1.)
    assert np.isclose(simple.sum_float(-1, 2.), 1.)
    assert np.isclose(simple.sum_double(-1., 2.), 1.)
    assert np.isclose(simple.sum_double(-1, 2.), 1.)
