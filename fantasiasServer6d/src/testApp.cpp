
#include "testApp.h"
#include "ofxSimpleGuiToo.h"

//--------------------------------------------------------------
void testApp::setup()
{
    //Panel de control
    gui.addTitle("Control Panel");
    gui.addToggle("No interaction", bStartInteraction);
    gui.addSlider("Current Movie", currentMovie, 0, _MAX_MOVIES);
    gui.addSlider("Volume", audioLevel, 0.0f, 1.0f);
    gui.addSlider("Volume12", audioLevel12, 0, 127);
    gui.addSlider("Volume34", audioLevel34, 0, 127);
    gui.addSlider("FFT 4", fftLevel4, 0.0, 50.0);
    gui.addSlider("Threshold Sonido", umbralSonido, 0, 50);
    gui.addToggle("Led 1", bLed1);
    gui.addToggle("Led 2", bLed2);
    gui.addToggle("Led 3", bLed3);

    gui.addTitle("Video 1").setNewColumn(true);
    gui.addContent("Live", colorImg);

    gui.addTitle("Video 2").setNewColumn(true);
    gui.addContent("Background", grayBg);
    gui.addButton("Click Capture Background", bLearnBakground);

    gui.addTitle("Video 3").setNewColumn(true);
    gui.addContent("Difference", grayDiff);
    gui.addSlider("Threshold Blobs", threshold, 0, 255);
    gui.addSlider("Min size Blobs", minBlob, 0, 500);
    gui.addSlider("Max size Blobs", maxBlob, 0, 800);

    gui.addTitle("Video blend");
	gui.addToggle("Show Blend", blender.showBlend);
	 for (int i=0; i<PROJECTOR_COUNT-2; i++)
    {
        gui.addSlider("Blend Power "+ofToString(i+1), blender.blendPower[i], 0.0, 4.0);
        gui.addSlider("Gamma "+ofToString(i+1), blender.gamma[i], 0.1, 4.0);
        gui.addSlider("Luminance "+ofToString(i+1), blender.luminance[i], 0.0, 1.0);
    }

    gui.loadFromXML();
    gui.show();

    bGrid= false;
    margenNegro=100;
    toleranciaCorazones=20;
    widthScreen1=1024;
    widthCapture=200;
    heightCapture=150;
    //test=0;
    // audioLevel=1.0f;

    //projector blend
     blender.setup(PROJECTOR_WIDTH, PROJECTOR_HEIGHT, PROJECTOR_COUNT, PIXEL_OVERLAP);
	blender.setWindowToDisplaySize();


    //OpenCv
#ifdef _USE_LIVE_VIDEO
    vidGrabber.setVerbose(true);
    vidGrabber.initGrabber(320,240);
#endif

    colorImg.allocate(320 ,240);
    grayImage.allocate(320, 240);
    grayBg.allocate(320,240);
    grayDiff.allocate(320,240);

    bLearnBakground = true;
    minBlob=30;
    maxBlob=105;
    umbralBlobs=20;

    soundStream.listDevices();
    soundStream.setDeviceID(1);
    soundStream.setup(this, 0,2, 44100, BUFFER_SIZE, 4);
    left = new float[BUFFER_SIZE];
    right = new float[BUFFER_SIZE];

    //Variables de Captura de sonido
    FFTanalyzer.setup(44100, BUFFER_SIZE/2, 2);
    FFTanalyzer.peakHoldTime = 5; // hold longer
    FFTanalyzer.peakDecayRate = 0.95f; // decay slower
    FFTanalyzer.linearEQIntercept = 0.9f; // reduced gain at lowest frequency
    FFTanalyzer.linearEQSlope = 0.01f; // increasing gain at higher frequencies
	freqFirstAnalysis=4;

    //Variables de OSC
    //ipClient = xml.getValue("sloadFile", "192.168.0.123");
    oscSender.setup(HOST, PORT);
    oscSender2.setup(HOST2, PORT2);

    //Variables de Arduino
    ard.connect("COM3", 57600);
    ofAddListener(ard.EInitialized, this, &testApp::setupArduino);
    bSetupArduino = false;
    bDebug= false;

    hearts.setup();

    nextMiniEvent=0;
    ofSetHexColor(0x666666);

    videoGrid.loadMovie("videos/videoGrid.mov");
    try
    {
        mMovies[0].loadMovie( "videos/a(loop).mov");
        mMovies[1].loadMovie( "videos/b.mov" );
        mMovies[2].loadMovie( "videos/c(loop).mov" );
        mMovies[3].loadMovie( "videos/d.mov" );
        mMovies[4].loadMovie( "videos/e(loop).mov" );
        mMovies[5].loadMovie( "videos/f.mov" );
        mMovies[6].loadMovie( "videos/g(loop).mov" );
        mMovies[7].loadMovie( "videos/h.mov" );
        mMovies[8].loadMovie( "videos/i.mov" );

        for (int i = 0; i < 9; i++)
        {
            mMovies[i].setLoopState(OF_LOOP_NONE);
        }
    }
    catch( ... )
    {
        exit();
    }
    videoGrid.play();

    currentMovie = -1;
    continueStatus = true;
    nextMovie();
    mMovies[currentMovie].play();

    errorStatus = 0;
    // umbralSonido = 2;
    ofEnableAlphaBlending();
    pasoUmbral = false;

    cout << "xml ip: " << ipClient << std::endl;



}

//--------------------------------------------------------------
void testApp::update()
{
    ofBackground(100,100,100);
    bool bNewFrame = false;

//Pide a cámara nuevos frames para mostrar
#ifdef _USE_LIVE_VIDEO
    vidGrabber.update();
    bNewFrame = vidGrabber.isFrameNew();
#else
    vidPlayer.idleMovie();
    bNewFrame = vidPlayer.isFrameNew();
#endif

    if (bNewFrame)
    {

#ifdef _USE_LIVE_VIDEO
        colorImg.setFromPixels(vidGrabber.getPixels(), 320,240);
#else
        colorImg.setFromPixels(vidPlayer.getPixels(), 320,240);
#endif

        grayImage = colorImg;
        if (bLearnBakground == true)
        {
            grayBg = grayImage;		// the = sign copys the pixels from grayImage into grayBg (operator overloading)
            bLearnBakground = false;
        }

        // take the abs value of the difference between background and incoming and then threshold:
        grayDiff.absDiff(grayBg, grayImage);
        grayDiff.threshold(threshold);

        // find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
        // also, find holes is set to true so we will get interior contours as well....
        //contourFinder.findContours(grayDiff, 20, (340*240)/3, 10, true);	// find holes
        contourFinder.findContours(grayDiff, minBlob, maxBlob, 10, true);
    }
    updateArduino();

    //Update de captura de sonido...
    fftLevel4=FFTanalyzer.averages[freqFirstAnalysis];
    static int index=0;
    float avg_power = 0.0f;

    myfft.powerSpectrum(0,(int)BUFFER_SIZE/2, left,BUFFER_SIZE,&magnitude[0],&phase[0],&power[0],&avg_power);
    FFTanalyzer.calculate(freq);
    for (int i = 0; i < (int)(BUFFER_SIZE/2); i++)
    {
        freq[i] = magnitude[i];
    }

    // Cálculo de umbralSonido...-
    if (FFTanalyzer.averages[freqFirstAnalysis] + 1.0 > umbralSonido)
    {
        pasoUmbral = true;
    }
    else
    {
        pasoUmbral = false;
    }

    //
    videoGrid.update();
    mMovies[currentMovie].setVolume(audioLevel);
    mMovies[currentMovie].update();

    if( mMovies[currentMovie].getCurrentFrame() > mMovies[currentMovie].getTotalNumFrames() - 5 )
    {
        // aquí es donde hacemos el siguiente video...
        nextMovie();
    }

    if (currentMovie == ESCENA_CORAZONES && ofGetElapsedTimeMillis()> nextMiniEvent && contourFinder.nBlobs > 4) // > nextMiniEvent && FFTanalyzer.averages[5] > umbralSonido) {
    {
        hearts.addHeart();
        nextMiniEvent = ofGetElapsedTimeMillis() + 500;
    }
    if(currentMovie>0)
    {
        bStartInteraction=false;
    }

    // OSC
    ofxOscMessage v;
    v.setAddress("/currentMovie");
    v.addIntArg(currentMovie);
    oscSender.sendMessage(v);

    ofxOscMessage a;
    a.setAddress("/audioLevel12");
    a.addIntArg(audioLevel12);
    oscSender2.sendMessage(a);

    ofxOscMessage b;
    b.setAddress("/audioLevel34");
    b.addIntArg(audioLevel34);
    oscSender2.sendMessage(b);
}

//--------------------------------------------------------------
void testApp::setupArduino(const int & version)
{
    // remove listener because we don't need it anymore
    ofRemoveListener(ard.EInitialized, this, &testApp::setupArduino);

    // it is now safe to send commands to the Arduino
    bSetupArduino = true;

    // print firmware name and version to the console
    cout << ard.getFirmwareName() << endl;
    cout << "firmata v" << ard.getMajorFirmwareVersion() << "." << ard.getMinorFirmwareVersion() << endl;

    // set pin D13 as digital output
    ard.sendDigitalPinMode(9, ARD_PWM);
    ard.sendDigitalPinMode(10, ARD_PWM);
    ard.sendDigitalPinMode(11, ARD_PWM);

}

//--------------------------------------------------------------
void testApp::updateArduino()
{

    // update the arduino, get any data or messages.
    // the call to ard.update() is required
    ard.update();

    // do not send anything until the arduino has been set up
    if (bSetupArduino)
    {
       //Acá armo los flags de cada led para que se enciendan las distintas tiras de leds...
        if (bLed1)
        {
            ard.sendPwm(11, 255);
        }
        else
        {
            ard.sendPwm(11, 0);
        }
        if (bLed2)
        {
            ard.sendPwm(10, 255);
        }
        else
        {
            ard.sendPwm(10, 0);
        }
        if (bLed3)
        {
            ard.sendPwm(9, 255);
        }
        else
        {
            ard.sendPwm(9, 0);
        }
    }
}



//--------------------------------------------------------------
void testApp::draw()
{
    static int index=0;
    float avg_power = 0.0f;

    ofPushMatrix();

    if (bDebug)
    {

        //level
        if (!pasoUmbral)
        {
            ofSetHexColor(0x99ffffff);
        }
        else
        {
            ofSetHexColor(0x3300ff00);
            ofRect(margenNegro +0,748, FFTanalyzer.averages[freqFirstAnalysis] + 1.0,20);
            ofSetHexColor(0x330000ff);
            ofRect(umbralSonido, 748, 5, 20);
        }

        ofSetHexColor(0xffffff);
        for (int i = 0; i < (int)(BUFFER_SIZE/2 - 1); i++)
        {
            //ofRect(200+(i*4),600,4,-freq[i]*10.0f);
        }

        for (int i = 0; i < FFTanalyzer.nAverages; i++)
        {
            ofRect(margenNegro +200+(i*20),600,20,-FFTanalyzer.averages[i]);
        }

        ofSetHexColor(0xffff00);
        ofRect(margenNegro +100, 600, 20, -umbralSonido);
        ofSetHexColor(0xff00ff);
        ofRect(margenNegro +120, 600, 20, -FFTanalyzer.averages[freqFirstAnalysis]);

        ofSetHexColor(0xcccccc);
        for (int i = 0; i < FFTanalyzer.nAverages; i++)
        {
            ofRect(margenNegro +200+(i*20),600-FFTanalyzer.peaks[i],20,-4);
        }

        ofSetColor(50,50,140, 150);
        ofRect(margenNegro +80, 200, ofGetWindowWidth()*.4, ofGetWindowHeight()*.6);
        ofSetColor(255,255,255, 255);

        ofDrawBitmapString("continueLoop: " +ofToString(!continueLoop()), margenNegro + 150, 300);
        ofDrawBitmapString("currentMovie: " +ofToString( currentMovie), margenNegro +150, 320);
        ofDrawBitmapString("FFT: " +ofToString( FFTanalyzer.averages[freqFirstAnalysis]+1.0), margenNegro +150, 340);
        ofDrawBitmapString("nextMiniEvent: " +ofToString( nextMiniEvent), margenNegro +150, 360);
        ofDrawBitmapString("umbral: " +ofToString( umbralSonido), margenNegro +150, 380);
        ofDrawBitmapString("Activar/Desactivar Luz 1: '6' " + ofToString(bLed1), margenNegro +150, 400);
        ofDrawBitmapString("Activar/Desactivar Luz 3: '8' " + ofToString(bLed3), margenNegro +150, 420);
        ofDrawBitmapString("FPS: " + ofToString(ofGetFrameRate()), margenNegro + 150, 440);
        ofDrawBitmapString("Activar/Desactivar Grilla: 'g'", margenNegro + 150, 460);
        ofDrawBitmapString("Guardar valores: 's'", margenNegro + 150, 480);
        ofDrawBitmapString("Paso Umbral: " +ofToString(pasoUmbral), margenNegro + 150, 500);
        ofDrawBitmapString("Subir umbral: 'w'", margenNegro + 150, 520);
        ofDrawBitmapString("Bajar Umbral: 'q'", margenNegro + 150, 540);
        ofDrawBitmapString("nBlobs: " + ofToString(contourFinder.nBlobs), margenNegro + 150, 580);
        ofDrawBitmapString("threshold: " + ofToString(threshold), margenNegro + 150, 600);

        ofPopMatrix();

        if (bGrid){
            videoGrid.draw(0, 0, 1024, 768);
        }
    }
    ofPopMatrix();

    //blob detection drawing

    ofPushMatrix();
        ofFill();
        ofSetHexColor(0x333333);
        ofTranslate(232, 293, 0);
        ofScale(.627, .627, 1);
        ofRect(0,0,320,240);

        for (int i = 0; i < contourFinder.nBlobs; i++){
            contourFinder.blobs[i].draw(0,0);
        }
    ofPopMatrix();

    ofPushMatrix();
        ofFill();
        ofSetColor(255,255,255);
        ofTranslate(widthScreen1, 0, 0);
         blender.begin();
        mMovies[currentMovie].draw(0,0, 2048, 768);
        hearts.render();
        ofSetHexColor(0x000000);
        ofRect(0, 0, margenNegro ,  ofGetHeight());
        ofRect(ofGetWidth()-margenNegro, 0, margenNegro ,  ofGetHeight());
        blender.end();
        blender.draw();
    ofPopMatrix();

    ofPushMatrix();
        ofTranslate(0, 0, 0 );
        gui.draw();
    ofPopMatrix();

    ofDrawBitmapString("Activar/desactivar Fullscreen: tecla f" , 20, 530);
    ofSetHexColor(0xffffff);

}

//--------------------------------------------------------------
void testApp::keyPressed(int key)
{
    if( key == 'f' )
    {
        ofToggleFullscreen();
    }
    else if ( key == 'p')
    {
        mMovies[currentMovie].setFrame(mMovies[currentMovie].getTotalNumFrames()-5);
    }
    else if ( key == 'o')
    {
        mMovies[currentMovie].setFrame(0);
    }
    else if ( key == 'a' && currentMovie == 2 )
    {
        hearts.addHeart();
        continueStatus = hearts.done();
    }
    else if (key == 'b')  // && (currentMovie == 0 || currentMovie == 2 || currentMovie == 4 || currentMovie == 6 || currentMovie == 8))
        continueStatus =! continueStatus;
    else if (key == 'u' && currentMovie > -1)
    {
        umbralSonido = FFTanalyzer.averages[freqFirstAnalysis] + 1.0;
        xml.setValue("umbralSonido", umbralSonido);
        xml.saveFile("settings.xml");
        //cout << "Establecido el umbral de sonido en " << umbralSonido << std::endl;
        //if (errorStatus == SONIDO_NOT_SETTED) errorStatus = 0;
    }
    else if (key == '-')
    {
        if (currentMovie > 0)
            currentMovie-=2;
        else currentMovie = 7;
        nextMovie();
    }
    else if (key == '+')
    {
        nextMovie();
    }
    else if (key == 'D')
    {
        debugStatus = 1;
        bDebug= true;
    }
    else if (key == 'd')
    {
        debugStatus = 0;
        bDebug= false;
        xml.saveFile("settings.xml");
    }
    else if (key == '1')
    {
        currentMovie=0;
    }
    else if (key == '3')
    {
        threshold++;
    }
    else if (key == '2')
    {
        threshold--;
    }
    else if (key == 'q')
    {
        umbralSonido -= .1;
        xml.setValue("umbralSonido",umbralSonido);
        xml.saveFile("settings.xml");
    }
    else if (key == 'w')
    {
        umbralSonido += .1;
        xml.setValue("umbralSonido",umbralSonido);
        xml.saveFile("settings.xml");
    }
    else if(key == 'g')
    {
        bGrid =! bGrid;
    }

    if (key == 's')
    {
        xml.setValue("umbralSonido", umbralSonido);
        xml.setValue("thresholdBlobs", threshold);
        xml.saveFile("settings.xml");
    }
    if (key == '6')
    {
        bLed1 =! bLed1;
    }
    if (key == '7')
    {
        bLed2 =! bLed2;
    }
    if (key == '8')
    {
        bLed3 =! bLed3;
    }
    if(key== ' ')
    {
        bLearnBakground = true;
    }
}

//--------------------------------------------------------------
void testApp::keyReleased(int key)
{

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y )
{

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button)
{

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button)
{

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button)
{

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h)
{

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg)
{

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo)
{

}

bool testApp::continueLoop( )
{
    //  || currentMovie == ESCENA_MURGA_DESARROLLO
    if (currentMovie == ESCENA_BRUJA)
    {
        continueStatus = FFTanalyzer.averages[freqFirstAnalysis] > umbralSonido;// pasoUmbral
    }
    else if (currentMovie == ESCENA_FINAL)
    {
        continueStatus = FFTanalyzer.averages[freqFirstAnalysis] < umbralSonido;
    }
    if(currentMovie==ESCENA_INICIAL)
    {
      //  continueStatus=bStartInteraction;
     continueStatus = FFTanalyzer.averages[freqFirstAnalysis] > umbralSonido;
        
        bLed1=true;
        bLed2=true;
        bLed3=true;
    }
    if (currentMovie == ESCENA_CORAZONES)
    {
        continueStatus = hearts.mParticles.size() > umbralBlobs;

        //continueStatus = hearts.mParticles.size() > toleranciaCorazones;

    }
    else
    {
        hearts.setup();
    }

    //cout << "FFTanalyzer.averages[5]:" << FFTanalyzer.averages[4] << " umbralSonido: " << umbralSonido << " continueStatus:" << continueStatus << std::endl;
    return continueStatus;
}
//--------------------------------------------------------------
void testApp::nextMovie( )
{
    try
    {
        //  || currentMovie == ESCENA_MURGA_DESARROLLO
        if((currentMovie == ESCENA_INICIAL || currentMovie == ESCENA_CORAZONES || currentMovie == ESCENA_BRUJA ||  currentMovie == ESCENA_FINAL) && !continueLoop())
        {
            // do nothing...
            mMovies[currentMovie].setPosition(0);
            mMovies[currentMovie].play();
        }
        else
        {
            cout << (_MAX_MOVIES-1) <<" currentMovie:"<< currentMovie <<" final:"<< ((currentMovie < _MAX_MOVIES-1 )?"T":"F") <<" "<< std::endl;
            continueStatus = false;
            pasoUmbral = false;

            if (currentMovie > -1 && mMovies[currentMovie].isPlaying())
            {
                mMovies[currentMovie].stop();
                mMovies[currentMovie].setFrame(0);
                //mTracks[currentMovie]->stop();

            }
            //if (currentMovie > -1) mTracks[currentMovie]->stop();

            if (currentMovie < ESCENA_FINAL ) currentMovie++;
            else currentMovie = ESCENA_INICIAL;


            if (currentMovie == ESCENA_CORAZONES)
            {
                hearts.setup();
            }
            mMovies[currentMovie].setLoopState(OF_LOOP_NONE);
            mMovies[currentMovie].play();
            mMovies[currentMovie].setFrame(0);
        }
        cout << "Starting to play... " << currentMovie << std::endl;
    }
    catch( ... )
    {
        cout << "Unable to play next movie... " << currentMovie << std::endl;
    }
}

static float adder = 0;

//--------------------------------------------------------------
void testApp::audioReceived 	(float * input, int bufferSize, int nChannels)
{
    for (int i = 0; i < bufferSize; i++)
    {
        left[i] = input[i*2];
        right[i] = input[i*2+1];
    }
}

