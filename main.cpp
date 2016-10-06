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

vector<string> split(const string &s, char delim) {
    vector<string> elems;
    split(s, delim, elems);
    return elems;
}

string parse_string(std::string);

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

/*
    stringstream ss;
*/

    if (argc == 2) {
        if (ends_with(argv[1], ".txt")) {
            // open the file
            ifstream my_file;

            int count = 0;

            my_file.open(argv[1]);

            //cout << argv[1] << "\n";

            if (my_file.is_open()) {

                cout << "File is open\n";

                while (getline(my_file, line)) {

                    line = parse_string(line);

                    vector1 = split(line, ',');

                    // assign

                    //cout << "Getting line: " << count << " from the file\n";
                    //cout << "Line =  " << line << "\n";

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



            cout << "\n" << argv[1] << " file was passes. yey\n" << endl;
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
                /*if (event.type==ButtonPress) {
                    *//* tell where the mouse Button was Pressed *//*
                    int x=event.xbutton.x,
                            y=event.xbutton.y;

                    //strcpy(text,"X is FUN!");
                    printf("You pressed a button at (%i,%i)\n",
                           event.xbutton.x,event.xbutton.y);

                    XSetForeground(dis, gc, (unsigned long) (rand() % event.xbutton.x % 255));
                    XDrawString(dis, win, gc, x, y, text, (int) strlen(text));
                }*/

                //cout << "Number of Triangles: " << count << "\n";

                for (int i = 0; i < count; i++) {
                    // 3 lines and points
/*
                    XDrawPoint(dis,win,gc,coordinates[i][0],coordinates[i][1]);
                    XDrawPoint(dis,win,gc,coordinates[i][2],coordinates[i][3]);
                    XDrawPoint(dis,win,gc,coordinates[i][4],coordinates[i][5]);
*/

                    /*cout << "Now drawing triangle: #"<< i << " " << coordinates[i][0] << " " << coordinates[i][1] << " - " << coordinates[i][2] << " " << coordinates[i][3] <<'\n';
                    cout << "Now drawing triangle: #"<< i << " " << coordinates[i][2] << " " << coordinates[i][3] << " - " << coordinates[i][4] << " " << coordinates[i][5] <<'\n';
                    cout << "Now drawing triangle: #"<< i << " " << coordinates[i][4] << " " << coordinates[i][5] << " - " << coordinates[i][0] << " " << coordinates[i][1] <<'\n';
*/

                    XDrawLine(dis,win,gc, coordinates[i][0],coordinates[i][1], coordinates[i][2],coordinates[i][3]);
                    XDrawLine(dis,win,gc, coordinates[i][2],coordinates[i][3], coordinates[i][4],coordinates[i][5]);
                    XDrawLine(dis,win,gc, coordinates[i][4],coordinates[i][5], coordinates[i][0],coordinates[i][1]);
                }



                // flatten the array

                /*int xy_points[count * 4][2];

                int index = 0;

                for (int j = 0; j < count; j++) {

                    xy_points[j][0] = (short) coordinates[j][index];
                    xy_points[j][1] = (short) coordinates[j][index + 1];

                }

                XPoint points[count];

                for (int i = 0; i < count; i++) {
                    points[i].x = (short) coordinates[i][0];
                    points[i].y = (short) coordinates[i][1];
                }*/

                /*XPoint points[] = {
                        {120, 120},
                        {15, 15},
                        {120, 15},
                        {120, 120}
                };*/

                /*int npoints = sizeof(points)/sizeof(XPoint);

                XDrawLines(dis, win, gc, points, npoints, CoordModeOrigin);*/


            }
#pragma clang diagnostic pop

            /*-------------------------------------------------------------------------------------------*/



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
                            500, 500, 5,black, white);


    XSetStandardProperties(dis,win,"Howdy","Hi",None,NULL,0,NULL);
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
};
