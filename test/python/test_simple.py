"""Test the "simple" library
"""
import numpy as np
from lib import simple


def test_display():
    simple.display("Hello world!")


def test_return_same():
    assert simple.return_same(0) == 0
    assert simple.return_same(1) == 1


def test_sum_int():
    assert simple.sum_int(-1, 2) == 1


def test_sum_float():
    assert np.isclose(simple.sum_float(-1., 2.), 1.)
    assert np.isclose(simple.sum_float(-1, 2.), 1.)


def test_sum_double():
    assert np.isclose(simple.sum_double(-1., 2.), 1.)
    assert np.isclose(simple.sum_double(-1, 2.), 1.)
