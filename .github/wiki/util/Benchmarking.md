
# Benchmarking   -   ONLY IN DEBUG
 ## Benchmarking Macros

  The `PFF_SCOPED_BENCHMARK` macro simplifies the use of benchmarking within a scoped block. It automatically collects the durations of each execution of the given scope. 
  When the count of collected samples is equal to the specefied number, it logs a message with the average duration.

  CAUTION - This means this Macro will only collect the duration of this scope, multiply calling this scope is the respensibility of the surounding code (best used in calculations done each frame)
  
  **Usage:**
  ```cpp
{
  PFF_SCOPED_BENCHMARK(700, "calc frustum planes", PFF::duration_precision::microseconds);

  // Perform operations to benchmark
}
  ```

   In this example:
   - `700` is the number of iterations the code will be executed.
   - `"calc frustum planes"` is the message that will be logged.
   - `microseconds` specifies the unit of time for the duration. Possible values for are: `microseconds`, `milliseconds` and `seconds`

  The `PFF_BENCHMARK_LOOP` macro is designed for quick benchmarking of a code block executed multiple times in a loop. It measures the total duration of the loop execution and logs the average duration per iteration. This macro is useful for performance testing when you want to evaluate the execution time of specific code segments.

  CAUTION - This macro executes the provided code within a loop to collect timing data. Note that compilers might optimize away operations between loop iterations, which can affect the accuracy of the results. Ensure that the code within the loop does enough work to avoid being optimized out.

  **Usage:**
  ```cpp
PFF_BENCHMARK_LOOP(100000, "example benchmark", microseconds,

  u16 the_truth = 42;    // do some calculations
);
  ```
  
   In this example:
   - `100000` is the number of iterations the code will be executed.
   - `"example benchmark"` is the message that will be logged.
   - `microseconds` specifies the unit of time for the duration. Possible values for are: `microseconds`, `milliseconds` and `seconds`
   - `u16 the_truth = 42;` is the code block whose performance is being measured. This is an example of code that the compiler could optimize away
     The complete message might look like this: `example benchmark => sample count: 100000 average duration: 4.9e-07 microseconds`

 ## Simple Benchmarking Class
  
  You can also call the `simple_bencharking` class manually if you want to use it to measure your performance
  
  **Key Features:** 
  - **Sample Collection**: Collects timing samples up to a specified number of tests.
  - **Average Calculation**: Computes and logs the average duration of the samples.
  - **Logging**: Outputs benchmarking results to the log with a message.
  
  **Usage:**
  
  ```cpp
PFF::benchmarking::simple_bencharking loc_benchark(10000, "example benchmark", PFF::duration_precision::milliseconds);

{   // a loop of some form
  PFF::stopwatch loc_stopwatch_for_benchmark(&loc_benchark.single_duration, PFF::duration_precision::milliseconds);
  if (loc_benchark.add_value())
    loc_stopwatch_for_benchmark.restart();
    
  // Perform operations to time
}
  ```
  
  **Constructor:**
  - `simple_bencharking(u64 num_of_tests, std::string message, PFF::duration_precision precision = PFF::duration_precision::milliseconds)`: Initializes with the number of tests, a message, and precision.
  
  **Methods:**
  - `bool add_value()`: Adds a timing sample and checks if all samples have been collected.
  