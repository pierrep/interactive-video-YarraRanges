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
    bKinectFound = device.setup(0);
    if(bKinectFound) tracker.setup(device);

    /* VIDEO */
    /* Create resources for new player */
    player1.init(HPV::NewPlayer());
    player2.init(HPV::NewPlayer());

    /* Try to load file and start playback */
    if (player1.load("media0/idle4.hpv"))
    {
        player1.setLoopState(OF_LOOP_PALINDROME);
        player1.play();
    }

    /* Try to load file and start playback */
    if (player2.load("media0/head1.hpv"))
    //if (player2.load("media0/knees7.hpv"))
    {
        player2.setLoopState(OF_LOOP_NONE);
        player2.play();
        player2.setPaused(true);
        player2.setDoubleBuffered(false);
    }

    /* AUDIO */
    headAudio.load("media0/head1.wav");
    shouldersAudio.load("media0/shoulders1.wav");
    kneesAudio.load("media0/knees1.wav");
    toesAudio.load("media0/toes1.wav");

    fbo.allocate(1920,1080,GL_RGBA);

    bAction = false;
    bFadeIn = false;
    bFadeOut = false;

    bShowFps = false;
    bDrawDebug = false;

}

//--------------------------------------------------------------
void ofApp::update(){
    /* Update happens on global level for all active players by HPV Manager */
    HPV::Update();

    /* Kinect update */
    if(bKinectFound) device.update();
}

//--------------------------------------------------------------
void ofApp::checkInteraction()
{
    if(trackedId > -1) {

        if((tracker.getUserByID(trackedId)->isVisible()) && (tracker.getUserByID(trackedId)->head.x != 0) && (tracker.getUserByID(trackedId)->head.y != 0)) {
            ofVec2f p1,p2;
            leftHand = tracker.getUserByID(trackedId)->leftHand;
            leftKnee = tracker.getUserByID(trackedId)->leftKnee;
            leftFoot = tracker.getUserByID(trackedId)->leftFoot;

            //Head
            p1  = tracker.getUserByID(trackedId)->leftHand - tracker.getUserByID(trackedId)->head;
            p2  = tracker.getUserByID(trackedId)->rightHand - tracker.getUserByID(trackedId)->head;
            if( (p1.length() <= 30) || (p2.length() <= 30) ) {
                touched[0] = true;
                return;
            } else {
                if(touched[0] == true) {
                    touched[0] = false;
                    triggerAnimation(0);
                    return;
                }
            }

            //Shoulders
            p1  = tracker.getUserByID(trackedId)->leftHand -  tracker.getUserByID(trackedId)->leftShoulder;
            p2  = tracker.getUserByID(trackedId)->rightHand -  tracker.getUserByID(trackedId)->rightShoulder;
            if( (p1.length() <= 40) && (p2.length() <= 40) ) {
                touched[1] = true;
                return;
            } else {
                if(touched[1] == true) {
                    touched[1] = false;
                    triggerAnimation(1);
                    return;
                }
            }

            //Knees
            p1  = tracker.getUserByID(trackedId)->leftHand -  tracker.getUserByID(trackedId)->leftKnee;
            p2  = tracker.getUserByID(trackedId)->rightHand -  tracker.getUserByID(trackedId)->rightKnee;
            if( (p1.length() <= 50) && (p2.length() <= 50) ) {
                touched[2] = true;
                return;
            } else {
                if(touched[2] == true) {
                    touched[2] = false;
                    triggerAnimation(2);
                    return;
                }
            }

            //Toes
            p1  = tracker.getUserByID(trackedId)->leftHand - tracker.getUserByID(trackedId)->leftFoot;
            p2  = tracker.getUserByID(trackedId)->rightHand - tracker.getUserByID(trackedId)->rightFoot;

            if( (p1.length() <= 50) || (p2.length() <= 50) ) {
                touched[3] = true;
                return;
            } else {
                if(touched[3] == true) {
                    touched[3] = false;
                    triggerAnimation(3);
                    return;
                }
            }
        }
        else {
            for(int i = 0;i < 4;i++) {
                touched[i] = false;
            }
        }
    }

}

//--------------------------------------------------------------
void ofApp::draw(){

    ofBackground(0);

    if(bKinectFound && (tracker.getNumUser() >= 1)) {
        trackedId = -1;
        for(int i =0; i < tracker.getNumUser();i++)
        {
            int state = tracker.getUser(i)->get().getSkeleton().getState();
            if(state == nite::SKELETON_TRACKED) {
                trackedId = tracker.getUser(i)->getId();
                break;
            }
        }

        checkInteraction();

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

    if(bShowFps) ofDrawBitmapString(ofToString(ofGetFrameRate()),20,ofGetHeight() - 20);
    if(bShowFps) ofDrawBitmapString("bAction: "+ofToString(bAction),100,ofGetHeight() - 20);
    if(bShowFps) ofDrawBitmapString("leftHand: "+ofToString(leftHand),20,ofGetHeight() - 40);
    if(bShowFps) ofDrawBitmapString("leftKnee: "+ofToString(leftKnee),320,ofGetHeight() - 40);
    if(bShowFps) ofDrawBitmapString("leftFoot: "+ofToString(leftFoot),620,ofGetHeight() - 40);

}

//--------------------------------------------------------------
void ofApp::exit()
{
    /* Clean up and destroy HPV Engine upon exit */
    HPV::DestroyHPVEngine();

    /* Clean up after Kinect */
    if(bKinectFound) tracker.exit();
    if(bKinectFound) device.exit();
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
    else if (key == '7')
    {
        triggerAnimation(0);
    }
    else if (key == '8')
    {
        triggerAnimation(1);
    }
    else if (key == '9')
    {
        triggerAnimation(2);
    }
    else if (key == '0')
    {
        triggerAnimation(3);
    }
    else if (key == ' ')
    {
        bPauseState = !bPauseState;
        player1.setPaused(bPauseState);
    }
    else if (key == '1') {
        readyAnimation("media0/head1.hpv");
    }
    else if (key == '2') {
        readyAnimation("media0/shoulders3.hpv");
    }
    else if (key == '3') {
        readyAnimation("media0/knees7.hpv");
    }
    else if (key == '4') {
        readyAnimation("media0/toes1.hpv");
    }
}

//--------------------------------------------------------------
void ofApp::triggerAnimation(unsigned int type)
{
    if(bAction) return;

    ofLogNotice() << "trigger animation" << type;
    if(type == 0) {
        readyAnimation("media0/head1.hpv");
        headAudio.play();
    } else if(type == 1) {
        readyAnimation("media0/shoulders3.hpv");
        shouldersAudio.play();
    } else if(type == 2) {
        readyAnimation("media0/knees7.hpv");
        kneesAudio.play();
    } else if(type == 3) {
        readyAnimation("media0/toes1.hpv");
        toesAudio.play();
    }

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
