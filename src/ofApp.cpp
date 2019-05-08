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

    device.setLogLevel(OF_LOG_WARNING);
    device.setup(0);
    tracker.setup(device);

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

    bShowFps = true;
    bDrawDebug = true;

}

//--------------------------------------------------------------
void ofApp::update(){
    /* Update happens on global level for all active players by HPV Manager */
    HPV::Update();

    /* Kinect update */
    device.update();
}

//--------------------------------------------------------------
void ofApp::draw(){

    ofBackground(0);

    if(tracker.getNumUser() >= 1) {
        if(tracker.getUser(0)->isVisible()) {
            ofVec2f p1  = tracker.getUser(0)->leftHand -  tracker.getUser(0)->leftShoulder;
            ofVec2f p2  = tracker.getUser(0)->rightHand -  tracker.getUser(0)->rightShoulder;

            if( (p1.length() <= 50) && (p2.length() <= 50) ) {
                triggerAnimation();
            }

        }
    }

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

    if(bDrawDebug) {
        depthPixels = tracker.getPixelsRef(1000, 4000);
        depthTexture.loadData(depthPixels);

        // draw in 2D
        ofSetColor(255);
        depthTexture.draw(0, 0);
        tracker.draw();
    }

    if(bShowFps) ofDrawBitmapString(ofToString(ofGetFrameRate()),20,20);
}

//--------------------------------------------------------------
void ofApp::exit()
{
    /* Clean up and destroy HPV Engine upon exit */
    HPV::DestroyHPVEngine();

    /* Clean up after Kinect */
    tracker.exit();
    device.exit();
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
    if (key == 'F')
    {
        ofToggleFullscreen();
    }
    else if (key == 'f') {
        bShowFps = !bShowFps;
    }
    else if (key == 'd') {
        bDrawDebug = !bDrawDebug;
    }
    else if (key == 'p')
    {
        triggerAnimation();
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
void ofApp::triggerAnimation()
{
    player2.play();
    bAction = true;
    bFadeIn = true;
    prevTime = curTime = ofGetElapsedTimeMillis();
    fade = 0.0f;
}

//--------------------------------------------------------------
void ofApp::readyAnimation(string filename)
{
    player2.close();
    player2.load(filename);
    player2.setLoopState(OF_LOOP_NONE);
    bAction = false;
}
