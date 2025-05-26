# Exercise 1

**Update this README with your answers to the questions below.**

## Learning How to Learn

- Answer the following questions in this file and commit and push your changes.
- Bonus sections are more difficult and optional.
- How can you find the information required to complete these tasks?
- How can you tell if the source of your information is good?
- How would you define "good" in this situation?

## Learn Basics of g++ CLI

- Compile the TCP client and server using `g++` from command line.
- What are the most important command line arguments to learn for `g++`?
  - -o : Names the output file
  - -c : Stops the compilation at object file
  - -Ox : Optimizes the code to Ox
  - -std=c++xx : Specifies the version of C++ to use
  - -g : Saves the debugging symbols for the code
- What is the difference between debug vs release versions?
  - A debug build is a build made for easy debugging while release version is a version that's released to public usage or to the clients
- What are the tradeoffs between debug and release versions?

  - debug version is slow and has a huge memory footprint but release versions are relatively lighter
- What arguments would you use in a debug build?
  - Maybe a -g flag to store the debug symbols
- What about for release?

  - For the release, one might use -O3 or -O2 flags for optimization
- What other kinds of build types are useful?

  - debug, test and so on

## Learn Basics of Make

- Create a Makefile that will speed up the process.
- [Quickstart tutorial to make](https://makefiletutorial.com/) - Learn make 
  fundamentals with practical examples and common patterns.
- How else can you learn about make?
- How can you tell if the resource you are using is correct?
- Create a makefile such that when you run `make` with no arguments, it will:
  - Create `build/` directory if it does not exist
  - Create executables **client** and **server** in `build/`, if needed
  - How does make know when it needs to rebuild the executables?
  - Change your Makefile such that `make clean` will remove `build/` and all
    its contents
- What are the most important command line arguments to learn for make?
- What are the most important directives to learn about in Makefile?
- What are the most important commands to implement in your Makefile?
- Which ones are essential, which ones are nice to haves?

## Learn Basics of Git

- Read through the code in `src/`
- Answer any `#Questions` as a comment
- Commit and push your changes to git
- Each commit should be responding to a single task or question
- Why is it important to keep your commit to a single task or question?
- Is it better to have a lot of very small commits, or one big commit when 
  everything is working?
- What are the most important commands to know in git?

## Introduction to Sockets

- Read the code in `src/tcp-echo-client.cc` and add a way to change the 
  message sent using command line arguments
- **Example**: `./client "hello message from the command prompt"` should send
  `"hello message from the command prompt"` to the server
- Commit your changes into git
- What do all these headers do?

  - Each header has one or the other functionality
- How do you find out which part of the below code comes from which header?

  - Ctrl + click
- How do you change the code so that you are sending messages to servers
  other than localhost?

  - Changing the address in the kServerAddress
- How do you change the code to send to a IPv6 address instead of IPv4?

  - Have to change the functions from IPv4 versions to IPv6
- **Bonus**: How do you change the client code to connect by hostname instead
  of IP address?
  
## Introduction to Memory Management

- What is happening in line 26 of `tcp-echo-client.cc`? 
  `if (inet_pton(AF_INET, kServerAddress.c_str(), &address.sin_addr) <= 0) {`

  - It's converting the IP address into binary form
- What is happening in line 31 of `tcp-echo-client.cc`?
  `if (connect(my_sock, (sockaddr *)&address, sizeof(address)) < 0) {`

  - It connects to the socket in the address
- What is the difference between a pointer and a reference?

  - Pointer is a variable storing address to another variable whereas a reference is an rvalue that points to another memory location
- When is it better to use a pointer?

  - When iterating and are sure about safety
- When is it better to use a reference?

  - In function calls and return and when you want it to always point to something and only one var
- What is the difference between `std::string` and a C-style string?

  - One is a class that stores the char array on heap while one is a char array on the stack
- What type is a C-style string?

  - A char array
- What happens when you iterate a pointer?

  - We can access memory that is not allocated yet
- What are the most important safety tips to know when using pointers?

  - Take care of free-after-free or use-after-free of memory or dangling pointers

## Learn Basics of Creating a C++ Project in Your IDE

- How do you compile and run your project in your IDE?

## Improving Interactions with LLMs

- What is the most authoritative source of information about `socket()`
  from `<sys/socket.h>`?

  - man page
- What is the most authoritative source of information about the TCP and IP
  protocols?

  - Sites like cloudflare or IBM
- What is the most authoritative source of information about the C++
  programming language?

  - cppreference.com or devdocs.io/cpp
- What information can you find about using Markdown when structuring prompts 
  to LLMs?

  - It helps GPT understand promts better (due to the better formatting???) 
- What is the difference between LLM and AI?

  - An LLM is a Language Model whereas AI includes a lot more intelligence techniques
- Is it grammatically correct in English to say "a LLM" or "an LLM"? Why?

  - an LLM
