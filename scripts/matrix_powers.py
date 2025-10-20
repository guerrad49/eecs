import numpy as np
MODULO = 5

M = np.array([[0,1],[2,3]])
powM = np.identity(2, dtype=int)
for i in range(1,25):
    powM = (powM @ M) % MODULO
    preStr = "{:2d}: ".format(i)
    print(preStr, np.array2string(powM, prefix=preStr))
    print()