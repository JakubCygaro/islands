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

The build is done via CMake

``` cmake -S <islands-repo-dir> -B <build-directory> -DCMAKE_BUILD_TYPE=<Debug or Release>```

## Features

- Multiple light sources simulation with shadows
- Orbit trails
- Custom textures for planets
- Trajectory prediction for selected celestial body

# Custom textures

Read the included `game_data/textures/custom/README.txt` file.

<img width="1918" height="1036" alt="Screenshot_20251013_024944" src="https://github.com/user-attachments/assets/34962962-2943-4a87-8621-9e3562d9a9e4" />

<img width="1918" height="1036" alt="Screenshot_20251013_025009" src="https://github.com/user-attachments/assets/8198c13c-826f-4ab9-aa84-18ae30a37620" />

<img width="1918" height="1036" alt="Screenshot_20251013_025056" src="https://github.com/user-attachments/assets/09658a6a-f8a1-4527-b498-d306f50169bf" />





