#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	// listen on the given port
	cout << "listening for osc messages on port " << PORT << "\n";
	receiver.setup(PORT);

	current_msg_string = 0;
	bDebug = false;
	bGrid = false;

	calibration.loadImage("images/calibracion_bola.jpg");

	try {
	    mMovies[0].loadMovie( "videos/bola_despertador.mov" );
		mMovies[1].loadMovie( "videos/bola_bruma.mov");
		mMovies[2].loadMovie( "videos/bola_corazones.mov" );
		mMovies[3].loadMovie( "videos/bola_bruja.mov" );
		mMovies[4].loadMovie( "videos/bola_silencio.mov" );
		mMovies[5].loadMovie( "videos/bola_rayos.mov" );

		for (int i=0; i==5; i++) {
			mMovies[i].setLoopState(OF_LOOP_NORMAL);
		}

	}
	catch( ... ) {
		exit();
	}
	currentMovie=0;
	oscMovie=0;
}

//--------------------------------------------------------------
void testApp::update(){

	// hide old messages
	for(int i = 0; i < NUM_MSG_STRINGS; i++){
		if(timers[i] < ofGetElapsedTimef()){
			msg_strings[i] = "";
		}
	}

	// check for waiting messages
	while(receiver.hasWaitingMessages()){
		// get the next message
		ofxOscMessage v;
		receiver.getNextMessage(&v);

		// check for message
		if(v.getAddress() == "/currentMovie"){
			oscMovie = v.getArgAsInt32(0);
		}
	}


	switch (oscMovie) {
		case 0:
			currentMovie=0;
			mMovies[currentMovie].play();
			break;
		case 1:
			currentMovie=1;
			mMovies[currentMovie].play();
			break;
		case 2:
			currentMovie=2;
			mMovies[currentMovie].play();
			break;
		case 3:
			currentMovie=1;
			mMovies[currentMovie].play();
			break;
		case 4:
			currentMovie=3;
			mMovies[currentMovie].play();
			break;
		case 5:
			currentMovie=1;
			mMovies[currentMovie].play();
			break;
        case 6:
			currentMovie=1;
			mMovies[currentMovie].play();
			break;
        case 7:
			currentMovie=1;
			mMovies[currentMovie].play();
			break;
        case 8:
			currentMovie=4;
			mMovies[currentMovie].play();
			break;
		default:
			currentMovie=0;
			break;
	}

	mMovies[currentMovie].update();
	cout << oscMovie << endl;

}


//--------------------------------------------------------------
void testApp::draw(){
	ofBackground(0xAAAAAA);

	mMovies[currentMovie].draw(0, 0);

	if (bDebug) {
		ofSetColor(50, 50, 140, 50);
		ofRect(100, 80, 400, 200);
		ofSetColor(255, 255, 255);
		ofDrawBitmapString("FullScreen: 'f'", 120, 120);
		ofDrawBitmapString("Activar/Desactivar debug: 'd'", 120, 140);
		ofDrawBitmapString("Mostrar/Ocultar grilla: 'g'", 120, 160);
		ofDrawBitmapString("Cambiar videos: '0-4'", 120, 180);
		ofDrawBitmapString("Video actual: " + ofToString(currentMovie), 120, 200);

		if (bGrid) {
			calibration.draw(0,0);
		}
		ofSetHexColor(0xFFFFFF);
	}
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

	if (key == 'f') {
		ofToggleFullscreen();
	}
	if (key == 'd') {
		bDebug =! bDebug;
	}
	if (key == 'g') {
		bGrid =! bGrid;
	}
	if (key == '0') {
		oscMovie=0;
	}
	if (key == '1') {
		oscMovie=1;
	}
	if (key == '2') {
		oscMovie=2;
	}
	if (key == '3') {
		oscMovie=3;
	}
	if (key == '4') {
		oscMovie=4;
	}
	if (key == '5') {
		oscMovie=5;
	}

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){

}
