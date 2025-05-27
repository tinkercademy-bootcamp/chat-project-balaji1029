# Exercise 3

**Update this README with your answers to the questions below.**

## Re-using Code

- Read the code in `src/tcp_echo_client.cc`
- A new function `check_error()` has been created and `create_socket()` from 
  exercise-2 has been refactored to make use of it
- What are the benefits of writing code in this way?
  - This is easier to use and removes redundant code so one doesn't have to write the same code multiple times
- Are there any costs to writing code like this?

  - No?
- Apply `check_error` to all the code in `src/`

## Introduction to Compiler Explorer

- Try out the old `create_socket()` and the new `check_error()` and 
  `create_socket()` in [Compiler Explorer](https://godbolt.org) - Interactive tool for exploring how C++ code compiles to assembly
  - It's calling a function instead of doing the same in the function
- What is happening here?

  - It's a funcion call and the code is calling the `check_error`
- Can you think of any different approaches to this problem?
- How can you modify your Makefile to generate assembly code instead of
  compiled code?

  - Using `-S` flag with `g++` or `objdump -d`
- **Note**: You can save the generated assembly from Compiler Explorer
- **Bonus**: Can you view assembly code using your IDE?
- **Bonus**: How do you see the assembly when you step through each line in
  debugging mode from your IDE?
  - With gdb, yes using tui enabled
- [x86 assembly reference](http://ref.x86asm.net/) - Comprehensive reference 
  for x86 assembly language instructions and syntax

## More About Memory Management

- Make sure you have `-fsanitize=address` in both your `CXX_FLAGS` and 
  `LD_FLAGS` in your Makefile
- What do `-fsanitize=address`, `CXX_FLAGS` and `LD_FLAGS` mean?

  - Address Sanitizer gives details of memory leakages and double-free and use-after-free bugs
- With the new tool of the Compiler Explorer, and keeping in mind what you 
  have learned about how to use debug mode
- What happens when you look at a `std::string` using the above methods?

  - You find it in the `.data` section of the program
- Where is the text in your `std::string`?

  - In the `.data` section
- What is `std::optional`?

  - `std::optional` is a container for nullable objects to make sure we don't run into accessing garbage memory. It stores an object or a null type and gives interface to use the object safely
- How do you find out the memory layout of a `std::optional`?

  - Can check it out by running a debugger on a code and checking the variable's data
- Read https://en.cppreference.com/w/cpp/memory#Smart_pointers - Guide to 
  modern C++ memory management using smart pointers
- Which pointer types are the most important to know about?

  - `shared_ptr`, `unique_ptr`
- Which smart pointer should you use by default if you can?

  - `unique_ptr` should be the go to pointer if we know that the reference is going to be unique, but `shared_ptr` is more useful in data structures like Linked List
- Does changing your optimization level in `CXXFLAGS` from `-O0` to `-O3` have
  any impact on the answers to any of the above questions?

  - No, the specifications of the C++ classes don't change by the optimisations used by the compiler

## More Thinking About Performance

- After your experiments with Compiler Explorer, do you have any updates for
  your answers in exercise-2?

  - We can check out the memory layout of statically defined character arrays in the `.data` section of the executable

### Bonus: Do Not Watch Now 

- [More about Compiler Explorer](https://www.youtube.com/watch?v=bSkpMdDe4g4) - 
  Deep dive into compiler optimization and assembly analysis
  - Create a struct which contain many different data types
  - Look at the memory layout in the debugger
  - Create a `char` or `uint8_t` pointer to the beginning of the struct, 
    iterate to the end of the struct, printing out the value of each byte in 
    hexadecimal
  - Try accessing different parts of the struct and see in compiler explorer
    what the assembly looks like
  - What happens if you iterate the pointer to outside the bounds of your
    array?
  - Let's say your struct is called `Foo`
  - What is the difference between `std::vector<Foo>` and `std::vector<Foo*>`?
  - What are the tradeoffs between using `std::vector<Foo>` vs 
    `std::vector<Foo*>`? 
  - Give an example where `std::vector<Foo>` is a better choice than 
    `std::vector<Foo*>`
  - Give another example where the opposite is true
  - Can you create `std::vector<Foo&>`? 
  - Can you create `std::vector<std::optional<Foo>>`?
  - What happens if your struct contains another struct?
  - What is the difference between a struct and a class?