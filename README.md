> Beneath the wind-turned wave
> 
> Infinite peace
>
> Islands join hands
>
> 'Neathe heaven's sea

<img width="1199" height="901" alt="image" src="https://github.com/user-attachments/assets/263b725b-530f-4707-af10-30de893653f9" />

<img width="1199" height="907" alt="image" src="https://github.com/user-attachments/assets/b793a87d-352f-4bda-b41e-b78cf4d3aaee" />

# Islands

This project is a simple 3D gravity simulation made in openGL. 
The purpose of this project is to test out what I've learned from the world famous [tutorial](https://learnopengl.com/).

The controls are explained on the welcome panel that appears on game start-up.

## Building

The build is done via CMake, with a little caveat.

The FreeType2 library has to be build and installed separately and the path to its install directory must be supplied to CMake via the FREETYPE_ROOT_PATHS variable. 
You can either do it with the `-D` flag at configure step:

``` cmake -S <islands-src-directory> -B <islands-build-directory> -DFREETYPE_ROOT_PATHS=<path-to-freetype2-install-dir>```

or by editing the variable directly in the CMakeCache.txt file.

## Things that I plan on doing

- Light simulation with planets and stars
- Orbit trails behind celestial bodies
- Possibly custom textures for planets
- Stuff


