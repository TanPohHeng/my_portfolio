//---------------------------------------------------------
// file:	vectorMathBasic.c
// author:	Tan Poh Heng
// email:	t.pohheng@digipen.edu
//
// brief:	Contains functions pertaining to Vectors and Angles calculation in 2D 
//
// documentation link:
// https://github.com/DigiPen-Faculty/CProcessing/wiki
//
// Copyright © 2022 DigiPen, All rights reserved.
//---------------------------------------------------------

#define _USE_MATH_DEFINES	//allows use of M_PI to represent pi = 3.141592654...

#include <math.h>


//find distance of vector from origin when plotted on a graph
double distanceOfVectorR2(
	double const x1,
	double const y1
) 
{	//distance of vector from origin is the square root of the square of x plus the square of y
	return sqrt((x1*x1)+(y1*y1));
}

//return dot product of 2 vectors, vector1(vector1x,vector1y) and vector2(vector2x,vector2y)
double dotProductR2
(
	double const x1,
	double const y1,
	double const x2,
	double const y2
) 
{
	return ((x1 * x2) + (y1 * y2));
}

//find and return angle between vectors in radians
double angleBetweenVectorsR2
(
	double const x1,
	double const y1,
	double const x2,
	double const y2
)
{	//angle between vectors is cosine inverse of dot product x.y divided by (the distance of x multiplied by distance of y)
	return acos(dotProductR2(x1, y1, x2, y2) / (distanceOfVectorR2(x1, y1) * distanceOfVectorR2(x2, y2)));
}

//find and return angle between vectors in degrees
double angleBetweenVectorsDegreesR2
(
	double const x1,
	double const y1,
	double const x2,
	double const y2
)
{	//angle between vectors is cosine inverse of dot product x.y divided by (the distance of x multiplied by distance of y), 
	//multiplied by 180/Pi to get angle in degrees
	return acos(dotProductR2(x1, y1, x2, y2) / (distanceOfVectorR2(x1, y1) * distanceOfVectorR2(x2, y2)))*(180/M_PI);
}

//find angle of point from origin, in degrees
double angleOfPointR2
(
	double const x1,
	double const y1
) 
{
	if (x1 == 0)	//catch domain errors
	{
		if (y1 > 0)	
		{return 90;}	//return 90 if Y is any positive number, as angle of point [0,x] to [0,0] is 90 degrees on a graph
		if (y1 < 0)
		{return 270;}	//return 270 if Y is any negative number, as angle of point [0,-x] to [0,0] is 270 degrees on a graph
		if (y1 == 0)
		{return 0;}		//zero vector equals no difference in angles thus return 0
	}
	double ret = atan2(y1,x1)*(180/M_PI);	//find the tangent inverse of Y1 over X1 to get angle from origin of point, multiplied by 180/pi to get it in degrees
	if (ret < 0) //ret negative, thus add to 360 to get angle 180 to 359 degrees
	{
		return (double)360 + ret;
	}
	else		//ret positive, thus return direct value of angle 0(360) to 179
	{
		return ret;
	}
}