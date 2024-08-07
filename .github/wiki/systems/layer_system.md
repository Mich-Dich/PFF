# Layer System
 The layer system in the PFF Game Engine is designed to provide a structured and flexible approach to managing various elements of a game, such as renderable geometry and user interfaces. The system ensures an organized rendering and event propagation process:
 
 PFF uses a layer stack to manage different layers and overlays. The layers are rendered in a specific order, with bottommost layer being rendered first to the topmost overlays. Events propagate in the reverse order, from the topmost overlay to the bottommost layer

 ## Key Components
  1. **Layer Stack**: Manages the collection of layers and overlays.
  2. **Layer**: Represents a single layer in the engine.
  3. **Application**: Manages the layer stack.

 ## Layer Stack
  The `layer_stack` class is responsible for managing the layers and overlays. It provides methods to push and pop layers and overlays, ensuring that they are rendered and handle events in the correct order.

  ### Methods

   ```cpp
void push_layer(layer* layer);      // Adds a layer to the stack
void pop_layer(layer* layer);       // Removes a layer from the stack
void push_overlay(layer* overlay);  // Adds an overlay to the stack
void pop_overlay(layer* overlay);   // Removes an overlay from the stack
void delete_all_layers();           // Removes all layers and overlays from the stack
   ```

 ## Layer
  The `layer` class represents an individual layer in the engine. It provides virtual methods for handling various events and rendering tasks.

   ```cpp
virtual void on_attach();                             // Called when the layer is attached to the stack.
virtual void on_detach();                             // Called when the layer is detached from the stack.
virtual void on_update(const f32 delta_time = 0.f);   // Called during the update phase.
virtual void on_event(event& event);                  // Called when an event is propagated to the layer.
virtual void on_imgui_render();                       // Called for rendering ImGui elements.
   ```

 ## Application
  The `application` class manages the overall application, including the layer stack. It provides methods to push and pop overlays and handles the main event loop.

   ```cpp
void push_layer(layer* overlay)     // pushes a layer on the layer_stack (at the end of other layers but bevor overlays)
void pop_layer(layer* overlay)      // pops a layer from layer_stack
void push_overlay(layer* overlay)   // pushes an overlay on the layer_stack end
void pop_overlay(layer* overlay)    // pops an overlay from layer_stack
   ```

  ### Example Usage
   In the `application` constructor, layers and overlays are initialized and added to the layer stack:

   ```cpp
application::application() {
    m_layerstack = create_ref<layer_stack>();

    m_world_layer = new world_layer();
    m_layerstack->push_layer(m_world_layer);

    m_imgui_layer = new UI::imgui_layer();
    m_layerstack->push_overlay(m_imgui_layer);
}
   ```

   During the application's main loop, each layer's update and render methods are called:

   ```cpp
void application::run() {
    while (m_running) {
        m_window->poll_events();

        for (layer* layer : *m_layerstack) {
            layer->on_update(m_delta_time);
        }

        m_renderer->draw_frame(m_delta_time);
    }
}
   ```
