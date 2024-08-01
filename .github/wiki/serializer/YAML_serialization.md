# YAML Serialization

## Introduction
Serialization is a crucial aspect of game development, allowing for the persistent storage and retrieval of game data. YAML serialization offers a human-readable and easily editable format for storing structured data. This document outlines the YAML serialization implementation within our game engine.

## Usage

### Overview
The YAML serializer provided in our game engine facilitates the storage of game data in YAML format. Users can utilize the provided functions to serialize and deserialize game objects efficiently.

### Usage Example
```cpp
struct example_data {

   std::string title{};
   int integer = 0;
   f32 float_num = 0.0f;
   bool done = false;
};

struct example_struct {

   std::string name{};
   bool used = false;
   std::vector<example_data> data_array{};
};

void test_serialization() {

   // Creating and initializing an example_struct variable
   example_struct my_example_struct = {};
   my_example_struct.name = "Name A";
   my_example_struct.used = true;
   my_example_struct.data_array.push_back({ "Title 1", 42, 3.1415f, false });
   my_example_struct.data_array.push_back({ "Title 2", 42, 3.1415f, true });

   std::string filename = "./config/game_data.txt";                                // Name of file that should be used to read/write
   std::string topic_name = "example_structure";                                   // Name of the serialized block in file
   PFF::serializer::option loc_option = PFF::serializer::option::load_from_file;   // Defines wheter the serializer should save or load. The options are: option::load_from_file or option::save_to_file
   
   // This is the actual serializer. I opted to not use a reflection system
   // You have to specify what data you want to save
   PFF::serializer::yaml(filename, topic_name, loc_option)			
      .entry(KEY_VALUE(my_example_struct.name))             // You can use the KEY_VALUE() macro to automaticly determin the name of the saved field. Here: "name"
      .entry("is_var_in_use",  my_example_struct.used)      // You can also provide a custom name for the field
      
      .vector(KEY_VALUE(my_example_struct.data_array), [&](serializer::yaml& yaml, const u64 x) {
      
         yaml.entry(KEY_VALUE(my_example_struct.data_array[x].title))
         .entry(KEY_VALUE(my_example_struct.data_array[x].integer))
         .entry(KEY_VALUE(my_example_struct.data_array[x].float_num))
         .entry(KEY_VALUE(my_example_struct.data_array[x].done));
      });
}
```

As the same funtion-chain can be used for serialization and deserilization, one can create a small [class] or [function] that is called with the needed [option]: 
```cpp
static void todo_list_serialize(todo_list* todo_list, serializer::option option, const std::string& filename = "./config/todo_list.txt") {

    serializer::yaml(filename, "todo_list", option)
        .vector(KEY_VALUE(todo_list->m_topics), [&](serializer::yaml& yaml, const u64 x) {

            yaml.entry(KEY_VALUE(todo_list->m_topics[x].name))
                .entry(KEY_VALUE(todo_list->m_topics[x].selected))
                .vector(KEY_VALUE(todo_list->m_topics[x].tasks), [&](serializer::yaml& inner, const u64 y) {

                inner.entry(KEY_VALUE(todo_list->m_topics[x].tasks[y].title))
                    .entry(KEY_VALUE(todo_list->m_topics[x].tasks[y].description))
                    .entry(KEY_VALUE(todo_list->m_topics[x].tasks[y].done));
            });
        });
}
```

### Function Explanation
- The `yaml` constructor in the `PFF::serializer` namespace is the primary entry point for YAML serialization. It takes the filename, section name, and serialization option as parameters.
- The `entry` function is used to save primitive types or structs with primitive members.
- The `vector` function is used to save vectors of structs.

## Implementation Details

### Main Class
The main class for YAML serialization is `yaml` in the `PFF::serializer` namespace. It provides the necessary functions for serializing and deserializing game data in YAML format.

#### Function Signature
```cpp
yaml(const std::string& filename, const std::string& section_name, option option);
```

- `filename`: The name of the file to which the data will be serialized or from which it will be deserialized.
- `section_name`: The name of the section under which the data will be stored in the file.
- `option`: The option specifying whether to serialize or deserialize the data.
