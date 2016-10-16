#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <math.h>
#include <unistd.h>

using namespace std;

bool ends_with(const std::string &, const std::string &);

void split(const string &, char, vector<string> &);

string parse_string(std::string);

int det(int, int, int, int, int, int);

void pair_generator(int, int, const int [][2]);

void check_edge(vector<int>,const int [][2]);

vector<string> split(const string &s, char delim) {
    vector<string> elems;
    split(s, delim, elems);
    return elems;
}

vector<int> points;
vector<int> pairs;

//int array[][2];

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
    int coordinates[number_of_lines][6];

    // start and target points
    int start_target[2][2];

/*
    stringstream ss;
*/

    if (argc == 2) {
        if (ends_with(argv[1], ".txt")) {
            // open the file
            ifstream my_file;

            int count = 0;

            my_file.open(argv[1]);

            if (my_file.is_open()) {

                while (getline(my_file, line)) {
                    // parse and split
                    line = parse_string(line);
                    vector1 = split(line, ',');

                    // assign
                    if (vector1[0] != "\n" ) {
                        coordinates[count][0] = atoi(vector1[0].c_str());
                        coordinates[count][1] = atoi(vector1[1].c_str());
                        coordinates[count][2] = atoi(vector1[3].c_str());
                        coordinates[count][3] = atoi(vector1[4].c_str());
                        coordinates[count][4] = atoi(vector1[6].c_str());
                        coordinates[count][5] = atoi(vector1[7].c_str());

                        count++;
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
                    if (text[0]=='q') {
                        close_x();
                    }
                    printf("You pressed the %c key!\n",text[0]);
                }

                // draw triangles
                for (int i = 0; i < count; i++) {
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
                        // generate pairs of 4 and test
                        // initialize the vector by number of triangles * 3
                        // because each triangle has 3 points.
                        for (int i = 0; i < count * 3; i++) {
                            points.push_back(i);
                        }
                        // call recursive pair generator
                        // 4 is the number of elements in a pair.
                        // first index the matrix for easy access

                        int rows = sizeof(coordinates) / sizeof(coordinates[0]);
                        int cols = sizeof(coordinates[0]) / sizeof(int);

                        int array[rows * 3][2];
                        int k = 0;

                        for (int i = 0; i < rows * 3;) {
                            for (int j = 0; j < cols; j += 2) {
                                array[i][0] = coordinates[k][j];
                                array[i][1] = coordinates[k][j + 1];
                                i++;
                            }
                            k++;
                        }

                        /*for (int i = 0; i < rows * 3; i++) {
                             for (int j = 0; j < 2; j++) {
                                    printf("array[%d][%d] = %d\n", i,j, array[i][j] );
                             }
                             printf("\n");
                         }*/

                        pair_generator(0, 4, array);


                    } else {
                        continue;
                    }
                    printf("You pressed a button at (%i,%i)\n", event.xbutton.x,event.xbutton.y);

                    XSetForeground(dis, gc, (unsigned long) (rand() % event.xbutton.x % 255));
                    XDrawPoint(dis, win, gc, x, y);
                    XDrawString(dis, win, gc, x, y, text, (int) strlen(text));
                }

                /*
                 *
                 *
                 *
                 *
                 *
                 * for (int i = 0; i < rows; i++) {
                    for (int j = 0; j < 3; j++) {
                        // at 0,0

                        for (int ini = i + 1; ini < rows - 1; ini++) {
                                // connects to 1,0 one to many
                                int edge_one = det(coordinates[i][0],coordinates[i][1],coordinates[ini][0],
                                                   coordinates[ini][1],coordinates[i][0],coordinates[i][1]);

                                int edge_two = det(coordinates[i][0],coordinates[i][1],coordinates[ini][0],
                                                   coordinates[ini][1],coordinates[j][0],coordinates[j][1]);

                                // p q intersect r s iff orient(pqr)

                        }
                    }
                }

                if (det(coordinates[ini][inj],coordinates[][],coordinates[][],
                        coordinates[][],coordinates[][],coordinates[][])) {

                }*/

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












void pair_generator(int offset, int k, const int coord[][2]) {
    if (k == 0) {
        check_edge(pairs, coord);
        return;
    }
    for (int i = offset; i <= points.size() - k; i++) {
        pairs.push_back(points[i]);
        // recurse
        pair_generator(i + 1, k - 1, coord);
        pairs.pop_back();
    }
}

void check_edge(vector<int> combination, const int array[][2]) {
    static int count = 0;
    cout << "combination no " << (count++) << ": [ ";
    for (int i = 0; i < combination.size(); i++) {
        cout << combination[i] << " (" << array[combination[i]][0] << "," << array[combination[i]][1] << ") ";
    }
    cout << "] " << endl;

    /*Just the formula
    If (((det (p, q , r) * det(p q s) ) < 0)
    And
     ((Det (s r p) * det ( s r q))< 0)
Then the edge pq intersects sr*/

    int pqr = 0, pqs = 0, srp = 0, srq = 0;

    // p - 0, q - 1, r - 2, s - 3
    // x - 0, y - y


    pqr = det(array[combination[0]][0],array[combination[0]][1], // p
              array[combination[1]][0],array[combination[1]][1], // q
              array[combination[2]][0],array[combination[2]][1]); // r

    pqs = det(array[combination[0]][0],array[combination[0]][1],
              array[combination[1]][0],array[combination[1]][1],
              array[combination[3]][0],array[combination[3]][1]);

    srp = det(array[combination[3]][0],array[combination[3]][1],
              array[combination[2]][0],array[combination[2]][1],
              array[combination[0]][0],array[combination[0]][1]);

    srq = det(array[combination[3]][0],array[combination[3]][1],
              array[combination[2]][0],array[combination[2]][1],
              array[combination[1]][0],array[combination[1]][1]);

    if (((pqr * pqs) < 0) && ((srp * srq) < 0)) {
        cout << "Intersection\n";
    } else {
        cout << "Nope\n";
    }


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


    /*std::size_t found = str.find_first_of(" ");

    while (found != std::string::npos) {
        str[found] = '\0';
        found = str.find_first_of(" ", found + 1);
    }*/

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

