### Shortest Path Problem GUI ([Xlib](https://en.wikipedia.org/wiki/Xlib)) simulation using [Dijkstra](https://en.wikipedia.org/wiki/Dijkstra's_algorithm)'s algorithm.

Program takes a set of triangle obstacles as input, as well as a start and target Point, and shows the shortest path between them. 

Two input modes:
* Command-Line: The file should contains obstacles, one per line, 
                each line starting with a T, as in the following format example:  
                T (20,100) (55,63) (30,50)
                
* Interactive: left mouseclicks define triangle vertices, each time three points were entered, the triangle is displayed. 
When a right mouse-click event is received, this ends the triangle input; if there are one or two points of an unfinished 
triangle left, they are discarded. Then two left mouse-click events, which give the start and target point of the shortest path. Yey! :simple_smile:

![alt text](https://github.com/NikaTsanka/ "Demo")