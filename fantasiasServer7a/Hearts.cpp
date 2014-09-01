/*
 *  Hearts.cpp
 *  Fantasias_20_1
 *
 *  Created by Federico Joselevich Puiggrós on 07/01/11.
 *  Copyright 2011 ludic.cc. All rights reserved.
 *
 */

#include "Hearts.h"

void Hearts::setup()
{
	imagen.loadImage("imgs/corazon_1.png");
	mParticles.clear();
	transparencia = 255;
}

void Hearts::addHeart()
{
	mParticles.push_back( HeartParticle( ofRandom(1200, 1500), ofRandom(100, 500), ofRandom(-1.0,1.0)*2,ofRandom(-1.0,1.0)* 2) );
	//cout << "Added a HeartParticle at " << mParticles[mParticles.size()-1].x << "x" << mParticles[mParticles.size()-1].y << std::endl;
}

void Hearts::render()
{
	ofSetColor(255,255,255,transparencia);
	//ofSetHexColor(0xffffff);
	for (int i = 0; i < mParticles.size(); i++)
	{
		ofPushMatrix();
		mParticles[i].x += mParticles[i].dx;
		mParticles[i].y += mParticles[i].dy;
		ofTranslate(mParticles[i].x, mParticles[i].y, 0);
		//ofRotate(mParticles.mRotation, 0, 0, 1);
		imagen.draw(0,0);
		ofPopMatrix();
	}
}

bool Hearts::done()
{
	return mParticles.size() >= MAX_HEART_PARTICLES;
}
