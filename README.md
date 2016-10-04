# shortest-path
The aim of this project is to create a program that takes a set of 
triangle obstacles as input, as well as a start and target point, 
and shows the shortest path between them.
It provides a visual feedback by drawing the points, obstacles, 
and the path on the screen using the xlib interface. The program takes 
either one command-line argument, a file name, or enters the triangle 
obstacles interactively (both must be supported) . If a file with 
triangle obstacles is given, the file contains obstacles, one per line, 
each line starting with a T, as in the following format example: 
T (20,100) (55,63) (30,50)
