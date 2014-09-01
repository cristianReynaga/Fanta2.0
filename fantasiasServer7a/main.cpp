#include "ofMain.h"
#include "testApp.h"
#include "ofAppGlutWindow.h"

//========================================================================
int main( ){

    ofAppGlutWindow window;
    ofSetupOpenGL(&window, 1366+2048,768, OF_WINDOW); //OF_GAME_MODE);			// <-------- setup the GL context
	ofSetWindowPosition(255,0);
	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp( new testApp());

}
