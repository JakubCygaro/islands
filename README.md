> Beneath the wind-turned wave
> 
> Infinite peace
>
> Islands join hands
>
> 'Neathe heaven's sea

<img width="1200" height="927" alt="image" src="https://github.com/user-attachments/assets/c339d626-6f55-40c0-852a-4c978d4b2c9e" />

<img width="1201" height="927" alt="image" src="https://github.com/user-attachments/assets/e59facfe-fb58-4324-94ca-294c9d163f4d" />

<img width="1202" height="928" alt="image" src="https://github.com/user-attachments/assets/df8f2479-2f9b-40e4-bc64-08c5d47fb03c" />

# Islands

This project is a simple 3D gravity simulation made in openGL. 
The purpose of this project is to test out what I've learned from the world famous [tutorial](https://learnopengl.com/).

The controls are explained on the welcome panel that appears on game start-up.

## Building

The build is done via CMake, with a little caveat (which doesnt seem to happen on linux).

The FreeType2 library has to be build and installed separately and the path to its install directory must be supplied to CMake via the FREETYPE_ROOT_PATHS variable. 
You can either do it with the `-D` flag at configure step:

``` cmake -S <islands-src-directory> -B <islands-build-directory> -DFREETYPE_ROOT_PATHS=<path-to-freetype2-install-dir>```

or by editing the variable directly in the CMakeCache.txt file.

## Things that I plan on doing

- Orbit trails behind celestial bodies
- Possibly custom textures for planets
- Stuff


