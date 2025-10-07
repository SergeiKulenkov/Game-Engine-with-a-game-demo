# Game Engine prototype with a game demo
## Features:
* Engine prototype with features described below
* Entity Component System with basic components—Transform, Rigidbody, Tag, Sprite, Box and Circle Colliders
* Controllable Timer—that can be started and asked for ellapsed time at any point—and ScopedTimer classes
* Update loop with deltaTime
* Input Manager class
* Image class for rendering image files through ImGUI and vulkan
* optional rendering of debug visuals for entities
* Physics class for collision and raycast processing
* dynamic—movable, resizable, can be hidden by pressing Tab—Debug window with an ability to register new displayble fields
* And Game demo with a movable Player object—entity with a few components
* game window's title and size are set in the main() function located in main.cpp

### Collisions demo
![CollisionsDemo](ReadmeVisuals/RigidbodyCollisions.gif)
Demo with controllable player sprite, 4 walls and 100 dynamic obstacles. Also showcases the AABB optimization for collision detection—without it the frame time is 2.5 times higher.

## Build system
Premake script files for the Engine static library and the Game main project.

Run Scripts/Setup.bat to create project files and VS solution.

### Requirements - installed Vulkan SDK

### Third party libraries - ImGUI, glm, glfw, Vulkan
