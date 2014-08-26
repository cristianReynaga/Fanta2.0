#ifndef _TEST_APP
#define _TEST_APP

#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxOsc.h"
#include "ofxXmlSettings.h"
#include "ofxProjectorBlend.h"


#include "Hearts.h"
#include "fft.h"
#include "FFTOctaveAnalyzer.h"

#define HOST "localhost"
#define PORT 12345
#define HOST2 "127.0.0.1"
#define PORT2 3100
#define BUFFER_SIZE 512

#define _USE_LIVE_VIDEO
#define _MAX_MOVIES 9
#define ESCENA_INICIAL 0
#define ESCENA_INTRO 1
#define ESCENA_CORAZONES 2
#define ESCENA_EXTRATERRESTRE 3
#define ESCENA_BRUJA 4
#define ESCENA_MURGA_INTRO 5
#define ESCENA_MURGA_DESARROLLO 6
#define ESCENA_MURGA_FINAL 7
#define ESCENA_FINAL 8

//projector blend
#define PROJECTOR_COUNT 3
#define PROJECTOR_WIDTH 1024
#define PROJECTOR_HEIGHT 768
#define PIXEL_OVERLAP 70


class testApp : public ofBaseApp
{

public:
    void setup();
    void update();
    void draw();

    void keyPressed  (int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

    void nextMovie();
    bool continueLoop();
    bool bStartInteraction;
    bool fullScreen;

    Hearts hearts;

    ofVideoPlayer				mMovies[_MAX_MOVIES], videoGrid;
    int							currentMovie;
    bool						continueStatus, bGrid, bDebug, bLed1, bLed2, bLed3;
    bool						pasoUmbral;

    float						umbralSonido;
    int							errorStatus, margenNegro;
    int                         threshold, toleranciaCorazones;
    string						ipClient;
    ofxOscSender				oscSender, oscSender2;

    ofSoundStream				soundStream;
    /* Audio Input */

    void audioReceived 	(float * input, int bufferSize, int nChannels);
    FFTOctaveAnalyzer FFTanalyzer;
    int freqFirstAnalysis;
    int nextMiniEvent;
    ofxXmlSettings xml;

    //Arduino
    ofArduino ard;
    bool bSetupArduino;

#ifdef _USE_LIVE_VIDEO
    ofVideoGrabber 		vidGrabber;
#else
    ofVideoPlayer 		vidPlayer;
#endif

    ofxCvColorImage			colorImg;
    ofxCvGrayscaleImage 	grayImage;
    ofxCvGrayscaleImage 	grayBg;
    ofxCvGrayscaleImage 	grayDiff;
    ofxCvContourFinder 	contourFinder;
    int minBlob, maxBlob, umbralBlobs, maxPixelsBlobs;


    bool				bLearnBakground;
    ofxOscSender sender;
    int widthScreen1, widthCapture, heightCapture;
    float audioLevel, fftLevel4;
    int audioLevel12, audioLevel34, test;

//projector blend
    ofxProjectorBlend blender;

private:

    float * left;
    float * right;
    int 	bufferCounter;
    fft		myfft;
    const char * bolaIP;

    float magnitude[BUFFER_SIZE];
    float phase[BUFFER_SIZE];
    float power[BUFFER_SIZE];
    float freq[BUFFER_SIZE/2];

    int 	drawCounter;
    int debugStatus;

    void setupArduino(const int & version);
    void updateArduino();

};

#endif
