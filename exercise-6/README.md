# Exercise 6

**Update this README with your answers to the questions below.**

## Adding Third Party Libraries or Source Code to Your Project

- There is a new file `src/spdlog-hello-world-main.cc`
- It uses the spdlog and fmt libraries. 
  [More info here](https://github.com/gabime/spdlog) - Fast C++ logging 
  library with various sink types and formatting options
- How do you compile this file using the `g++` CLI?

  - `g++ src/spdlog-hello-world-main.cc -lfmt -lspdlog`
- What do you need to change in your makefile to use this library?

  - We just have to add the same into the recipe of the targets where this file is required
- How many different ways can this library be added into your project?

  - This can be created as an object file and then linked seperately or just linked directly, or given as a library along with the libraries in g++ and include in the include.h so the compiler includes it automatically
- What are the tradeoffs in the different ways?

  - The second way requires us to also send the g++ environment with the project, which can be done with a docker
- Why are there so many different ways to do it?

  - For flexibility and use according to the use-case
  
## Static Linking vs Dynamic Linking

- What are the differences between static linking and dynamic linking?

  - Static linking is when the functions used by the code are linked into the executable while compiling it, whereas in the case of dynamic linking, the object files are dynamically linked during the runtime
- What are the tradeoffs?

  - Static Linking leads to a lot of redundant space in the memory as well as the executable size, whereas dynamic linking leads to slower runtime as it includes linking during runtime
- How do you enable static linking or dynamic linking in your makefile?

  - To compile a library file to a dynamically linker file,
  ```bash
    g++ -fPIC -c hello.cpp
    g++ -shared -o libhello.so hello.o 
  ```
  Then to link the library to a program
  ```bash
    g++ -o main main.cpp -L<path/to/libhello.so> -lhello
  ```

  We can automate this with a makefile

## Git Clone and Building from Source

- Where is `g++` looking for the include files and library files?

  - It searches for include files from `/usr/include/c++/13` or according to the version, and library files in `/usr/lib/gcc/x86_64-linux-gnu/13`
- How do you find out?

  - I found out by running `find /usr -name iostream` and `find /usr -name libstdc++.so`
## C++ Package Managers

- Why are there so many C++ package managers?

  - Because a lot of companies started to work on C++ independently and they have their own implementations of libraries
- Which one is the best one?

  - Best package manager is subjective and depends on the choice, but Conan seems to be the most agreed upon by developers, though I've neveer used one
- Which are the most notable ways to manage the compilation of C++ projects?

  - We can use Make or CMake to manage the compilation of C++, we can also use bash scripts to do the same, although that's a bad idea considering a bash script wouldn't consider the last edit time