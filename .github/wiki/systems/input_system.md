# Input System
 The input system in the PFF engine is designed to handle various user inputs through a structured and flexible approach. This system allows for the creation and management of input actions, which can be mapped to different keys or mouse events. The following sections will provide an in-depth look at the key components and their functionalities.
 ## Input Action Definitions
  In the `src/engine/io_handler/input_action.h` file, various constants and structures are defined to handle input actions.
  ### Trigger and Modifier Flags
   These flags define how and when input actions are triggered or modified. They are categorized into key triggers, mouse triggers, and modifiers.

   - **Key Trigger Flags**   sda 
     - `INPUT_ACTION_TRIGGER_KEY_DOWN`: Activates input when a key is pressed down (this can repeat).
     - `INPUT_ACTION_TRIGGER_KEY_UP`: Activates input when a key is released (this can repeat).
     - `INPUT_ACTION_TRIGGER_KEY_HOLD`: Activates input when a key is held down longer than a specified duration.
     - `INPUT_ACTION_TRIGGER_KEY_TAP`: Activates input when a key is pressed and released within a specified duration.
     - `INPUT_ACTION_TRIGGER_KEY_MOVE_DOWN`: Activates input when starting to press a key.
     - `INPUT_ACTION_TRIGGER_KEY_MOVE_UP`: Activates input when releasing a key.
   
   - **Mouse Trigger Flags**
     - `INPUT_ACTION_TRIGGER_MOUSE_POSITIVE`: Activates input for positive mouse actions.
     - `INPUT_ACTION_TRIGGER_MOUSE_NEGATIVE`: Activates input for negative mouse actions.
     - `INPUT_ACTION_TRIGGER_MOUSE_POS_AND_NEG`: Activates input for both positive and negative mouse actions.
   
   - **Modifier Flags**
     - `INPUT_ACTION_MODEFIER_NONE`: Never activate input.
     - `INPUT_ACTION_MODEFIER_NEGATE`: Negate the input action.
     - `INPUT_ACTION_MODEFIER_USE_VEC_NORMAL`: Use vector normalization.
     - `INPUT_ACTION_MODEFIER_AXIS_1_NEGATIVE`: Use the negative value of the first axis.
     - `INPUT_ACTION_MODEFIER_AXIS_2`: Use the second axis.
     - `INPUT_ACTION_MODEFIER_AXIS_2_NEGATIVE`: Use the negative value of the second axis.
     - `INPUT_ACTION_MODEFIER_AXIS_3`: Use the third axis.
     - `INPUT_ACTION_MODEFIER_AXIS_3_NEGATIVE`: Use the negative value of the third axis.
     - `INPUT_ACTION_MODEFIER_AUTO_RESET`: Automatically reset the input action.
     - `INPUT_ACTION_MODEFIER_AUTO_RESET_ALL`: Automatically reset all input actions.

 ## Input Action Struct
  The `input_action` struct is the core component of the input system. It defines an input action with various attributes such as description, trigger conditions, and data type.
  
  ```cpp
f32 measured_time{};
{
    PFF::stopwatch loc_stopwatch(&measured_time, PFF::duration_precision::microseconds);

    // Perform operations to time
}

// [measured_time] now contains the elapsed time in microseconds
  ```
  
  
  ```cpp
struct input_action {
    // Attributes
    std::string description;
    bool triger_when_paused;
    u16 flags;
    input::action_type value;
    input_action_data data;
    f32 duration_in_sec;
    std::vector<input::key_binding_details> keys_bindings;

    // Methods
    const size_t                    get_length() const;
    input::key_binding_details*     get_key(u32 index);
    void                            set_name(std::string_view name);
    std::string_view                get_name() const;

private:
    std::string                                         m_name;
    input_action_data                                   target;
    std::chrono::time_point<std::chrono::steady_clock>  time_stamp;
};
  ``` 

 ## Key Binding Details
  The `key_binding_details` struct defines the details of a key binding, including the key code, trigger flags, and modifier flags.

  ```cpp
struct key_binding_details {
    key_code key;
    u16 trigger_flags;
    u16 modefier_flags;
    int16 active;

    key_binding_details();
    key_binding_details(key_code key, u16 trigger_flags = 0, u16 modefier_flags = 0);
};
  ```

 ## Input Mapping
  The `input_mapping` class manages the registration and retrieval of input actions. It ensures that default values are retained unless explicitly changed by the player.

  ### Registering Input Actions
   Input actions are registered using the `REGISTER_INPUT_ACTION` macro, which sets the name of the action and calls the `register_action` method.

   ```cpp
#define REGISTER_INPUT_ACTION(action) action.set_name(#action); register_action(&action, FORCE_OVERRIDE);
   ```

   The `register_action` method loads or saves the input action settings from/to the `input.ini` file.


# Example: Editor Inputs
 An example of how the input system is used can be found in the `PFF_editor/src/controller/editor_inputs.h` file. Here, various input actions are defined for the editor, such as moving the camera and toggling the FPS limiter.

 ```cpp
class editor_inputs: public PFF::input_mapping {
public:
    editor_inputs();
    ~editor_inputs();

   PFF::input_action move;
   PFF::input_action capture_mouse;
   PFF::input_action change_move_speed;
   PFF::input_action look;
   PFF::input_action toggle_fps;
};

editor_inputs::editor_inputs() {

   move = PFF::input_action{};
   move.description = "reposition the editor camera";
   move.triger_when_paused = false;
   move.flags = INPUT_ACTION_MODEFIER_SMOOTH_INTERP | INPUT_ACTION_MODEFIER_USE_VEC_NORMAL | INPUT_ACTION_MODEFIER_AUTO_RESET;
   move.value = PFF::input::action_type::vec_3D;
   move.duration_in_sec = 0.5f;
   move.keys_bindings = {
      {key_code::key_W, INPUT_ACTION_TRIGGER_KEY_DOWN, INPUT_ACTION_MODEFIER_NEGATE},
      {key_code::key_S, INPUT_ACTION_TRIGGER_KEY_DOWN},
      {key_code::key_A, INPUT_ACTION_TRIGGER_KEY_DOWN, INPUT_ACTION_MODEFIER_AXIS_2 | INPUT_ACTION_MODEFIER_NEGATE},
      {key_code::key_D, INPUT_ACTION_TRIGGER_KEY_DOWN, INPUT_ACTION_MODEFIER_AXIS_2},
      {key_code::key_space, INPUT_ACTION_TRIGGER_KEY_DOWN, INPUT_ACTION_MODEFIER_AXIS_3 | INPUT_ACTION_MODEFIER_NEGATE},
      {key_code::key_left_shift, INPUT_ACTION_TRIGGER_KEY_DOWN, INPUT_ACTION_MODEFIER_AXIS_3 },
   };								
   REGISTER_INPUT_ACTION(move);

   capture_mouse = PFF::input_action{};
   capture_mouse.description = "left clicl to change the camera orientation";
   capture_mouse.triger_when_paused = false;
   capture_mouse.flags = 0;
   capture_mouse.value = PFF::input::action_type::boolean;
   capture_mouse.duration_in_sec = 0.5f;
   capture_mouse.keys_bindings = {
      {key_code::mouse_bu_right, INPUT_ACTION_TRIGGER_KEY_DOWN},
   };
   REGISTER_INPUT_ACTION(capture_mouse);

   look = PFF::input_action{};
   look.description = "change the direction of the editor camera";
   look.triger_when_paused = false;
   look.flags = INPUT_ACTION_MODEFIER_AUTO_RESET_ALL | INPUT_ACTION_MODEFIER_SMOOTH_INTERP;
   look.value = PFF::input::action_type::vec_2D;
   look.duration_in_sec = 0.5f;
   look.keys_bindings = {
      {key_code::mouse_moved_x, INPUT_ACTION_TRIGGER_MOUSE_POS_AND_NEG, INPUT_ACTION_MODEFIER_NEGATE},
      {key_code::mouse_moved_y, INPUT_ACTION_TRIGGER_MOUSE_POS_AND_NEG, INPUT_ACTION_MODEFIER_AXIS_2 | INPUT_ACTION_MODEFIER_NEGATE},
   };
   REGISTER_INPUT_ACTION(look);

   // Additional input actions are defined similarly...
}
 ```
 