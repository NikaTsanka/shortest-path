#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

using namespace std;

/*******************************************************************************/
/*

// A structure to represent a node in adjacency list
struct AdjListNode
{
    int dest;
    int weight;
    struct AdjListNode* next;
};

// A structure to represent an adjacency liat
struct AdjList
{
    struct AdjListNode *head;  // pointer to head node of list
};

// A structure to represent a graph. A graph is an array of adjacency lists.
// Size of array will be V (number of vertices in graph)
struct Graph
{
    int V;
    struct AdjList* array;
};

// A utility function to create a new adjacency list node
struct AdjListNode* newAdjListNode(int dest, int weight)
{
    struct AdjListNode* newNode =
            (struct AdjListNode*) malloc(sizeof(struct AdjListNode));
    newNode->dest = dest;
    newNode->weight = weight;
    newNode->next = NULL;
    return newNode;
}

// A utility function that creates a graph of V vertices
struct Graph* createGraph(int V)
{
    struct Graph* graph = (struct Graph*) malloc(sizeof(struct Graph));
    graph->V = V;

    // Create an array of adjacency lists.  Size of array will be V
    graph->array = (struct AdjList*) malloc(V * sizeof(struct AdjList));

    // Initialize each adjacency list as empty by making head as NULL
    for (int i = 0; i < V; ++i)
        graph->array[i].head = NULL;

    return graph;
}

// Adds an edge to an undirected graph
void addEdge(struct Graph* graph, int src, int dest, int weight)
{
    // Add an edge from src to dest.  A new node is added to the adjacency
    // list of src.  The node is added at the begining
    struct AdjListNode* newNode = newAdjListNode(dest, weight);
    newNode->next = graph->array[src].head;
    graph->array[src].head = newNode;

    // Since graph is undirected, add an edge from dest to src also
    newNode = newAdjListNode(src, weight);
    newNode->next = graph->array[dest].head;
    graph->array[dest].head = newNode;
}

// Structure to represent a min heap node
struct MinHeapNode
{
    int  v;
    int dist;
};

// Structure to represent a min heap
struct MinHeap
{
    int size;      // Number of heap nodes present currently
    int capacity;  // Capacity of min heap
    int *pos;     // This is needed for decreaseKey()
    struct MinHeapNode **array;
};

// A utility function to create a new Min Heap Node
struct MinHeapNode* newMinHeapNode(int v, int dist)
{
    struct MinHeapNode* minHeapNode =
            (struct MinHeapNode*) malloc(sizeof(struct MinHeapNode));
    minHeapNode->v = v;
    minHeapNode->dist = dist;
    return minHeapNode;
}

// A utility function to create a Min Heap
struct MinHeap* createMinHeap(int capacity)
{
    struct MinHeap* minHeap =
            (struct MinHeap*) malloc(sizeof(struct MinHeap));
    minHeap->pos = (int *)malloc(capacity * sizeof(int));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array =
            (struct MinHeapNode**) malloc(capacity * sizeof(struct MinHeapNode*));
    return minHeap;
}

// A utility function to swap two nodes of min heap. Needed for min heapify
void swapMinHeapNode(struct MinHeapNode** a, struct MinHeapNode** b)
{
    struct MinHeapNode* t = *a;
    *a = *b;
    *b = t;
}

// A standard function to heapify at given idx
// This function also updates position of nodes when they are swapped.
// Position is needed for decreaseKey()
void minHeapify(struct MinHeap* minHeap, int idx)
{
    int smallest, left, right;
    smallest = idx;
    left = 2 * idx + 1;
    right = 2 * idx + 2;

    if (left < minHeap->size &&
        minHeap->array[left]->dist < minHeap->array[smallest]->dist )
        smallest = left;

    if (right < minHeap->size &&
        minHeap->array[right]->dist < minHeap->array[smallest]->dist )
        smallest = right;

    if (smallest != idx)
    {
        // The nodes to be swapped in min heap
        MinHeapNode *smallestNode = minHeap->array[smallest];
        MinHeapNode *idxNode = minHeap->array[idx];

        // Swap positions
        minHeap->pos[smallestNode->v] = idx;
        minHeap->pos[idxNode->v] = smallest;

        // Swap nodes
        swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[idx]);

        minHeapify(minHeap, smallest);
    }
}

// A utility function to check if the given minHeap is ampty or not
int isEmpty(struct MinHeap* minHeap)
{
    return minHeap->size == 0;
}

// Standard function to extract minimum node from heap
struct MinHeapNode* extractMin(struct MinHeap* minHeap)
{
    if (isEmpty(minHeap))
        return NULL;

    // Store the root node
    struct MinHeapNode* root = minHeap->array[0];

    // Replace root node with last node
    struct MinHeapNode* lastNode = minHeap->array[minHeap->size - 1];
    minHeap->array[0] = lastNode;

    // Update position of last node
    minHeap->pos[root->v] = minHeap->size-1;
    minHeap->pos[lastNode->v] = 0;

    // Reduce heap size and heapify root
    --minHeap->size;
    minHeapify(minHeap, 0);

    return root;
}

// Function to decreasy dist value of a given vertex v. This function
// uses pos[] of min heap to get the current index of node in min heap
void decreaseKey(struct MinHeap* minHeap, int v, int dist)
{
    // Get the index of v in  heap array
    int i = minHeap->pos[v];

    // Get the node and update its dist value
    minHeap->array[i]->dist = dist;

    // Travel up while the complete tree is not hepified.
    // This is a O(Logn) loop
    while (i && minHeap->array[i]->dist < minHeap->array[(i - 1) / 2]->dist)
    {
        // Swap this node with its parent
        minHeap->pos[minHeap->array[i]->v] = (i-1)/2;
        minHeap->pos[minHeap->array[(i-1)/2]->v] = i;
        swapMinHeapNode(&minHeap->array[i],  &minHeap->array[(i - 1) / 2]);

        // move to parent index
        i = (i - 1) / 2;
    }
}

// A utility function to check if a given vertex
// 'v' is in min heap or not
bool isInMinHeap(struct MinHeap *minHeap, int v)
{
    if (minHeap->pos[v] < minHeap->size)
        return true;
    return false;
}

// A utility function used to print the solution
void printArr(int dist[], int n)
{
    printf("Vertex   Distance from Source\n");
    for (int i = 0; i < n; ++i)
        printf("%d \t\t %d\n", i, dist[i]);
}

// The main function that calulates distances of shortest paths from src to all
// vertices. It is a O(ELogV) function
void dijkstra(struct Graph* graph, int src)
{
    int V = graph->V;// Get the number of vertices in graph
    int dist[V];      // dist values used to pick minimum weight edge in cut

    // minHeap represents set E
    struct MinHeap* minHeap = createMinHeap(V);

    // Initialize min heap with all vertices. dist value of all vertices
    for (int v = 0; v < V; ++v)
    {
        dist[v] = INT_MAX;
        minHeap->array[v] = newMinHeapNode(v, dist[v]);
        minHeap->pos[v] = v;
    }

    // Make dist value of src vertex as 0 so that it is extracted first
    minHeap->array[src] = newMinHeapNode(src, dist[src]);
    minHeap->pos[src]   = src;
    dist[src] = 0;
    decreaseKey(minHeap, src, dist[src]);

    // Initially size of min heap is equal to V
    minHeap->size = V;

    // In the followin loop, min heap contains all nodes
    // whose shortest distance is not yet finalized.
    while (!isEmpty(minHeap))
    {
        // Extract the vertex with minimum distance value
        struct MinHeapNode* minHeapNode = extractMin(minHeap);
        int u = minHeapNode->v; // Store the extracted vertex number

        // Traverse through all adjacent vertices of u (the extracted
        // vertex) and update their distance values
        struct AdjListNode* pCrawl = graph->array[u].head;
        while (pCrawl != NULL)
        {
            int v = pCrawl->dest;

            // If shortest distance to v is not finalized yet, and distance to v
            // through u is less than its previously calculated distance
            if (isInMinHeap(minHeap, v) && dist[u] != INT_MAX &&
                pCrawl->weight + dist[u] < dist[v])
            {
                dist[v] = dist[u] + pCrawl->weight;

                // update distance value in min heap also
                decreaseKey(minHeap, v, dist[v]);
            }
            pCrawl = pCrawl->next;
        }
    }

    // print the calculated shortest distances
    printArr(dist, V);
}
*/

/*******************************************************************************/

const int INIT_COLS = 6;
const int NUM_COLS = 2;

bool ends_with(const std::string &, const std::string &);

void split(const string &, char, vector<string> &);

string parse_string(std::string);

int det(int, int, int, int, int, int);

void check_edge(const int [][2]);

int calc_dist(const int x1, const int y1, const int x2, const int y2);

vector<string> split(const string &s, char delim) {
    vector<string> elems;
    split(s, delim, elems);
    return elems;
}
/* here are our X variables */
Display *dis;
int screen;
Window win;

GC gc;
/* here are our X routines declared! */
void init_x();
void close_x();

void redraw();

int main(int argc, char *argv[]) {
    // cout << "Hello, World!" << endl;

    string line;

    vector<string> vector1;

    // count lines and initialize
    int number_of_lines = 0;
    std::string lines;
    std::ifstream myFile(argv[1]);

    while (std::getline(myFile, lines)) {
        ++number_of_lines;
    }
    myFile.close();

    // coordinate container
    int coordinates[number_of_lines][INIT_COLS];

    // start and target points
    int start_target[2][2];

    if (argc == 2) {
        if (ends_with(argv[1], ".txt")) {
            // open the file
            ifstream my_file;

            int line_count = 0;

            my_file.open(argv[1]);

            if (my_file.is_open()) {

                while (getline(my_file, line)) {
                    // parse and split
                    line = parse_string(line);
                    vector1 = split(line, ',');

                    // assign
                    if (vector1[0] != "\n" ) {
                        coordinates[line_count][0] = atoi(vector1[0].c_str());
                        coordinates[line_count][1] = atoi(vector1[1].c_str());
                        coordinates[line_count][2] = atoi(vector1[3].c_str());
                        coordinates[line_count][3] = atoi(vector1[4].c_str());
                        coordinates[line_count][4] = atoi(vector1[6].c_str());
                        coordinates[line_count][5] = atoi(vector1[7].c_str());

                        line_count++;
                    }
                }
            } else {
                cout << "Couldn't open the specified file\n";
                return 0;
            }

            cout << "(" << argv[1] << ") file was passed.\n" << endl;
            my_file.close();

            /*for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 6; j++) {
                    //pInt[i][j] = '.';
                    printf("pInt[%d][%d] = %d\n", i,j, coordinates[i][j] );
                }
                printf("\n");
            }*/

            /*----------------------------------------------------------------------------------------*/

            XEvent event;		/* the XEvent declaration !!! */
            KeySym key;		/* a dealie-bob to handle KeyPress Events */
            char text[255];		/* a char buffer for KeyPress Events */

            init_x();

            int num_of_clicks = 0;

            /* look for events forever... */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
            while(1) {
                /* get the next event and stuff it into our event variable.
                   Note:  only events we set the mask for are detected!
                */
                XNextEvent(dis, &event);

                if (event.type==Expose && event.xexpose.count==0) {
                    /* the window was exposed redraw it! */
                    redraw();
                }
                if (event.type==KeyPress&&
                    XLookupString(&event.xkey,text,255,&key,0)==1) {
                    /* use the XLookupString routine to convert the invent
                       KeyPress data into regular text.  Weird but necessary...
                    */
                    if (text[0]=='q' || text[0]=='Q') {
                        close_x();
                    }
                    printf("You pressed the %c key!\n",text[0]);
                }

                // draw triangles
                for (int i = 0; i < line_count; i++) {
                    // 3 lines and points
                    XDrawLine(dis,win,gc, coordinates[i][0],coordinates[i][1], coordinates[i][2],coordinates[i][3]);
                    XDrawLine(dis,win,gc, coordinates[i][2],coordinates[i][3], coordinates[i][4],coordinates[i][5]);
                    XDrawLine(dis,win,gc, coordinates[i][4],coordinates[i][5], coordinates[i][0],coordinates[i][1]);
                }

                // Now get the two points
                if (event.type == ButtonPress) {

                    num_of_clicks++;

                    //tell where the mouse Button was Pressed
                    int x = event.xbutton.x;
                    int y = event.xbutton.y;

                    if (num_of_clicks == 1) {
                        strcpy(text,"Start");
                        start_target[0][0] = x;
                        start_target[0][1] = y;
                    } else if (num_of_clicks == 2) {
                        strcpy(text,"Target");
                        start_target[1][0] = x;
                        start_target[1][1] = y;
                        // now that we have everything.
                        // we make connections and check for valid edges
                        const int num_of_vertex = (line_count * 3) + 2;

                        // first index the matrix for easy access
                        int shrink_array[num_of_vertex][NUM_COLS];
                        int k = 0;

                        // add start at 0 and target at as last point

                        shrink_array[0][0] = start_target[0][0];
                        shrink_array[0][1] = start_target[0][1];
                        for (int i = 1; i < line_count * 3;) {
                            for (int j = 0; j < INIT_COLS; j += 2) {
                                shrink_array[i][0] = coordinates[k][j];
                                shrink_array[i][1] = coordinates[k][j + 1];
                                i++;
                            }
                            k++;
                        }
                        shrink_array[(line_count * 3) + 1][0] = start_target[1][0];
                        shrink_array[(line_count * 3) + 1][1] = start_target[1][1];

                        for (int i = 0; i < num_of_vertex; i++) {
                             for (int j = 0; j < 2; j++) {
                                    printf("shrink_array[%d][%d] = %d\n", i,j, shrink_array[i][j] );
                             }
                             printf("\n");
                         }

                        int p[2];
                        int q[2];

                        int r[2];
                        int s[2];

                        /*// create the graph
                        // pick two vertex and create an edge
                        for (int l = 0; l < num_of_vertex; l++) {
                            // pick two points
                            p[0] = shrink_array[l][0];
                            p[1] = shrink_array[l][1];
                            q[0] = shrink_array[l + 1][0];
                            q[1] = shrink_array[l + 1][1];

                            // now compare to all possible edges from point 0
                            for (int i = 0; i < num_of_vertex; i++) {
                                // create an edge
                                r

                            }
                        }*/

                    } else {
                        continue;
                    }
                    printf("You pressed a button at (%i,%i)\n", event.xbutton.x,event.xbutton.y);

                    XSetForeground(dis, gc, (unsigned long) (rand() % event.xbutton.x % 255));
                    XDrawPoint(dis, win, gc, x, y);
                    XDrawString(dis, win, gc, x, y, text, (int) strlen(text));
                }
            }

#pragma clang diagnostic pop

            /*-------------------------------------------------------------------------------------------*/
            // now we have everything yeyyyyyyy.



        } else {
            cout << "The argument supplied is " << argv[1] << endl;
            cout << "But its not .txt format\n";
            cout << "Please pass a valid file\n";
            return 1;
        }
    } else if (argc < 2) {
        cout << "No arguments were passes start manual process\n";
        // call the window methods

        /*----------------------------------------------------------------------------------------*/

        XEvent event;		/* the XEvent declaration !!! */
        KeySym key;		/* a dealie-bob to handle KeyPress Events */
        char text[255];		/* a char buffer for KeyPress Events */

        init_x();

        /* look for events forever... */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
        while(1) {
            /* get the next event and stuff it into our event variable.
               Note:  only events we set the mask for are detected!
            */
            XNextEvent(dis, &event);

            if (event.type==Expose && event.xexpose.count==0) {
                /* the window was exposed redraw it! */
                redraw();
            }
            if (event.type==KeyPress&&
                XLookupString(&event.xkey,text,255,&key,0)==1) {
                /* use the XLookupString routine to convert the invent
                   KeyPress data into regular text.  Weird but necessary...
                */
                if (text[0]=='q') {
                    close_x();
                }
                printf("You pressed the %c key!\n",text[0]);
            }
            if (event.type==ButtonPress) {
                /* tell where the mouse Button was Pressed */
                int x=event.xbutton.x,
                        y=event.xbutton.y;

                //strcpy(text,"X is FUN!");
                printf("You pressed a button at (%i,%i)\n",
                       event.xbutton.x,event.xbutton.y);

                XSetForeground(dis, gc, (unsigned long) (rand() % event.xbutton.x % 255));
                XDrawString(dis, win, gc, x, y, text, (int) strlen(text));
            }
        }
#pragma clang diagnostic pop

        /*-------------------------------------------------------------------------------------------*/


    } else {
        cout << "Too many arguments bro. Need just one file\n";
    }

    return 0;
}

void check_edge(const int coord_array[][2]) {

    /*Just the formula
        If (((det (p, q , r) * det(p q s) ) < 0)
            And
     ((Det (s r p) * det ( s r q))< 0)

        Then the edge pq intersects sr*/

    int pqr = 0, pqs = 0, srp = 0, srq = 0;

    int distpq = 0, distsr = 0;

    // p - 0, q - 1, r - 2, s - 3
    // x - 0, y - y


    /*pqr = det(coord_array[combination[0]][0],coord_array[combination[0]][1], // p
              coord_array[combination[1]][0],coord_array[combination[1]][1], // q
              coord_array[combination[2]][0],coord_array[combination[2]][1]); // r

    pqs = det(coord_array[combination[0]][0],coord_array[combination[0]][1],
              coord_array[combination[1]][0],coord_array[combination[1]][1],
              coord_array[combination[3]][0],coord_array[combination[3]][1]);

    srp = det(coord_array[combination[3]][0],coord_array[combination[3]][1],
              coord_array[combination[2]][0],coord_array[combination[2]][1],
              coord_array[combination[0]][0],coord_array[combination[0]][1]);

    srq = det(coord_array[combination[3]][0],coord_array[combination[3]][1],
              coord_array[combination[2]][0],coord_array[combination[2]][1],
              coord_array[combination[1]][0],coord_array[combination[1]][1]);*/

    if (((pqr * pqs) < 0) && ((srp * srq) < 0)) {
        cout << "Intersection\n";
    } else {
        cout << "Nope\n";
        // calc distance from p to q, r to s
        /*distpq = calc_dist(coord_array[combination[0]][0],coord_array[combination[0]][1],
                           coord_array[combination[1]][0],coord_array[combination[1]][1]);

        distsr = calc_dist(coord_array[combination[2]][0],coord_array[combination[2]][1],
                           coord_array[combination[3]][0],coord_array[combination[3]][1]);

        std::string s0 = std::to_string(combination[0]) + " " + std::to_string(combination[1]);
        std::string s1 = std::to_string(combination[2]) + " " + std::to_string(combination[3]);*/

        /*// add to the graph by index
        if (std::find(edges.begin(), edges.end(), s0) == edges.end()) {

            cout << combination[0] << " distpq: " << distpq << " " << combination[1] << "\n";
            //addEdge(graph, combination[0], distpq, combination[1] );

            edges.push_back(s0);
        }
        if (std::find(edges.begin(), edges.end(), s1) == edges.end()) {

            cout << combination[2] << " distsr: " << distsr << " " << combination[3] << "\n";
            //addEdge(graph, combination[2], distsr, combination[3] );

            edges.push_back(s1);
        }*/


    }
}

int calc_dist(const int x1, const int y1, const int x2, const int y2) {
    return (int) sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

int det(int px, int py, int qx, int qy, int rx, int ry) {
    return (px*qy)+(py*rx)+(qx*ry)-(qy*rx)-(px*ry)-(py*qx);
};






bool ends_with(const std::string &str, const std::string &suffix) {
    return str.size() >= suffix.size() &&
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

void split(const string &s, char delim, vector<string> &elems) {
    stringstream ss;
    ss.str(s);
    string item;
    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }
}

string parse_string(std::string str) {
    // erase the front of the string
    str.erase(0,3);

    // erase the ending
    str.erase(str.length() - 1, 1);

    std::replace(str.begin(), str.end(), ' ', '\0');
    std::replace(str.begin(), str.end(), '(', ',');
    std::replace(str.begin(), str.end(), ')', ',');

    return str;
}



void init_x() {
/* get the colors black and white (see section for details) */
    unsigned long black,white;

    dis=XOpenDisplay((char *)0);
    screen=DefaultScreen(dis);
    black=BlackPixel(dis,screen), white=WhitePixel(dis, screen);


    win=XCreateSimpleWindow(dis,DefaultRootWindow(dis),0,0,
                            400, 420, 5,black, white);


    XSetStandardProperties(dis,win,"Shortest Path","Hi",None,NULL,0,NULL);
    XSelectInput(dis, win, ExposureMask|ButtonPressMask|KeyPressMask);


    gc=XCreateGC(dis, win, 0,0);

    XSetBackground(dis,gc,white);
    XSetForeground(dis,gc,black);


    XClearWindow(dis, win);
    XMapRaised(dis, win);
};

void close_x() {
    XFreeGC(dis, gc);
    XDestroyWindow(dis,win);
    XCloseDisplay(dis);
    exit(1);
};

void redraw() {
    XClearWindow(dis, win);
}

