# Exercise 6

**Update this README with your answers to the questions below.**

## Adding Third Party Libraries or Source Code to Your Project

- There is a new file `src/spdlog-hello-world-main.cc`
- It uses the spdlog and fmt libraries. 
  [More info here](https://github.com/gabime/spdlog) - Fast C++ logging 
  library with various sink types and formatting options
- How do you compile this file using the `g++` CLI?

  - Just by including this file in the command-line argument of `g++`
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
- What are the tradeoffs?
- How do you enable static linking or dynamic linking in your makefile?

## Git Clone and Building from Source

- Where is `g++` looking for the include files and library files?
- How do you find out?

## C++ Package Managers

- Why are there so many C++ package managers?
- Which one is the best one?
- Which are the most notable ways to manage the compilation of C++ projects?