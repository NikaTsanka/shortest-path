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

typedef vector<vector<pair<int,float> > > Graph;

class Comparator
{
public:
    int operator() ( const pair<int,float>& p1, const pair<int,float> &p2)
    {
        return p1.second>p2.second;
    }
};

void dijkstra(const Graph  &G,const int &source,const int &destination,vector<int> &path)
{
    vector<float> d(G.size());
    vector<int> parent(G.size());
    for(unsigned int i = 0 ;i < G.size(); i++)
    {
        d[i] = std::numeric_limits<float>::max();
        parent[i] = -1;
    }
    priority_queue<pair<int,float>, vector<pair<int,float> >, Comparator> Q;
    d[source] = 0.0f;
    Q.push(make_pair(source,d[source]));
    while(!Q.empty())
    {
        int u = Q.top().first;
        if(u==destination) break;
        Q.pop();
        for(unsigned int i=0; i < G[u].size(); i++)
        {
            int v= G[u][i].first;
            float w = G[u][i].second;
            if(d[v] > d[u]+w)
            {
                d[v] = d[u]+w;
                parent[v] = u;
                Q.push(make_pair(v,d[v]));
            }
        }
    }
    path.clear();
    int p = destination;
    path.push_back(destination);
    while(p!=source)
    {
        p = parent[p];
        path.push_back(p);
    }
}

const int INIT_COLS = 6;
const int NUM_COLS = 2;
struct Point{
    int x;
    int y;
};

bool ends_with(const std::string &, const std::string &);

void split(const string &, char, vector<string> &);

string parse_string(std::string);

double det(int, int, int, int, int, int);

bool check_intersection(Point, Point, Point, Point);

int calc_dist(const int, const int, const int, const int);

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

void drawing_board(int[][NUM_COLS], int, bool manual);

void compute(XEvent &event, int vertices[][NUM_COLS], int line_count);

bool check_point(int x, int y, vector<pair<int, int> >& pVector);

bool check_triangle(int x, int y, int x1, int y1, int x2, int y2, int x3, int y3);

float area(int x1, int y1, int x2, int y2, int x3, int y3);

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
            /*for (int index = 0; index < num_of_vertex; index++) {
                for (int j = 0; j < 2; j++) {
                    printf("shrink_array[%d][%d] = %d\n", index,j, shrink_array[index][j] );
                }
                printf("\n");
            }*/
            cout << "\'" << argv[1] << "\' file accepted\n" << endl;
            split_vector.clear();
            input_file.close();

            // start the x-server with the read coordinates
            drawing_board(shrink_array, num_of_vertex, false);

        } else {
            cout << "Couldn't open the specified file\n";
            return 0;
        }
    } else if (argc < 2) {
        cout << "No valid file arguments was passes. Starting manual process\n";
        // call the window methods
        drawing_board(0, 0, true);
    } else {
        cout << "Too many arguments bro. Need just one file\n";
    }
    return 0;
}

void drawing_board(int vertices[][NUM_COLS], int num_of_vertex, bool manual) {
    /*for (int index = 0; index < num_of_vertex; index++) {
        for (int j = 0; j < 2; j++) {
            printf("shrink_array[%d][%d] = %d\n", index,j, shrink_array[index][j] );
        }
        printf("\n");
    }*/
    /*----------------------------------------------------------------------------------------*/
    XEvent event;		/* the XEvent declaration !!! */
    KeySym key;		/* a dealie-bob to handle KeyPress Events */
    char text[255];		/* a char buffer for KeyPress Events */
    init_x();
    int x, y, start_x = 0, start_y = 0, index_j = 0, index = 0, num_of_clicks = 0;
    bool right_click = false;
    vector<pair<int, int> > coordinates;
    /* look for events forever... */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    while(1) {
        /* get the next event and stuff it into our event variable.
           Note:  only events we set the mask for are detected!
        */
        XNextEvent(dis, &event);
        switch (event.type) {
            case Expose:
                if (event.type==Expose && event.xexpose.count==0) {
                    /* the window was exposed redraw it! */
                    redraw();
                    if (!manual) {
                        //const int num_of_vertex = (line_count * 3);
                        // draw triangles
                        for (int i = 1; i < num_of_vertex - 2; i+=3) {
                            // 3 lines and points
                            //cout << i << " \n";
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
                //tell where the mouse Button was Pressed
                x = event.xbutton.x;
                y = event.xbutton.y;

                if (num_of_vertex == 0) {
                    if (event.xbutton.button == Button1 && !right_click) {

                        coordinates.resize((unsigned long) index++);
                        //cout << "Left click\n";
                        //printf("\nLeft click at (%i,%i)\n", x, y);
                        index -=1;
                        coordinates.push_back(make_pair(x, y));
                        //cout << coordinates[index].first << " " << coordinates[index].second << endl;
                        index++;
                        //cout << index << " " << coordinates.size() << endl;
                        if (index % 3 == 0) {
                            //cout << "hi";

                            XDrawLine(dis,win,gc, coordinates[index_j].first,coordinates[index_j].second,
                                      coordinates[index_j + 1].first,coordinates[index_j + 1].second);
                            XDrawLine(dis,win,gc, coordinates[index_j + 1].first,coordinates[index_j + 1].second,
                                      coordinates[index_j + 2].first,coordinates[index_j + 2].second);
                            XDrawLine(dis,win,gc, coordinates[index_j].first,coordinates[index_j].second,
                                      coordinates[index_j + 2].first,coordinates[index_j + 2].second);
                            index_j += 3;
                        }
                    } else if (event.xbutton.button == Button3 ) {
                        //cout << "Right click\n";
                        right_click = true;
                    } else if (event.xbutton.button == Button1 && right_click) {
                        // Now get the two points
                        num_of_clicks++;
                        int tmp_resize = (int) coordinates.size();

                        if (num_of_clicks == 1) {
                            //printf("\nStart Left click at (%i,%i)\n", x, y);

                            //cout << tmp_resize << " before, ";
                            // discard if not divisible by 3 and resize
                            while (tmp_resize % 3 != 0) {
                                tmp_resize -= 1;
                            }
                            coordinates.resize((unsigned long) tmp_resize);

                            //cout << "after " << tmp_resize << ".\n";






                            // check if start point is in any of the triangles.
                            // if yes then try to get it again.

                            if (!check_point(x, y, coordinates)) {
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
                            //printf("\nTarget Left click at (%i,%i)\n", x, y);

                            //cout << tmp_resize << " before, ";
                            // discard if not divisible by 3 and resize
                            while (tmp_resize % 3 != 0) {
                                tmp_resize -= 1;
                            }
                            coordinates.resize((unsigned long) tmp_resize);

                            //cout << "after " << tmp_resize << ".\n";
                            //cout << "numofclicks " << num_of_clicks << ".\n";

                            if (!check_point(x, y, coordinates)) {
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
                                //cout << "line count = " << coordinates.size()<< endl;

                                XSetForeground(dis, gc, (unsigned long) (rand() % event.xbutton.x % 255));
                                XDrawPoint(dis, win, gc, x, y);
                                XDrawString(dis, win, gc, x, y, text, (int) strlen(text));

                                compute(event, manual_vertices, (int) coordinates.size());
                            } else {
                                //cout << "target inside\n";
                                // 1 because the start is not in any triangle and it's accepted.
                                num_of_clicks = 1;
                            }
                        }
                    }
                } else {
                    // get the start and target and set
                    num_of_clicks++;
                    if (num_of_clicks == 1) {
                        printf("\nStart Left click at (%i,%i)\n", x, y);
                        strcpy(text,"Start");
                        start_x = x;
                        start_y = y;
                        XSetForeground(dis, gc, (unsigned long) (rand() % event.xbutton.x % 255));
                        XDrawPoint(dis, win, gc, x, y);
                        XDrawString(dis, win, gc, x, y, text, (int) strlen(text));
                    } else if (num_of_clicks == 2) {
                        printf("\nTarget Left click at (%i,%i)\n", x, y);
                        strcpy(text,"Target");
                        vertices[0][0] = start_x;
                        vertices[0][1] = start_y;
                        vertices[num_of_vertex - 1][0] = x;
                        vertices[num_of_vertex - 1][1] = y;
                        /*for (int index_k = 0; index_k < num_of_vertex; index_k++) {
                            for (int j = 0; j < 2; j++) {
                                printf("vertices[%d][%d] = %d\n", index_k,j, vertices[index_k][j] );
                            }
                            printf("\n");
                        }*/
                        XSetForeground(dis, gc, (unsigned long) (rand() % event.xbutton.x % 255));
                        XDrawPoint(dis, win, gc, x, y);
                        XDrawString(dis, win, gc, x, y, text, (int) strlen(text));

                        compute(event, vertices, num_of_vertex);
                    }
                    //compute(event, vertices, line_count);
                }
                //printf("\nYou pressed a button at (%i,%i)\n", event.xbutton.x,event.xbutton.y);
                break;
            default:break;
        }
    }

#pragma clang diagnostic pop

/*-------------------------------------------------------------------------------------------*/

}

bool check_point(int x, int y, vector<pair<int, int> >& pVector) {

    /*cout << "pVector size: " << pVector.size() << endl;
    for (int i = 0; i < pVector.size(); i += 3) {
        cout << "[" << pVector[i].first << "," << pVector[i].second << "] -- [" <<
             pVector[i + 1].first  << "," << pVector[i + 1].second << "] -- [" <<
             pVector[i + 2].first << "," << pVector[i + 2].second << "]\n";
    }*/

    for (int i = 0; i < pVector.size(); i += 3) {
        // print
        /*cout << "inside: " << i << "th triangle\n";
        cout << pVector[i].first << "," << pVector[i].second << " -- " <<
             pVector[i + 1].first  << "," << pVector[i + 1].second << " -- " <<
             pVector[i + 2].first << "," << pVector[i + 2].second << "\n";*/
        // get 3 points
        if (check_triangle(x, y,
                           pVector[i].first, pVector[i].second,
                           pVector[i + 1].first, pVector[i + 1].second,
                           pVector[i + 2].first, pVector[i + 2].second)) {
            //cout << "inside this triangle: " << i << "\n";
            // if true then it lies in a triangle.
            return true;
        }
    }
    // if it doesn't lie in any triangle then false.
    return false;
}

bool check_triangle(int x, int y, int x1, int y1, int x2, int y2, int x3, int y3) {
    // x1 = a, x2 = b, x3 = c, p = x

    /* Calculate area of triangle ABC */
    float A = area (x1, y1, x2, y2, x3, y3);

    /* Calculate area of triangle PBC */
    float A1 = area (x, y, x2, y2, x3, y3);

    /* Calculate area of triangle PAC */
    float A2 = area (x1, y1, x, y, x3, y3);

    /* Calculate area of triangle PAB */
    float A3 = area (x1, y1, x2, y2, x, y);

    /* Check if sum of A1, A2 and A3 is same as A */
    return (A == A1 + A2 + A3);
}

/* A utility function to calculate area of triangle formed by (x1, y1),
   (x2, y2) and (x3, y3) */
float area(int x1, int y1, int x2, int y2, int x3, int y3)
{
    return abs((x1*(y2-y3) + x2*(y3-y1)+ x3*(y1-y2))/2.0);
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
    p1 = {vertices[0][0], vertices[0][1]};
    q1 = {vertices[num_of_vertex-1][0], vertices[num_of_vertex-1][1]};
    for (int triangle = 1; triangle < num_of_vertex - 1; triangle++) { // (line_count * 3) + 1
        // 1 to 2 / 2 to 3 / 1 to 3
        if ((triangle % 3) == 0) {
            p2 = {vertices[triangle - 2][0], vertices[triangle - 2][1]};
            q2 = {vertices[triangle][0], vertices[triangle][1]};
        } else {
            p2 = {vertices[triangle][0], vertices[triangle][1]};
            q2 = {vertices[triangle + 1][0], vertices[triangle + 1][1]};
        }
        // now check
        if (check_intersection(p1, q1, p2, q2)) {
            //intersection
            edge_start_to_target++;
            /*cout << "intersection " << triangle << " p1: (" << p1.x << ", " << p1.y << ") "
                                "q1: (" << q1.x << ", " << q1.y << ") and "
                                     "p2: (" << p2.x << ", " << p2.y << ") "
                                     "q2: (" << q2.x << ", " << q2.y << ") \n";*/
        }
    }
    if (edge_start_to_target == 0) {
        cout << "There exists a direct edge from start to target\n";
        XDrawLine(dis,win,gc, vertices[0][0],vertices[0][1],vertices[num_of_vertex-1][0],vertices[num_of_vertex-1][1]);
    } else {
        for (int l = 0; l < num_of_vertex; l++) {
            // pick en edge l and l + 1
            p1 = {vertices[l][0], vertices[l][1]};
            for (int m = l + 1; m < num_of_vertex; m++) {
                edge_i_to_i_plus_one = 0;
                q1 = {vertices[m][0], vertices[m][1]};
                for (int triangle = 1; triangle < num_of_vertex - 1; triangle++) {
                    // 1 to 2 / 2 to 3 / 1 to 3
                    if ((triangle % 3) == 0) {
                        p2 = {vertices[triangle - 2][0], vertices[triangle - 2][1]};
                        q2 = {vertices[triangle][0], vertices[triangle][1]};
                    } else {
                        p2 = {vertices[triangle][0], vertices[triangle][1]};
                        q2 = {vertices[triangle + 1][0], vertices[triangle + 1][1]};
                    }
                    // now check
                    if (check_intersection(p1, q1, p2, q2)) {
                        //cout << "Yes\n";
                        edge_i_to_i_plus_one++;
                        /*cout << "intersection " << l << " p1: (" << p1.x << ", " << p1.y << ") "
                                "q1: (" << q1.x << ", " << q1.y << ") and "
                                     "p2: (" << p2.x << ", " << p2.y << ") "
                                     "q2: (" << q2.x << ", " << q2.y << ") \n";*/
                    } else {
                        /*cout << "no intersection " << l << " p1: (" << p1.x << ", " << p1.y << ") "
                                "q1: (" << q1.x << ", " << q1.y << ") and "
                                     "p2: (" << p2.x << ", " << p2.y << ") "
                                     "q2: (" << q2.x << ", " << q2.y << ") \n";*/
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
        dijkstra(g,0,num_of_vertex-1,path);
        //int index[path.size() - 1];
        for(int i = (int) (path.size() - 1); i >= 0; i--) {
            //cout<< path[i] << "->" << path[i - 1];
            if (i == 0) {
                continue;
            }
            //cout << "\ni: " << i << " i - 1: " << i - 1 << endl;
            XDrawLine(dis,win,gc, vertices[path[i]][0],vertices[path[i]][1],vertices[path[i - 1]][0],vertices[path[i - 1]][1]);
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