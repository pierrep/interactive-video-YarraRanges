#pragma once

#include "ofMain.h"
#include "ofxHPVPlayer.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
        void exit();

		void keyPressed(int key);
        void readyAnimation(string filename);
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
        const float fadeTime = 1.0f;

        ofFbo fbo;
};
