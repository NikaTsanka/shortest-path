#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <list>
#include <algorithm>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <queue>
#include <limits>
#include <cmath>

using namespace std;

// Graph is represented as adjacency List
typedef vector<vector<pair<int,float> > > Graph;

class Comparator {
public:
    int operator() ( const pair<int, float>& p1, const pair<int, float> &p2) {
        return p1.second > p2.second;
    }
};

void dijkstra(const Graph &, const int &, const int &, vector<int> &);

const int INIT_COLS = 6;
const int NUM_COLS = 2;
struct Point{
    int x;
    int y;
};

bool ends_with(const std::string &, const std::string &);

void split(const string &, char, vector<string> &);

string parse_string(std::string);

vector<string> split(const string &s, char delim) {
    vector<string> elems;
    split(s, delim, elems);
    return elems;
}
// here are our X variables
Display *dis;

int screen;

Window win;
GC gc;
// here are our X routines declared
void init_x(int bound);

void close_x();
void redraw();

bool check_intersection(Point, Point, Point, Point);

int calc_dist(const int, const int, const int, const int);

double det(int, int, int, int, int, int);

void drawing_board(int[][NUM_COLS], int, bool, int);

void compute(XEvent &event, int vertices[][NUM_COLS], int line_count);

bool check_point_vector(int x, int y, vector<pair<int, int> > &pVector);

bool check_triangle(int x, int y, int x1, int y1, int x2, int y2, int x3, int y3);

float area(int x1, int y1, int x2, int y2, int x3, int y3);

bool check_point_array(int x, int y, int pInt[][NUM_COLS], int array_size);

int main(int argc, char *argv[]) {
    if (argc == 2 && ends_with(argv[1], ".txt")) {
        // vector for splitting lines
        vector<string> split_vector;
        // count lines and initialize
        string lines;
        ifstream input_file(argv[1]);
        int number_of_lines = 0;
        while (getline(input_file, lines)) {
            ++number_of_lines;
        }
        input_file.close();
        // coordinate container
        int coordinates[number_of_lines][INIT_COLS];

        int line_count = 0;
        input_file.open(argv[1]);
        if (input_file.is_open()) {
            while (getline(input_file, lines)) {
                // parse and split
                lines = parse_string(lines);
                split_vector = split(lines, ',');
                // assign
                if (split_vector[0] != "\n" ) {
                    coordinates[line_count][0] = atoi(split_vector[0].c_str());
                    coordinates[line_count][1] = atoi(split_vector[1].c_str());
                    coordinates[line_count][2] = atoi(split_vector[3].c_str());
                    coordinates[line_count][3] = atoi(split_vector[4].c_str());
                    coordinates[line_count][4] = atoi(split_vector[6].c_str());
                    coordinates[line_count][5] = atoi(split_vector[7].c_str());
                    line_count++;
                }
            }
            const int num_of_vertex = (line_count * 3) + 2;
            // first index the matrix for easy access
            int shrink_array[num_of_vertex][NUM_COLS];
            int k = 0;
            // leave out first and the last elements
            for (int i = 1; i < num_of_vertex - 2;) {
                for (int j = 0; j < INIT_COLS; j += 2) {
                    shrink_array[i][0] = coordinates[k][j];
                    shrink_array[i][1] = coordinates[k][j + 1];
                    i++;
                }
                k++;
            }
            // get the max array element and add 10 %
            // skip the first element and the last
            int max = shrink_array[1][0];
            for (int l = 1; l < num_of_vertex - 1; ++l) {
                for (int i = 0; i < NUM_COLS; ++i) {
                    if (max < shrink_array[l][i]) {
                        max = shrink_array[l][i];
                    }
                }
            }
            // add 10 %. get the ceiling. doesn't really matter.
            max = (int) ceil((max + (max * 0.1)));
            cout << "\'" << argv[1] << "\' file accepted\n" << endl;
            split_vector.clear();
            input_file.close();
            // start the x-server with the read coordinates
            drawing_board(shrink_array, num_of_vertex, false, max);
        } else {
            cout << "Couldn't open the specified file\n";
            return 0;
        }
    } else if (argc < 2) {
        cout << "No file arguments was passes. Starting manual process\n";
        // call the window methods
        // default boundary = 800 by 800. doesn't really matter.
        drawing_board(0, 0, true, 0);
    } else {
        cout << "Something went wrong. Please check your arguments.\n";
    }
    return 0;
}

void drawing_board(int vertices[][NUM_COLS], int num_of_vertex, bool manual, int boundary) {
    /*----------------------------------------------------------------------------------------*/
    int x, y, start_x = 0, start_y = 0, index_j = 0, index = 0, num_of_clicks = 0;
    bool right_click = false;
    vector<pair<int, int> > coordinates;
    // the XEvent declaration
    XEvent event;
    // to handle KeyPress Events
    KeySym key;
    // for KeyPress Events
    char text[255];

    if (boundary > 0) {
        init_x(boundary);
    } else {
        init_x(800);
    }
    // look for events forever
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    cout << "You can use \'q\' or \'Q\' to terminate the program at any time. :)\n";
    while(1) {
        /* get the next event and stuff it into our event variable.
           Note:  only events we set the mask for are detected!
        */
        XNextEvent(dis, &event);
        switch (event.type) {
            case Expose:
                if (event.type == Expose && event.xexpose.count == 0) {
                    // the window was exposed redraw it
                    redraw();
                    if (!manual) {
                        // draw triangles
                        for (int i = 1; i < num_of_vertex - 2; i+=3) {
                            // 3 lines and points
                            XDrawLine(dis,win,gc, vertices[i][0],vertices[i][1],
                                      vertices[i + 1][0],vertices[i + 1][1]);
                            XDrawLine(dis,win,gc, vertices[i + 1][0],vertices[i + 1][1],
                                      vertices[i + 2][0],vertices[i + 2][1]);
                            XDrawLine(dis,win,gc, vertices[i][0],vertices[i][1],
                                      vertices[i +  2][0],vertices[i + 2][1]);
                        }
                    }
                }
                break;
            case KeyPress:
                if (XLookupString(&event.xkey,text,255,&key,0)==1) {
                    /* use the XLookupString routine to convert the invent
                       KeyPress data into regular text.  Weird but necessary...
                    */
                    if (text[0]=='q' || text[0]=='Q') {
                        close_x();
                    }
                    printf("You pressed the %c key!\n",text[0]);
                }
            case ButtonPress:
                // tell where the mouse Button was Pressed
                x = event.xbutton.x;
                y = event.xbutton.y;

                if (num_of_vertex == 0) {
                    if (event.xbutton.button == Button1 && !right_click) {
                        coordinates.resize((unsigned long) index++);
                        index -=1;
                        coordinates.push_back(make_pair(x, y));
                        index++;

                        if (index % 3 == 0) {
                            XDrawLine(dis,win,gc, coordinates[index_j].first,coordinates[index_j].second,
                                      coordinates[index_j + 1].first,coordinates[index_j + 1].second);
                            XDrawLine(dis,win,gc, coordinates[index_j + 1].first,coordinates[index_j + 1].second,
                                      coordinates[index_j + 2].first,coordinates[index_j + 2].second);
                            XDrawLine(dis,win,gc, coordinates[index_j].first,coordinates[index_j].second,
                                      coordinates[index_j + 2].first,coordinates[index_j + 2].second);
                            index_j += 3;
                        }
                    } else if (event.xbutton.button == Button3 ) {
                        right_click = true;
                    } else if (event.xbutton.button == Button1 && right_click) {
                        // Now get the two points
                        num_of_clicks++;
                        int tmp_resize = (int) coordinates.size();

                        if (num_of_clicks == 1) {
                            // discard if not divisible by 3 and resize
                            while (tmp_resize % 3 != 0) {
                                tmp_resize -= 1;
                            }
                            coordinates.resize((unsigned long) tmp_resize);
                            // check if start point is in any of the triangles.
                            // if yes then try to get it again.
                            if (!check_point_vector(x, y, coordinates)) {
                                //cout << "start outside\n";
                                strcpy(text,"Start");
                                start_x = x;
                                start_y = y;
                                XSetForeground(dis, gc, (unsigned long) (rand() % event.xbutton.x % 255));
                                XDrawPoint(dis, win, gc, x, y);
                                XDrawString(dis, win, gc, x, y, text, (int) strlen(text));

                            } else {
                                //cout << "start inside\n";
                                num_of_clicks = 0;
                            }
                        } else if (num_of_clicks == 2) {
                            // discard if not divisible by 3 and resize
                            while (tmp_resize % 3 != 0) {
                                tmp_resize -= 1;
                            }
                            coordinates.resize((unsigned long) tmp_resize);

                            if (!check_point_vector(x, y, coordinates)) {
                                //cout << "target outside\n";
                                strcpy(text, "Target");
                                // add 2 more.
                                coordinates.resize((unsigned long) tmp_resize + 2);

                                int manual_vertices[coordinates.size()][NUM_COLS];
                                manual_vertices[0][0] = start_x;
                                manual_vertices[0][1] = start_y;
                                for (int i = 0; i < coordinates.size(); i++) {
                                    manual_vertices[i + 1][0] = coordinates[i].first;
                                    manual_vertices[i + 1][1] = coordinates[i].second;
                                }
                                manual_vertices[coordinates.size() - 1][0] = x;
                                manual_vertices[coordinates.size() - 1][1] = y;
                                XSetForeground(dis, gc, (unsigned long) (rand() % event.xbutton.x % 255));
                                XDrawPoint(dis, win, gc, x, y);
                                XDrawString(dis, win, gc, x, y, text, (int) strlen(text));
                                // let's go.
                                compute(event, manual_vertices, (int) coordinates.size());
                            } else {
                                //cout << "target inside\n";
                                // 1 because the start is not in any triangle and it's accepted.
                                num_of_clicks = 1;
                            }
                        } else {
                            num_of_clicks = 0;
                        }
                    }
                } else {
                    // get the start and target and set
                    num_of_clicks++;
                    if (num_of_clicks == 1) {
                        if (!check_point_array(x, y, vertices, num_of_vertex)) {
                            //cout << "start outside\n";
                            strcpy(text,"Start");
                            start_x = x;
                            start_y = y;
                            XSetForeground(dis, gc, (unsigned long) (rand() % event.xbutton.x % 255));
                            XDrawPoint(dis, win, gc, x, y);
                            XDrawString(dis, win, gc, x, y, text, (int) strlen(text));
                        } else {
                            num_of_clicks = 0;
                        }
                    } else if (num_of_clicks == 2) {
                        if (!check_point_array(x, y, vertices, num_of_vertex)) {
                            strcpy(text,"Target");
                            vertices[0][0] = start_x;
                            vertices[0][1] = start_y;
                            vertices[num_of_vertex - 1][0] = x;
                            vertices[num_of_vertex - 1][1] = y;
                            XSetForeground(dis, gc, (unsigned long) (rand() % event.xbutton.x % 255));
                            XDrawPoint(dis, win, gc, x, y);
                            XDrawString(dis, win, gc, x, y, text, (int) strlen(text));
                            // go
                            compute(event, vertices, num_of_vertex);
                        } else {
                            //cout << "target inside\n";
                            // 1 because the start is not in any triangle and it's accepted.
                            num_of_clicks = 1;
                        }
                    }
                }
                break;
            default:break;
        }
    }
#pragma clang diagnostic pop
/*-------------------------------------------------------------------------------------------*/
}

void dijkstra(const Graph  &my_graph, const int &start, const int &target, vector<int> &path) {
    // total number of vertices my_graph.size()
    vector<float> d(my_graph.size());
    vector<int> parent(my_graph.size());

    // fill with Maximum value for int.
    for(int i = 0 ; i < my_graph.size(); i++) {
        d[i] = numeric_limits<float>::max();
        parent[i] = -1;
    }

    // priority queue on d
    priority_queue<pair<int, float>, vector<pair<int, float> >, Comparator> priority_queue1;

    d[start] = 0.0f;
    priority_queue1.push(make_pair(start, d[start]));

    while(!priority_queue1.empty()) {
        int u = priority_queue1.top().first;

        if(u == target) {
            break;
        }
        priority_queue1.pop();

        for(int i = 0; i < my_graph[u].size(); i++) {
            int v = my_graph[u][i].first;
            float w = my_graph[u][i].second;
            if(d[v] > d[u] + w) {
                d[v] = d[u] + w;
                parent[v] = u;
                priority_queue1.push(make_pair(v, d[v]));
            }
        }
    }

    path.clear();
    int p = target;
    path.push_back(target);

    while(p != start) {
        p = parent[p];
        path.push_back(p);
    }
}

bool check_point_array(int x, int y, int pInt[][NUM_COLS], int array_size) {
    for (int i = 1; i < array_size - 2; i += 3) {
        if (check_triangle(x, y, pInt[i][0],pInt[i][1],
                           pInt[i + 1][0],pInt[i + 1][1],
                           pInt[i + 2][0],pInt[i + 2][1])) {
            return true;
        }
    }
    return false;
}

bool check_point_vector(int x, int y, vector<pair<int, int> > &pVector) {
    for (int i = 0; i < pVector.size(); i += 3) {
        // get 3 points
        if (check_triangle(x, y,
                           pVector[i].first, pVector[i].second,
                           pVector[i + 1].first, pVector[i + 1].second,
                           pVector[i + 2].first, pVector[i + 2].second)) {
            // if true then it lies in a triangle.
            return true;
        }
    }
    // if it doesn't lie in any triangle then false.
    return false;
}

bool check_triangle(int x, int y, int x1, int y1, int x2, int y2, int x3, int y3) {
    // x1 = A, x2 = B, x3 = C, P = x
    // triangle ABC
    float A = area(x1, y1, x2, y2, x3, y3);
    // triangle PBC
    float A1 = area(x, y, x2, y2, x3, y3);
    // triangle PAC
    float A2 = area(x1, y1, x, y, x3, y3);
    // triangle PAB
    float A3 = area(x1, y1, x2, y2, x, y);

    // if sum of A1, A2 and A3 is same as A then inside.
    return (A == A1 + A2 + A3);
}

float area(int x1, int y1, int x2, int y2, int x3, int y3) {
    return (float) abs((x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2)) / 2.0);
}

void compute(XEvent &event, int vertices[][NUM_COLS], int line_count) {
    // start and target points
    // now that we have everything.
    // we make connections and check for valid edges
    const int num_of_vertex = line_count;
    int edge_start_to_target = 0;
    int edge_i_to_i_plus_one;
    int distpq = 0;
    struct Point p1, q1, p2, q2;

    Graph g;
    g.resize((unsigned long) (num_of_vertex));

    // 1. check if direct edge exists between start and target
    // connect the two and check with all other existing triangle edges
    // this is the edge from start to target
    p1.x = vertices[0][0], p1.y = vertices[0][1];
    q1.x = vertices[num_of_vertex-1][0], q1.y = vertices[num_of_vertex-1][1];
    for (int triangle = 1; triangle < num_of_vertex - 1; triangle++) { // (line_count * 3) + 1
        // 1 to 2 / 2 to 3 / 1 to 3
        if ((triangle % 3) == 0) {
            p2.x = vertices[triangle - 2][0], p2.y = vertices[triangle - 2][1];
            q2.x = vertices[triangle][0], q2.y = vertices[triangle][1];
        } else {
            p2.x = vertices[triangle][0], p2.y = vertices[triangle][1];
            q2.x = vertices[triangle + 1][0], q2.y = vertices[triangle + 1][1];
        }
        // now check
        if (check_intersection(p1, q1, p2, q2)) {
            // intersection
            edge_start_to_target++;
        }
    }
    if (edge_start_to_target == 0) {
        //cout << "There exists a direct edge from start to target\n";
        XDrawLine(dis,win,gc, vertices[0][0],vertices[0][1],vertices[num_of_vertex-1][0],vertices[num_of_vertex-1][1]);
    } else {
        for (int l = 0; l < num_of_vertex; l++) {
            // pick en edge l and l + 1
            p1.x = vertices[l][0], p1.y = vertices[l][1];
            for (int m = l + 1; m < num_of_vertex; m++) {
                edge_i_to_i_plus_one = 0;
                q1.x = vertices[m][0], q1.y = vertices[m][1];
                for (int triangle = 1; triangle < num_of_vertex - 1; triangle++) {
                    // 1 to 2 / 2 to 3 / 1 to 3
                    if ((triangle % 3) == 0) {
                        p2.x = vertices[triangle - 2][0], p2.y = vertices[triangle - 2][1];
                        q2.x = vertices[triangle][0], q2.y = vertices[triangle][1];
                    } else {
                        p2.x = vertices[triangle][0], p2.y = vertices[triangle][1];
                        q2.x = vertices[triangle + 1][0], q2.y = vertices[triangle + 1][1];
                    }
                    // now check
                    if (check_intersection(p1, q1, p2, q2)) {
                        //cout << "Yes\n";
                        edge_i_to_i_plus_one++;
                    }
                }
                if (edge_i_to_i_plus_one == 0) {
                    // calc distance from p to q
                    distpq = calc_dist(p1.x, p1.y, q1.x, q1.y);
                    //cout << "edge from " << l << " to " << m << ". distance = " << distpq << endl;
                    g[l].push_back(make_pair(m, distpq));
                    g[m].push_back(make_pair(l, distpq));
                } else if (edge_i_to_i_plus_one > 0) {
                    //cout << "no edge from " << l << " to " << m << endl;
                }
            }
            //cout << "\n";
        }
        vector<int> path;
        dijkstra(g, 0, num_of_vertex-1, path);
        for(int i = (int) (path.size() - 1); i >= 0; i--) {
            if (i == 0) {
                continue;
            }
            XDrawLine(dis,win,gc, vertices[path[i]][0],vertices[path[i]][1],
                      vertices[path[i - 1]][0],vertices[path[i - 1]][1]);
        }
    }
}

bool check_intersection(Point p1, Point q1, Point p2, Point q2) {
    /*Determinant formula
      If (((det (p, q , r) * det(p q s) ) < 0)
      And ((Det (s r p) * det ( s r q))< 0)
      Then the edge pq intersects sr
     */
    double pqr = 0, pqs = 0, srp = 0, srq = 0;
    // p1 = p, q1 = q, r = p2, s = q2

    pqr = det(p1.x, p1.y, // p
              q1.x, q1.y, // q
              p2.x, p2.y); // r

    pqs = det(p1.x, p1.y, // p
              q1.x, q1.y, // q
              q2.x, q2.y); // s

    srp = det(q2.x, q2.y, //s
              p2.x, p2.y, //r
              p1.x, p1.y); // p

    srq = det(q2.x, q2.y, //s
              p2.x, p2.y, //r
              q1.x, q1.y); //q

    if (((pqr * pqs) < 0) && ((srp * srq) < 0)) {
        //cout << "Intersection\n";
        //cout << pqr << " * " << pqs << " " << srp << " * " << srq << endl;
        return true;
    } else {
        //cout << "Nope\n";
        //cout << pqr << " * " << pqs << " " << srp << " * " << srq << endl;
        return false;
    }
}

int calc_dist(const int x1, const int y1, const int x2, const int y2) {
    return (int) sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

double det(int px, int py, int qx, int qy, int rx, int ry) {
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

void init_x(int bound) {
// get the colors black and white (see section for details)
    unsigned long black,white;

    dis=XOpenDisplay((char *)0);
    screen=DefaultScreen(dis);
    black=BlackPixel(dis,screen), white=WhitePixel(dis, screen);

    win=XCreateSimpleWindow(dis, DefaultRootWindow(dis), 0, 0,
                            (unsigned int) bound, (unsigned int) bound, 5, black, white);

    XSetStandardProperties(dis,win,"Shortest Path - Dijkstra's Algorithm",NULL,None,NULL,0,NULL);
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