from lib import containers
import numpy as np


def test_return_same_vector():
    a = [1, 2, 3, 4]
    assert np.allclose(a, containers.return_same_vector_int(a))
    a = list(map(float, a))
    assert np.allclose(a, containers.return_same_vector_float(a))
