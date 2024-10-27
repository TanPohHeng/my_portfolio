//---------------------------------------------------------
// file:	FOV.c
// author:	Tan Poh Heng
// email:	t.pohheng@digipen.edu
//
// brief:	Contains all functions pertaining to FOV functions such as setting gobal illumination, 
//			setting player's FOV in either a radius around a grid position, in a cone or in a rectangle. 
//
// documentation link:
// https://github.com/DigiPen-Faculty/CProcessing/wiki
//
// Copyright © 2022 DigiPen, All rights reserved.
//---------------------------------------------------------

#include "cprocessing.h"
#include "grid.h"
#include "game.h"
#include "vectorMathBasic.h"
#include <math.h>


#define FOG_MAX_X 800
#define FOG_MAX_Y 800
//fog grid of same size as base game to draw over the render layers as FOV, is 1 when FOV is blocked, 0 when FOV not blocked
int fog[FOG_MAX_X][FOG_MAX_Y];		

int isTrailsActive = 0;		//global variable for setting if trails(easy mode) is enabled

//anonymus enums for easy settingand tracking of fog levels
enum FogDensity {
	FOG_MAX = 2,
	FOG_HALF = 1,
	FOG_MIN = 0,
};

//a static structure to hold information related to Allownace angles used to calulate wall logics
static struct AllowanceAngle
{
	double angle;
	double allowance;
	int fovLayer;
}allowanceAngles[48];	//a 48 angle array to store up to the maximum angles blocked in a FOV with radius of 6

//static int for trails
static struct trailsPosition
{
	int x;
	int y;
}trailsPosition[100000];

static int trailsCount = 0;

/*!
/	Allowance angle functions, so as not to clutter other functions
/
/
*/

//resets the allowance angles array
static void resetAllowanceAngles(void) 
{
	for (int i = 0; i < (sizeof(allowanceAngles) / sizeof(allowanceAngles[0]));i++)
	{	//set all elements in allowance angles to a value that will be expected to not affect any angle calculations in other functions
		allowanceAngles[i].angle = -1;
		allowanceAngles[i].allowance = 0;
		allowanceAngles[i].fovLayer = 99;
	}
}

//checks if an angle has been added to the array of allowance angles
static int isAngleAlreadyAdded(double angleToCheck)
{	//checks all allowance angles
	for (int i = 0; i < (sizeof(allowanceAngles) / sizeof(allowanceAngles[0])); i++)
	{	//if matches, within a certian buffer range for floating point inaccuracy
		if (allowanceAngles[i].angle <= angleToCheck+0.000001
			&& allowanceAngles[i].angle >= angleToCheck - 0.000001)
		{
			return 1;	//return true
		}
	}
	return 0;	//return false
}

//check if non-wall tiles are blocked by walls
static int isTileBlockedByWall(double tileAngle) 
{
	double differenceInAngle = 359.0f;				//create variable to store difference in player-to-tile and allowance angles
	
	for (int iterator = 0; iterator < (sizeof(allowanceAngles) / sizeof(allowanceAngles[0])); iterator++)	//for all the angles that was added due to a wall
	{	//find the difference in tile angle to the allowance angle
		differenceInAngle = tileAngle - allowanceAngles[iterator].angle;
		differenceInAngle = (fabs(differenceInAngle) >= 180) ? fabs(fabs(differenceInAngle) - 360) : fabs(differenceInAngle);
		//if the difference in player-to-tile and allowance angles is less than equals to the angle of allowance
		if (differenceInAngle <= allowanceAngles[iterator].allowance)
		{
			return 1;	//return true
		}
	}
	return 0;			//return false
}

//special check for if wall tiles are blocked by other walls. essentially the same as isTileBlockedByWall() but with reduced allowance for shaded areas
static int isWallBlockedByWall(double tileAngle)
{
	double differenceInAngle = 359.0f;				//create variable to store difference in player-to-tile and allowance angles

	for (int iterator = 0; iterator < (sizeof(allowanceAngles) / sizeof(allowanceAngles[0])); iterator++)	//for all the angles that was added due to a wall
	{
		differenceInAngle = tileAngle - allowanceAngles[iterator].angle;
		differenceInAngle = (fabs(differenceInAngle) >= 180) ? fabs(fabs(differenceInAngle) - 360) : fabs(differenceInAngle);
		differenceInAngle++;
		//if the difference in player-to-tile and allowance angles is less than equals to the angle of allowance
		if (differenceInAngle <= allowanceAngles[iterator].allowance)
		{
			return 1;
		}
	}
	return 0;
}

//finds the angle blocking(casting a shadow) on the current tile given it's angle from player
static int angleBlockingTile(double tileAngle)
{
	double differenceInAngle = 359.0f;				//create variable to store difference in player-to-tile and allowance angles

	for (int iterator = 0; iterator < (sizeof(allowanceAngles) / sizeof(allowanceAngles[0])); iterator++)	//for all the angles that was added due to a wall
	{
		differenceInAngle = tileAngle - allowanceAngles[iterator].angle;
		differenceInAngle = (fabs(differenceInAngle) >= 180) ? fabs(fabs(differenceInAngle) - 360) : fabs(differenceInAngle);
		//if the difference in player-to-tile and allowance angles is less than equals to the angle of allowance
		if (differenceInAngle <= allowanceAngles[iterator].allowance)
		{
			return iterator;
		}
	}
	return 999;
}

/*!
/
/	END OF ALLOWANCE ANGLE FUNCTIONS
/	
*/

/*!
/	
/	TRAILS MANAGEMENT FUNCTIONS
/
*/

//reset trails array
void resetTrails(void ) 
{
	trailsCount = 0;
	for (int i = 0; i < (sizeof(trailsPosition) / sizeof(trailsPosition[0])); i++)
	{
		trailsPosition[i].x = -1;
		trailsPosition[i].y = -1;
	}
}

//find if the current position was previously added to the trails
static int isTrailAlreadyAdded(int const x1, int const y1) 
{
	for (int i = 0; i < trailsCount; i++)
	{
		if (x1 == trailsPosition[i].x && y1 == trailsPosition[i].y) 
		{
			return 1;
		}
	}
	return 0;
}

static void addToTrails(int const x1, int const y1)
{
	if (!isTrailAlreadyAdded(x1, y1) 
		&& trailsCount < (sizeof(trailsPosition)/sizeof(trailsPosition[0]))) {
		trailsPosition[trailsCount].x = x1;
		trailsPosition[trailsCount].y = y1;
		trailsCount++;
	}
}

/*!
/
/	END OF TRAILS FUNCTIONS
/
*/

//function checks if x1 and y1 are within grid with grid size gridSizeX and gridSizeY in X and Y axis respectively
static int isInGrid(int x1, int y1, int gridSizeX, int gridSizeY)
{
	if (!(x1 >= gridSizeX)
		&& !(x1 < 0)
		&& !(y1 >= gridSizeY)
		&& !(y1 < 0)) 
	{
		return 1;
	}
	return 0;
}

//forcibly sets a tile to be clear of any shadows given a (x,y) position
void setTileLit(int xPos, int yPos) 
{
	if ((xPos > 0 && xPos < returnBounds(Tile_Size)) || (yPos > 0 && yPos < returnBounds(Tile_Size)))
	fog[xPos][yPos] = FOG_MIN;
	return;
}

//forcibly sets a tile to have half opacity shadows given a (x,y) position
void setTileHalfLit(int xPos, int yPos)
{
	if(( xPos > 0 && xPos < returnBounds(Tile_Size) ) || (yPos > 0 && yPos < returnBounds(Tile_Size)))
	fog[xPos][yPos] = FOG_HALF;
	return;
}

//forcibly sets a tile to have full opacity shadows given a (x,y) position
void setTileUnlit(int xPos, int yPos)
{
	if ((xPos > 0 && xPos < returnBounds(Tile_Size)) || (yPos > 0 && yPos < returnBounds(Tile_Size)))
	fog[xPos][yPos] = FOG_MAX;
	return;
}

//sets all value in fog to be fully obscured
void clearFogBackground(void)
{
	for (int i = 0; i < FOG_MAX_X; i++)		//for each row in fog
	{
		for (int j = 0; j < FOG_MAX_Y; j++)	//for each column in fog
		{
			fog[i][j] = FOG_MAX;			//set element to be fully obscure
		}
	}
}

//update basic logic for drawing a layer of black tiles in the level, except in a circle around the player
void setIllumination
(
	int const playerXPos,		//player's current X position on the grid
	int const playerYPos,		//player's current Y position on the grid
	int const gridSizeX,		//size of the grid in the X axis for the level, used to find the bounds required of the fog grid
	int const gridSizeY,		//size of the grid in the Y axis for the level, used to find the bounds required of the fog grid
	int const fovRadius			//the radius of the global illumination in terms of number of tiles
)
{
	//Check if grid size out of array, if is larger than acceptable, return function
	if (gridSizeX > FOG_MAX_X || gridSizeY > FOG_MAX_Y)
	{
		return;
	}

	for (int i = 0; i < gridSizeX; i++)			//for each row
	{
		for (int j = 0; j < gridSizeY; j++)		//for each column
		{
			// if in an circular area with radius of fovRadius tiles around the player
			if (
				sqrt((i - playerXPos) * (i - playerXPos)+(j - playerYPos)* (j - playerYPos))
				< fovRadius)
			{
				fog[i][j] = FOG_MIN;							//set fog in this tile to none
			}
		}
	}
}

//add player's location to a trails array and set render logic for all trails position around the previous trails
void setIlluminationTrails
(
	int const gridSizeX,		//size of the grid in the X axis for the level, used to find the bounds required of the fog grid
	int const gridSizeY		//size of the grid in the Y axis for the level, used to find the bounds required of the fog grid
)
{
	if (gridSizeX > FOG_MAX_X || gridSizeY > FOG_MAX_Y)
	{
		return;
	}

	for (int i = 0; i < trailsCount; i++)
	{
		if (isInGrid(trailsPosition[i].x, trailsPosition[i].y, gridSizeX, gridSizeY))							//dont set value if out of array
		{fog[trailsPosition[i].x][trailsPosition[i].y] = FOG_HALF;}
	}
}

//makes illumination in a radius around previous steps
void setIlluminationTrails2
(
	int const trailXPos,		//player's current X position on the grid
	int const trailYPos,		//player's current Y position on the grid
	int const gridSizeX,		//size of the grid in the X axis for the level, used to find the bounds required of the fog grid
	int const gridSizeY,		//size of the grid in the Y axis for the level, used to find the bounds required of the fog grid
	int const fovRadius			//the radius of the global illumination in terms of number of tiles
)
{
	if (gridSizeX > FOG_MAX_X || gridSizeY > FOG_MAX_Y || fovRadius > 6)
	{
		return;
	}

	addToTrails(trailXPos, trailYPos);

	for (int i = 0; i < trailsCount; i++)
	{
		if (isInGrid(trailsPosition[i].x, trailsPosition[i].y, gridSizeX, gridSizeY))							//dont set value if out of array
		{
			fog[trailsPosition[i].x][trailsPosition[i].y] = FOG_HALF;
		}

		if (fovRadius > 1)
		{
			for (int currentRadius = 1; currentRadius < fovRadius; currentRadius++)			//for the size of the radius
			{
				for (int xAxis = trailsPosition[i].x - currentRadius; xAxis < (trailsPosition[i].x + currentRadius + 1); xAxis += (currentRadius != 0) ? 2 * currentRadius : 1)	//check outer layer of X axis within current radius 
				{
					for (int yAxis = trailsPosition[i].y - currentRadius; yAxis < (trailsPosition[i].y + currentRadius + 1); yAxis++)		//check of Y axis under outer layer of X axis within current radius 
					{
						if (isInGrid(xAxis, yAxis, gridSizeX, gridSizeY)
							&& sqrt((xAxis - trailsPosition[i].x) * (xAxis - trailsPosition[i].x) + (yAxis - trailsPosition[i].y) * (yAxis - trailsPosition[i].y)) < fovRadius)							//dont set value if out of array
						{
							fog[xAxis][yAxis] = FOG_HALF;
						}
					}
				}
				for (int yAxis = trailsPosition[i].y - currentRadius; yAxis < (trailsPosition[i].y + currentRadius + 1); yAxis += (currentRadius != 0) ? 2 * currentRadius : 1)	//check outer layer of Y axis within current radius that remains to be checked 
				{
					for (int xAxis = trailsPosition[i].x - currentRadius; xAxis < (trailsPosition[i].x + currentRadius); xAxis++)	//check X axis under outer layer of Y axis within current radius 
					{
						if (isInGrid(xAxis, yAxis, gridSizeX, gridSizeY)
							&& sqrt((xAxis - trailsPosition[i].x) * (xAxis - trailsPosition[i].x) + (yAxis - trailsPosition[i].y) * (yAxis - trailsPosition[i].y)) < fovRadius)								//dont set value if out of array
						{
							fog[xAxis][yAxis] = FOG_HALF;
						}
					}
				}
			}
		}
	}
}

//Assumes setIllumination()/setIllumintationAdvance() was called before hand, Works up to FOV radius of 5, all walls in the FOV are not shaded
void setIlluminationWallLogic
(
	int const playerXPos,		//player's current X position on the grid
	int const playerYPos,		//player's current Y position on the grid
	int const gridSizeX,		//size of the grid in the X axis for the level, used to find the bounds required of the fog grid
	int const gridSizeY,		//size of the grid in the Y axis for the level, used to find the bounds required of the fog grid
	int const fovRadius			//the radius of the global illumination in terms of number of tiles
)
{
	//Check if grid size out of array, if is larger than acceptable, return function. Allow fuction to work up to 6 tiles wide.
	if (gridSizeX > FOG_MAX_X || gridSizeY > FOG_MAX_Y || fovRadius>6)
	{
		return;
	}

	//int walls[FOG_MAX_X][FOG_MAX_Y];
	double anglesToBeShaded[48],			//angles to be shaded are stored here, by the degrees in cartesian
		   angleOfAllowance[48];			//the angle of allowance for each angle to be shaded
	int anglesToBeShadedSize = 0;			//the number of angles to be shaded currently stored in the above array, increment if new angles are added

	double currentTileAngle;				//store the angle of the current tile to player with 0 starting on player's right

	for (int currentRadius = 0;currentRadius<fovRadius;currentRadius++)			//for the size of the radius
	{	
		//WALL LOGIC SECTION
		//do wall logic first, then handle render. requires double to loops, but achieves a double buffering effect. Look for ways to refactor(WIP)
		for (int xAxis = playerXPos - currentRadius; xAxis < (playerXPos + currentRadius + 1); xAxis += (currentRadius != 0) ? 2 * currentRadius : 1)	//check outer layer of X axis within current radius 
		{
			for (int yAxis = playerYPos - currentRadius; yAxis < (playerYPos + currentRadius + 1); yAxis++)		//check of Y axis under outer layer of X axis within current radius 
			{
				if (isInGrid(xAxis,yAxis,gridSizeX,gridSizeY))							//dont set value if out of array
				{
					currentTileAngle = angleOfPointR2(xAxis - playerXPos, yAxis - playerYPos);	//find current angle of tile from player position and store in currentTileAngle
					if (tiles[xAxis][yAxis].type == WALL)		//if tile found is wall
					{
						anglesToBeShaded[anglesToBeShadedSize] = currentTileAngle;	//add current tile angle to angles that need to be shaded
						// find alowance angle by finding current angle converted to quadrant where x and y axis are positive and 
						// compare to angle of one grid unit forward in x axis, taking the absoulute of their difference as angle of allowance
						double allowance =
							fabs( angleOfPointR2(fabs(xAxis - playerXPos), fabs(yAxis - playerYPos))
								- angleOfPointR2(fabs(xAxis - playerXPos) + 1, fabs(yAxis - playerYPos)));
						//if allowance is 0(impossible irl, but is edge case due to how the angle is calulated), use another axis increment to 
						//determine the allowance angle instead
						allowance = (allowance == 0) ? fabs(angleOfPointR2(fabs(xAxis - playerXPos), fabs(yAxis - playerYPos))
							- angleOfPointR2(fabs(xAxis - playerXPos), fabs(yAxis - playerYPos)+1)) : allowance;
						allowance = fmod(allowance, 46);		//make allowance angle 45 or less
						allowance += 0.000001;					//add floating point padding

						angleOfAllowance[anglesToBeShadedSize] = allowance;			//set the angle of allowance to the current angle of allowance
						anglesToBeShadedSize++;					//increase the counter of the number of angles to check
					}
				}
			}
		}
		for (int yAxis = playerYPos - currentRadius; yAxis < (playerYPos + currentRadius + 1); yAxis += (currentRadius != 0) ? 2 * currentRadius : 1)	//check outer layer of Y axis within current radius that remains to be checked 
		{
			for (int xAxis = playerXPos - currentRadius; xAxis < (playerXPos + currentRadius); xAxis++)	//check X axis under outer layer of Y axis within current radius 
			{
				if (isInGrid(xAxis,yAxis,gridSizeX,gridSizeY))								//dont set value if out of array
				{
					currentTileAngle = angleOfPointR2(xAxis - playerXPos, yAxis - playerYPos);	//find current angle of tile from player position and store in currentTileAngle
					if (tiles[xAxis][yAxis].type == WALL)		//if tile found is wall
					{
						anglesToBeShaded[anglesToBeShadedSize] = currentTileAngle;	//add current tile angle to angles that need to be shaded
						// find alowance angle by finding current angle converted to quadrant where x and y axis are positive and 
						// compare to angle of one grid unit forward in x axis, taking the absoulute of their difference as angle of allowance
						double allowance =
							fabs(angleOfPointR2(fabs(xAxis - playerXPos), fabs(yAxis - playerYPos))
								- angleOfPointR2(fabs(xAxis - playerXPos) + 1, fabs(yAxis - playerYPos)));	
						//if allowance is 0(impossible irl, but is edge case due to how the angle is calulated), use another axis increment to 
						//determine the allowance angle instead
						allowance = (allowance == 0) ? fabs(angleOfPointR2(fabs(xAxis - playerXPos), fabs(yAxis - playerYPos))
							- angleOfPointR2(fabs(xAxis - playerXPos), fabs(yAxis - playerYPos) + 1)) : allowance;
						allowance = fmod(allowance,46);			//make allowance angle 45 or less
						allowance += 0.000001;					//add floating point padding

						angleOfAllowance[anglesToBeShadedSize] = allowance;			//set the angle of allowance to the current angle of allowance
						anglesToBeShadedSize++;					//increase the counter of the number of angles to check
					}
				}
			}
		}
		//END OF WALL LOGIC SECTION

		//HANDLING RENDER LOGIC SECTION
		for (int xAxis = playerXPos - currentRadius; xAxis < (playerXPos + currentRadius + 1); xAxis += (currentRadius != 0) ? 2 * currentRadius : 1)	//check outer layer of X axis within current radius 
		{
			for (int yAxis = playerYPos - currentRadius; yAxis < (playerYPos + currentRadius + 1); yAxis++)		//check of Y axis under outer layer of X axis within current radius 
			{
				if (isInGrid(xAxis, yAxis, gridSizeX, gridSizeY))								//dont set value if out of array
				{	
					double differenceInAngle = 359.0f;				//create variable to store difference in player-to-tile and allowance angles
					currentTileAngle = angleOfPointR2(xAxis - playerXPos, yAxis - playerYPos);	//find current angle of tile from player position and store in currentTileAngle

					for (int iterator = 0; iterator < anglesToBeShadedSize;iterator++)	//for all the angles that was added due to a wall
					{	//find the difference in player-to-tile and allowance angles, then make sure overflow differences are reduced to below 360
						differenceInAngle = currentTileAngle - anglesToBeShaded[iterator];
						differenceInAngle = (fabs(differenceInAngle) >= 180) ? fabs(fabs(differenceInAngle) - 360) : fabs(differenceInAngle);

						//if the difference in player-to-tile and allowance angles is less than equals to the angle of allowance
						if (differenceInAngle <= angleOfAllowance[iterator] )			
						{
							fog[xAxis][yAxis] = FOG_MAX;			//set the tile to be covered by fog
						}
					}
					//if the covered tile is a wall within the radius of the FOV
					if (tiles[xAxis][yAxis].type == WALL			
						&&sqrt((xAxis - playerXPos) * (xAxis - playerXPos) + (yAxis - playerYPos) * (yAxis - playerYPos)) < fovRadius
						)		
					{
						fog[xAxis][yAxis] = (int)FOG_MIN;			//make tile not covered by fog
					}
				}
			}
		}
		for (int yAxis = playerYPos - currentRadius; yAxis < (playerYPos + currentRadius + 1); yAxis += (currentRadius != 0) ? 2 * currentRadius : 1)	//check outer layer of Y axis within current radius that remains to be checked 
		{
			for (int xAxis = playerXPos - currentRadius; xAxis < (playerXPos + currentRadius); xAxis++)	//check X axis under outer layer of Y axis within current radius 
			{
				if (isInGrid(xAxis, yAxis, gridSizeX, gridSizeY))								//dont set value if out of array
				{
					double differenceInAngle = 359.0f;				//create variable to store difference in player-to-tile and allowance angles
					currentTileAngle = angleOfPointR2(xAxis - playerXPos, yAxis - playerYPos);	//find current angle of tile from player position and store in currentTileAngle

					for (int iterator = 0; iterator < anglesToBeShadedSize; iterator++)	//for all the angles that was added due to a wall
					{//find the difference in player-to-tile and allowance angles, then make sure overflow differences are reduced to below 360
						differenceInAngle = currentTileAngle - anglesToBeShaded[iterator];
						differenceInAngle = (fabs(differenceInAngle) >= 180) ? fabs(fabs(differenceInAngle) - 360) : fabs(differenceInAngle);

						//if the difference in player-to-tile and allowance angles is less than equals to the angle of allowance
						if (differenceInAngle <= angleOfAllowance[iterator])
						{
							fog[xAxis][yAxis] = FOG_MAX;			//set the tile to be covered by fog
						}
					}
					//if the covered tile is a wall within the radius of the FOV
					if (tiles[xAxis][yAxis].type == WALL
						&& sqrt((xAxis - playerXPos) * (xAxis - playerXPos) + (yAxis - playerYPos) * (yAxis - playerYPos)) < fovRadius
						)		
					{
						fog[xAxis][yAxis] = (int)FOG_MIN;			//set the tile to be clear of fog
					}
				}
			}
		}
		//END OF RENDER LOGIC SECTION
	}

}

//works like calling setIllumination() and setIlluminationWallLogic at the same time, but logic is implemented so walls are not rendered if in the shadows. Works up to FOV radius of 5
void setIlluminationWallLogicOnce
(
	int const playerXPos,		//player's current X position on the grid
	int const playerYPos,		//player's current Y position on the grid
	int const gridSizeX,		//size of the grid in the X axis for the level, used to find the bounds required of the fog grid
	int const gridSizeY,		//size of the grid in the Y axis for the level, used to find the bounds required of the fog grid
	int const fovRadius			//the radius of the global illumination in terms of number of tiles
)
{//Check if grid size out of array, if is larger than acceptable, return function. Allow fuction to work up to 6 tiles wide.
	if (gridSizeX > FOG_MAX_X || gridSizeY > FOG_MAX_Y || fovRadius > 6)
	{
		return;
	}

	resetAllowanceAngles();

	int anglesToBeShadedSize = 0;			//the number of angles to be shaded currently stored in the above array, increment if new angles are added

	double currentTileAngle;				//store the angle of the current tile to player with 0 starting on player's right

	for (int currentRadius = 0; currentRadius < fovRadius; currentRadius++)			//for the size of the radius
	{
		//WALL LOGIC SECTION
		//do wall logic first, then handle render. requires double to loops, but achieves a double buffering effect. Look for ways to refactor(WIP)
		for (int xAxis = playerXPos - currentRadius; xAxis < (playerXPos + currentRadius + 1); xAxis += (currentRadius != 0) ? 2 * currentRadius : 1)	//check outer layer of X axis within current radius 
		{
			for (int yAxis = playerYPos - currentRadius; yAxis < (playerYPos + currentRadius + 1); yAxis++)		//check of Y axis under outer layer of X axis within current radius 
			{
				if (isInGrid(xAxis, yAxis, gridSizeX, gridSizeY))							//dont set value if out of array
				{
					currentTileAngle = angleOfPointR2(xAxis - playerXPos, yAxis - playerYPos);	//find current angle of tile from player position and store in currentTileAngle


					if (tiles[xAxis][yAxis].type == WALL
						&& !isAngleAlreadyAdded(currentTileAngle)
						&& !isWallBlockedByWall(currentTileAngle)
						&& sqrt((xAxis - playerXPos) * (xAxis - playerXPos) + (yAxis - playerYPos) * (yAxis - playerYPos)) < fovRadius)		//if tile found is wall
					{
						fog[xAxis][yAxis] = FOG_MIN;
						allowanceAngles[anglesToBeShadedSize].angle = currentTileAngle;	//add current tile angle to angles that need to be shaded
						// find alowance angle by finding current angle converted to quadrant where x and y axis are positive and 
						// compare to angle of one grid unit forward in x axis, taking the absoulute of their difference as angle of allowance
						double allowance =
							fabs(angleOfPointR2(fabs(xAxis - playerXPos), fabs(yAxis - playerYPos))
								- angleOfPointR2(fabs(xAxis - playerXPos) + 1, fabs(yAxis - playerYPos)));
						//if allowance is 0(impossible irl, but is edge case due to how the angle is calulated), use another axis increment to 
						//determine the allowance angle instead
						allowance = (allowance == 0) ? fabs(angleOfPointR2(fabs(xAxis - playerXPos), fabs(yAxis - playerYPos))
							- angleOfPointR2(fabs(xAxis - playerXPos), fabs(yAxis - playerYPos) + 1)) : allowance;
						allowance = fmod(allowance, 46);
						allowance += 0.000001;					//add floating point padding for arithemetic

						allowanceAngles[anglesToBeShadedSize].fovLayer = currentRadius;
						allowanceAngles[anglesToBeShadedSize].allowance = allowance;			//set the angle of allowance to the current angle of allowance
						anglesToBeShadedSize++;					//increase the counter of the number of angles to check
					}
				}
			}
		}
		for (int yAxis = playerYPos - currentRadius; yAxis < (playerYPos + currentRadius + 1); yAxis += (currentRadius != 0) ? 2 * currentRadius : 1)	//check outer layer of Y axis within current radius that remains to be checked 
		{
			for (int xAxis = playerXPos - currentRadius; xAxis < (playerXPos + currentRadius); xAxis++)	//check X axis under outer layer of Y axis within current radius 
			{
				if (isInGrid(xAxis, yAxis, gridSizeX, gridSizeY))							//dont set value if out of array
				{
					currentTileAngle = angleOfPointR2(xAxis - playerXPos, yAxis - playerYPos);	//find current angle of tile from player position and store in currentTileAngle

					if (tiles[xAxis][yAxis].type == WALL
						&& !isAngleAlreadyAdded(currentTileAngle)
						&& !isWallBlockedByWall(currentTileAngle)
						&&sqrt((xAxis - playerXPos) * (xAxis - playerXPos) + (yAxis - playerYPos) * (yAxis - playerYPos)) < fovRadius)		//if tile found is wall
					{
						fog[xAxis][yAxis] = FOG_MIN;
						allowanceAngles[anglesToBeShadedSize].angle = currentTileAngle;	//add current tile angle to angles that need to be shaded
						// find alowance angle by finding current angle converted to quadrant where x and y axis are positive and 
						// compare to angle of one grid unit forward in x axis, taking the absoulute of their difference as angle of allowance
						double allowance =
							fabs(angleOfPointR2(fabs(xAxis - playerXPos), fabs(yAxis - playerYPos))
								- angleOfPointR2(fabs(xAxis - playerXPos) + 1, fabs(yAxis - playerYPos)));
						//if allowance is 0(impossible irl, but is edge case due to how the angle is calulated), use another axis increment to 
						//determine the allowance angle instead
						allowance = (allowance == 0) ? fabs(angleOfPointR2(fabs(xAxis - playerXPos), fabs(yAxis - playerYPos))
							- angleOfPointR2(fabs(xAxis - playerXPos), fabs(yAxis - playerYPos) + 1)) : allowance;
						allowance = fmod(allowance, 46);
						allowance += 0.000001;			//add floating point padding for arithemetic

						allowanceAngles[anglesToBeShadedSize].fovLayer = currentRadius;
						allowanceAngles[anglesToBeShadedSize].allowance = allowance;			//set the angle of allowance to the current angle of allowance
						anglesToBeShadedSize++;					//increase the counter of the number of angles to check
					}
				}
			}
		}
		//END OF WALL LOGIC SECTION

		//HANDLING RENDER LOGIC SECTION
		//do wall logic first, then handle render. requires double to loops, but achieves a double buffering effect. Look for ways to refactor(WIP)
		for (int xAxis = playerXPos - currentRadius; xAxis < (playerXPos + currentRadius + 1); xAxis += (currentRadius != 0) ? 2 * currentRadius : 1)	//check outer layer of X axis within current radius 
		{
			for (int yAxis = playerYPos - currentRadius; yAxis < (playerYPos + currentRadius + 1); yAxis++)		//check of Y axis under outer layer of X axis within current radius 
			{
				if (isInGrid(xAxis, yAxis, gridSizeX, gridSizeY))							//dont set value if out of array
				{
					currentTileAngle = angleOfPointR2(xAxis - playerXPos, yAxis - playerYPos);	//find current angle of tile from player position and store in currentTileAngle
					if (tiles[xAxis][yAxis].type != WALL
						&& sqrt((xAxis - playerXPos) * (xAxis - playerXPos) + (yAxis - playerYPos) * (yAxis - playerYPos)) < fovRadius)
					{
						if (isTileBlockedByWall(currentTileAngle))//when blocked by a wall
						{
							fog[xAxis][yAxis] = FOG_MAX;		//set to be covered by fog
							continue;							//move to next tile
						}
						fog[xAxis][yAxis] = FOG_MIN;			//else set to not be covered by fog
						continue;								//move to next tile
					}
					if (tiles[xAxis][yAxis].type == WALL
						&& sqrt((xAxis - playerXPos) * (xAxis - playerXPos) + (yAxis - playerYPos) * (yAxis - playerYPos)) < fovRadius
						&& currentRadius < allowanceAngles[angleBlockingTile(currentTileAngle)].fovLayer) {	//walls of same radius dont affect each other
						fog[xAxis][yAxis] = FOG_MIN;
						continue;
					}
				}
			}
		}
		for (int yAxis = playerYPos - currentRadius; yAxis < (playerYPos + currentRadius + 1); yAxis += (currentRadius != 0) ? 2 * currentRadius : 1)	//check outer layer of Y axis within current radius that remains to be checked 
		{
			for (int xAxis = playerXPos - currentRadius; xAxis < (playerXPos + currentRadius); xAxis++)	//check X axis under outer layer of Y axis within current radius 
			{
				if (isInGrid(xAxis, yAxis, gridSizeX, gridSizeY))							//dont set value if out of array
				{
					currentTileAngle = angleOfPointR2(xAxis - playerXPos, yAxis - playerYPos);	//find current angle of tile from player position and store in currentTileAngle
					if (tiles[xAxis][yAxis].type != WALL
						&& sqrt((xAxis - playerXPos) * (xAxis - playerXPos) + (yAxis - playerYPos) * (yAxis - playerYPos)) < fovRadius)
					{
						if (isTileBlockedByWall(currentTileAngle))//when blocked by a wall
						{
							fog[xAxis][yAxis] = FOG_MAX;		//set to be covered by fog
							continue;							//move to next tile
						}
						fog[xAxis][yAxis] = FOG_MIN;			//else set to not be covered by fog
						continue;
					}
					if (tiles[xAxis][yAxis].type == WALL
						&& sqrt((xAxis - playerXPos) * (xAxis - playerXPos) + (yAxis - playerYPos) * (yAxis - playerYPos)) < fovRadius
							&& currentRadius < allowanceAngles[angleBlockingTile(currentTileAngle)].fovLayer) {	//walls of same radius dont affect each other
						fog[xAxis][yAxis] = FOG_MIN;
						continue;
					}
				}
			}
		}
		//END OF RENDER LOGIC SECTION
	}
}

//equivalent to setIllumintaionWallLogicOnce(), but also adds trails to the trails array on top of the shadows
void setIlluminationWallTrailsLogic
(
	int const playerXPos,		//player's current X position on the grid
	int const playerYPos,		//player's current Y position on the grid
	int const gridSizeX,		//size of the grid in the X axis for the level, used to find the bounds required of the fog grid
	int const gridSizeY,		//size of the grid in the Y axis for the level, used to find the bounds required of the fog grid
	int const fovRadius			//the radius of the global illumination in terms of number of tiles
)
{//Check if grid size out of array, if is larger than acceptable, return function. Allow fuction to work up to 6 tiles wide.
	if (gridSizeX > FOG_MAX_X || gridSizeY > FOG_MAX_Y || fovRadius > 6)
	{
		return;
	}

	resetAllowanceAngles();

	setIlluminationTrails(gridSizeX, gridSizeY);

	int anglesToBeShadedSize = 0;			//the number of angles to be shaded currently stored in the above array, increment if new angles are added

	double currentTileAngle;				//store the angle of the current tile to player with 0 starting on player's right

	for (int currentRadius = 0; currentRadius < fovRadius; currentRadius++)			//for the size of the radius
	{
		//WALL LOGIC SECTION
		//do wall logic first, then handle render. requires double to loops, but achieves a double buffering effect. Look for ways to refactor(WIP)
		for (int xAxis = playerXPos - currentRadius; xAxis < (playerXPos + currentRadius + 1); xAxis += (currentRadius != 0) ? 2 * currentRadius : 1)	//check outer layer of X axis within current radius 
		{
			for (int yAxis = playerYPos - currentRadius; yAxis < (playerYPos + currentRadius + 1); yAxis++)		//check of Y axis under outer layer of X axis within current radius 
			{
				if (isInGrid(xAxis, yAxis, gridSizeX, gridSizeY))							//dont set value if out of array
				{
					currentTileAngle = angleOfPointR2(xAxis - playerXPos, yAxis - playerYPos);	//find current angle of tile from player position and store in currentTileAngle


					if (tiles[xAxis][yAxis].type == WALL
						&& !isAngleAlreadyAdded(currentTileAngle)
						&& !isWallBlockedByWall(currentTileAngle)
						&& sqrt((xAxis - playerXPos) * (xAxis - playerXPos) + (yAxis - playerYPos) * (yAxis - playerYPos)) < fovRadius)		//if tile found is wall
					{
						fog[xAxis][yAxis] = FOG_MIN;
						allowanceAngles[anglesToBeShadedSize].angle = currentTileAngle;	//add current tile angle to angles that need to be shaded
						// find alowance angle by finding current angle converted to quadrant where x and y axis are positive and 
						// compare to angle of one grid unit forward in x axis, taking the absoulute of their difference as angle of allowance
						double allowance =
							fabs(angleOfPointR2(fabs(xAxis - playerXPos), fabs(yAxis - playerYPos))
								- angleOfPointR2(fabs(xAxis - playerXPos) + 1, fabs(yAxis - playerYPos)));
						//if allowance is 0(impossible irl, but is edge case due to how the angle is calulated), use another axis increment to 
						//determine the allowance angle instead
						allowance = (allowance == 0) ? fabs(angleOfPointR2(fabs(xAxis - playerXPos), fabs(yAxis - playerYPos))
							- angleOfPointR2(fabs(xAxis - playerXPos), fabs(yAxis - playerYPos) + 1)) : allowance;
						allowance = fmod(allowance, 46);
						allowance += 0.000001;					//add floating point padding for arithemetic

						allowanceAngles[anglesToBeShadedSize].fovLayer = currentRadius;
						allowanceAngles[anglesToBeShadedSize].allowance = allowance;			//set the angle of allowance to the current angle of allowance
						anglesToBeShadedSize++;					//increase the counter of the number of angles to check
					}
				}
			}
		}
		for (int yAxis = playerYPos - currentRadius; yAxis < (playerYPos + currentRadius + 1); yAxis += (currentRadius != 0) ? 2 * currentRadius : 1)	//check outer layer of Y axis within current radius that remains to be checked 
		{
			for (int xAxis = playerXPos - currentRadius; xAxis < (playerXPos + currentRadius); xAxis++)	//check X axis under outer layer of Y axis within current radius 
			{
				if (isInGrid(xAxis, yAxis, gridSizeX, gridSizeY))							//dont set value if out of array
				{
					currentTileAngle = angleOfPointR2(xAxis - playerXPos, yAxis - playerYPos);	//find current angle of tile from player position and store in currentTileAngle

					if (tiles[xAxis][yAxis].type == WALL
						&& !isAngleAlreadyAdded(currentTileAngle)
						&& !isWallBlockedByWall(currentTileAngle)
						&& sqrt((xAxis - playerXPos) * (xAxis - playerXPos) + (yAxis - playerYPos) * (yAxis - playerYPos)) < fovRadius)		//if tile found is wall
					{
						fog[xAxis][yAxis] = FOG_MIN;
						allowanceAngles[anglesToBeShadedSize].angle = currentTileAngle;	//add current tile angle to angles that need to be shaded
						// find alowance angle by finding current angle converted to quadrant where x and y axis are positive and 
						// compare to angle of one grid unit forward in x axis, taking the absoulute of their difference as angle of allowance
						double allowance =
							fabs(angleOfPointR2(fabs(xAxis - playerXPos), fabs(yAxis - playerYPos))
								- angleOfPointR2(fabs(xAxis - playerXPos) + 1, fabs(yAxis - playerYPos)));
						//if allowance is 0(impossible irl, but is edge case due to how the angle is calulated), use another axis increment to 
						//determine the allowance angle instead
						allowance = (allowance == 0) ? fabs(angleOfPointR2(fabs(xAxis - playerXPos), fabs(yAxis - playerYPos))
							- angleOfPointR2(fabs(xAxis - playerXPos), fabs(yAxis - playerYPos) + 1)) : allowance;
						allowance = fmod(allowance, 46);
						allowance += 0.000001;			//add floating point padding for arithemetic

						allowanceAngles[anglesToBeShadedSize].fovLayer = currentRadius;
						allowanceAngles[anglesToBeShadedSize].allowance = allowance;			//set the angle of allowance to the current angle of allowance
						anglesToBeShadedSize++;					//increase the counter of the number of angles to check
					}
				}
			}
		}
		//END OF WALL LOGIC SECTION

		//HANDLING RENDER LOGIC SECTION
		//do wall logic first, then handle render. requires double to loops, but achieves a double buffering effect. Look for ways to refactor(WIP)
		for (int xAxis = playerXPos - currentRadius; xAxis < (playerXPos + currentRadius + 1); xAxis += (currentRadius != 0) ? 2 * currentRadius : 1)	//check outer layer of X axis within current radius 
		{
			for (int yAxis = playerYPos - currentRadius; yAxis < (playerYPos + currentRadius + 1); yAxis++)		//check of Y axis under outer layer of X axis within current radius 
			{
				if (isInGrid(xAxis, yAxis, gridSizeX, gridSizeY))							//dont set value if out of array
				{
					currentTileAngle = angleOfPointR2(xAxis - playerXPos, yAxis - playerYPos);	//find current angle of tile from player position and store in currentTileAngle
					if (tiles[xAxis][yAxis].type != WALL
						&& sqrt((xAxis - playerXPos) * (xAxis - playerXPos) + (yAxis - playerYPos) * (yAxis - playerYPos)) < fovRadius)
					{
						if (isTileBlockedByWall(currentTileAngle))//when blocked by a wall
						{
							fog[xAxis][yAxis] = FOG_MAX;		//set to be covered by fog
							continue;							//move to next tile
						}
						fog[xAxis][yAxis] = FOG_MIN;			//else set to not be covered by fog
						addToTrails(xAxis, yAxis);
						continue;								//move to next tile
					}
					if (tiles[xAxis][yAxis].type == WALL
						&& sqrt((xAxis - playerXPos) * (xAxis - playerXPos) + (yAxis - playerYPos) * (yAxis - playerYPos)) < fovRadius
						&& currentRadius < allowanceAngles[angleBlockingTile(currentTileAngle)].fovLayer) {	//walls of same radius dont affect each other
						fog[xAxis][yAxis] = FOG_MIN;
						addToTrails(xAxis, yAxis);
						continue;
					}
				}
			}
		}
		for (int yAxis = playerYPos - currentRadius; yAxis < (playerYPos + currentRadius + 1); yAxis += (currentRadius != 0) ? 2 * currentRadius : 1)	//check outer layer of Y axis within current radius that remains to be checked 
		{
			for (int xAxis = playerXPos - currentRadius; xAxis < (playerXPos + currentRadius); xAxis++)	//check X axis under outer layer of Y axis within current radius 
			{
				if (isInGrid(xAxis, yAxis, gridSizeX, gridSizeY))							//dont set value if out of array
				{
					currentTileAngle = angleOfPointR2(xAxis - playerXPos, yAxis - playerYPos);	//find current angle of tile from player position and store in currentTileAngle
					if (tiles[xAxis][yAxis].type != WALL
						&& sqrt((xAxis - playerXPos) * (xAxis - playerXPos) + (yAxis - playerYPos) * (yAxis - playerYPos)) < fovRadius)
					{
						if (isTileBlockedByWall(currentTileAngle))//when blocked by a wall
						{
							fog[xAxis][yAxis] = FOG_MAX;		//set to be covered by fog
							continue;							//move to next tile
						}
						fog[xAxis][yAxis] = FOG_MIN;			//else set to not be covered by fog
						addToTrails(xAxis, yAxis);
						continue;
					}
					if (tiles[xAxis][yAxis].type == WALL
						&& sqrt((xAxis - playerXPos) * (xAxis - playerXPos) + (yAxis - playerYPos) * (yAxis - playerYPos)) < fovRadius
						&& currentRadius < allowanceAngles[angleBlockingTile(currentTileAngle)].fovLayer) {	//walls of same radius dont affect each other
						fog[xAxis][yAxis] = FOG_MIN;
						addToTrails(xAxis, yAxis);
						continue;
					}
				}
			}
		}
		//END OF RENDER LOGIC SECTION
	}
}

//is setIlluminationWallLogic with attempts to get half opacity FOV to work, is experimental i.e. WIP(not important)
void setIlluminationAdvance
(
	int const playerXPos,		//player's current X position on the grid
	int const playerYPos,		//player's current Y position on the grid
	int const gridSizeX,		//size of the grid in the X axis for the level, used to find the bounds required of the fog grid
	int const gridSizeY,		//size of the grid in the Y axis for the level, used to find the bounds required of the fog grid
	int const fovRadius,		//the radius of the global illumination in terms of number of tiles
	int const diffuseRadius		//the radius of the global illumination that is at half opacity in terms of number of tiles
)
{
	//Check if grid size out of array, if is larger than acceptable, return function
	if (gridSizeX > FOG_MAX_X || gridSizeY > FOG_MAX_Y)
	{
		return;
	}

	for (int i = 0; i < gridSizeX; i++)			//for each row
	{
		for (int j = 0; j < gridSizeY; j++)		//for each column
		{
			if (
				sqrt((i - playerXPos) * (i - playerXPos) + (j - playerYPos) * (j - playerYPos))
				< diffuseRadius)
			{
				fog[i][j] = FOG_HALF;							//set fog in this tile to none
			}
			// if in an circular area with radius of fovRadius tiles around the player
			if (
				sqrt((i - playerXPos) * (i - playerXPos) + (j - playerYPos) * (j - playerYPos))
				< fovRadius)
			{
				fog[i][j] = FOG_MIN;							//set fog in this tile to none
			}
		}
	}
}

//update basic logic for not drawing fog along player's line of sight
void setPlayerFOV(
	int const playerXPos,		//player's current X position on the grid
	int const playerYPos,		//player's current Y position on the grid
	int const direction,		//the direction the player is currently facing, 0 is up, 1 is right, 2 is down, 3 is left
	int const gridSizeX,		//size of the grid in the X axis for the level, used to find the bounds required of the fog grid
	int const gridSizeY,		//size of the grid in the Y axis for the level, used to find the bounds required of the fog grid
	int const range				//range in terms of grid space for FOV distance
)
{
	//Check if grid size out of array, if is larger than acceptable, return function
	if (gridSizeX >= FOG_MAX_X || gridSizeY >= FOG_MAX_Y)
	{
		return;
	}

	// make sure direction is 0 to 3
	int facingDirection = (abs(direction) % 4);

	switch (facingDirection)							// if 0, face up. 1 faces right, 2 faces down, 3 faces left
	{

	case 0:														//look up
		//for each columns pass player to top of screen, up till end of screen or the maximum range
		for (int j = playerYPos; j >= 0 && abs(j - playerYPos) <= range; j--)
		{
			//for rows surrounding/close to player in horizontal axis
			for (int i = playerXPos - 1;
				(i < playerXPos + 1);
				i++)
			{
				if (isInGrid(i, j, gridSizeX, gridSizeY))				//dont set value if out of array(horizontal check)
				{
					fog[i][j] = FOG_MIN;								//set fog in this tile to none
				}

			}
		}
		break;

	case 1:
		//for each row pass player to right side of screen, up till end of screen or the maximum range
		for (int i = playerXPos; i < gridSizeX && (i - playerXPos) <= range; i++)
		{
			//for columns on top, below (increase addtional 1 size  based on expansion factor) and occupied by player (vertical axis)
			for (int j = playerYPos - 1;
				(j < playerYPos + 1);
				j++)
			{
				if (isInGrid(i, j, gridSizeX, gridSizeY))								//dont set value if out of array(vertical check)
				{
					fog[i][j] = FOG_MIN;								//set fog in this tile to none
				}
			}
		}
		break;

	case 2:														//look down
		//for each columns pass player to bottom of screen, up till end of screen or the maximum range
		for (int j = playerYPos; j < gridSizeY && abs(j - playerYPos) <= range; j++)
		{
			//for rows surrounding/close to player in horizontal axis
			for (int i = playerXPos - 1;
				(i < playerXPos + 1);
				i++)
			{
				//dont set value if out of array!
				if (isInGrid(i, j, gridSizeX, gridSizeY))
				{
					fog[i][j] = FOG_MIN;								//set fog in this tile to none
				}
			}
		}
		break;

	case 3:
		//for each row pass player to the left side of the screen, up till end of screen or the maximum range
		for (int i = playerXPos; i >= 0 && abs(i - playerXPos) <= range; i--)
		{
			//for columns on top, below and occupied by player (vertical axis)
			for (int j = playerYPos - 1;
				(j < playerYPos + 1);
				j++)
			{
				if (isInGrid(i, j, gridSizeX, gridSizeY))								//dont set value if out of array(vertical check)
				{
					fog[i][j] = FOG_MIN;								//set fog in this tile to none
				}
			}
		}
		break;
	}

}

//update basic logic for not drawing fog along player's line of sight
void setPlayerFOVFunnel(
	int const playerXPos,		//player's current X position on the grid
	int const playerYPos,		//player's current Y position on the grid
	int const direction,		//the direction the player is currently facing, 0 is up, 1 is right, 2 is down, 3 is left
	int const gridSizeX,		//size of the grid in the X axis for the level, used to find the bounds required of the fog grid
	int const gridSizeY,		//size of the grid in the Y axis for the level, used to find the bounds required of the fog grid
	int const expansionFactor,	//the rate of change of the FOV cone's expansion based on grid space 
	int const coneRange			//the range of our FOV funnel
)
{
	//Check if grid size out of array, if is larger than acceptable, return function
	if (gridSizeX >= FOG_MAX_X || gridSizeY >= FOG_MAX_Y)
	{
		return;
	}

	// make sure direction is 0 to 3
	int facingDirection = (abs(direction) % 4);

	//	int drawY,			//is one if fov is not blocked by wall, unused till further prototyping
	//		drawX;			//is one if fov is not blocked by wall, unused till further prototyping

	int currentFactor = expansionFactor;		//the current rate of the FOV cone's expansion based on grid space, used in loop

		switch (facingDirection)							// if 0, face up. 1 faces right, 2 faces down, 3 faces left
		{

		case 0:														//look up
			//for each columns pass player to top of screen, up till end of screen or the maximum cone range
			for (int j = playerYPos; j >= 0 && abs(j - playerYPos) <= coneRange; j--)
			{
				//for rows surrounding/close to player in horizontal axis
				for (int i = playerXPos - (currentFactor / expansionFactor);
					(i < playerXPos + (currentFactor / expansionFactor) + 1);
					i++)
				{
					if (isInGrid(i, j, gridSizeX, gridSizeY))				//dont set value if out of array(horizontal check)
					{
						fog[i][j] = FOG_MIN;								//set fog in this tile to none
					}

				}
				currentFactor++;									//increase the current factor for FOV size
			}
			break;

		case 1:
			//for each row pass player to right side of screen, up till end of screen or the maximum cone range
			for (int i = playerXPos; i < gridSizeX && (i - playerXPos) <= coneRange; i++)
			{
				//for columns on top, below (increase addtional 1 size  based on expansion factor) and occupied by player (vertical axis)
				for (int j = playerYPos - (currentFactor / expansionFactor);
					(j < playerYPos + (currentFactor / expansionFactor) + 1);
					j++)
				{
					if (isInGrid(i, j, gridSizeX, gridSizeY))								//dont set value if out of array(vertical check)
					{
						fog[i][j] = FOG_MIN;								//set fog in this tile to none
					}
				}

				currentFactor++;									//increase the current factor for FOV size
			}
			break;
		
		case 2:														//look down
			//for each columns pass player to bottom of screen, up till end of screen or the maximum cone range
			for (int j = playerYPos; j < gridSizeY && abs(j - playerYPos) <= coneRange; j++)
			{
				//for rows surrounding/close to player in horizontal axis
				for (int i = playerXPos - (currentFactor / expansionFactor);
					(i < playerXPos + (currentFactor / expansionFactor) + 1);
					i++)
				{
					//dont set value if out of array!
					if (isInGrid(i, j, gridSizeX, gridSizeY))
					{
						fog[i][j] = FOG_MIN;								//set fog in this tile to none
					}
				}
				currentFactor++;									//increase the current factor for FOV size
			}
			break;

		case 3:
			//for each row pass player to the left side of the screen, up till end of screen or the maximum cone range
			for (int i = playerXPos; i >= 0 && abs(i - playerXPos) <= coneRange; i--)
			{
				//for columns on top, below and occupied by player (vertical axis)
				for (int j = playerYPos - (currentFactor / expansionFactor);
					(j < playerYPos + (currentFactor / expansionFactor) + 1);
					j++)
				{
					if (isInGrid(i, j, gridSizeX, gridSizeY))								//dont set value if out of array(vertical check)
					{
						fog[i][j] = FOG_MIN;								//set fog in this tile to none
					}
				}
				currentFactor++;									//increase the current factor for FOV size
			}
			break;
		}
}

//sets the Fog covered spots in FOV for type funnel FOV, implementation assumes setPlayerFOVFunnel() was called before calling this function,
//also is dependent on global variable tiles[] to check for obstacles to vision
void setFOVFunnelWallLogic(
	int const playerXPos,		//horizontal position of player character on the grid
	int const playerYPos,		//vertical position of player character on the grid
	int const direction,		//the direction the player is currently facing, 0 is up, 1 is right, 2 is down, 3 is left
	int const gridSizeX,		//horizontal size of grid
	int const gridSizeY,		//vertical size of grid
	int const expansionFactor,	//the rate at which the funnel will expand(i.e. grow by 1 tile every (expansionFactor) wort of tiles away from player)
	int const coneRange			//the maximum range of the cone
)

{
	//Check if grid size out of array, if is larger than acceptable, return function
	if (gridSizeX >= FOG_MAX_X || gridSizeY >= FOG_MAX_Y)
	{
		return;
	}

	// make sure direction is 0 to 3
	int facingDirection = (abs(direction) % 4);

	int drawLowerBounds = -1,					//use to check if any fog should be drawn, for elements higher than player position
		drawUpperBounds = FOG_MAX_X;			//use to check if any fog should be drawn, for elements lower than player position

	//assumes size of fog X and Y axis are similiar, stores if the element in one axis is a wall, 1 means wall, 0 means not a wall
	//by default, all elements are assumed to not be a wall
	int wallArray[FOG_MAX_X] = { 0 };
	for (int* p = wallArray, *q = wallArray + FOG_MAX_X;
		p < q;
		*p = 0, p++);

	int currentFactor = expansionFactor;		//the current rate of the FOV cone's expansion based on grid space, used in loop

	switch (facingDirection)							// if 0, face up. 1 faces right, 2 faces down, 3 faces left
	{

	case 0:														//look up
		//for each columns pass player to top of screen, up till end of screen or the maximum cone range
		for (int j = playerYPos; j >= 0 && abs(j - playerYPos) <= coneRange; j--)
		{
			//for rows surrounding/close to player in horizontal axis
			for (int i = playerXPos - (currentFactor / expansionFactor);
				(i < playerXPos + (currentFactor / expansionFactor) + 1);
				i++)
			{
				if (isInGrid(i, j, gridSizeX, gridSizeY))								//dont set value if out of array(horizontal check)
				{
					if (i < drawLowerBounds)						//if x axis is further lower than the last known left side wall position touching the edge of the player's FOV
					{
						fog[i][j] = FOG_MAX;							//set fog to true
						continue;								//skip further checks to save on memory and processes, proceed with next loop check
					}
					if (i > drawUpperBounds)					//if x axis is further higher than the last known right side wall position touching the edge of the player's FOV
					{
						fog[i][j] = FOG_MAX;							//set fog to true
						continue;								//skip further checks to save on memory and processes, proceed with next loop check
					}
					if (wallArray[i]) { fog[i][j] = FOG_MAX; }			//if previously checked to be a wall, set fog to true
					if (tiles[i][j].type == WALL)				//if the current element is a type of wall in the global tiles array
					{
						//int variable used to add one to following checks to disable edge case of corner peeking
						//change variable to 0 to enable corner peeking
						int ifPreviouslyExpand = (currentFactor / expansionFactor) - ((currentFactor - 1) / expansionFactor);

						//Lower Bounds check for FOV
						//if the checked tile is a wall and is in the leftmost corner plus one in the FOV, only on rows that are larger than the previous row
						if (i - ifPreviouslyExpand == (playerXPos - (currentFactor / expansionFactor)))
						{
							drawLowerBounds = i;				//make all tiles beyond this row that is left of the current tile have fog
							fog[i - 1][j] = FOG_MAX;					//set the corner to be covered by fog to prevent peeking
						}
						//if the checked tile is a wall and is in the furthest left corner in the FOV
						else if (i == (playerXPos - (currentFactor / expansionFactor)))
						{
							drawLowerBounds = i;				//make all tiles beyond this row that is left of the current tile have fog
						}

						//Upper Bounds check for FOV
						//if checked tile is a wall and is in the furthest right corner minus one in the FOV, only on rows that are larger than the previous row
						if (i + ifPreviouslyExpand == (playerXPos + (currentFactor / expansionFactor)))
						{	//no need to set fog to next row as setting upper bound here prevents next row from being set to clear
							drawUpperBounds = i;				//make all tiles beyond this row that is right of the current tile have fog
						}
						//if the checked tile is a wall and is in the furthest right corner in the FOV
						else if (i == (playerXPos + (currentFactor / expansionFactor)))
						{
							drawUpperBounds = i;				//make all tiles beyond this row that is right of the current tile have fog
						}
						wallArray[i] = 1;						//make every row behind the current wall have fog added on further checks
						//if the next element behind the wall is another wall, let it be not covered in fog
						if (tiles[i][j + 1].type == WALL) { wallArray[j] = 0; }
					}
				}
			}
			currentFactor++;									//increase the current factor for FOV size
		}
		break;

	case 1:														//look right
		//for each row pass player to right side of screen, up till end of screen or the maximum cone range
		for (int i = playerXPos; i < gridSizeX && (i - playerXPos) <= coneRange; i++)
		{
			//for columns on top, below (increase addtional 1 size  based on expansion factor) and occupied by player (vertical axis)
			for (int j = playerYPos - (currentFactor / expansionFactor);
				(j < playerYPos + (currentFactor / expansionFactor) + 1);
				j++)
			{
				if (isInGrid(i, j, gridSizeX, gridSizeY))								//dont set valuae if out of array(vertical check)
				{
					if (j < drawLowerBounds)					//if y axis is further lower than the last known left side wall position touching the edge of the player's FOV
					{
						fog[i][j] = FOG_MAX;							//set fog to true
						continue;								//skip further checks to save on memory and processes, proceed with next loop check
					}
					if (j > drawUpperBounds)					//if y axis is further higher than the last known right side wall position touching the edge of the player's FOV
					{
						fog[i][j] = FOG_MAX;							//set fog to true
						continue;								//skip further checks to save on memory and processes, proceed with next loop check
					}
					if (wallArray[j]) { fog[i][j] = FOG_MAX; }		//if previously checked to be a wall, set fog to true
					if (tiles[i][j].type == WALL)				//if the current element is a type of wall in the global tiles array
					{
						//int variable used to add one to following checks to disable edge case of corner peeking
						//change variable to 0 to enable corner peeking
						int ifPreviouslyExpand = (currentFactor / expansionFactor) - ((currentFactor - 1) / expansionFactor);

						//Lower Bounds check for FOV
						//if the checked tile is a wall and is in the top-most corner plus one in the FOV, only on rows that are larger than the previous columns
						if (j - ifPreviouslyExpand == (playerYPos - (currentFactor / expansionFactor)))
						{
							drawLowerBounds = j;				//make all tiles beyond this column that is above(minus y axis) of the current tile have fog
							fog[i][j - 1] = FOG_MAX;					//set the corner to be covered by fog to prevent peeking
						}
						//if the checked tile is a wall and is in the furthest top corner in the FOV
						else if (j == (playerYPos - (currentFactor / expansionFactor)))
						{
							drawLowerBounds = j;				//make all tiles beyond this row that is left of the current tile have fog
						}

						//Upper Bounds check for FOV
						//if checked tile is a wall and is in the furthest bottom corner in FOV
						if (j + 1 + ifPreviouslyExpand == (playerYPos + (currentFactor / expansionFactor) + 1))
						{
							drawUpperBounds = j;				//make all tiles beyond this row that is bottom of the current tile have fog
						}
						//if checked tile is a wall and is in the furthest bottom corner in FOV
						else if (j + 1 == (playerYPos + (currentFactor / expansionFactor) + 1))
						{
							drawUpperBounds = j;				//make all tiles beyond this row that is bottom of the current tile have fog
						}
						wallArray[j] = 1;						//make every row behind the wall have fog added on further checks
						//if the next element behind the wall is another wall, let it be not covered in fog
						if (tiles[i + 1][j].type == WALL) { wallArray[j] = 0; }
					}
				}
			}

			currentFactor++;									//increase the current factor for FOV size
		}
		break;

	case 2:														//look down
		//for each columns pass player to bottom of screen, up till end of screen or the maximum cone range
		for (int j = playerYPos; j < gridSizeY && abs(j - playerYPos) <= coneRange; j++)
		{
			//for rows surrounding/close to player in horizontal axis
			for (int i = playerXPos - (currentFactor / expansionFactor);
				(i < playerXPos + (currentFactor / expansionFactor) + 1);
				i++)
			{
				//dont set value if out of array!
				if (isInGrid(i, j, gridSizeX, gridSizeY))
				{
					if (i < drawLowerBounds)						//if x axis is further lower than the last known left side wall position touching the edge of the player's FOV
					{
						fog[i][j] = FOG_MAX;							//set fog to true
						continue;								//skip further checks to save on memory and processes, proceed with next loop check
					}
					if (i > drawUpperBounds)					//if x axis is further higher than the last known right side wall position touching the edge of the player's FOV
					{
						fog[i][j] = FOG_MAX;							//set fog to true
						continue;								//skip further checks to save on memory and processes, proceed with next loop check
					}
					if (wallArray[i]) { fog[i][j] = FOG_MAX; }			//if previously checked to be a wall, set fog to true
					if (tiles[i][j].type == WALL)				//if the current element is a type of wall in the global tiles array
					{
						//int variable used to add one to following checks to disable edge case of corner peeking
						//change variable to 0 to enable corner peeking
						int ifPreviouslyExpand = (currentFactor / expansionFactor) - ((currentFactor - 1) / expansionFactor);

						//Lower Bounds check for FOV
						//if the checked tile is a wall and is in the leftmost corner plus one in the FOV, only on rows that are larger than the previous row
						if (i - ifPreviouslyExpand == (playerXPos - (currentFactor / expansionFactor)))
						{
							drawLowerBounds = i;				//make all tiles beyond this row that is left of the current tile have fog
							fog[i - 1][j] = FOG_MAX;					//set the corner to be covered by fog to prevent peeking
						}
						//if the checked tile is a wall and is in the furthest left corner in the FOV
						else if (i == (playerXPos - (currentFactor / expansionFactor)))
						{
							drawLowerBounds = i;				//make all tiles beyond this row that is left of the current tile have fog
						}

						//Upper Bounds check for FOV
						//if checked tile is a wall and is in the furthest right corner minus one in the FOV, only on rows that are larger than the previous row
						if (i + ifPreviouslyExpand == (playerXPos + (currentFactor / expansionFactor)))
						{	//no need to set fog to next row as setting upper bound here prevents next row from being set to clear
							drawUpperBounds = i;				//make all tiles beyond this row that is right of the current tile have fog
						}
						//if the checked tile is a wall and is in the furthest right corner in the FOV
						else if (i == (playerXPos + (currentFactor / expansionFactor)))
						{
							drawUpperBounds = i;				//make all tiles beyond this row that is right of the current tile have fog
						}
						wallArray[i] = 1;						//make every row behind the current wall have fog added on further checks
						//if the next element behind the wall is another wall, let it be not covered in fog
						if (tiles[i][j - 1].type == WALL) { wallArray[j] = 0; }
					}

				}

			}
			currentFactor++;									//increase the current factor for FOV size
		}
		break;

	case 3:														//look left
		//for each row pass player to the left side of the screen, up till end of screen or the maximum cone range
		for (int i = playerXPos; i >= 0 && abs(i - playerXPos) <= coneRange; i--)
		{
			//for columns on top, below and occupied by player (vertical axis)
			for (int j = playerYPos - (currentFactor / expansionFactor);
				(j < playerYPos + (currentFactor / expansionFactor) + 1);
				j++)
			{
				if (isInGrid(i, j, gridSizeX, gridSizeY))								//dont set value if out of array(vertical check)
				{
					if (j < drawLowerBounds)					//if y axis is further lower than the last known left side wall position touching the edge of the player's FOV
					{
						fog[i][j] = FOG_MAX;							//set fog to true
						continue;								//skip further checks to save on memory and processes, proceed with next loop check
					}
					if (j > drawUpperBounds)					//if y axis is further higher than the last known right side wall position touching the edge of the player's FOV
					{
						fog[i][j] = FOG_MAX;							//set fog to true
						continue;								//skip further checks to save on memory and processes, proceed with next loop check
					}
					if (wallArray[j]) { fog[i][j] = FOG_MAX; }		//if previously checked to be a wall, set fog to true
					if (tiles[i][j].type == WALL)				//if the current element is a type of wall in the global tiles array
					{						//int variable used to add one to following checks to disable edge case of corner peeking
						//change variable to 0 to enable corner peeking
						int ifPreviouslyExpand = (currentFactor / expansionFactor) - ((currentFactor - 1) / expansionFactor);

						//Lower Bounds check for FOV
						//if the checked tile is a wall and is in the top-most corner plus one in the FOV, only on rows that are larger than the previous columns
						if (j - ifPreviouslyExpand == (playerYPos - (currentFactor / expansionFactor)))
						{
							drawLowerBounds = j;				//make all tiles beyond this column that is above(minus y axis) of the current tile have fog
							fog[i][j - 1] = FOG_MAX;					//set the corner to be covered by fog to prevent peeking
						}
						//if the checked tile is a wall and is in the furthest top corner in the FOV
						else if (j == (playerYPos - (currentFactor / expansionFactor)))
						{
							drawLowerBounds = j;				//make all tiles beyond this row that is left of the current tile have fog
						}

						//Upper Bounds check for FOV
						//if checked tile is a wall and is in the furthest bottom corner in FOV
						if (j + 1 + ifPreviouslyExpand == (playerYPos + (currentFactor / expansionFactor) + 1))
						{
							drawUpperBounds = j;				//make all tiles beyond this row that is bottom of the current tile have fog
						}
						//if checked tile is a wall and is in the furthest bottom corner in FOV
						else if (j + 1 == (playerYPos + (currentFactor / expansionFactor) + 1))
						{
							drawUpperBounds = j;				//make all tiles beyond this row that is bottom of the current tile have fog
						}
						wallArray[j] = 1;						//make every row behind the wall have fog added on further checks
						//if the next element behind the wall is another wall, let it be not covered in fog
						if (tiles[i - 1][j].type == WALL) { wallArray[j] = 0; }
					}
				}
			}
			currentFactor++;									//increase the current factor for FOV size
		}
		break;
	}
}

//renders the most basic version of the FOV fog	
void renderFOVBasic(
	int const gridSizeX,		//size(number of horizontal elements) of grid in game level
	int const gridSizeY,		//size(number of vertical elements) of grid in game level
	int const tileSizePX		//tile size in pixels
)
{
	//Check if grid size out of array, if is larger than acceptable, return function
	if (gridSizeX >= FOG_MAX_X || gridSizeY >= FOG_MAX_Y)
	{
		return;
	}
	int fogLevel = 0;
	
	for (int i = 0; i < gridSizeX; i++)			//for each row
	{
		for (int j = 0; j < gridSizeY; j++)		//for each column
		{
			fogLevel = (fog[i][j] > 0);
			switch (fogLevel) {
				case 1:
					CP_Settings_Stroke(CP_Color_Create(0, 0, 0, 255));	//set outline black	
					CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));	//set block color black
					//draw the tile
					CP_Graphics_DrawRect((float)(i * tileSizePX), (float)(j * tileSizePX), (float)(tileSizePX), (float)(tileSizePX));
					break;
				default:
					break;
			}
		}
	}
}

//renders the FOV fog to various degrees of opacity according to enum FogDensity
void renderFOVAdvance(
	int const gridSizeX,		//size(number of horizontal elements) of grid in game level
	int const gridSizeY,		//size(number of vertical elements) of grid in game level
	int const tileSizePX		//tile size in pixels
)
{
	//Check if grid size out of array, if is larger than acceptable, return function
	if (gridSizeX >= FOG_MAX_X || gridSizeY >= FOG_MAX_Y)
	{
		return;
	}

	for (int i = 0; i < gridSizeX; i++)			//for each row
	{
		for (int j = 0; j < gridSizeY; j++)		//for each column
		{
			switch (fog[i][j]) {
			case FOG_MAX:
				CP_Settings_Stroke(CP_Color_Create(0, 0, 0, 255));	//set outline black	
				CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));	//set block color black
				//draw the tile
				CP_Graphics_DrawRect((float)(i * tileSizePX), (float)(j * tileSizePX), (float)(tileSizePX), (float)(tileSizePX));
				break;
			case FOG_HALF:
				CP_Settings_Stroke(CP_Color_Create(0, 0, 0, 127));	//set outline black	of half opacity
				CP_Settings_Fill(CP_Color_Create(0, 0, 0, 127));	//set block color black of half opacity
				//draw the tile
				CP_Graphics_DrawRect((float)(i * tileSizePX), (float)(j * tileSizePX), (float)(tileSizePX), (float)(tileSizePX));
				break;
			case FOG_MIN:
				break;
			default:
				CP_Settings_Stroke(CP_Color_Create(0, 0, 0, 255));	//set outline black	
				CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));	//set block color black
				//draw the tile
				CP_Graphics_DrawRect((float)(i * tileSizePX), (float)(j * tileSizePX), (float)(tileSizePX), (float)(tileSizePX));
				break;
			}
		}
	}
}
