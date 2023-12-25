# PFF - Procedurally Focused Framework

Introducing PFF (Procedurally Focused Framework), a powerful 2D game engine crafted in C, designed with a primary focus on harnessing the potential of procedural generation. Primarily developed for Windows, PFF is poised to extend its capabilities to Linux in the future, aspiring to offer a versatile and dynamic platform for game development.

## Procedural Generation Unleashed

At the heart of PFF lies its commitment to procedural generation, an innovative approach that opens up a realm of possibilities for game developers. Procedural generation empowers creators to generate content algorithmically, dynamically shaping game worlds and assets. This technique not only enhances replayability but also optimizes resource utilization, offering an efficient and scalable solution for game environments.

### Possibilities with Procedural Generation:

1. **Infinite Worlds:** Generate vast, ever-changing landscapes without the need for extensive manual design, providing players with a unique experience in each playthrough.

2. **Dynamic Level Design:** Create levels on-the-fly, adapting to player actions and choices, ensuring a tailored and engaging gameplay experience.

3. **Randomized Assets:** Generate diverse and unique assets, such as textures, models, and environments, promoting diversity and creativity in game design.

4. **Efficient Resource Management:** Procedural generation minimizes the need for static, pre-designed assets, resulting in optimized resource utilization and smaller game file sizes.

5. **Adaptive Challenges:** Generate challenges and obstacles based on player skill level or in-game progression, ensuring a balanced and responsive difficulty curve.

**Examples:**
- **Terrain Generation:** Procedurally generate realistic terrains with varying elevations, landscapes, and biomes.
- **Dungeon Layouts:** Dynamically create intricate dungeon layouts, ensuring no two dungeons are the same.
- **Weather Systems:** Generate diverse weather conditions, affecting gameplay and immersion.

By leveraging procedural generation, PFF empowers game developers to break free from conventional constraints, fostering creativity, and delivering unique and immersive gaming experiences. Explore the endless possibilities with PFF! 🌐✨
## Features

### Input System
PFF boasts a flexible input system capable of handling various input types, including boolean, 1-axis, and 2-axis. The system supports multiple trigger events such as key down, key up, key tap, and key hold, as well as,  several modifiers including negate, smooth, and normalize vec2.

## Planned Features

### Editor
A comprehensive editor is in the pipeline to enhance the development experience within PFF.

### Physics Engine
PFF plans to incorporate a robust physics engine to facilitate realistic in-game interactions.

### Enemy AI System
An advanced Enemy AI System is on the horizon to bring life to your game environments.

## Inspiration

The initial inspiration for PFF came from the [engine-from-scratch](https://github.com/Falconerd/engine-from-scratch) GitHub repository. We express our gratitude to the contributors of that project.

## Project Structure

- **PFF (Engine)**
  - Contains the entire source code for the engine.
  - Houses 3rd party code used in the engine.

- **Sandbox**
  - Starter project bundled with the engine for quick setup.

- **/bin and /bin-int**
  - Folders for compiled results.

- **/vendor**
  - Contains 3rd party code shared across all projects.

- **AutoGenerateSolution.bat**
  - Build script for generating the Visual Studio 2022 solution.

## Build System

PFF utilizes the premake5 build system to generate the Visual Studio 2022 solution. To generate the solution, simply execute the `AutoGenerateSolution.bat` script.

## Contributing
Contributions to PFF are welcome! If you have ideas for improvements, new features, or find any issues, please open an issue or submit a pull request.

## Acknowledgments

Special thanks to the [engine-from-scratch](https://github.com/Falconerd/engine-from-scratch) repository for providing initial inspiration.

## License

PFF is published under the [Apache License 2.0](LICENSE). Feel free to explore, contribute, and build upon this framework.

Happy coding with PFF! 🚀
