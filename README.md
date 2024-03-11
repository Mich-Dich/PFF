# PFF - Procedurally Focused Framework

![Main Logo](/resources/Logo2.png?raw=true "mich")

PFF (Procedurally Focused Framework) is a 3D game engine crafted in C/C++, designed with a primary focus on harnessing the potential of procedural generation. Primarily developed for Windows, PFF is poised to extend its capabilities to Linux in the future, aspiring to offer a versatile and dynamic platform for game development.

### Here are screenshots that show the current state of PFF.

This shows a mesh with 3.5 million vertices
![first_image](/resources/PFF_editor_vHol9hqUU4.png?raw=true "image of the editor as it is now")

Contains a ToDo-List by default
![second_image](/resources/PFF_editor_zXmikPE9jC.png?raw=true "image of the editor as it is now")

(UI is not finished yet, e.g. Logo missing)

## Getting Started
To get started with PFF, follow these simple steps:

1. Clone the repository using:
   ```
   git clone --recursive https://github.com/Mich-Dich/PFF
   ```

2. Execute the following file to create a VS 2022 solution:
   ```
   AutoGenerateSolution.bat
   ```


## Dependencies
PFF relies on the following dependencies:

- [premake5](https://premake.github.io/)
- [glfw](https://www.glfw.org/)
- [ImGui](https://github.com/ocornut/imgui)
- [glm](https://github.com/icaven/glm)

- [Vulkan](https://www.lunarg.com/vulkan-sdk/)
Make sure you install Vulkan SDK and update the path in [premake5.lua]

## Usage
You can start using PFF by either utilizing the provided project, [Sandbox], or creating your own project and linking it by adjusting the `premake5.lua` file. 
Your game (main class) has to inherit from the `application` class.

## Procedural Generation Unleashed

At the heart of PFF lies its commitment to procedural generation, an innovative approach that opens up a realm of possibilities for game developers. Procedural generation empowers creators to generate content algorithmically, dynamically shaping game worlds and assets. This technique not only enhances replayability but also optimizes resource utilization, offering an efficient and scalable solution for game environments.

### Possibilities with Procedural Generation:

1. **Infinite Worlds:** Generate vast, ever-changing landscapes without the need for extensive manual design, providing players with a unique experience in each playthrough.

2. **Dynamic Level Design:** Create levels on-the-fly, adapting to player actions and choices, ensuring a tailored and engaging gameplay experience.

3. **Randomized Assets:** Generate diverse and unique assets, such as textures, models, and environments, promoting diversity and creativity in game design.

4. **Efficient Resource Management:** Procedural generation minimizes the need for static, pre-designed assets.

5. **Adaptive Challenges:** Generate challenges and obstacles based on player skill level or in-game progression, ensuring a balanced and responsive difficulty curve.

### Procedurally Focused Systems:

#### 1. Procedural Animation:
   - Dynamically generate character animations, offering fluid and realistic movements that adapt to in-game scenarios.

#### 2. Procedural Sound Design:
   - Create immersive audio experiences through algorithmic sound generation, enhancing the atmosphere and realism of the game environment.

#### 3. Procedural Mesh Generation:
   - Generate diverse and complex 3D meshes on-the-fly, allowing for unique and intricate designs without the need for pre-made assets.

**Examples:**
- **Terrain Generation:** Procedurally generate realistic terrains with varying elevations, landscapes, and biomes.
- **Dungeon Layouts:** Dynamically create intricate dungeon layouts, ensuring no two dungeons are the same.
- **Weather Systems:** Generate diverse weather conditions, affecting gameplay and immersion.

By leveraging procedural generation, PFF empowers game developers to break free from conventional constraints, fostering creativity, and delivering unique and immersive gaming experiences. Explore the endless possibilities with PFF! 🌐✨

## Roadmap
PFF's development roadmap outlines key focus areas:

### General Systems
- Editor
- Launcher
- Asset manager

### Procedural Systems
- Animation
- Sound
- Mesh generation

Feel free to contribute and help shape the future of PFF. Thank you for your interest and support!

## Acknowledgments
This project ia a significant undertaking, and I owe a debt of gratitude to the following individuals and projects:

- [TheCherno](https://github.com/TheCherno)
- [Falconerd/engine-from-scratch](https://github.com/Falconerd/engine-from-scratch)

## License

PFF is open-source under the [Apache License 2.0](LICENSE). Feel free to explore, contribute, and build upon this framework.

Happy coding with PFF! 🚀
