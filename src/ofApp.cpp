#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetVerticalSync(true);
    ofHideCursor();
    ofSetLogLevel(OF_LOG_NOTICE);
    ofBackground(255);

    /* Init HPV Engine */
    HPV::InitHPVEngine();
    HPV::AddEventListener(this, &ofApp::onHPVEvent);


    /* Create resources for new player */
    player1.init(HPV::NewPlayer());
    player2.init(HPV::NewPlayer());

    /* Try to load file and start playback */
    if (player1.load("idle4.hpv"))
    {
        player1.setLoopState(OF_LOOP_PALINDROME);
        player1.play();
    }

    /* Try to load file and start playback */
    if (player2.load("head1.hpv"))
    //if (player2.load("knees7.hpv"))
    {
        player2.setLoopState(OF_LOOP_NONE);
        player2.play();
        player2.setPaused(true);
        player2.setDoubleBuffered(false);
    }

    fbo.allocate(1920,1080,GL_RGBA);

    bAction = false;
    bFadeIn = false;
    bFadeOut = false;

}

//--------------------------------------------------------------
void ofApp::update(){
    /* Update happens on global level for all active players by HPV Manager */
    HPV::Update();
}

//--------------------------------------------------------------
void ofApp::draw(){

    ofBackground(0);

    fbo.begin();
    ofClear(0,0,0,255);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if((bAction) && (bFadeIn)) {
        curTime = ofGetElapsedTimeMillis();
        fade = ((curTime - prevTime)/1000.0f)/fadeTime;
        if(fade > 1.0f) {
            bFadeIn = false;
            fade = 1.0f;
        }

        //cout << "Fade in: " << fade << endl;

        ofSetColor(255,255,255,255.0f);
        player2.draw(0,0,ofGetWidth(), ofGetHeight());

        ofSetColor(255,255,255,(1.0f-fade)*255.0f);
        player1.draw(0,0,ofGetWidth(), ofGetHeight());

    } else if((!bAction) && (bFadeOut)) {
        curTime = ofGetElapsedTimeMillis();
        fade = ((curTime - prevTime)/1000.0f)/fadeTime;
        if(fade > 1.0f) {
            bFadeOut = false;
            fade = 1.0f;
        }

       // cout << "Fade out: " << fade << endl;

        ofSetColor(255,255,255,255.0f);
        player1.draw(0,0,ofGetWidth(), ofGetHeight());

        ofSetColor(255,255,255,(1.0f-fade)*255.0f);
        player2.draw(0,0,ofGetWidth(), ofGetHeight());

    } else if(bAction) {
        player2.draw(0,0,ofGetWidth(), ofGetHeight());
    } else {
        player1.draw(0,0,ofGetWidth(), ofGetHeight());
    }

    fbo.end();

    ofSetColor(255);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    fbo.draw(0,0,1920,1080);

    ofEnableAlphaBlending();
}

//--------------------------------------------------------------
void ofApp::exit()
{
    /* Cleanup and destroy HPV Engine upon exit */
   // HPV::DestroyHPVEngine();
}

//--------------------------------------------------------------
void ofApp::onHPVEvent(const HPVEvent& event)
{
    switch (event.type)
    {
        case HPV::HPVEventType::HPV_EVENT_PLAY:
            ofLogNotice() << "'" << event.player->getFilename() << "': play event";
            break;
        case HPV::HPVEventType::HPV_EVENT_PAUSE:
            ofLogNotice() << "'" << event.player->getFilename() << "': pause event";
            break;
        case HPV::HPVEventType::HPV_EVENT_RESUME:
            ofLogNotice() << "'" << event.player->getFilename() << "': resume event";
            break;
        case HPV::HPVEventType::HPV_EVENT_STOP:
            ofLogNotice() << "'" << event.player->getFilename() << "': stop event";
            break;
        case HPV::HPVEventType::HPV_EVENT_LOOP:
            ofLogNotice() << "'" << event.player->getFilename() << "': loop event";

            if(event.player->getFilename() != player1.getFilename()) {
                ofLogNotice() << "Do fade out for '"  << event.player->getFilename() << "'";
                bAction = false;
                bFadeOut = true;
                prevTime = curTime = ofGetElapsedTimeMillis();
                fade = 0.0f;
            }
            break;
        case HPV::HPVEventType::HPV_EVENT_NUM_TYPES:
        default:
            break;
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
    if (key == 'f')
    {
        ofToggleFullscreen();
    }
    else if (key == 'p')
    {
        player2.play();
        bAction = true;
        bFadeIn = true;
        prevTime = curTime = ofGetElapsedTimeMillis();
        fade = 0.0f;
    }
    else if (key == ' ')
    {
        bPauseState = !bPauseState;
        player1.setPaused(bPauseState);
    }
    else if (key == '1') {
        readyAnimation("head1.hpv");
    }
    else if (key == '2') {
        readyAnimation("knees7.hpv");
    }
    else if (key == '3') {
        readyAnimation("shoulders3.hpv");
    }
    else if (key == '4') {
        readyAnimation("toes1.hpv");
    }
}

//--------------------------------------------------------------
void ofApp::readyAnimation(string filename)
{
    player2.close();
    player2.load(filename);
    player2.setLoopState(OF_LOOP_NONE);
    bAction = false;
}
