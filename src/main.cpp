#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
    
    ofGLFWWindowSettings settings;
    settings.resizable = false;
    settings.setSize(600, 400);
    ofCreateWindow(settings);
    return ofRunApp(new ofApp);
    
	//ofSetupOpenGL(800,400,OF_WINDOW);			// <-------- setup the GL context

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	//ofRunApp(new ofApp());

}
