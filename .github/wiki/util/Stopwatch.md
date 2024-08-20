# Stopwatch

 The PFF Stopwatch and Benchmarking Module is a lightweight timing utility designed to help measure and benchmark code execution in C++. It consists of two main components: the `stopwatch` class and the `simple_bencharking` class, along with a macro for convenient benchmarking.

  ```cpp
f32 measured_time{};
{
   PFF::stopwatch loc_stopwatch(&measured_time, PFF::duration_precision::microseconds);

   // Perform operations to time
}

// [measured_time] now contains the elapsed time in microseconds
  ```
  
  **Methods:**
  - `f32 stop()`: Stops the stopwatch and calculates the elapsed time. Updates the result pointer if provided.
  - `void restart()`: Restarts the stopwatch from the current time.
  
  **Constructor:**
  - `stopwatch(duration_precision precision = duration_precision::milliseconds)`: Constructs a stopwatch with the specified precision.
  - `stopwatch(f32* result_pointer, duration_precision precision = duration_precision::milliseconds)`: Constructs a stopwatch that updates a result pointer with elapsed time.
  