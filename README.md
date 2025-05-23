# PFF - Procedurally Focused Framework

<table style="border: none; width: 100%;">
  <tr>
    <td>
      <img src="/.github/resources/Logo2.png?raw=true" alt="Main Logo" title="mich" style="max-width: 100%; height: auto; border: none;">
    </td>
    <td style="vertical-align: middle; padding-right: 20px;">
      <strong>PFF (Procedurally Focused Framework)</strong> is a 3D game engine crafted in C/C++, designed with a primary focus on harnessing the potential of procedural generation. Primarily developed for Windows, PFF is poised to extend its capabilities to Linux in the future, aspiring to offer a versatile and dynamic platform for game development.
    </td>
  </tr>
</table>

### Here are screenshots that show the current state of PFF.

![first_image](/.github/resources/Screenshot_20250413_191317.png?raw=true "image of the editor as it is now")

|![first_image](/.github/resources/Screenshot_20250413_191704.png?raw=true "image of the editor as it is now")|![first_image](/.github/resources/Screenshot_20250413_145916.png?raw=true "image of the editor as it is now")|
|-|-|
|![first_image](/.github/resources/Screenshot_20250413_191412.png?raw=true "image of the editor as it is now")|![first_image](/.github/resources/Screenshot_20250413_180229.png?raw=true "image of the editor as it is now")|

## Getting Started
To get started with PFF, follow these simple steps
(You need to have the following software preinstalled: ```Git, Python```):

1. Clone the repository using:
   ```
   git clone --recursive git@github.com:Mich-Dich/PFF.git
   ```

2. On Windows/Linux execute the setup script (Apple currently not supported)
   ```
   python setup.py
   ```
   This will:
      - Update repo & submoduls.
      - Check Envirement setup.
      - Setup workspace
        - Windows: create a VS 2022 solution.
        - Linux: (still in dev)

3. Compile the code:
   ```
   make -j
   ```

4. Launch the editor (dedicated `launcher` not created yet):
   Replace "Debug-linux-x86_64" with the version you want to launch
   ```
   bin/Debug-linux-x86_64/PFF_editor/PFF_editor
   ```

Summary:
   ```
   git clone --recursive git@github.com:Mich-Dich/PFF.git
   cd PFF
   python setup.py
   make -j
   bin/Debug-linux-x86_64/PFF_editor/PFF_editor
   ```

For more detailed instructions please visit our [Instalation wiki](https://github.com/Mich-Dich/PFF/wiki/Instalation)

### Tiling Window Manager
   If you're using a tiling window manager, it's recommended to add a window rule.
   Specifically, under your rules for floating windows, include 'PFF - ' to ensure new windows float by default — especially important for popups and dialog boxes.

## Usage

The engine always need a project to work with. As the `launcher` is still under development, it is recomended to start with our [PFF test project](https://github.com/Mich-Dich/PFF_test_project).

You can direcktly start the Project, this will open the editor and then load the project.

Or you can start the engine, but this will ask you everytime witch project to open. Please select the [.pffproj] file of the project you want to use

## Dependencies
PFF relies on the following dependencies:

- [premake5](https://premake.github.io/)
- [glfw](https://www.glfw.org/)
- [ImGui](https://github.com/ocornut/imgui)
- [ImGuizmo](https://github.com/CedricGuillemet/ImGuizmo)
- [glm](https://github.com/icaven/glm)
- [Vulkan](https://www.lunarg.com/vulkan-sdk/) install Vulkan SDK and add it to your PATH

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
