# Utility Functions and Classes

 ## Template Functions

  ### `contains`
   ```cpp
template<typename key_type, typename value_type>
bool contains(const std::unordered_map<key_type, value_type>& map, const key_type& key);
   ```
   - **Description:** Checks if a given key exists in an unordered map.
   - **Parameters:**
     - `map`: The unordered map to search within.
     - `key`: The key to search for.
   - **Returns:** `true` if the key is found; otherwise, `false`.

 ## Smart Pointers

  `ref<T>`: Alias for `std::shared_ptr<T>`, representing a smart pointer to a resource of type `T`.
  `create_ref<T, args...>`: Creates a `ref<T>` smart pointer using `std::make_shared`.
    - **Parameters:** `args...`: Arguments to pass to the constructor of `T`.
    - **Returns:** A `std::shared_ptr<T>`.
  
  `scope_ref<T>`: Alias for `std::unique_ptr<T>`, representing a smart pointer for scope-bound resource management.
  `create_scoped_ref<T, args...>`: Creates a `scope_ref<T>` using `std::make_unique`.
    - **Parameters:** `args...`: Arguments to pass to the constructor of `T`.
    - **Returns:** A `std::unique_ptr<T>`.
  
 ## Random Number Generation
  ```cpp
util::random loc_random{};
loc_random.get_f32(0.f, 2 * glm::pi<f32>());

  ```
    - **Description:** Provides methods for generating random numbers using the Mersenne Twister engine.
    - **Methods:**
      `get_f32(f32 min = 0.0f, f32 max = 1.0f)`: Generates a random `float` in the range `[min, max]`.
      `get_f64(f64 min = 0.0, f64 max = 1.0)`: Generates a random `double` in the range `[min, max]`.
      `get_u32(u32 min = 0, u32 max = std::numeric_limits<u32>::max())`: Generates a random `u32` in the range `[min, max]`.
      `get_u64(u64 min = 0, u64 max = std::numeric_limits<u64>::max())`: Generates a random `u64` in the range `[min, max]`.
  
 ## String Manipulation

 `extract_part_after_delimiter(std::string& dest, const std::string& input, const char* delimiter)`
  - **Description:** Extracts the substring after the last occurrence of the specified delimiter.
  - **Parameters:**
    - `dest`: The string to store the extracted part.
    - `input`: The input string.
    - `delimiter`: The delimiter character.
  - **Returns:** None.

 `extract_part_before_delimiter(std::string& dest, const std::string& input, const char* delimiter)`
  - **Description:** Extracts the substring before the last occurrence of the specified delimiter.
  - **Parameters:**
    - `dest`: The string to store the extracted part.
    - `input`: The input string.
    - `delimiter`: The delimiter character.
  - **Returns:** None.

 `extract_variable_name(const std::string& input)`
  - **Description:** Extracts the variable name from a variable access chain.
  - **Parameters:**
    - `input`: The input string representing the variable access chain.
  - **Returns:** The name of the variable.

 `high_precision_sleep(f32 duration_in_milliseconds)`
  - **Description:** Pauses execution with high precision for the specified duration.
  - **Parameters:**
    - `duration_in_milliseconds`: Duration to pause in milliseconds.
  - **Returns:** None.

 `add_spaces(const u32 multiple_of_indenting_spaces, u32 num_of_indenting_spaces = 2)`
  - **Description:** Adds spaces for indentation purposes.
  - **Parameters:**
    - `multiple_of_indenting_spaces`: Number of spaces to add.
    - `num_of_indenting_spaces`: Number of spaces per indent level.
  - **Returns:** A string containing the added spaces.

 `measure_indentation(const std::string& str, u32 num_of_indenting_spaces = 2)`
  - **Description:** Measures the amount of indentation in a string.
  - **Parameters:**
    - `str`: The input string.
    - `num_of_indenting_spaces`: Number of spaces per indent level.
  - **Returns:** The number of spaces used for indentation.

 `count_lines(const char* text)`
  - **Description:** Counts the number of lines in a given text.
  - **Parameters:**
    - `text`: The input text.
  - **Returns:** The number of lines in the text.

 `remove_substring(const char(&source)[N], const char(&remove)[K])`
  - **Description:** Removes a substring from a character array.
  - **Parameters:**
    - `source`: The source character array.
    - `remove`: The substring to remove.
  - **Returns:** A character array with the specified substring removed.

 `extract_after_pff(const std::string& path)`
  - **Description:** Extracts the part of the string after the second occurrence of "PFF".
  - **Parameters:**
    - `path`: The input string.
  - **Returns:** The extracted substring.

 ### Timer

 - **Struct: `timer`
  - **Description:** Represents a timer that can be used to schedule callbacks.
  - **Members:**
   - `future`: A `std::future<void>` associated with the timer.
   - `shared_state`: A shared state used to manage timer cancellation and completion.

 `timer_async(std::chrono::duration<_rep, _period> duration, std::function<void()> callback, std::function<void()> cancel_callback = NULL)`
  - **Description:** Starts a timer asynchronously with a specified duration and callback function.
  - **Parameters:**
    - `duration`: The duration of the timer.
    - `callback`: The callback function to execute when the timer finishes.
    - `cancel_callback`: An optional callback function to execute if the timer is canceled.
  - **Returns:** A reference to the `std::future<void>` associated with the timer.

 `cancel_timer(timer& timer)`
  - **Description:** Cancels the specified timer.
  - **Parameters:**
    - `timer`: The timer to cancel.
  - **Returns:** None.

 `is_timer_ready(std::future<void>& timer, std::chrono::duration<_rep, _period> duration = 1ms)`
  - **Description:** Checks if the specified timer has finished.
  - **Parameters:**
    - `timer`: The timer to check.
    - `duration`: The duration to wait before checking the timer status.
  - **Returns:** `true` if the timer has finished; otherwise, `false`.

 ## Utility Template Functions

 `str_to_num(const std::string& str)`
  - **Description:** Converts a string to a numeric value of type `T`.
  - **Parameters:**
    - `str`: The input string.
  - **Returns:** The converted numeric value.

 `num_to_str(const T& num)`
  - **Description:** Converts a numeric value of type `T` to a string.
  - **Parameters:**
    - `num`: The numeric value.
  - **Returns:** The string representation of the numeric value.

 `hash_combine(std::size_t& seed, const T& v, const Rest&... rest)`
  - **Description:** Combines hash values of multiple values.
  - **Parameters:**
    - `seed`: The seed value for the hash.
    - `v`: The value to hash.
    - `rest`: Additional values to hash.
  - **Returns:** None.

 `convert_to_string(const T& src_value, std::string& dest_string)`
  - **Description:** Converts a value of type `T` to its string representation.
  - **Parameters:**
    - `src_value`: The value to convert.
    - `dest_string`: The string to store the converted value.
  - **Returns:** None.

 `to_string(const T& src_value)`
  - **Description:** Converts a value of type `T` to its string representation.
  - **Parameters:**
    - `src_value`: The value to convert.
  - **Returns:** The string representation of the value.

 `convert_from_string(const std::string& src_string, T& dest_value)`
  - **Description:** Converts a string to a value of type `T`.
  - **Parameters:**
    - `src_string`: The string to convert.
    - `dest_value`: The variable to store the converted value.
  - **Returns:** None.
 