INF = (0,0)  # Point at infinity.

def slope(P, Q, mod, a):
    if (P == Q):
        num = 3 * P[0]**2 + a
        den = 2 * P[1]
    else:
        num = Q[1] - P[1]
        den = Q[0] - P[0]
    
    denInv = pow(den, -1, mod)
    return (num * denInv) % mod

def ec_sum(P, Q, mod, a):
    if (P == INF):
        return Q
    elif (Q == INF):
        return P
    elif ( (P[0] == Q[0]) and (P[1] + Q[1] == mod) ):
        return INF  # Adding inverses.
    else:
        m = slope(P, Q, mod, a)
        xr = (m**2 - P[0] - Q[0]) % mod
        yr = (m*(P[0] - xr) - P[1]) % mod
        return (xr, yr)