/* ===========================================================================

	Project: Minimax AI player for Blokus

	Description:
	 Defines heuristic functions for use with the minimax algorithm.

    Copyright (C) 2011 Lucas Sherman, David Gloe, Mary Southern, Tobias Gulden

	Lucas Sherman, email: LucasASherman@gmail.com

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

=========================================================================== */

// Type definitions
#include "Includes.h"

// Include header
#include "Heuristic.h"

// Board evaluation heuristic function prototypes
float simple( short grid[][14], int pieces[][3], int score[], int player );
float random( short grid[][14], int pieces[][3], int score[], int player );
float liberties( short grid[][14], int pieces[][3], int score[], int player );
float lib_diff( short grid[][14], int pieces[][3], int score[], int player );	//does not work properly (yet)

//Utility functions for board evaluation functions
	//For heuristic liberties
	float libValue_uni( short grid[][14], int x, int y, int player);
	float libValue_diff( short grid[][14], int x, int y, int player);

// Board evaluation function count
const int Heuristic::nEvaluationFunctions = 3;

// Board evaluation function descriptions
const char* Heuristic::evalFunctionName[nEvaluationFunctions] = {
	"Random utility evaluation heuristic.",
	"Simple score based evaluation heuristic.",
	"A heuristic which takes the liberties into account, with relative weights."
};

// Board evaluation function pointers
const EvalFunction Heuristic::evalFunction[nEvaluationFunctions] = {
	&random, // "Random utility evaluation heuristic."
	&simple, // "Simple score based evaluation heuristic"
	&liberties, //"Heuristic with relative weights for the liberties"
};

// --------------------------------------------------------
//	Random board evaluation heuristic. Returns a random
//  utility value between FLT_MAX and FLT_MIN.
// --------------------------------------------------------
float random( short grid[][14], int pieces[][3], 
			 int score[], int player ) 
{
	return (float)( rand( ) - RAND_MAX/2 ) ;
}

// --------------------------------------------------------
//	Simple board evaluation heuristic. Returns the total
//  difference in score between max player and min player.
// --------------------------------------------------------
float simple( short grid[][14], int pieces[][3], 
			 int score[], int player ) 
{
	return (float)( score[PLAYER_MAX] - score[PLAYER_MIN] );
}

// --------------------------------------------------------
//The previous attempt for assigning a value to a liberty, based on space
//So far, this is the best working try I have - on ply 3, it can compete with me
// --------------------------------------------------------
float liberties( short grid[][14], int pieces[][3], 
			 int score[], int player ) 
{
	//Consider the Liberties, i.e. the squares where each player can play
	float lib_MAX=0;
	float lib_MIN=0;
	float weight_Lib=3;     //specifier for how many placed pieces an entirely free liberty makes up
	
	for (int x=0; x<14; x++){
		for (int y=0; y<14; y++){
			if ( ( grid[x][y]&(1<<(player+4)) )||( grid[x][y]&(1<<(player+6)) )||( grid[x][y]&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
				||( grid[x][y]&(1<<(player+10)) ) ){         //check whether it's a liberty for the current player
				lib_MAX += libValue_uni(grid, x, y, player);     //give in the grid position (x,y)
			}
			if ( ( grid[x][y]&(1<<(5-player)) )||( grid[x][y]&(1<<(7-player)) )||( grid[x][y]&(1<<(9-player)) )//(1-player)+2*i+4
				||( grid[x][y]&(1<<(11-player)) ) ){		 //check whether it's a liberty for the opposing player
				lib_MIN += libValue_uni(grid, x, y, 1-player);   //give in the grid position (x,y)
			}
		}
	}
	
	//Calculate the heuristic value out of the current score and the amount of space around liberties
	float BoardScore=0;
	BoardScore = score[player] - score[1-player] + weight_Lib * (lib_MAX - lib_MIN);
	return BoardScore;
}

//assigns a relative weight to every liberty, where every free spot in space has the same contribution
float libValue_uni( short grid[][14], int x, int y, int player){
	//easy first version for debugging, minimax only maximizes the number of available liberties
	//under the constraint of placing the biggest piece:
	//	return (float) 1.0;
	
	/* Note: the assigned values for each step may be adjusted, first attempt with 1 each */
	/* Note: need an overall prefactor, otherwise huge weight on liberties... */
	int value=0;
	
	//go through all possible paths, every node has 3 new child nodes.
	//exception 1: first liberty has 4 (simply do that check, then don't need to check on kind of liberty)
	//			   2 trees will be cut right at the root -> same number of if-statements
	//exception 2: on depth 4 possibility of returning to liberty, don't need to check, trivially available
	
	//Tree written for depth 4 behind liberty (max size of a piece)
	
	/* Note: this seems quite bulky in code, there are more elegant solutions, but in my humble opinion
	this should be the better one for performance. Later on assign different values for different depths,
	and if these are safe spots, fighting liberties, or other characteristics.*/

	//Comments denote the direction of the moves
	
	if ( (x<13) && !(grid[x+1][y]&(1<<(player+2))) ){//depth 1, first branch (x)
		value++;
		if ( (x<12) && !(grid[x+2][y]&(1<<(player+2))) ){//depth 2, first branch (xx)
			value++;
			if ( (x<11) && !(grid[x+3][y]&(1<<(player+2))) ){//depth 3, first branch (xxx)
				value++;
				if ( (x<10) && !(grid[x+4][y]&(1<<(player+2))) ){//(xxxx)
					value++;
				}
				if ( (y<13) && !(grid[x+3][y+1]&(1<<(player+2))) ){//(xxxy)
					value++;
				}
				if ( (y>0) && !(grid[x+3][y-1]&(1<<(player+2))) ){//(xxx-y)
					value++;
				}
			}
			if ( (y<13) && !(grid[x+2][y+1]&(1<<(player+2))) ){//depth 3, second branch (xxy)
				value++;
				if ( (x<11) && !(grid[x+3][y+1]&(1<<(player+2))) ){//(xxyx)
					value++;
				}
				if ( (y<12) && !(grid[x+2][y+2]&(1<<(player+2))) ){//(xxyy)
					value++;
				}
				if ( !(grid[x+1][y+1]&(1<<(player+2))) ){//(xxy-x)
					value++;
				}
			}
			if ( (y>0) && !(grid[x+2][y-1]&(1<<(player+2))) ){//depth 3, third branch (xx-y)
				value++;
				if ( (x<11) && !(grid[x+3][y-1]&(1<<(player+2))) ){//(xx-yx)
					value++;
				}
				if ( (y>1) && !(grid[x+2][y-2]&(1<<(player+2))) ){//(xx-y-y)
					value++;
				}
				if ( !(grid[x+1][y-1]&(1<<(player+2))) ){//(xx-y-x)
					value++;
				}
			}
		}
		if ( (y<13) && !(grid[x+1][y+1]&(1<<(player+2))) ){//depth 2, second branch (xy)
			value++;
			if ( (x<12) && !(grid[x+2][y+1]&(1<<(player+2))) ){//depth 3, first branch (xyx)
				value++;
				if ( (x<11) && !(grid[x+3][y+1]&(1<<(player+2))) ){//(xyxx)
					value++;
				}
				if ( (y<12) && !(grid[x+2][y+2]&(1<<(player+2))) ){//(xyxy)
					value++;
				}
				if ( !(grid[x+2][y]&(1<<(player+2))) ){//(xyx-y)
					value++;
				}
			}
			if ( (y<12) && !(grid[x+1][y+2]&(1<<(player+2))) ){//depth 3, second branch (xyy)
				value++;
				if ( (x<12) && !(grid[x+2][y+2]&(1<<(player+2))) ){//(xyyx)
					value++;
				}
				if ( !(grid[x][y+2]&(1<<(player+2))) ){//(xyy-x)
					value++;
				}
				if ( (y<11) && !(grid[x+1][y+3]&(1<<(player+2))) ){//(xyyy)
					value++;
				}
			}
			if ( !(grid[x][y+1]&(1<<(player+2))) ){//depth 3, third branch (xy-x)
				value++;
				if ( (x>0) && !(grid[x-1][y+1]&(1<<(player+2))) ){//(xy-x-x)
					value++;
				}
				if ( (y<12) && !(grid[x][y+2]&(1<<(player+2))) ){//(xy-xy)
					value++;
				}
				//(xy-x-y) would be return to initial liberty - trivially available
			}
		}
		if ( (y>0) && !(grid[x+1][y-1]&(1<<(player+2))) ){//depth 2, third branch (x-y)
			value++;
			if ( (x<12) && !(grid[x+2][y-1]&(1<<(player+2))) ){//depth 3, first branch (x-yx)
				value++;
				if ( (x<11) && !(grid[x+3][y-1]&(1<<(player+2))) ){//(x-yxx)
					value++;
				}
				if ( (y>1) && !(grid[x+2][y-2]&(1<<(player+2))) ){//(x-yx-y)
					value++;
				}
				if ( !(grid[x+2][y]&(1<<(player+2))) ){//(x-yxy)
					value++;
				}
			}
			if ( (y>1) && !(grid[x+1][y-2]&(1<<(player+2))) ){//depth 3, second branch (x-y-y)
				value++;
				if ( (x<12) && !(grid[x+2][y-2]&(1<<(player+2))) ){//(x-y-yx)
					value++;
				}
				if ( !(grid[x][y-2]&(1<<(player+2))) ){//(x-y-y-x)
					value++;
				}
				if ( (y>2) && !(grid[x+1][y-3]&(1<<(player+2))) ){//(x-y-y-y)
					value++;
				}
			}
			if ( !(grid[x][y-1]&(1<<(player+2))) ){//depth 3, third branch (x-y-x)
				value++;
				if ( (x>0) && !(grid[x-1][y-1]&(1<<(player+2))) ){//(x-y-x-x)
					value++;
				}
				if ( (y>1) && !(grid[x][y-2]&(1<<(player+2))) ){//(x-y-x-y)
					value++;
				}
				//(x-y-xy) would be return to initial liberty - trivially available
			}
		}
	}
	
	if ( (x>0) && !(grid[x-1][y]&(1<<(player+2))) ){//depth 1, second branch (-x)
		value++;
		if ( (x>1) && !(grid[x-2][y]&(1<<(player+2))) ){//depth 2, first branch (-x-x)
			value++;
			if ( (x>2) && !(grid[x-3][y]&(1<<(player+2))) ){//depth 3, first branch (-x-x-x)
				value++;
				if ( (x>3) && !(grid[x-4][y]&(1<<(player+2))) ){//(-x-x-x-x)
					value++;
				}
				if ( (y<13) && !(grid[x-3][y+1]&(1<<(player+2))) ){//(-x-x-xy)
					value++;
				}
				if ( (y>0) && !(grid[x-3][y-1]&(1<<(player+2))) ){//(-x-x-x-y)
					value++;
				}
			}
			if ( (y<13) && !(grid[x-2][y+1]&(1<<(player+2))) ){//depth 3, second branch (-x-xy)
				value++;
				if ( (x>2) && !(grid[x-3][y+1]&(1<<(player+2))) ){//(-x-xy-x)
					value++;
				}
				if ( (y<12) && !(grid[x-2][y+2]&(1<<(player+2))) ){//(-x-xyy)
					value++;
				}
				if ( !(grid[x-1][y+1]&(1<<(player+2))) ){//(-x-xyx)
					value++;
				}
			}
			if ( (y>0) && !(grid[x-2][y-1]&(1<<(player+2))) ){//depth 3, third branch (-x-x-y)
				value++;
				if ( (x>2) && !(grid[x-3][y-1]&(1<<(player+2))) ){//(-x-x-y-x)
					value++;
				}
				if ( (y>1) && !(grid[x-2][y-2]&(1<<(player+2))) ){//(-x-x-y-y)
					value++;
				}
				if ( !(grid[x-1][y-1]&(1<<(player+2))) ){//(-x-x-yx)
					value++;
				}
			}
		}
		if ( (y<13) && !(grid[x-1][y+1]&(1<<(player+2))) ){//depth 2, second branch (-xy)
			value++;
			if ( (x>1) && !(grid[x-2][y+1]&(1<<(player+2))) ){//depth 3, first branch (-xy-x)
				value++;
				if ( (x>2) && !(grid[x-3][y+1]&(1<<(player+2))) ){//(-xy-x-x)
					value++;
				}
				if ( (y<12) && !(grid[x-2][y+2]&(1<<(player+2))) ){//(-xy-xy)
					value++;
				}
				if ( !(grid[x-2][y]&(1<<(player+2))) ){//(-xy-x-y)
					value++;
				}
			}
			if ( (y<12) && !(grid[x-1][y+2]&(1<<(player+2))) ){//depth 3, second branch (-xyy)
				value++;
				if ( (x>1) && !(grid[x-2][y+2]&(1<<(player+2))) ){//(-xyy-x)
					value++;
				}
				if ( !(grid[x][y+2]&(1<<(player+2))) ){//(-xyyx)
					value++;
				}
				if ( (y<11) && !(grid[x-1][y+3]&(1<<(player+2))) ){//(-xyyy)
					value++;
				}
			}
			if ( !(grid[x][y+1]&(1<<(player+2))) ){//depth 3, third branch (-xyx)
				value++;
				if ( (x<13) && !(grid[x+1][y+1]&(1<<(player+2))) ){//(-xyxx)
					value++;
				}
				if ( (y<12) && !(grid[x][y+2]&(1<<(player+2))) ){//(-xyxy)
					value++;
				}
				//(-xyx-y) would be return to initial liberty - trivially available
			}
		}
		if ( (y>0) && !(grid[x-1][y-1]&(1<<(player+2))) ){//depth 2, third branch (-x-y)
			value++;
			if ( (x>1) && !(grid[x-2][y-1]&(1<<(player+2))) ){//depth 3, first branch (-x-y-x)
				value++;
				if ( (x>2) && !(grid[x-3][y-1]&(1<<(player+2))) ){//(-x-y-x-x)
					value++;
				}
				if ( (y>1) && !(grid[x-2][y-2]&(1<<(player+2))) ){//(-x-y-x-y)
					value++;
				}
				if ( !(grid[x-2][y]&(1<<(player+2))) ){//(-x-y-xy)
					value++;
				}
			}
			if ( (y>1) && !(grid[x-1][y-2]&(1<<(player+2))) ){//depth 3, second branch (-x-y-y)
				value++;
				if ( (x>1) && !(grid[x-2][y-2]&(1<<(player+2))) ){//(-x-y-y-x)
					value++;
				}
				if ( !(grid[x][y-2]&(1<<(player+2))) ){//(-x-y-yx)
					value++;
				}
				if ( (y>2) && !(grid[x-1][y-3]&(1<<(player+2))) ){//(-x-y-y-y)
					value++;
				}
			}
			if ( !(grid[x][y-1]&(1<<(player+2))) ){//depth 3, third branch (-x-yx)
				value++;
				if ( (x<13) && !(grid[x+1][y-1]&(1<<(player+2))) ){//(-x-yxx)
					value++;
				}
				if ( (y>1) && !(grid[x][y-2]&(1<<(player+2))) ){//(-x-yx-y)
					value++;
				}
				//(-x-yxy) would be return to initial liberty - trivially available
			}
		}
	}

	if ( (y<13) && !(grid[x][y+1]&(1<<(player+2))) ){//depth 1, third branch (y)
		value++;
		if ( (y<12) && !(grid[x][y+2]&(1<<(player+2))) ){//depth 2, first branch (yy)
			value++;
			if ( (y<11) && !(grid[x][y+3]&(1<<(player+2))) ){//depth 3, first branch (yyy)
				value++;
				if ( (y<10) && !(grid[x][y+4]&(1<<(player+2))) ){//(yyyy)
					value++;
				}
				if ( (x<13) && !(grid[x+1][y+3]&(1<<(player+2))) ){//(yyyx)
					value++;
				}
				if ( (x>0) && !(grid[x-1][y+3]&(1<<(player+2))) ){//(yyy-x)
					value++;
				}
			}
			if ( (x<13) && !(grid[x+1][y+2]&(1<<(player+2))) ){//depth 3, second branch (yyx)
				value++;
				if ( (y<11) && !(grid[x+1][y+3]&(1<<(player+2))) ){//(yyxy)
					value++;
				}
				if ( (x<12) && !(grid[x+2][y+2]&(1<<(player+2))) ){//(yyxx)
					value++;
				}
				if ( !(grid[x+1][y+1]&(1<<(player+2))) ){//(yyx-y)
					value++;
				}
			}
			if ( (x>0) && !(grid[x-1][y+2]&(1<<(player+2))) ){//depth 3, third branch (yy-x)
				value++;
				if ( (y<11) && !(grid[x-1][y+3]&(1<<(player+2))) ){//(yy-xy)
					value++;
				}
				if ( (x>1) && !(grid[x-2][y+2]&(1<<(player+2))) ){//(yy-x-x)
					value++;
				}
				if ( !(grid[x-1][y+1]&(1<<(player+2))) ){//(yy-x-y)
					value++;
				}
			}
		}
		if ( (x<13) && !(grid[x+1][y+1]&(1<<(player+2))) ){//depth 2, second branch (yx)
			value++;
			if ( (y<12) && !(grid[x+1][y+2]&(1<<(player+2))) ){//depth 3, first branch (yxy)
				value++;
				if ( (y<11) && !(grid[x+1][y+3]&(1<<(player+2))) ){//(yxyy)
					value++;
				}
				if ( (x<12) && !(grid[x+2][y+2]&(1<<(player+2))) ){//(yxyx)
					value++;
				}
				if ( !(grid[x][y+2]&(1<<(player+2))) ){//(yxy-x)
					value++;
				}
			}
			if ( (x<12) && !(grid[x+2][y+1]&(1<<(player+2))) ){//depth 3, second branch (yxx)
				value++;
				if ( (y<12) && !(grid[x+2][y+2]&(1<<(player+2))) ){//(yxxy)
					value++;
				}
				if ( !(grid[x+2][y]&(1<<(player+2))) ){//(yxx-y)
					value++;
				}
				if ( (x<11) && !(grid[x+3][y+1]&(1<<(player+2))) ){//(yxxx)
					value++;
				}
			}
			if ( !(grid[x+1][y]&(1<<(player+2))) ){//depth 3, third branch (yx-y)
				value++;
				if ( (y>0) && !(grid[x+1][y-1]&(1<<(player+2))) ){//(yx-y-y)
					value++;
				}
				if ( (x<12) && !(grid[x+2][y]&(1<<(player+2))) ){//(yx-yx)
					value++;
				}
				//(yx-y-x) would be return to initial liberty - trivially available
			}
		}
		if ( (x>0) && !(grid[x-1][y+1]&(1<<(player+2))) ){//depth 2, third branch (y-x)
			value++;
			if ( (y<12) && !(grid[x-1][y+2]&(1<<(player+2))) ){//depth 3, first branch (y-xy)
				value++;
				if ( (y<11) && !(grid[x-1][y+3]&(1<<(player+2))) ){//(y-xyy)
					value++;
				}
				if ( (x>1) && !(grid[x-2][y+2]&(1<<(player+2))) ){//(y-xy-x)
					value++;
				}
				if ( !(grid[x][y+2]&(1<<(player+2))) ){//(y-xyx)
					value++;
				}
			}
			if ( (x>1) && !(grid[x-2][y+1]&(1<<(player+2))) ){//depth 3, second branch (y-x-x)
				value++;
				if ( (y<12) && !(grid[x-2][y+2]&(1<<(player+2))) ){//(y-x-xy)
					value++;
				}
				if ( !(grid[x-2][y]&(1<<(player+2))) ){//(y-x-x-y)
					value++;
				}
				if ( (x>2) && !(grid[x-3][y+1]&(1<<(player+2))) ){//(y-x-x-x)
					value++;
				}
			}
			if ( !(grid[x-1][y]&(1<<(player+2))) ){//depth 3, third branch (y-x-y)
				value++;
				if ( (y>0) && !(grid[x-1][y-1]&(1<<(player+2))) ){//(y-x-y-y)
					value++;
				}
				if ( (x>1) && !(grid[x-2][y]&(1<<(player+2))) ){//(y-x-y-x)
					value++;
				}
				//(y-x-yx) would be return to initial liberty - trivially available
			}
		}
	}
	
	if ( (y>0) && !(grid[x][y-1]&(1<<(player+2))) ){//depth 1, fourth branch (-y)
		value++;
		if ( (y>1) && !(grid[x][y-2]&(1<<(player+2))) ){//depth 2, first branch (-y-y)
			value++;
			if ( (y>2) && !(grid[x][y-3]&(1<<(player+2))) ){//depth 3, first branch (-y-y-y)
				value++;
				if ( (y>3) && !(grid[x][y-4]&(1<<(player+2))) ){//(-y-y-y-y)
					value++;
				}
				if ( (x<13) && !(grid[x+1][y-3]&(1<<(player+2))) ){//(-y-y-yx)
					value++;
				}
				if ( (x>0) && !(grid[x-1][y-3]&(1<<(player+2))) ){//(-y-y-y-x)
					value++;
				}
			}
			if ( (x<13) && !(grid[x+1][y-2]&(1<<(player+2))) ){//depth 3, second branch (-y-yx)
				value++;
				if ( (y>2) && !(grid[x+1][y-3]&(1<<(player+2))) ){//(-y-yx-y)
					value++;
				}
				if ( (x<12) && !(grid[x+2][y-2]&(1<<(player+2))) ){//(-y-yxx)
					value++;
				}
				if ( !(grid[x+1][y-1]&(1<<(player+2))) ){//(-y-yxy)
					value++;
				}
			}
			if ( (x>0) && !(grid[x-1][y-2]&(1<<(player+2))) ){//depth 3, third branch (-y-y-x)
				value++;
				if ( (y>2) && !(grid[x-1][y-3]&(1<<(player+2))) ){//(-y-y-x-y)
					value++;
				}
				if ( (x>1) && !(grid[x-2][y-2]&(1<<(player+2))) ){//(-y-y-x-x)
					value++;
				}
				if ( !(grid[x-1][y-1]&(1<<(player+2))) ){//(-y-y-xy)
					value++;
				}
			}
		}
		if ( (x<13) && !(grid[x+1][y-1]&(1<<(player+2))) ){//depth 2, second branch (-yx)
			value++;
			if ( (y>1) && !(grid[x+1][y-2]&(1<<(player+2))) ){//depth 3, first branch (-yx-y)
				value++;
				if ( (y>2) && !(grid[x+1][y-3]&(1<<(player+2))) ){//(-yx-y-y)
					value++;
				}
				if ( (x<12) && !(grid[x+2][y-2]&(1<<(player+2))) ){//(-yx-yx)
					value++;
				}
				if ( !(grid[x][y-2]&(1<<(player+2))) ){//(-yx-y-x)
					value++;
				}
			}
			if ( (x<12) && !(grid[x+2][y-1]&(1<<(player+2))) ){//depth 3, second branch (-yxx)
				value++;
				if ( (y>1) && !(grid[x+2][y-2]&(1<<(player+2))) ){//(-yxx-y)
					value++;
				}
				if ( !(grid[x+2][y]&(1<<(player+2))) ){//(-yxxy)
					value++;
				}
				if ( (x<11) && !(grid[x+3][y-1]&(1<<(player+2))) ){//(-yxxx)
					value++;
				}
			}
			if ( !(grid[x+1][y]&(1<<(player+2))) ){//depth 3, third branch (-yxy)
				value++;
				if ( (y<13) && !(grid[x+1][y+1]&(1<<(player+2))) ){//(-yxyy)
					value++;
				}
				if ( (x<12) && !(grid[x+2][y]&(1<<(player+2))) ){//(-yxyx)
					value++;
				}
				//(-yxy-x) would be return to initial libertx - triviallx available
			}
		}
		if ( (x>0) && !(grid[x-1][y-1]&(1<<(player+2))) ){//depth 2, third branch (-y-x)
			value++;
			if ( (y>1) && !(grid[x-1][y-2]&(1<<(player+2))) ){//depth 3, first branch (-y-x-y)
				value++;
				if ( (y>2) && !(grid[x-1][y-3]&(1<<(player+2))) ){//(-y-x-y-y)
					value++;
				}
				if ( (x>1) && !(grid[x-2][y-2]&(1<<(player+2))) ){//(-y-x-y-x)
					value++;
				}
				if ( !(grid[x][y-2]&(1<<(player+2))) ){//(-y-x-yx)
					value++;
				}
			}
			if ( (x>1) && !(grid[x-2][y-1]&(1<<(player+2))) ){//depth 3, second branch (-y-x-x)
				value++;
				if ( (y>1) && !(grid[x-2][y-2]&(1<<(player+2))) ){//(-y-x-x-y)
					value++;
				}
				if ( !(grid[x-2][y]&(1<<(player+2))) ){//(-y-x-xy)
					value++;
				}
				if ( (x>2) && !(grid[x-3][y-1]&(1<<(player+2))) ){//(-y-x-x-x)
					value++;
				}
			}
			if ( !(grid[x-1][y]&(1<<(player+2))) ){//depth 3, third branch (-y-xy)
				value++;
				if ( (y<13) && !(grid[x-1][y+1]&(1<<(player+2))) ){//(-y-xyy)
					value++;
				}
				if ( (x>1) && !(grid[x-2][y]&(1<<(player+2))) ){//(-y-xy-x)
					value++;
				}
				//(-y-xyx) would be return to initial liberty - trivially available
			}
		}
	}
	float result = ((float) value)/68;
	return result;
}



// --------------------------------------------------------
//My try of improving the previous evaluation function,
//by taking more liberty and space features into account
//such as no possible obstruction, enemy liberties, double counting for 2 nearby liberties
//does not work properly (yet)
// --------------------------------------------------------
float lib_diff( short grid[][14], int pieces[][3], //this one doesn't work properly (yet)
			 int score[], int player ) 
{
	//Consider the Liberties, i.e. the squares where each player can play
	float lib_MAX=0;
	float lib_MIN=0;
	float weight_Lib=4;     //specifier for how many placed pieces an entirely free liberty makes up
	
	for (int x=0; x<14; x++){
		for (int y=0; y<14; y++){
			if ( ( grid[x][y]&(1<<(player+4)) )||( grid[x][y]&(1<<(player+6)) )||( grid[x][y]&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
				||( grid[x][y]&(1<<(player+10)) ) ){         //check whether it's a liberty for the current player
				lib_MAX += libValue_diff(grid, x, y, player);     //give in the grid position (x,y)
			}
			if ( ( grid[x][y]&(1<<(5-player)) )||( grid[x][y]&(1<<(7-player)) )||( grid[x][y]&(1<<(9-player)) )//(1-player)+2*i+4
				||( grid[x][y]&(1<<(11-player)) ) ){		 //check whether it's a liberty for the opposing player
				lib_MIN += libValue_diff(grid, x, y, 1-player);   //give in the grid position (x,y)
			}
		}
	}
	
	//Calculate the heuristic value out of the current score and the amount of space around liberties
	//non-linear in the liberty values, since the more liberties available, the less each individual one is worth
	float BoardScore=0;
	BoardScore = score[player] - score[1-player] + weight_Lib * (sqrt(lib_MAX) - sqrt(lib_MIN));
	return BoardScore;
}

//assigns a relative weight to every liberty
float libValue_diff( short grid[][14], int x, int y, int player){	//in here there's something largely wrong
	
	//go through all possible paths, every node has 3 new child nodes.
	//exception 1: first liberty has 4 (simply do that check, then don't need to check on kind of liberty)
	//			   2 trees will be cut right at the root -> same number of if-statements
	//exception 2: on depth 4 possibility of returning to liberty, don't need to check, trivially available
	
	//Tree written for depth 4 behind liberty (max size of a piece)
	
	/* Note: this seems quite bulky in code, there are more elegant solutions, but in my humble opinion
	this should be the better one for performance. Later on assign different values for different depths,
	and if these are safe spots, fighting liberties, or other characteristics.*/

	//initialize the markers whether an own or an opposing liberty was hit, or whether the entire path up to that square is safe
	/* Note: the assigned values for each step may be adjusted, first attempt with 1 each */
	float value=1.0;	//minimum value for having any kind of liberty at all
	short safe_dist=-1;
	short lib_own_dist=0;	//how deep into the search tree
	short lib_other=0;	//counts how many opposing liberties endanger that one
	short marker=0;		//gets the currently investigated board tiles' reformatted value assigned

	//initialize specifiers for how much each property is worth
	float safe_value=1;
	float depth1=1, depth2=1, depth3=1, depth4=1;	//uniform values for all depths, attention to possibly double-visiting
	float normalization=69;

	//initialize by investigating starting liberty
	if (grid[x][y]&(1<<(3-player))){	//safe-marker, how far can we build without possible interference
		safe_dist=0;
	}

	if ( (grid[x][y]&(1<<(5-player)) )||( grid[x][y]&(1<<(7-player)) )||( grid[x][y]&(1<<(9-player)) )//(1-player)+2*i+4
		||( grid[x][y]&(1<<(11-player)) ) ){	//This liberty also liberty for other player?
		lib_other++;
	}	
	
	//Comments denote the direction of the moves
	if ( (x<13) && !((marker=grid[x+1][y])&(1<<(player+2))) ){//depth 1, first branch (x)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==0) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth1);
}
else {		//normal spot
value = value + depth1;
}
}
		if ( (x<12) && !((marker=grid[x+2][y])&(1<<(player+2))) ){//depth 2, first branch (xx)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==1) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth2);
}
else {		//normal spot
value = value + depth2;
}
}
			if ( (x<11) && !((marker=grid[x+3][y])&(1<<(player+2))) ){//depth 3, first branch (xxx)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==2) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth3);
}
else {		//normal spot
value = value + depth3;
}
}
				if ( (x<10) && !((marker=grid[x+4][y])&(1<<(player+2))) ){//(xxxx)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				safe_dist = ((safe_dist>3) ? 3 : safe_dist);
				}
				if ( (y<13) && !((marker=grid[x+3][y+1])&(1<<(player+2))) ){//(xxxy)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				safe_dist = ((safe_dist>3) ? 3 : safe_dist);
				}
				if ( (y>0) && !((marker=grid[x+3][y-1])&(1<<(player+2))) ){//(xxx-y)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				}
			safe_dist = ((safe_dist>2) ? 2 : safe_dist);
			}
			if ( (y<13) && !((marker=grid[x+2][y+1])&(1<<(player+2))) ){//depth 3, second branch (xxy)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==2) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth3);
}
else {		//normal spot
value = value + depth3;
}
}
				if ( (x<11) && !((marker=grid[x+3][y+1])&(1<<(player+2))) ){//(xxyx)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				safe_dist = ((safe_dist>3) ? 3 : safe_dist);
				}
				if ( (y<12) && !((marker=grid[x+2][y+2])&(1<<(player+2))) ){//(xxyy)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				safe_dist = ((safe_dist>3) ? 3 : safe_dist);
				}
				if ( !((marker=grid[x+1][y+1])&(1<<(player+2))) ){//(xxy-x)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				}
			safe_dist = ((safe_dist>2) ? 2 : safe_dist);
			}
			if ( (y>0) && !((marker=grid[x+2][y-1])&(1<<(player+2))) ){//depth 3, third branch (xx-y)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==2) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth3);
}
else {		//normal spot
value = value + depth3;
}
}
				if ( (x<11) && !((marker=grid[x+3][y-1])&(1<<(player+2))) ){//(xx-yx)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				safe_dist = ((safe_dist>3) ? 3 : safe_dist);
				}
				if ( (y>1) && !((marker=grid[x+2][y-2])&(1<<(player+2))) ){//(xx-y-y)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				safe_dist = ((safe_dist>3) ? 3 : safe_dist);
				}
				if ( !((marker=grid[x+1][y-1])&(1<<(player+2))) ){//(xx-y-x)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				}
			}
		safe_dist = ((safe_dist>1) ? 1 : safe_dist);
		}
		if ( (y<13) && !((marker=grid[x+1][y+1])&(1<<(player+2))) ){//depth 2, second branch (xy)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==1) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth2);
}
else {		//normal spot
value = value + depth2;
}
}
			if ( (x<12) && !((marker=grid[x+2][y+1])&(1<<(player+2))) ){//depth 3, first branch (xyx)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==2) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth3);
}
else {		//normal spot
value = value + depth3;
}
}
				if ( (x<11) && !((marker=grid[x+3][y+1])&(1<<(player+2))) ){//(xyxx)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				safe_dist = ((safe_dist>3) ? 3 : safe_dist);
				}
				if ( (y<12) && !((marker=grid[x+2][y+2])&(1<<(player+2))) ){//(xyxy)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				safe_dist = ((safe_dist>3) ? 3 : safe_dist);
				}
				if ( !((marker=grid[x+2][y])&(1<<(player+2))) ){//(xyx-y)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				}
			safe_dist = ((safe_dist>2) ? 2 : safe_dist);
			}
			if ( (y<12) && !((marker=grid[x+1][y+2])&(1<<(player+2))) ){//depth 3, second branch (xyy)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==2) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth3);
}
else {		//normal spot
value = value + depth3;
}
}
				if ( (x<12) && !((marker=grid[x+2][y+2])&(1<<(player+2))) ){//(xyyx)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				safe_dist = ((safe_dist>3) ? 3 : safe_dist);
				}
				if ( !((marker=grid[x][y+2])&(1<<(player+2))) ){//(xyy-x)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				safe_dist = ((safe_dist>3) ? 3 : safe_dist);
				}
				if ( (y<11) && !((marker=grid[x+1][y+3])&(1<<(player+2))) ){//(xyyy)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?

lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				}
			safe_dist = ((safe_dist>2) ? 2 : safe_dist);
			}
			if ( !((marker=grid[x][y+1])&(1<<(player+2))) ){//depth 3, third branch (xy-x)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==2) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth3);
}
else {		//normal spot
value = value + depth3;
}
}
				if ( (x>0) && !((marker=grid[x-1][y+1])&(1<<(player+2))) ){//(xy-x-x)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				safe_dist = ((safe_dist>3) ? 3 : safe_dist);
				}
				if ( (y<12) && !((marker=grid[x][y+2])&(1<<(player+2))) ){//(xy-xy)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				}
				//(xy-x-y) would be return to initial liberty - trivially available
			}
		safe_dist = ((safe_dist>1) ? 1 : safe_dist);
		}
		if ( (y>0) && !((marker=grid[x+1][y-1])&(1<<(player+2))) ){//depth 2, third branch (x-y)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==1) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth2);
}
else {		//normal spot
value = value + depth2;
}
}
			if ( (x<12) && !((marker=grid[x+2][y-1])&(1<<(player+2))) ){//depth 3, first branch (x-yx)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==2) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth3);
}
else {		//normal spot
value = value + depth3;
}
}
				if ( (x<11) && !((marker=grid[x+3][y-1])&(1<<(player+2))) ){//(x-yxx)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				safe_dist = ((safe_dist>3) ? 3 : safe_dist);
				}
				if ( (y>1) && !((marker=grid[x+2][y-2])&(1<<(player+2))) ){//(x-yx-y)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				safe_dist = ((safe_dist>3) ? 3 : safe_dist);
				}
				if ( !((marker=grid[x+2][y])&(1<<(player+2))) ){//(x-yxy)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				}
			safe_dist = ((safe_dist>2) ? 2 : safe_dist);
			}
			if ( (y>1) && !((marker=grid[x+1][y-2])&(1<<(player+2))) ){//depth 3, second branch (x-y-y)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==2) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth3);
}
else {		//normal spot
value = value + depth3;
}
}
				if ( (x<12) && !((marker=grid[x+2][y-2])&(1<<(player+2))) ){//(x-y-yx)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				safe_dist = ((safe_dist>3) ? 3 : safe_dist);
				}
				if ( !((marker=grid[x][y-2])&(1<<(player+2))) ){//(x-y-y-x)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				safe_dist = ((safe_dist>3) ? 3 : safe_dist);
				}
				if ( (y>2) && !((marker=grid[x+1][y-3])&(1<<(player+2))) ){//(x-y-y-y)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				}
			safe_dist = ((safe_dist>2) ? 2 : safe_dist);
			}
			if ( !((marker=grid[x][y-1])&(1<<(player+2))) ){//depth 3, third branch (x-y-x)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==2) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth3);
}
else {		//normal spot
value = value + depth3;
}
}
				if ( (x>0) && !((marker=grid[x-1][y-1])&(1<<(player+2))) ){//(x-y-x-x)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				safe_dist = ((safe_dist>3) ? 3 : safe_dist);
				}
				if ( (y>1) && !((marker=grid[x][y-2])&(1<<(player+2))) ){//(x-y-x-y)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				}
				//(x-y-xy) would be return to initial liberty - trivially available
			}
		}
	safe_dist = ((safe_dist>0) ? 0 : safe_dist);
	}
	
	if ( (x>0) && !((marker=grid[x-1][y])&(1<<(player+2))) ){//depth 1, second branch (-x)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==0) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth1);
}
else {		//normal spot
value = value + depth1;
}
}
		if ( (x>1) && !((marker=grid[x-2][y])&(1<<(player+2))) ){//depth 2, first branch (-x-x)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==1) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth2);
}
else {		//normal spot
value = value + depth2;
}
}
			if ( (x>2) && !((marker=grid[x-3][y])&(1<<(player+2))) ){//depth 3, first branch (-x-x-x)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==2) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth3);
}
else {		//normal spot
value = value + depth3;
}
}
				if ( (x>3) && !((marker=grid[x-4][y])&(1<<(player+2))) ){//(-x-x-x-x)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				safe_dist = ((safe_dist>3) ? 3 : safe_dist);
				}
				if ( (y<13) && !((marker=grid[x-3][y+1])&(1<<(player+2))) ){//(-x-x-xy)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				safe_dist = ((safe_dist>3) ? 3 : safe_dist);
				}
				if ( (y>0) && !((marker=grid[x-3][y-1])&(1<<(player+2))) ){//(-x-x-x-y)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				}
			safe_dist = ((safe_dist>2) ? 2 : safe_dist);
			}
			if ( (y<13) && !((marker=grid[x-2][y+1])&(1<<(player+2))) ){//depth 3, second branch (-x-xy)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==2) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth3);
}
else {		//normal spot
value = value + depth3;
}
}
				if ( (x>2) && !((marker=grid[x-3][y+1])&(1<<(player+2))) ){//(-x-xy-x)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				safe_dist = ((safe_dist>3) ? 3 : safe_dist);
				}
				if ( (y<12) && !((marker=grid[x-2][y+2])&(1<<(player+2))) ){//(-x-xyy)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				safe_dist = ((safe_dist>3) ? 3 : safe_dist);
				}
				if ( !((marker=grid[x-1][y+1])&(1<<(player+2))) ){//(-x-xyx)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				}
			safe_dist = ((safe_dist>2) ? 2 : safe_dist);
			}
			if ( (y>0) && !((marker=grid[x-2][y-1])&(1<<(player+2))) ){//depth 3, third branch (-x-x-y)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==2) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth3);
}
else {		//normal spot
value = value + depth3;
}
}
				if ( (x>2) && !((marker=grid[x-3][y-1])&(1<<(player+2))) ){//(-x-x-y-x)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				safe_dist = ((safe_dist>3) ? 3 : safe_dist);
				}
				if ( (y>1) && !((marker=grid[x-2][y-2])&(1<<(player+2))) ){//(-x-x-y-y)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				safe_dist = ((safe_dist>3) ? 3 : safe_dist);
				}
				if ( !((marker=grid[x-1][y-1])&(1<<(player+2))) ){//(-x-x-yx)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				}
			}
		safe_dist = ((safe_dist>1) ? 1 : safe_dist);
		}
		if ( (y<13) && !((marker=grid[x-1][y+1])&(1<<(player+2))) ){//depth 2, second branch (-xy)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==1) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth2);
}
else {		//normal spot
value = value + depth2;
}
}
			if ( (x>1) && !((marker=grid[x-2][y+1])&(1<<(player+2))) ){//depth 3, first branch (-xy-x)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==2) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth3);
}
else {		//normal spot
value = value + depth3;
}
}
				if ( (x>2) && !((marker=grid[x-3][y+1])&(1<<(player+2))) ){//(-xy-x-x)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				safe_dist = ((safe_dist>3) ? 3 : safe_dist);
				}
				if ( (y<12) && !((marker=grid[x-2][y+2])&(1<<(player+2))) ){//(-xy-xy)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				safe_dist = ((safe_dist>3) ? 3 : safe_dist);
				}
				if ( !((marker=grid[x-2][y])&(1<<(player+2))) ){//(-xy-x-y)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				}
			safe_dist = ((safe_dist>2) ? 2 : safe_dist);
			}
			if ( (y<12) && !((marker=grid[x-1][y+2])&(1<<(player+2))) ){//depth 3, second branch (-xyy)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==2) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth3);
}
else {		//normal spot
value = value + depth3;
}
}
				if ( (x>1) && !((marker=grid[x-2][y+2])&(1<<(player+2))) ){//(-xyy-x)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				safe_dist = ((safe_dist>3) ? 3 : safe_dist);
				}
				if ( !((marker=grid[x][y+2])&(1<<(player+2))) ){//(-xyyx)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				safe_dist = ((safe_dist>3) ? 3 : safe_dist);
				}
				if ( (y<11) && !((marker=grid[x-1][y+3])&(1<<(player+2))) ){//(-xyyy)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				}
			safe_dist = ((safe_dist>2) ? 2 : safe_dist);
			}
			if ( !((marker=grid[x][y+1])&(1<<(player+2))) ){//depth 3, third branch (-xyx)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==2) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth3);
}
else {		//normal spot
value = value + depth3;
}
}
				if ( (x<13) && !((marker=grid[x+1][y+1])&(1<<(player+2))) ){//(-xyxx)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				safe_dist = ((safe_dist>3) ? 3 : safe_dist);
				}
				if ( (y<12) && !((marker=grid[x][y+2])&(1<<(player+2))) ){//(-xyxy)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				}
				//(-xyx-y) would be return to initial liberty - trivially available
			}
		safe_dist = ((safe_dist>1) ? 1 : safe_dist);
		}
		if ( (y>0) && !((marker=grid[x-1][y-1])&(1<<(player+2))) ){//depth 2, third branch (-x-y)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==1) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth2);
}
else {		//normal spot
value = value + depth2;
}
}
			if ( (x>1) && !((marker=grid[x-2][y-1])&(1<<(player+2))) ){//depth 3, first branch (-x-y-x)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==2) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth3);
}
else {		//normal spot
value = value + depth3;
}
}
				if ( (x>2) && !((marker=grid[x-3][y-1])&(1<<(player+2))) ){//(-x-y-x-x)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				safe_dist = ((safe_dist>3) ? 3 : safe_dist);
				}
				if ( (y>1) && !((marker=grid[x-2][y-2])&(1<<(player+2))) ){//(-x-y-x-y)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				safe_dist = ((safe_dist>3) ? 3 : safe_dist);
				}
				if ( !((marker=grid[x-2][y])&(1<<(player+2))) ){//(-x-y-xy)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				}
			safe_dist = ((safe_dist>2) ? 2 : safe_dist);
			}
			if ( (y>1) && !((marker=grid[x-1][y-2])&(1<<(player+2))) ){//depth 3, second branch (-x-y-y)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==2) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth3);
}
else {		//normal spot
value = value + depth3;
}
}
				if ( (x>1) && !((marker=grid[x-2][y-2])&(1<<(player+2))) ){//(-x-y-y-x)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				safe_dist = ((safe_dist>3) ? 3 : safe_dist);
				}
				if ( !((marker=grid[x][y-2])&(1<<(player+2))) ){//(-x-y-yx)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				safe_dist = ((safe_dist>3) ? 3 : safe_dist);
				}
				if ( (y>2) && !((marker=grid[x-1][y-3])&(1<<(player+2))) ){//(-x-y-y-y)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				}
			safe_dist = ((safe_dist>2) ? 2 : safe_dist);
			}
			if ( !((marker=grid[x][y-1])&(1<<(player+2))) ){//depth 3, third branch (-x-yx)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==2) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth3);
}
else {		//normal spot
value = value + depth3;
}
}
				if ( (x<13) && !((marker=grid[x+1][y-1])&(1<<(player+2))) ){//(-x-yxx)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				safe_dist = ((safe_dist>3) ? 3 : safe_dist);
				}
				if ( (y>1) && !((marker=grid[x][y-2])&(1<<(player+2))) ){//(-x-yx-y)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				}
				//(-x-yxy) would be return to initial liberty - trivially available
			}
		}
	safe_dist = ((safe_dist>0) ? 0 : safe_dist);
	}

	if ( (y<13) && !((marker=grid[x][y+1])&(1<<(player+2))) ){//depth 1, third branch (y)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==0) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth1);
}
else {		//normal spot
value = value + depth1;
}
}
		if ( (y<12) && !((marker=grid[x][y+2])&(1<<(player+2))) ){//depth 2, first branch (yy)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==1) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth2);
}
else {		//normal spot
value = value + depth2;
}
}
			if ( (y<11) && !((marker=grid[x][y+3])&(1<<(player+2))) ){//depth 3, first branch (yyy)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==2) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth3);
}
else {		//normal spot
value = value + depth3;
}
}
				if ( (y<10) && !((marker=grid[x][y+4])&(1<<(player+2))) ){//(yyyy)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				safe_dist = ((safe_dist>3) ? 3 : safe_dist);
				}
				if ( (x<13) && !((marker=grid[x+1][y+3])&(1<<(player+2))) ){//(yyyx)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				safe_dist = ((safe_dist>3) ? 3 : safe_dist);
				}
				if ( (x>0) && !((marker=grid[x-1][y+3])&(1<<(player+2))) ){//(yyy-x)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				}
			safe_dist = ((safe_dist>2) ? 2 : safe_dist);
			}
			if ( (x<13) && !((marker=grid[x+1][y+2])&(1<<(player+2))) ){//depth 3, second branch (yyx)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==2) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth3);
}
else {		//normal spot
value = value + depth3;
}
}
				if ( (y<11) && !((marker=grid[x+1][y+3])&(1<<(player+2))) ){//(yyxy)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				safe_dist = ((safe_dist>3) ? 3 : safe_dist);
				}
				if ( (x<12) && !((marker=grid[x+2][y+2])&(1<<(player+2))) ){//(yyxx)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				safe_dist = ((safe_dist>3) ? 3 : safe_dist);
				}
				if ( !((marker=grid[x+1][y+1])&(1<<(player+2))) ){//(yyx-y)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				}
			safe_dist = ((safe_dist>2) ? 2 : safe_dist);
			}
			if ( (x>0) && !((marker=grid[x-1][y+2])&(1<<(player+2))) ){//depth 3, third branch (yy-x)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==2) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth3);
}
else {		//normal spot
value = value + depth3;
}
}
				if ( (y<11) && !((marker=grid[x-1][y+3])&(1<<(player+2))) ){//(yy-xy)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				safe_dist = ((safe_dist>3) ? 3 : safe_dist);
				}
				if ( (x>1) && !((marker=grid[x-2][y+2])&(1<<(player+2))) ){//(yy-x-x)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				safe_dist = ((safe_dist>3) ? 3 : safe_dist);
				}
				if ( !((marker=grid[x-1][y+1])&(1<<(player+2))) ){//(yy-x-y)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				}
			}
		safe_dist = ((safe_dist>1) ? 1 : safe_dist);
		}
		if ( (x<13) && !((marker=grid[x+1][y+1])&(1<<(player+2))) ){//depth 2, second branch (yx)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==1) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth2);
}
else {		//normal spot
value = value + depth2;
}
}
			if ( (y<12) && !((marker=grid[x+1][y+2])&(1<<(player+2))) ){//depth 3, first branch (yxy)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==2) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth3);
}
else {		//normal spot
value = value + depth3;
}
}
				if ( (y<11) && !((marker=grid[x+1][y+3])&(1<<(player+2))) ){//(yxyy)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				safe_dist = ((safe_dist>3) ? 3 : safe_dist);
				}
				if ( (x<12) && !((marker=grid[x+2][y+2])&(1<<(player+2))) ){//(yxyx)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				safe_dist = ((safe_dist>3) ? 3 : safe_dist);
				}
				if ( !((marker=grid[x][y+2])&(1<<(player+2))) ){//(yxy-x)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				}
			safe_dist = ((safe_dist>2) ? 2 : safe_dist);
			}
			if ( (x<12) && !((marker=grid[x+2][y+1])&(1<<(player+2))) ){//depth 3, second branch (yxx)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==2) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth3);
}
else {		//normal spot
value = value + depth3;
}
}
				if ( (y<12) && !((marker=grid[x+2][y+2])&(1<<(player+2))) ){//(yxxy)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				safe_dist = ((safe_dist>3) ? 3 : safe_dist);
				}
				if ( !((marker=grid[x+2][y])&(1<<(player+2))) ){//(yxx-y)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				safe_dist = ((safe_dist>3) ? 3 : safe_dist);
				}
				if ( (x<11) && !((marker=grid[x+3][y+1])&(1<<(player+2))) ){//(yxxx)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				}
			safe_dist = ((safe_dist>2) ? 2 : safe_dist);
			}
			if ( !((marker=grid[x+1][y])&(1<<(player+2))) ){//depth 3, third branch (yx-y)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==2) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth3);
}
else {		//normal spot
value = value + depth3;
}
}
				if ( (y>0) && !((marker=grid[x+1][y-1])&(1<<(player+2))) ){//(yx-y-y)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				safe_dist = ((safe_dist>3) ? 3 : safe_dist);
				}
				if ( (x<12) && !((marker=grid[x+2][y])&(1<<(player+2))) ){//(yx-yx)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				}
				//(yx-y-x) would be return to initial liberty - trivially available
			}
		safe_dist = ((safe_dist>1) ? 1 : safe_dist);
		}
		if ( (x>0) && !((marker=grid[x-1][y+1])&(1<<(player+2))) ){//depth 2, third branch (y-x)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==1) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth2);
}
else {		//normal spot
value = value + depth2;
}
}
			if ( (y<12) && !((marker=grid[x-1][y+2])&(1<<(player+2))) ){//depth 3, first branch (y-xy)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==2) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth3);
}
else {		//normal spot
value = value + depth3;
}
}
				if ( (y<11) && !((marker=grid[x-1][y+3])&(1<<(player+2))) ){//(y-xyy)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				safe_dist = ((safe_dist>3) ? 3 : safe_dist);
				}
				if ( (x>1) && !((marker=grid[x-2][y+2])&(1<<(player+2))) ){//(y-xy-x)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				safe_dist = ((safe_dist>3) ? 3 : safe_dist);
				}
				if ( !((marker=grid[x][y+2])&(1<<(player+2))) ){//(y-xyx)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				}
			safe_dist = ((safe_dist>2) ? 2 : safe_dist);
			}
			if ( (x>1) && !((marker=grid[x-2][y+1])&(1<<(player+2))) ){//depth 3, second branch (y-x-x)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==2) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth3);
}
else {		//normal spot
value = value + depth3;
}
}
				if ( (y<12) && !((marker=grid[x-2][y+2])&(1<<(player+2))) ){//(y-x-xy)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				safe_dist = ((safe_dist>3) ? 3 : safe_dist);
				}
				if ( !((marker=grid[x-2][y])&(1<<(player+2))) ){//(y-x-x-y)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				safe_dist = ((safe_dist>3) ? 3 : safe_dist);
				}
				if ( (x>2) && !((marker=grid[x-3][y+1])&(1<<(player+2))) ){//(y-x-x-x)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				}
			safe_dist = ((safe_dist>2) ? 2 : safe_dist);
			}
			if ( !((marker=grid[x-1][y])&(1<<(player+2))) ){//depth 3, third branch (y-x-y)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==2) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth3);
}
else {		//normal spot
value = value + depth3;
}
}
				if ( (y>0) && !((marker=grid[x-1][y-1])&(1<<(player+2))) ){//(y-x-y-y)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				safe_dist = ((safe_dist>3) ? 3 : safe_dist);
				}
				if ( (x>1) && !((marker=grid[x-2][y])&(1<<(player+2))) ){//(y-x-y-x)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				}
				//(y-x-yx) would be return to initial liberty - trivially available
			}
		}
	safe_dist = ((safe_dist>0) ? 0 : safe_dist);
	}
	
	if ( (y>0) && !((marker=grid[x][y-1])&(1<<(player+2))) ){//depth 1, fourth branch (-y)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==0) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth1);
}
else {		//normal spot
value = value + depth1;
}
}
		if ( (y>1) && !((marker=grid[x][y-2])&(1<<(player+2))) ){//depth 2, first branch (-y-y)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==1) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth2);
}
else {		//normal spot
value = value + depth2;
}
}
			if ( (y>2) && !((marker=grid[x][y-3])&(1<<(player+2))) ){//depth 3, first branch (-y-y-y)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==2) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth3);
}
else {		//normal spot
value = value + depth3;
}
}
				if ( (y>3) && !((marker=grid[x][y-4])&(1<<(player+2))) ){//(-y-y-y-y)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				safe_dist = ((safe_dist>3) ? 3 : safe_dist);
				}
				if ( (x<13) && !((marker=grid[x+1][y-3])&(1<<(player+2))) ){//(-y-y-yx)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				safe_dist = ((safe_dist>3) ? 3 : safe_dist);
				}
				if ( (x>0) && !((marker=grid[x-1][y-3])&(1<<(player+2))) ){//(-y-y-y-x)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				}
			safe_dist = ((safe_dist>2) ? 2 : safe_dist);
			}
			if ( (x<13) && !((marker=grid[x+1][y-2])&(1<<(player+2))) ){//depth 3, second branch (-y-yx)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==2) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth3);
}
else {		//normal spot
value = value + depth3;
}
}
				if ( (y>2) && !((marker=grid[x+1][y-3])&(1<<(player+2))) ){//(-y-yx-y)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				safe_dist = ((safe_dist>3) ? 3 : safe_dist);
				}
				if ( (x<12) && !((marker=grid[x+2][y-2])&(1<<(player+2))) ){//(-y-yxx)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				safe_dist = ((safe_dist>3) ? 3 : safe_dist);
				}
				if ( !((marker=grid[x+1][y-1])&(1<<(player+2))) ){//(-y-yxy)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				}
			safe_dist = ((safe_dist>2) ? 2 : safe_dist);
			}
			if ( (x>0) && !((marker=grid[x-1][y-2])&(1<<(player+2))) ){//depth 3, third branch (-y-y-x)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==2) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth3);
}
else {		//normal spot
value = value + depth3;
}
}
				if ( (y>2) && !((marker=grid[x-1][y-3])&(1<<(player+2))) ){//(-y-y-x-y)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				safe_dist = ((safe_dist>3) ? 3 : safe_dist);
				}
				if ( (x>1) && !((marker=grid[x-2][y-2])&(1<<(player+2))) ){//(-y-y-x-x)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				safe_dist = ((safe_dist>3) ? 3 : safe_dist);
				}
				if ( !((marker=grid[x-1][y-1])&(1<<(player+2))) ){//(-y-y-xy)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				}
			}
		safe_dist = ((safe_dist>1) ? 1 : safe_dist);
		}
		if ( (x<13) && !((marker=grid[x+1][y-1])&(1<<(player+2))) ){//depth 2, second branch (-yx)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==1) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth2);
}
else {		//normal spot
value = value + depth2;
}
}
			if ( (y>1) && !((marker=grid[x+1][y-2])&(1<<(player+2))) ){//depth 3, first branch (-yx-y)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==2) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth3);
}
else {		//normal spot
value = value + depth3;
}
}
				if ( (y>2) && !((marker=grid[x+1][y-3])&(1<<(player+2))) ){//(-yx-y-y)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				safe_dist = ((safe_dist>3) ? 3 : safe_dist);
				}
				if ( (x<12) && !((marker=grid[x+2][y-2])&(1<<(player+2))) ){//(-yx-yx)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				safe_dist = ((safe_dist>3) ? 3 : safe_dist);
				}
				if ( !((marker=grid[x][y-2])&(1<<(player+2))) ){//(-yx-y-x)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				}
			safe_dist = ((safe_dist>2) ? 2 : safe_dist);
			}
			if ( (x<12) && !((marker=grid[x+2][y-1])&(1<<(player+2))) ){//depth 3, second branch (-yxx)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==2) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth3);
}
else {		//normal spot
value = value + depth3;
}
}
				if ( (y>1) && !((marker=grid[x+2][y-2])&(1<<(player+2))) ){//(-yxx-y)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				safe_dist = ((safe_dist>3) ? 3 : safe_dist);
				}
				if ( !((marker=grid[x+2][y])&(1<<(player+2))) ){//(-yxxy)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				safe_dist = ((safe_dist>3) ? 3 : safe_dist);
				}
				if ( (x<11) && !((marker=grid[x+3][y-1])&(1<<(player+2))) ){//(-yxxx)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				}
			safe_dist = ((safe_dist>2) ? 2 : safe_dist);
			}
			if ( !((marker=grid[x+1][y])&(1<<(player+2))) ){//depth 3, third branch (-yxy)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==2) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth3);
}
else {		//normal spot
value = value + depth3;
}
}
				if ( (y<13) && !((marker=grid[x+1][y+1])&(1<<(player+2))) ){//(-yxyy)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				safe_dist = ((safe_dist>3) ? 3 : safe_dist);
				}
				if ( (x<12) && !((marker=grid[x+2][y])&(1<<(player+2))) ){//(-yxyx)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				}
				//(-yxy-x) would be return to initial libertx - triviallx available
			}
		safe_dist = ((safe_dist>1) ? 1 : safe_dist);
		}
		if ( (x>0) && !((marker=grid[x-1][y-1])&(1<<(player+2))) ){//depth 2, third branch (-y-x)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==1) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth2);
}
else {		//normal spot
value = value + depth2;
}
}
			if ( (y>1) && !((marker=grid[x-1][y-2])&(1<<(player+2))) ){//depth 3, first branch (-y-x-y)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==2) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth3);
}
else {		//normal spot
value = value + depth3;
}
}
				if ( (y>2) && !((marker=grid[x-1][y-3])&(1<<(player+2))) ){//(-y-x-y-y)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				safe_dist = ((safe_dist>3) ? 3 : safe_dist);
				}
				if ( (x>1) && !((marker=grid[x-2][y-2])&(1<<(player+2))) ){//(-y-x-y-x)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				safe_dist = ((safe_dist>3) ? 3 : safe_dist);
				}
				if ( !((marker=grid[x][y-2])&(1<<(player+2))) ){//(-y-x-yx)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				}
			safe_dist = ((safe_dist>2) ? 2 : safe_dist);
			}
			if ( (x>1) && !((marker=grid[x-2][y-1])&(1<<(player+2))) ){//depth 3, second branch (-y-x-x)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==2) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth3);
}
else {		//normal spot
value = value + depth3;
}
}
				if ( (y>1) && !((marker=grid[x-2][y-2])&(1<<(player+2))) ){//(-y-x-x-y)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				safe_dist = ((safe_dist>3) ? 3 : safe_dist);
				}
				if ( !((marker=grid[x-2][y])&(1<<(player+2))) ){//(-y-x-xy)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				safe_dist = ((safe_dist>3) ? 3 : safe_dist);
				}
				if ( (x>2) && !((marker=grid[x-3][y-1])&(1<<(player+2))) ){//(-y-x-x-x)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				}
			safe_dist = ((safe_dist>2) ? 2 : safe_dist);
			}
			if ( !((marker=grid[x-1][y])&(1<<(player+2))) ){//depth 3, third branch (-y-xy)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==2) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth3);
}
else {		//normal spot
value = value + depth3;
}
}
				if ( (y<13) && !((marker=grid[x-1][y+1])&(1<<(player+2))) ){//(-y-xyy)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				safe_dist = ((safe_dist>3) ? 3 : safe_dist);
				}
				if ( (x>1) && !((marker=grid[x-2][y])&(1<<(player+2))) ){//(-y-xy-x)
if ( (marker&(1<<(5-player)) )||(marker&(1<<(7-player)) )||(marker&(1<<(9-player)) )//(1-player)+2*i+4
||(marker&(1<<(11-player)) ) ){	//liberty for other player?
lib_other++;
}
if ( ( marker&(1<<(player+4)) )||( marker&(1<<(player+6)) )||( marker&(1<<(player+8)) )//player+2*i+4, i=0,1,2,3
||( marker&(1<<(player+10)) ) ){;}	//Stop searching that direction if hitting an own liberty -> double counting
else {	if ((safe_dist==3) && (marker&(1<<(3-player))) ){		//safe to play up to that point without possible interference
safe_dist++;
value = value + (safe_value * depth4);
}
else {		//normal spot
value = value + depth4;
}
}
				}
				//(-y-xyx) would be return to initial liberty - trivially available
			}
		}
	}
	safe_dist=-1;

	float result = value/normalization;	//hitting liberties from other player (fighting liberties!) not taken into account
	return result;
}
