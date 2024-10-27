#pragma once
//---------------------------------------------------------
// file:	vectorMathBasic.h
// author:	Tan Poh Heng
// email:	t.pohheng@digipen.edu
//
// brief:	Contains declaration functions pertaining to Vectors and Angles calculation in 
//			2D to be used outside vectorMathBasic.c
//
// documentation link:
// https://github.com/DigiPen-Faculty/CProcessing/wiki
//
// Copyright © 2022 DigiPen, All rights reserved.
//---------------------------------------------------------


//returns the distance of a vector
double distanceOfVectorR2(
	double const vectorX,
	double const vectorY
);

//determines and returns the dot product of 2 2d vectors
double dotProductR2
(
	double const vector1X,
	double const vector1Y,
	double const vector2X,
	double const vector2Y
);

//find and return angle between vectors
double angleBetweenVectorsR2
(
	double const X1,
	double const Y1,
	double const X2,
	double const Y2
);

//finds and returns the angle between 2 vectors in 2D in degrees
double angleBetweenVectorsDegreesR2
(
	double const X1,
	double const Y1,
	double const X2,
	double const Y2
);

//finds the angle of a point from origin and returns the angle in degrees
double angleOfPointR2
(
	double const X1,
	double const Y1
);