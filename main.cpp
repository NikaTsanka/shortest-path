#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

using namespace std;

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

