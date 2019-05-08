#pragma once

#include "ofMain.h"
#include "ofxHPVPlayer.h"
#include "ofxNI2.h"
#include "ofxNiTE2.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
        void exit();

		void keyPressed(int key);
        void readyAnimation(string filename);
        void triggerAnimation();
        void onHPVEvent(const HPVEvent& event);

        ofxHPVPlayer player1;
        ofxHPVPlayer player2;

        bool bPauseState;
        bool bAction;
        bool bFadeIn;
        bool bFadeOut;
        float fade;
        float curTime;
        float prevTime;
        const float fadeTime = 0.25f;

        ofFbo fbo;
        bool bShowFps;
        bool bDrawDebug;

        /* Kinect */
        ofxNI2::Device device;
        ofxNiTE2::UserTracker tracker;
        ofPixels depthPixels;
        ofTexture depthTexture;
};
