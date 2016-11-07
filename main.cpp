#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <list>
#include <algorithm>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

using namespace std;

const int INIT_COLS = 6;
const int NUM_COLS = 2;

struct Point{
    int x;
    int y;

};

int orientation(struct Point p0,struct Point p1,struct Point p2)
{
    //calculate cross product of (p1-p0)X (p2-p0)

    int x1= p1.x- p0.x;
    int y1 = p1.y - p0.y;
    int x2= p2.x- p0.x;
    int y2 = p2.y - p0.y;

    int determinant = x1*y2 - x2*y1;

    if(determinant==0)
        return 0;

    if(determinant>0)
        return 1;

    if(determinant<0)
        return 2;

}

int onsegment(struct Point p0,struct Point p1,struct Point p2)
{
    if(p2.x >= min(p0.x,p1.x)&& p2.x <= max(p0.x,p1.x))
        return 1;

    return 0;
}

bool dointersect(struct Point p1,struct Point q1,struct Point p2,struct Point q2)
{
    int o1 = orientation(p1,q1,q2);
    int o2 = orientation(p1,q1,p2);
    int o3 = orientation(p2,q2,p1);
    int o4 = orientation(p2,q2,q1);

    /*if(o1!=o2&&o3!=o4)  //handles general cases
        return 1;

    if(o1==0&&o2==0&&o3==0&&o4==0)  //handles special cases when all four points are collinear
    {
        if(onsegment(p1,q1,p2)||onsegment(p1,q1,q2))
            return 1;

    }
    return 0;*/

    // General case
    if (o1 != o2 && o3 != o4)
        return true;

    // Special Cases
    // p1, q1 and p2 are colinear and p2 lies on segment p1q1
    if (o1 == 0 && onsegment(p1, p2, q1)) return true;

    //  p1, q1 and "q2" are colinear and q2 lies on segment p1q1
    if (o2 == 0 && onsegment(p1, q2, q1)) return true;

    // p2, q2 and p1 are colinear and p1 lies on segment p2q2
    if (o3 == 0 && onsegment(p2, p1, q2)) return true;

    // p2, q2 and q1 are colinear and q1 lies on segment p2q2
    if (o4 == 0 && onsegment(p2, q1, q2)) return true;

    return false; // Doesn't fall in any of the above cases

}

bool ends_with(const std::string &, const std::string &);

void split(const string &, char, vector<string> &);

string parse_string(std::string);

int det(int, int, int, int, int, int);

bool check_intersection(Point p1, Point q1, Point p2, Point q2);

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

                        // add start at 0 and target at as last Point

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

                        for (int index = 0; index < num_of_vertex; index++) {
                            for (int j = 0; j < 2; j++) {
                                printf("shrink_array[%d][%d] = %d\n", index,j, shrink_array[index][j] );
                            }
                            printf("\n");
                        }

                        int sEdget = 0;
                        int iEdgei1;

                        struct Point p1, q1, p2, q2;

                        // 1. check if direct edge exists between start and target
                        // connect the two and check with all other existing triangle edges
                        // this is the edge from start to target
                        p1 = {shrink_array[0][0], shrink_array[0][1]};
                        q1 = {shrink_array[num_of_vertex-1][0], shrink_array[num_of_vertex-1][1]};
                        for (int triangle = 1; triangle < (line_count * 3) + 1; triangle++) {
                            // 1 to 2 / 2 to 3 / 1 to 3
                            if ((triangle % 3) == 0) {
                                p2 = {shrink_array[triangle - 2][0], shrink_array[triangle - 2][1]};
                                q2 = {shrink_array[triangle][0], shrink_array[triangle][1]};
                            } else {
                                p2 = {shrink_array[triangle][0], shrink_array[triangle][1]};
                                q2 = {shrink_array[triangle + 1][0], shrink_array[triangle + 1][1]};
                            }
                            // now check
                            if (dointersect(p1, q1, p2, q2)) {
                                //intersection
                                sEdget++;
                            }
                        }
                        if (sEdget == 0) {
                            cout << "There exists a direct edge from start to target\n";
                        } else {
                            for (int l = 0; l < num_of_vertex; l++) {
                                iEdgei1 = 0;
                                // pick en edge l and l + 1

                                p1 = {shrink_array[l][0], shrink_array[l][1]};

                                for (int m = l + 1; m < num_of_vertex; m++) {
                                    q1 = {shrink_array[m][0], shrink_array[m][1]};

                                    for (int triangle = 1; triangle < (line_count * 3) + 1; triangle++) {
                                        // 1 to 2 / 2 to 3 / 1 to 3
                                        if ((triangle % 3) == 0) {
                                            p2 = {shrink_array[triangle - 2][0], shrink_array[triangle - 2][1]};
                                            q2 = {shrink_array[triangle][0], shrink_array[triangle][1]};
                                        } else {
                                            p2 = {shrink_array[triangle][0], shrink_array[triangle][1]};
                                            q2 = {shrink_array[triangle + 1][0], shrink_array[triangle + 1][1]};
                                        }
                                        // now check
                                        if (check_intersection(p1, q1, p2, q2)) {
                                            //cout << "Yes\n";
                                            //iEdgei1++;
                                            cout << "intersection " << l << " p1: (" << p1.x << ", " << p1.y << ") "
                                                    "q1: (" << q1.x << ", " << q1.y << ") and "
                                                         "p2: (" << p2.x << ", " << p2.y << ") "
                                                         "q2: (" << q2.x << ", " << q2.y << ") \n";
                                        } else {
                                            cout << "no intersection " << l << " p1: (" << p1.x << ", " << p1.y << ") "
                                                    "q1: (" << q1.x << ", " << q1.y << ") and "
                                                         "p2: (" << p2.x << ", " << p2.y << ") "
                                                         "q2: (" << q2.x << ", " << q2.y << ") \n";
                                        }
                                    }
                                }

                                /*if (iEdgei1 == 0) {
                                    cout << "edge\n";
                                } else if (iEdgei1 > 0) {
                                    cout << "no edge\n";
                                }*/
                            }
                        }

/*
                        // build the list of triangles
                        // skip first and the last points

                        std::vector <std::list<int >> triangles((unsigned long) (line_count));

                        int in_i = 1;

                        for (int l = 0; l < line_count; l++) {
                            triangles[l].push_back(in_i);
                            triangles[l].push_back(in_i + 1);
                            triangles[l].push_back(in_i + 2);
                            in_i += 3;
                        }

                        std::vector <std::list<int>>::iterator i;

                        int tri_num = 0;

                        for (i = triangles.begin(); i != triangles.end(); ++i) {
                            cout << "triangle[" << tri_num << "] = ";
                            std::list<int> li = *i;

                            for (std::list<int>::iterator iter = li.begin(); iter != li.end(); ++iter) {
                                cout << *iter << " ";
                            }
                            cout << endl;
                            tri_num++;
                        }*/


                        /*int p[2];
                        int q[2];

                        int r[2];
                        int s[2];*/

                        // create the graph
                        // pick two vertex and create an edge




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

bool check_intersection(struct Point p1, struct Point q1, struct Point p2, struct Point q2) {
    /*Determinant formula
      If (((det (p, q , r) * det(p q s) ) < 0)
      And ((Det (s r p) * det ( s r q))< 0)
      Then the edge pq intersects sr
      */

    int pqr = 0, pqs = 0, srp = 0, srq = 0;

//    int distpq = 0, distsr = 0;

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
        return True;
    } else {
        //cout << "Nope\n";
        return False;
    }
    // calc distance from p to q, r to s
    /*distpq = calc_dist(coord_array[combination[0]][0],coord_array[combination[0]][1],
                       coord_array[combination[1]][0],coord_array[combination[1]][1]);

    distsr = calc_dist(coord_array[combination[2]][0],coord_array[combination[2]][1],
                       coord_array[combination[3]][0],coord_array[combination[3]][1]);*/
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

