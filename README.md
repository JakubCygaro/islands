> Beneath the wind-turned wave
> 
> Infinite peace
>
> Islands join hands
>
> 'Neathe heaven's sea

# Islands

This project is a simple 3D gravity simulation made with OpenGL. 
The purpose of this project is to test out what I've learned from the world famous [tutorial](https://learnopengl.com/).

The controls are explained on the welcome panel that appears on game start-up.

The game works in two modes: edit and normal. While in normal mode you can only spectate the simulation, in edit mode you can tinker with the celestial bodies and configure the game. 

## Building

The build is done via CMake.

Configure the build directory with:
``` cmake -S <islands-repo-dir> -B <build-directory> -DCMAKE_BUILD_TYPE=<Debug or Release>```

And then build the project with:
```cmake --build <path-to-build-directory>```

## Requirements

__Beside a C++ compiler (I use only gcc) you need to have python installed.__

### Libraries used (that are not included with the project)

- glfw3
- freetype2
- glm

### Linux

Install the required libraries in case they are not already installed and build the project.

### Windows

Use MinGW, but do not install cmake through MinGW. Add the `-G 'MinGW Makefiles'` option while configuring the build with cmake. Install all required libraries with MinGW. Freetype could possibly cause some problems (at least it always does for me) in that case I tend to build and install it manually and then set the required CMake variables myself to point to the `include` directory and `libfreetype.a` file.

**There is a windows release available for you to download in case you are not interested in a manual build**

## Features

- Multiple light sources simulation with shadows
- Orbit trails
- Custom textures for planets
- Trajectory prediction for selected celestial body

## Custom textures

Read the included `game_data/textures/custom/README.txt` file.

<img width="1918" height="1036" alt="Screenshot_20251013_024944" src="https://github.com/user-attachments/assets/34962962-2943-4a87-8621-9e3562d9a9e4" />

<img width="1918" height="1036" alt="Screenshot_20251013_025009" src="https://github.com/user-attachments/assets/8198c13c-826f-4ab9-aa84-18ae30a37620" />

<img width="1918" height="1036" alt="Screenshot_20251013_025056" src="https://github.com/user-attachments/assets/09658a6a-f8a1-4527-b498-d306f50169bf" />





