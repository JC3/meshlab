/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005                                                \/)\/    *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *   
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
*                                                                           *
****************************************************************************/
/****************************************************************************
History
Revision 1.0 2008/02/20 Alessandro Maione, Federico Bellucci
FIRST RELEASE

****************************************************************************/

#ifndef _UTIL_H_
#define _UTIL_H_

#include "const_types.h"


#include <cassert>

//these functions return a relative-absolute value conversion respectively in float and int (rounded to closer integer value)
float relative2AbsoluteValf(float relative_val, float max_val);
int relative2AbsoluteVali(float relative_val, float max_val);

//these functions return a absolute-relative value conversion respectively in float and int (rounded to closer integer value)
float absolute2RelativeValf(float absolute_val, float max_val);
int absolute2RelativeVali(float absolute_val, float max_val);

// Converts a relative value in an absolute one after applying an exponential function val^exp
float relative2QualityValf(float relative_val, float min_q, float max_q, float exp);

//struct containing info about a chart (at graphics level)
//It stores info about borders, size and so on...
struct CHART_INFO
{
	float leftBorder;
	float rightBorder;
	float upperBorder;
	float lowerBorder;
	float chartWidth;
	float chartHeight;
	int	numOfItems;
	int	yScaleStep;
	float minX;
	float maxX;
	float minY;
	float maxY;
	float dX;
	float dY;

	CHART_INFO( int view_width=0, int view_height=0, float min_X=0.0f, float max_X=0.0f, float min_Y=0.0f, float max_Y=0.0f )
	{
		this->updateChartInfo(view_width, view_height, min_X, max_X, min_Y, max_Y);
	}
	
	void updateChartInfo( int view_width=0, int view_height=0, float min_X=0.0f, float max_X=0.0f, float min_Y=0.0f, float max_Y=0.0f )
	{
		assert(numOfItems != 0);

		leftBorder	= CANVAS_BORDER_DISTANCE;
		rightBorder	= view_width - CANVAS_BORDER_DISTANCE;
		upperBorder	= CANVAS_BORDER_DISTANCE;
		lowerBorder	= view_height - CANVAS_BORDER_DISTANCE;
		chartWidth = rightBorder - leftBorder;
		chartHeight = lowerBorder - upperBorder;
		numOfItems = NUMBER_OF_HISTOGRAM_BARS;
		yScaleStep = Y_SCALE_STEP;
		minX = min_X;
		maxX = max_X;
		minY = min_Y;
		maxY = max_Y;
		
		dX = chartWidth / (float)numOfItems;
		dY = chartHeight / (float)numOfItems;
	}
};


#endif
