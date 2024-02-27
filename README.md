# Battle Tanks

A small, single-player World of Tanks inspired game implemented in OpenGL and Visual Studio.

The player has a tank, and it can:
- move using WASD, rotate the third-person camera using the mouse;
- shoot missiles and deal damage to other tanks by left-clicking. The missiles will automatically disappear after a short while if the user misses.
- take damage (each shoot takes 30 HP, and each tank has by default 100HP. 4 shoots and you're dead!). When a tank takes damage, it starts to shrink and become redder (I achieved this using shaders). 
The game has a collision detection algorithm implemented so that tanks cannot move through buildings or other tanks.

The tank model was taken from [here](https://www.turbosquid.com/3d-models/abrams-tank-3d-model-1251659).
