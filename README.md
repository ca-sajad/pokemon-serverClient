## Server Client Program for Pokemon Database Search

The program creates
- a server whose task is to search [pokemon.csv](https://github.com/ca-sajad/pokemon-serverClient/blob/main/pokemon.csv)
   for a \"type 1\" Pokemon search word entered by the user. Server sends found records to the client,
   or notifies the client if no Pokemons are found.
- a client that gives the user three options: a. type a search word, b. write the results of option a query to a file,
  or c. exit the client program.

Program highlights:
- Uses C to create local server and client applications
- Uses threads for options a and b to give the user a seamless experience
  (in case delayed server response results in a slow option a)
- Uses TCP to transfer data between client and server
- Modularizes the code into server and client-side files

To compile and run the program:
- Open Terminal and enter: make
- Enter: ./pps
- Open a second Terminal and Enter: ./pqc

For more information on program workflow and requirements see 
[Design Document](https://github.com/ca-sajad/pokemon-serverClient/blob/main/Design%20Document.pdf)
and [Program Requirements](https://github.com/ca-sajad/pokemon-serverClient/blob/main/Program%20Requirements.pdf).

This program was coded for the assignment 4 of Carleton University COMP 2401 course in summer 2023.
