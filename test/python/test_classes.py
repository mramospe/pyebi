from lib import classes
import numpy as np


def test_some_class():
    a = classes.py_some_class(1, "string")
    assert np.allclose(a.integer(), 1)
    assert a.string() == "string"
