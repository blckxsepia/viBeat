#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    w=320;
    h=240;
    threshold = 170;
    thresholdLive = 103;
    min=5;
    max=340*240/4;
    percentualCoverage=0.7;
    vidGrabber.setVerbose(true);
    vidGrabber.setup(w,h);
    colorImg.allocate(w,h);
    grayImage.allocate(w,h);
    grayBg.allocate(w,h);
    grayDiff.allocate(w,h);
}

//--------------------------------------------------------------
void ofApp::update(){
    ofBackground(100,100,100);
    vidGrabber.update();
    bool isFrameNew = vidGrabber.isFrameNew();
    if (isFrameNew){
        colorImg.setFromPixels(vidGrabber.getPixels());
        grayImage = colorImg;
        if (learnBackground == true){
            grayBg = grayImage;
            grayBg.invert();
            grayBg.threshold(threshold);
            findDrumpad();
            learnBackground = false;
        }
        grayDiff.threshold(140);
        grayDiff.absDiff(grayBg, grayImage);
        grayDiff.threshold(thresholdLive);
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetHexColor(0xffffff);
    colorImg.draw(20,20);
    grayImage.draw(360,20);
    grayBg.draw(20,280);
    grayDiff.draw(360,280);
    drawBlobs();
    learnAreas();
    drawReport();
    playSounds();
}

//--------------------------------------------------------------
void ofApp::findDrumpad(){
    contourFinder.findContours(grayBg, min, max, 12, false, true);
    for (int i = 0; i < contourFinder.nBlobs; i++){
        pad[i]=countWhitePixels(i);
        countWhitePixelForLivePad(i);
    }
    
}

//--------------------------------------------------------------
void ofApp::drawBlobs(){
    ofFill();
    ofSetHexColor(0x333333);
    ofDrawRectangle(360,540,w,h);
    ofSetColor(255);
    for (int i = 0; i < contourFinder.nBlobs; i++){
        contourFinder.blobs[i].draw(360,540);
        //show names of .wav files near to blob (press 'y' to show/hide)
        if(showInstrumentNames){
            string name = ofToString(instruments[i]);
            ofRectangle rect = contourFinder.blobs[i].boundingRect;
            ofDrawBitmapString(ofUTF8Substring(name, 0, name.length() - 4), rect.getCenter().x + 20, rect.getCenter().y + 20);
        }
    }
}

//--------------------------------------------------------------
void ofApp::countWhitePixelForLivePad(int i){
    livepad[i]=countWhitePixels(i);
}

//--------------------------------------------------------------
void ofApp::learnAreas(){
    for (int i = 0; i < contourFinder.nBlobs; i++){
        float x = contourFinder.blobs[i].boundingRect.x;
        float y = contourFinder.blobs[i].boundingRect.y;
        float w = contourFinder.blobs[i].boundingRect.width;
        float h = contourFinder.blobs[i].boundingRect.height;
        ofxCvColorImage grayImage_cont;
        cvSetImageROI(grayDiff.getCvImage(), cvRect(x,y,w,h)); //set the ROI
        grayImage_cont.allocate(w, h);
        grayImage_cont = grayDiff;
        grayImage_cont.draw(360+x,540+y);
        cvResetImageROI(grayDiff.getCvImage());
    }
}

//--------------------------------------------------------------
void ofApp::drawReport(){
    ofSetHexColor(0xffffff);
    stringstream reportStr;
    reportStr << "press space to capture bg" << endl
    << "threshold " << threshold << " (press: +/-)" << endl
    << "thresholdLive " << thresholdLive << " (press: o/l)" << endl
    << "min " << min << " (press: q/w)" << endl
    << "max " << max << " (press: e/r)" << endl
    << "percentualCoverage " << percentualCoverage << " (press: i/k)" << endl
    << "show Instrumentnames " << " (press: y)" << endl
    << "num blobs found " << contourFinder.nBlobs << endl;
    for (int i = 0; i < contourFinder.nBlobs ; i++){
        reportStr << ofToString(instruments[i]) << " " << soundPlayer[i].isPlaying()  << " " <<  padPlaying[i] << endl;
    }
    ofDrawBitmapString(reportStr.str(), 20, 550);
}


/*{
    string playing;
    string stopping;

    if(soundPlayer[i].isPlaying()==1){
               playing="is Playing";
           }  else{
               playing="";

           }
    if(padPlaying[i]==50){
        stopping="Loop will be stopped";
        else{
            stopping="";
        }
    }
    reportStr << ofToString(instruments[i]) << " " << playing << " " <<  stopping  << endl;
}*/

//--------------------------------------------------------------
void ofApp::playSounds(){
    for (int i = 0; i < contourFinder.nBlobs ; i++){
        if(soundPlayer[i].isPlaying()){
            // while the sound is still playing and your hand is on the blob/pad,
            if(livepad[i] < pad[i] * percentualCoverage){
                // increment padPlaying[i] till it is 50 (maybe adjust 50 a bit higher/lower)
                if(padPlaying[i] < 30){
                    padPlaying[i]++;
                }
                // when it reach 50 turn of looping
                else {
                    soundPlayer[i].setLoop(false);
                    // uncomment the line below if the sound should also stop
                    //soundPlayer[i].stop();
                }
            }
        } else {
            // current amount of white pixel < initial amount of white pixel * x%
            // e.g. 1000 * 70% = 700 => play sound when the current amount of white pixel are 70% of initial amount or less
            // (30% black pixel)
            if(livepad[i] < pad[i] * percentualCoverage){
                soundPlayer[i].load(ofToString(instruments[i]));
                soundPlayer[i].play();
                soundPlayer[i].setLoop(true);
                // set padPlaying[i] to 0 so you can stop the loop again (first if statement in this method)
                padPlaying[i]=0;
            }
        }
        // set the current amount of white pixel for blob[i]
        countWhitePixelForLivePad(i);
    }
}

//--------------------------------------------------------------
int ofApp::countWhitePixels(int i){
    float x = contourFinder.blobs[i].boundingRect.x;
    float y = contourFinder.blobs[i].boundingRect.y;
    float w = contourFinder.blobs[i].boundingRect.width;
    float h = contourFinder.blobs[i].boundingRect.height;
    return grayDiff.countNonZeroInRegion(x,y,w,h);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch (key){
        case ' ':
            learnBackground = true;
            break;
        case '+':
            threshold ++;
            if (threshold > 255) threshold = 255;
            break;
        case '-':
            threshold --;
            if (threshold < 0) threshold = 0;
            break;
        case 'o':
            thresholdLive ++;
            if (thresholdLive > 255) thresholdLive = 255;
            break;
        case 'l':
            thresholdLive --;
            if (thresholdLive < 0) thresholdLive = 0;
            break;
        case 'i':
            percentualCoverage =percentualCoverage+0.01;
            break;
        case 'k':
            percentualCoverage = percentualCoverage-0.01;
            if (percentualCoverage < 0) percentualCoverage = 0;
            break;
        case 'q':
            min --;
            if (min < 0) min = 0;
            break;
        case 'w':
            min ++;
            if (min > 10000) min = 10000;
            break;
        case 'e':
            max --;
            if (max < 0) max = 0;
            break;
        case 'r':
            max ++;
            if (max > 10000) max = 10000;
            break;
        case 's':
            for (int i = 0; i < 12; i++){
                soundPlayer[i].stop();
            }
            break;
        case 'y':
            if(showInstrumentNames){
                showInstrumentNames = false;
            } else {
                showInstrumentNames = true;
            }
            break;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
}
