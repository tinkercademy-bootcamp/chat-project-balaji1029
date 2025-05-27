# Exercise 2

**Update this README with your answers to the questions below.**

## Sources of Information for Questions from Before

### Socket 
- https://man7.org/linux/man-pages/man2/socket.2.html - System call reference
  for creating communication endpoints
- Or type `man socket` in terminal
- https://man7.org/linux/man-pages/man7/socket.7.html - Socket interface 
  overview and protocol families
- Or type `man 7 socket` in terminal
- When would you want to use a `SOCK_RAW` stream?

  - We use SOCK_RAW when we want to not use the standard TCP/IP protocols

### TCP and IP Protocols
- [IPv4](https://www.rfc-editor.org/info/rfc791) - Internet Protocol 
  specification defining packet structure and routing
- [IPv6](https://www.rfc-editor.org/info/rfc8200) - Next-generation Internet 
  Protocol with expanded address space
- [TCP](https://datatracker.ietf.org/doc/html/rfc9293) - Transmission Control 
  Protocol providing reliable, ordered data delivery
    
### C++
- [C++23 ISO standard draft](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/n4950.pdf) - 
  Working draft of the C++ language specification
- Is the above the official C++23 spec? 

  - Nope, it is an initial draft and it is incomplete
- Where is the official C++23 spec?

  - [The standard](https://isocpp.org/std/the-standard)
- Why was this link chosen instead?

  - Idkkk, cuz it's a pdf which is compiled?
- Is this a helpful reference for learning C++?

  - Yeahh looks like it
- Can the various implementations of C++ compilers be different from the
  C++ standard?

  - No, they should follow the specifications
- What are the most widely used and most significant C++ compilers?

  - g++, clang
- Where is the equivalent spec for C++26?

  - [C++26 draft](https://github.com/cplusplus/draft)
- Where do you find the spec for the HTTP protocol?

  - [Specs](https://httpwg.org/specs/)
- What about HTTPS? Is there a spec for that protocol?

  - Not that I could find

## Introduction to C++ and Sockets Programming

- Read the code in `src/`
- Are there any bugs in this code? 

  - Just one, the `tcp_echo_client.cc` should have `message.size() + 1` in line 49.
- What can you do to identify if there are bugs in the code?

  - Run testcases and check abnormal behaviours

## Refactoring: Extract Function

- What is different in this code compared to exercise-1?

  - This code is more modularized
- Is this code better or worse than exercise-1?

  - This is easier to read and work with and debug
- What are the tradeoffs compared to exercise-1?

  - This can be debugged and tested easily by creating tests for each function
- Are you able to spot any mistakes or inconsistencies in the changes?

  - No, not that I could find
  
## Thinking About Performance

- Does writing code this way have any impact on performance?

  - More the function calls adds up the stack memory but shouldn't affect much unless working on small scale programs
- What do we mean when we say performance?

  - The time overhead
- How do we measure performance in a program?

  - We can measure the performance in a program with `time` or `gprof`

## Play with Git

- There isn't necessarily a single correct answer for how to abstract the 
  code from exercise-1 into functions
- Try different ways to refactor the code from exercise-1 to make it more
  readable.
- Make sure to commit each change as small and self-contained commit
- This will make it easier to revert your code if you need to
- What is `git tag`? How is `git tag` different from `git branch`?

  - A tag is a naming for a specific commit to name it as a version of the code
- How can you use `git tag` and `git branch` to make programming easier and
  more fun?

## Learn Basics of Debugging in Your IDE

- How do you enable debug mode in your IDE?

  - Can use `launch.json`
- In debug mode, how do you add a watch?

  - Right click and select watch
- In debug mode, how do you add a breakpoint?

  - Click on the red button next to the line
- In debug mode, how do you step through code?

  - Step with the Step button

### Memory Management and Debug Mode in Your IDE

- How do you see the memory layout of a `std::string` from your IDE debug mode?
  - Extend the string's section in the variables section
- How do you see the memory layout of a struct from your IDE debug mode?
  
  - Same but for the structs