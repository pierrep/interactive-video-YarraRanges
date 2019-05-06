#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){

    ofGLWindowSettings settings;
    settings.setSize(1440,900);
    settings.windowMode = OF_FULLSCREEN;
    settings.setGLVersion(4, 1);

    ofCreateWindow(settings);

	ofRunApp(new ofApp());

}
