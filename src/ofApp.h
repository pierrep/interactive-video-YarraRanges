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
        void triggerAnimation(unsigned int type);
        void onHPVEvent(const HPVEvent& event);

        ofxHPVPlayer player1;
        ofxHPVPlayer player2;

        ofSoundPlayer headAudio;
        ofSoundPlayer shouldersAudio;
        ofSoundPlayer kneesAudio;
        ofSoundPlayer toesAudio;

        bool bPauseState;
        bool bAction;
        bool bFadeIn;
        bool bFadeOut;
        float fade;
        float curTime;
        float prevTime;
        const float fadeTime = 0.25f;

        bool touched[4]; // 0 = head, 1 = shoulders, 2 = knees, 3 = toes

        ofFbo fbo;
        bool bShowFps;
        bool bDrawDebug;
        bool bKinectFound;

        /* Kinect */
        ofxNI2::Device device;
        ofxNiTE2::UserTracker tracker;
        ofPixels depthPixels;
        ofTexture depthTexture;
};
