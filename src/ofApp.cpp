#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetVerticalSync(true);
    ofHideCursor();
    ofSetLogLevel(OF_LOG_VERBOSE);

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

    bAction = false;
}

//--------------------------------------------------------------
void ofApp::update(){
    /* Update happens on global level for all active players by HPV Manager */
    HPV::Update();
}

//--------------------------------------------------------------
void ofApp::draw(){

    ofBackground(0);

    player1.draw(0,0,ofGetWidth(), ofGetHeight());

    if(bAction)
    player2.draw(0,0,ofGetWidth(), ofGetHeight());
}

//--------------------------------------------------------------
void ofApp::exit()
{
    /* Cleanup and destroy HPV Engine upon exit */
    HPV::DestroyHPVEngine();
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

            if(event.player->getFilename() != player1.getFilename()) bAction = false;
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
    player2.setPaused(true);
    bAction = false;
}
