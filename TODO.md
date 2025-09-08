# TODO LIST

- add trails to objects
- more control over planet velocity
- velocity vector visualization
- grid
- planet predicted path
- skybox
- custom textures for planets



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
