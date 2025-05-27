# Exercise 4

**Update this README with your answers to the questions below.**

## Learning Multi-File C++ Projects

- Move code that can be shared between `tcp_echo_server.cc` and 
  `tcp_echo_client.cc` to separate `.h` and `.cc` files
- How would you compile from the command line?

  - `g++ src/tcp_echo_server.cpp server_helper.cc -o build/server`
- How would you compile using make?

  - Same thing in make but I used a bit more magic variables
- How would you compile using VS Code?

### Compiling vs Linking

- What is the difference between compiling and linking in C++?

  - Compiling refers to creation of object files, and linking refers to linking thees object files together to form an executable
- What is the difference between creating an executable and creating a 
  library?

  - An executable should contain a `main()` function to run but a library can just be an object file ready to be linked to an executable
- How do you compile a library, and then use that library to compile an
  executable?

  - Compile with a `-c` flag to make a library and combine it while compiling

### Reminder 
[Quickstart tutorial to make](https://makefiletutorial.com/) - Learn make 
fundamentals with practical examples and common patterns.

## Experiments in Performance

- Is it better to have many small files spread across many directories, or
  a few large files across fewer directories?

  - For a programmer, it's better to have organized files over large files as they are easier to read and debug
- Is there a difference in compilation time between the two?

  - The compilation takes more time to search for the functions in other files and link them
- How can you assess performance other than compilation speed?

  - The performance of the executable shouldn't depend on the number of files as the linking has been done and the functions are linked