/*
 *  Hearts.h
 *  Fantasias_20_1
 *
 *  Created by Federico Joselevich Puiggrós on 07/01/11.
 *  Copyright 2011 ludic.cc. All rights reserved.
 *
 */
#pragma once

#include "ofMain.h"

#define MAX_HEART_PARTICLES 10

class HeartParticle {
public:
	HeartParticle(float _x, float _y, float _dx, float _dy) { x = _x; y = _y; dx = _dx; dy = _dy; };
	float x;
	float y;
	float dx;
	float dy;
	float mRotation;
};

class Hearts {
public:
	vector<HeartParticle>	mParticles;	
	
	void setup();
	void addHeart();
	void render();
	
	bool done();
	ofImage imagen;
	
	int transparencia;
};