# PFF - Procedurally Focused Framework

<table style="border-collapse: collapse; width: 100%; border: none;">
  <tr>
    <td style="border: none;">
      <img src="/.github/resources/Logo2.png?raw=true" alt="Main Logo" title="mich" style="max-width: 100%; height: auto; border: none;">
    </td>
    <td style="vertical-align: middle; padding-right: 20px; border: none;">
      <strong>PFF (Procedurally Focused Framework)</strong> is a 3D game engine crafted in C/C++, designed with a primary focus on harnessing the potential of procedural generation. Primarily developed for Windows, PFF is poised to extend its capabilities to Linux in the future, aspiring to offer a versatile and dynamic platform for game development.
    </td>
  </tr>
</table>

### Here are screenshots that show the current state of PFF.

![first_image](/.github/resources/PFF_editor_CfQonJ3asw.png?raw=true "image of the editor as it is now")

## Getting Started
To get started with PFF, follow these simple steps:

1. Clone the repository using:
   ```
   git clone --recursive https://github.com/Mich-Dich/PFF
   ```

Windows:

2. Execute the following file to create a VS 2022 solution:
   ```
   AutoGenerateSolution.bat
   ```
Linux/Apple: (currently not supported)

## Dependencies
PFF relies on the following dependencies:

- [premake5](https://premake.github.io/)
- [glfw](https://www.glfw.org/)
- [ImGui](https://github.com/ocornut/imgui)
- [glm](https://github.com/icaven/glm)
- [Vulkan](https://www.lunarg.com/vulkan-sdk/) install Vulkan SDK and add it to your PATH or update the variable [VULKAN_SDK] in [dependencies.lua]

## Usage

You can start using PFF by either utilizing the provided project, [Sandbox], or creating your own project and linking it by adjusting the `premake5.lua` file. 
Your game (main class) has to inherit from the `application` class.

## Documentation

To help you get familiar with PFF, a [wiki](https://github.com/Mich-Dich/PFF/wiki) is available and currently under development. The wiki will provide detailed documentation, guides, and examples to assist you in navigating and utilizing the framework effectively. Check back regularly for updates as more content is added.

You can also find this wiki in the editor under: Windows->Engine Wiki


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

### General Systems
- Linux/Apple support
- Launcher
- Asset manager
- Texture Editor
- Material Editor

### Procedural Systems
- Mesh generation
   - Composite-mesh
   - Tree
   - Terrain
   - Voxel
- Sound
- Animation
- Texture
- Materials

Feel free to contribute and help shape the future of PFF. Thank you for your interest and support!

## Acknowledgments
This project ia a significant undertaking, and I owe a debt of gratitude to the following individuals and projects:

- [TheCherno](https://github.com/TheCherno)
- [Falconerd/engine-from-scratch](https://github.com/Falconerd/engine-from-scratch)

## License

PFF is open-source under the [Apache License 2.0](LICENSE). Feel free to explore, contribute, and build upon this framework.

Happy coding with PFF! 🚀
