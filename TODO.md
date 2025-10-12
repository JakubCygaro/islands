# TODO LIST

- custom textures for planets
- rotation
- fix resolution fuckery

## Multithreaded gravity calculation algorithm proposition

Input: slice of Celestial Bodies

Output: the same slice with gravitational forces calculated between each body


if len(V) == 1:
    return V

M := midpoint of V

CalcGrav(0, M)
CalcGrav(M, len(V))

for i := 0 to M:
    for j := M to len(V):
        g := grav(V[i], V[j])
return V
