#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"

#define _USE_LIVE_VIDEO        // uncomment this to use a live camera
// otherwise, we'll use a movie file

class ofApp : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    void findDrumpad();
    void drawBlobs();
    void drawReport();
    void learnAreas();
    int countWhitePixels(int i);
    void playSounds();
    void countWhitePixelForLivePad(int i);

    ofVideoGrabber          vidGrabber;
    ofxCvColorImage         colorImg;
    ofxCvGrayscaleImage     grayImage;
    ofxCvGrayscaleImage     grayBg;
    ofxCvGrayscaleImage     grayDiff;
    ofxCvGrayscaleImage     grayDiffROI;
    ofxCvContourFinder      contourFinder;
    ofSoundPlayer           soundPlayer[12];

    //Settings variables
    int   threshold;
    int   thresholdLive;
    float percentualCoverage;
    int   min;
    int   max;
    int   w;
    int   h;
    bool  showInstrumentNames = false;

    // press space to set learnBackground to true
    bool  learnBackground = true;

    // count variable for white pixels
    int   pad [12];
    // count runtime-variable for white pixels
    int   livepad [12];
    // variable for stopping loop when they reach 50
    int   padPlaying [12] = {
        0,0,0,0,0,0,0,0,0,0,0,0
    };
    //
    char  instruments [12][30] = {
        "bass.wav",
        "chords.wav",
        "drum.wav",
        "melody.wav",
        "bass.wav",
        "chords.wav",
        "drum.wav",
        "melody.wav",
        "bass.wav",
        "chords.wav",
        "drum.wav",
        "melody.wav",
    };
    
};

