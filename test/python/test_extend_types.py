from lib import extend_types
import numpy as np


def test_return_same_custom_array():
    a = np.random.normal(0, 1, 10).tolist()
    assert np.allclose(a, extend_types.return_same_custom_array_double(a))
