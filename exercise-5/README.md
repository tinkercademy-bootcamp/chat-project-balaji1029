# Exercise 5

**Update this README with your answers to the questions below.**

## Comparing File Changes

- What's an easy way to identify the changes in source code from one exercise
  to another?

  - We can use `diff -qr ` across the directories to get the diff
    
## Using Classes

- Here is one way to separate out the code into multiple files
- Is this the best way to do it? 

  - There is no best way to do it? It all depends on how modularised and easy to browse the code is
- What are the advantages and disadvantages?

  - The advantages are that the functions are seperated according to the purpose and hence are easier to look for and work with

## Introduction to Namespace

- There are different ways namespace is being used in this exercise
- Which way is better? What are the advantages and disadvantages?

  - The namespaces here is the convention suggested by the Rust book with the directory structure and gives a huge tree structure of directories when there are too many features in the program

## Abstracting Code into Classes

- Abstract the client and server logic into a `Client` and `Server` class
- **Note**: You don't have to use the code in this exercise as a starting point
- You can use the code you wrote from previous exercises instead
- How should you divide the code into files?
- What namespace and directory structure should you use? Why?

## Programming Sense of Taste

- How do you judge what makes a particular choice of namespace and directory
  structure? 
- How do you judge what makes a good naming convention or programming style?

## "Senses" in Programming

- Have you ever heard of programmers referring to "code smells"? How can code
  smell?
- What does it mean to have a sense of taste in programming? How can code
  taste?
- Is there an analogue for every sense?
- What other code senses can you think of?
- How many senses do humans have?
- When would you want to see something before you touch it?
- When would you want to touch something before you see it?