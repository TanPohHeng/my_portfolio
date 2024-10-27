#pragma once
//---------------------------------------------------------
// file:	FOV.h
// author:	Tan Poh Heng
// email:	t.pohheng@digipen.edu
//
// brief:	Function declaration for FOV render logics and rendering functions for use outside FOV.c
//			
//
// documentation link:
// https://github.com/DigiPen-Faculty/CProcessing/wiki
//
// Copyright © 2022 DigiPen, All rights reserved.
//---------------------------------------------------------

extern int isTrailsActive;		//extern variable for tracking if trails(easy mode) is required in logic

//sets all value in fog to be fully obscured
void clearFogBackground(void);

//sets all trails to values that should not affect renders
void resetTrails(void);

//set a tile at a given X and Y position to not be covered by a fog 
void setTileLit(
	int xPos,					// X position of the tile, in grid terms, to be illuminated
	int yPos					// Y position of the tile, in grid terms, to be illuminated
);

//set a tile at a given X and Y position to be covered by a fog 
void setTileUnlit(
	int xPos,					// X position of the tile, in grid terms, to be covered by fog
	int yPos					// Y position of the tile, in grid terms, to be covered by fog
);

//set a tile at a given X and Y position to be covered by a fog that is half opacity
void setTileHalfLit(
	int xPos,					// X position of the tile, in grid terms, to be covered by fog
	int yPos					// Y position of the tile, in grid terms, to be covered by fog
);

//set logic to do general illumination of area around player
void setIllumination
(
	int const playerXPos,		//horizontal position of player character on the grid
	int const playerYPos,		//vertical position of player character on the grid
	int const gridSizeX,		//horizontal size of grid
	int const gridSizeY,		//vertical size of grid
	int const fovRadius			//radius of the illumination around the player
);

//add player's location to a trails array and set render logic for all trails position in a radius around the previous trails
void setIlluminationTrails
(
	int const gridSizeX,		//size of the grid in the X axis for the level, used to find the bounds required of the fog grid
	int const gridSizeY		//size of the grid in the Y axis for the level, used to find the bounds required of the fog grid
);

//sets the wall logic for setIllumination() and setIlluminationAdvance() functions, all walls are rendered and walls cast shadows onto tiles
//based on their relative position to player. Assumes clearFogBackground() is called before calling this function
void setIlluminationWallLogic
(
	int const playerXPos,		//player's current X position on the grid
	int const playerYPos,		//player's current Y position on the grid
	int const gridSizeX,		//size of the grid in the X axis for the level, used to find the bounds required of the fog grid
	int const gridSizeY,		//size of the grid in the Y axis for the level, used to find the bounds required of the fog grid
	int const fovRadius			//the radius of the global illumination in terms of number of tiles
);

//sets illumination similiar to calling setIllumination() and setIlluminationWallLogic(), but walls that are blocked by other walls in the field of views are shaded
void setIlluminationWallLogicOnce
(
	int const playerXPos,		//player's current X position on the grid
	int const playerYPos,		//player's current Y position on the grid
	int const gridSizeX,		//size of the grid in the X axis for the level, used to find the bounds required of the fog grid
	int const gridSizeY,		//size of the grid in the Y axis for the level, used to find the bounds required of the fog grid
	int const fovRadius			//the radius of the global illumination in terms of number of tiles
);

//similar to setIlluminationWallLogicOnce(), but also renders areas previously rendered in FOV outside current FOV to half shaded instead of fully shaded 
void setIlluminationWallTrailsLogic
(
	int const playerXPos,		//player's current X position on the grid
	int const playerYPos,		//player's current Y position on the grid
	int const gridSizeX,		//size of the grid in the X axis for the level, used to find the bounds required of the fog grid
	int const gridSizeY,		//size of the grid in the Y axis for the level, used to find the bounds required of the fog grid
	int const fovRadius			//the radius of the global illumination in terms of number of tiles
);

//update basic logic for drawing a layer of black tiles in the level, except in a circle around the player. Last param is radius of half 
//opacity tiles in the fog. fully clear tiles always has higher preccedence vs half opacity tiles
void setIlluminationAdvance
(
	int const playerXPos,		//player's current X position on the grid
	int const playerYPos,		//player's current Y position on the grid
	int const gridSizeX,		//size of the grid in the X axis for the level, used to find the bounds required of the fog grid
	int const gridSizeY,		//size of the grid in the Y axis for the level, used to find the bounds required of the fog grid
	int const fovRadius,		//the radius of the global illumination in terms of number of tiles
	int const diffuseRadius		//the radius of half illuminated in terms of number of tiles, meant to be a number higher than fovRadius
);

//set logic for basic line of sight of player before render
void setPlayerFOV(
	int const playerXPos,		//horizontal position of player character on the grid
	int const playerYPos,		//vertical position of player character on the grid
	int const direction,		//the direction the player is currently facing, 0 is up, 1 is right, 2 is down, 3 is left
	int const gridSizeX,		//horizontal size of grid
	int const gridSizeY,		//vertical size of grid
	int const range				//the range of the FOV in terms of grid space
);


//set logic for line of sight of player before render
void setPlayerFOVFunnel(
	int const playerXPos,		//horizontal position of player character on the grid
	int const playerYPos,		//vertical position of player character on the grid
	int const direction,		//the direction the player is currently facing, 0 is up, 1 is right, 2 is down, 3 is left
	int const gridSizeX,		//horizontal size of grid
	int const gridSizeY,		//vertical size of grid
	int const expansionFactor,	//the rate at which the funnel will expand(i.e. grow by 1 tile every (expansionFactor) wort of tiles away from player)
	int const coneRange			//the maximum range of the cone
);

//set logic for the line of sight of the player when walls are taken into account. Currently dependent on a previous run of setPlayerFOVFunnel()
void setFOVFunnelWallLogic(
	int const playerXPos,		//horizontal position of player character on the grid
	int const playerYPos,		//vertical position of player character on the grid
	int const direction,		//the direction the player is currently facing, 0 is up, 1 is right, 2 is down, 3 is left
	int const gridSizeX,		//horizontal size of grid
	int const gridSizeY,		//vertical size of grid
	int const expansionFactor,	//the rate at which the funnel will expand(i.e. grow by 1 tile every (expansionFactor) wort of tiles away from player)
	int const coneRange			//the maximum range of the cone
);

//render array of FOV based on previous logics set by other FOV functions, renders an overlay grid similiar to the movement grid
void renderFOVBasic(
	int const gridSizeX,		//size(number of horizontal elements) of grid in game level
	int const gridSizeY,		//size(number of vertical elements) of grid in game level
	int const tileSizePX		//tile size in pixels
);

//renders the FOV fog to various degrees of opacity according to enum FogDensity
void renderFOVAdvance(
	int const gridSizeX,		//size(number of horizontal elements) of grid in game level
	int const gridSizeY,		//size(number of vertical elements) of grid in game level
	int const tileSizePX		//tile size in pixels
);