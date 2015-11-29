#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <pthread.h>
#include <unistd.h>

//#define MT
//#define ANN_DATA

#define GRID_COVER_NONE 0

// Extended board format 
#define EX_GRID_NOT_SAFE    2
#define EX_GRID_LBTY     4

#define TRUE (1==1)
#define FALSE (!TRUE)

#define MAX_THREADS 16

typedef short dtype;

typedef struct {
  int x;
  int y;
  int piece;
  int rotate;
} move;

#ifdef MT
typedef struct{
	dtype board[16][16];
	dtype available[2][21];
	int score[2];
	int player;
	float utility;
	int depth;
	float alpha;
	float beta;
	int moveindex;
	int completed;
} MtGameState;
#endif

int score[2];
int player_max, player_min;
int liberty_mask; //liberty 
int notsafe_mask; 

extern int piece_sizes[21];
extern int first_player;
extern int piece_priority[21];
extern int piece_y[21][8][4];
extern int piece_x[21][8][4];
extern int first_turn_p1;
extern int first_turn_p2;
extern int first_player;

#ifdef ANN_DATA
#include "ann_struct.h"
FILE* fp;
#endif

extern int timeval_subtract(struct timeval *a, struct timeval *b);

void reformatBoard( const dtype board_cur[16][16], const dtype available_cur[2][21], dtype board_new[16][16], dtype available_new[2][21] );

move getminimaxmove( const dtype board[16][16], const dtype available[2][21], const int player, const dtype maxsearchdepth); 

#ifdef MT
move getminimaxmove_mt( const dtype board[16][16], const dtype available[2][21], const int player, const dtype maxsearchdepth);

void* getminimaxutility (void* thdata);
#endif

void encode_move(const move m,char * code);

int getMoveList( move moves[], const dtype board[16][16], const dtype available[2][21], const int player, const int score[2] );

void simulateMove( const move m, const dtype board[16][16], const dtype available[2][21], const int score[2], const int player, 
						dtype n_board[16][16], dtype n_available[2][21], int n_score[2], int* n_player );

float minimax( const dtype board[16][16], const dtype available[2][21], const int score[2], const int player, 
				const int depth, float alpha, float beta );

int isMoveAvailable( const dtype board[16][16], const dtype available[2][21], const int player );

int Isvalidmove(const move m, const dtype board[16][16], const dtype available[2][21], const int player);

float evalfunc ( const dtype board[16][16], const dtype available[2][21], const int score[2], const int player);

	
float libValue_uni(const dtype board[16][16], const int x, const int y, const int player);

#ifdef ANN_DATA
void store_ann_data(const dtype board[16][16], const dtype available[2][21], const int score_diff, const int player, const float BoardScore);
#endif

void make_move(int player, int turn, char* code, int board[16][16], int available[2][21]){	

	dtype board_cur[16][16];
	dtype available_cur[2][21];
	int x,y;
	move m;

	struct timeval start, stop;
	gettimeofday(&start, NULL);

	player_max = player;
	player_min = 3-player;
	
	if(turn<2){
		if(player==1){
			//m.piece = (rand()%12) + 9;
			m.piece = first_turn_p1;
			m.rotate = 0;
			m.x = 5;
			m.y = 5;
			encode_move(m,code);
			
		}
		else{
			//m.piece = (rand()%12) + 9;
			m.piece = first_turn_p2;
			m.rotate = 0;
			m.x = 10;
			m.y = 10;
			encode_move(m,code);
		}
		printf("\n%s\n",code);
		//if(player_max==1) printf("\n$:%d\n",m.piece);
		score[player-1] = 5;
		return;
	}

	#ifdef ANN_DATA
		if((fp = fopen(FILENAME_ANN_DATA, "a")) == NULL)
			{
			printf("\nCould not open file to store ann data\n");
			return;
		}
	#endif
	
	for(y=0;y<16;y++){
		for(x=0;x<16;x++){
			board_cur[y][x] = board[y][x];
		}
	}
	for(y=0;y<2;y++){
		for(x=0;x<21;x++){
			available_cur[y][x] = available[y][x];
		}
	}

	dtype board_new[16][16]; 
	dtype available_new[2][21];

	dtype maxsearchdepth;
	
	if(player == 1) maxsearchdepth = 2;
	else maxsearchdepth = 2;

	reformatBoard( board_cur, available_cur, board_new, available_new );

	#ifndef MT
		m = getminimaxmove (board_new, available_new, player, maxsearchdepth-1);
	#else
		m = getminimaxmove_mt(board_new, available_new, player, maxsearchdepth-1);
	#endif
	
	encode_move(m,code);
	printf("\n%s\n",code);
	//if(player_max==1) printf("\n$:%d\n",m.piece);
	#ifdef ANN_DATA
		fclose(fp);
	#endif
	
	gettimeofday(&stop, NULL);
	printf("\nTime: %d msec\n",timeval_subtract(&stop, &start));
	
	return;
}


void reformatBoard( const dtype board_cur[16][16], const dtype available_cur[2][21], dtype board_new[16][16], dtype available_new[2][21] ){
	int p,i,j;
	for( p = 0; p < 2; p++ ){
		for( i = 0; i < 21; i++ ){
			available_new[p][i] = available_cur[p][i];
		}
	}
	
	for( i = 0; i < 16; i++ ){
		for( j = 0; j < 16; j++ ){
			board_new[i][j] = board_cur[i][j];
		}
	}

	// Convert board to extended format
	for( i = 1; i < 15; i++ )
		for( j = 1; j < 15; j++ )
		{
			// Check if grid square is covered
			if( board_cur[i][j] != GRID_COVER_NONE )
			{
				// Mark tile as unsafe for all
				board_new[i][j] = board_cur[i][j];
				board_new[i][j] |= (0x3<<EX_GRID_NOT_SAFE);
			}

			// Check for liberties and safeties
			else  
			{ 
				// Check for adjacent covered tile
				if( (j<14) && board_cur[i][j+1]!=GRID_COVER_NONE )
					board_new[i][j] |= ((board_cur[i][j+1])<<EX_GRID_NOT_SAFE);
				
				if( (i<14) && board_cur[i+1][j]!=GRID_COVER_NONE )
					board_new[i][j] |= ((board_cur[i+1][j])<<EX_GRID_NOT_SAFE);
				
				if( (j>1) && board_cur[i][j-1]!=GRID_COVER_NONE )
					board_new[i][j] |= ((board_cur[i][j-1])<<EX_GRID_NOT_SAFE);
				
				if( (i>1) && board_cur[i-1][j]!=GRID_COVER_NONE ) 
					board_new[i][j] |= ((board_cur[i-1][j])<<EX_GRID_NOT_SAFE);
				
				// Check if this tile is a liberty for any players
				if( (i>1) && (j<14) && (board_cur[i-1][j+1]!=GRID_COVER_NONE) && !((board_new[i][j])&((board_cur[i-1][j+1])<<EX_GRID_NOT_SAFE)) ) 
					board_new[i][j] |= ((board_cur[i-1][j+1]) << EX_GRID_LBTY);
				
				if( (i<14) && (j<14) && (board_cur[i+1][j+1]!=GRID_COVER_NONE) && !((board_new[i][j])&((board_cur[i+1][j+1])<<EX_GRID_NOT_SAFE)) ) 
					board_new[i][j] |= ((board_cur[i+1][j+1]) << EX_GRID_LBTY);
				
				if( (i<14) && (j>1) && (board_cur[i+1][j-1]!=GRID_COVER_NONE) && !((board_new[i][j])&((board_cur[i+1][j-1])<<EX_GRID_NOT_SAFE)) ) 
					board_new[i][j] |= ((board_cur[i+1][j-1]) << EX_GRID_LBTY);
				
				if( (i>1) && (j>1) && (board_cur[i-1][j-1]!=GRID_COVER_NONE) && !((board_new[i][j])&((board_cur[i-1][j-1])<<EX_GRID_NOT_SAFE)) ) 
					board_new[i][j] |= ((board_cur[i-1][j-1]) << EX_GRID_LBTY);
			}
		}
}

void encode_move(const move m,char * code){

*code = ((m.x<10) ? m.x+'0' : m.x+'a'-10);
*(code+1) = ((m.y<10) ? m.y+'0' : m.y+'a'-10);
*(code+2) =  m.piece+'a';
*(code+3) =  m.rotate+'0';
*(code+4) = 0;

}

move getminimaxmove( const dtype board[16][16], const dtype available[2][21], const int player, const dtype maxsearchdepth){
	
	move moves[2000];
	int movesFound, i;

	movesFound = getMoveList( moves, board, available, player, score );
	
	int move_num = -1; float alpha = -FLT_MAX, beta = FLT_MAX; 

	for( i = 0; i < movesFound; i++ ){

		dtype n_board[16][16]; dtype n_available[2][21]; int n_score[2]; int n_player;

		simulateMove( moves[i], board, available, score, player, 
						n_board, n_available, n_score, &n_player );
		float newUtility = minimax( n_board, n_available, n_score, n_player, maxsearchdepth, alpha, beta );
		
		if( player == player_max ) { if( newUtility > alpha ) { alpha = newUtility; move_num = i; } }
		else if( newUtility < beta ) { beta = newUtility; move_num = i; }
	}
	
	if(move_num == -1) move_num = 0;
	move m = moves[move_num];
	score[player-1] += piece_sizes[m.piece];
	return m;
}

#ifdef MT

move getminimaxmove_mt( const dtype board[16][16], const dtype available[2][21], const int player, const dtype maxsearchdepth)
{

	// Create thread state arrays
	MtGameState threadStates[MAX_THREADS];
	pthread_t pth[MAX_THREADS];
	
	// Get available moves list
	move moves[2000];
	int movesFound;
	
	movesFound = getMoveList( moves, board, available, player, score );
	
	int move_num = -1; float alpha = -FLT_MAX, beta = FLT_MAX; 

	// Set thread count limit 
	int nThreads = MAX_THREADS;
	if( nThreads > movesFound )	nThreads = movesFound;

	//init thread data
	int i = 0;
	for( i = 0; i < nThreads; i++ ) {
		
		threadStates[i].completed = TRUE;
		threadStates[i].depth = maxsearchdepth;
		threadStates[i].utility = (player == player_max) ? alpha : beta; 
	}

	// Get utility values for each move
	int nextMoveIndex = 0;
	while( nextMoveIndex < movesFound+nThreads )
	{
		// Check for a completed thread
		for( i = 0; i < nThreads; i++ ){
			if( threadStates[i].completed ){
				
				// Mark completed thread
				threadStates[i].completed = FALSE;

				// Update alpha-beta parameters
				if( player == player_max ) { 
					if( threadStates[i].utility > alpha ) 	{ 
						alpha = threadStates[i].utility; move_num = threadStates[i].moveindex; } 
				}
				else { 
					if( threadStates[i].utility < beta ) { 
						beta = threadStates[i].utility; move_num = threadStates[i].moveindex; 
					}
				}

				// Check for all threads initiated
				if( nextMoveIndex < movesFound )
				{
					// Store the new threads move index
					threadStates[i].moveindex = nextMoveIndex;

					// Set new alpha-beta parameters
					threadStates[i].alpha = alpha; threadStates[i].beta = beta;

					// Simulate the selected move on the board for minimax 
					simulateMove( moves[nextMoveIndex], board, available, score, player, 
								threadStates[i].board, threadStates[i].available, threadStates[i].score, &threadStates[i].player );

					// Begin the utility ranking thread
					pthread_create(&(pth[i]),NULL,&getminimaxutility,&(threadStates[i]));
				}

				// Increment index
				nextMoveIndex++; 
			} 
		}
		// Yield CPU
		//Sleep( 5 );
	}
	
	if(move_num == -1) move_num = 0;
	move m = moves[move_num];
	score[player-1] += piece_sizes[m.piece];
	return m;
}

void* getminimaxutility (void* thdata){
	
	MtGameState* state = (MtGameState*)thdata;
	state->utility = minimax(state->board, state->available, state->score, state->player, state->depth, state->alpha, state->beta);
	state->completed = TRUE;
}

#endif

float minimax( const dtype board[16][16], const dtype available[2][21], const int score[2], const int player, 
				const int depth, float alpha, float beta ){
	
	if(depth == 0){
		float utility = evalfunc ( board, available, score, player_max);
		return utility;
	}

	move moves[2000];
	int movesFound, i,j;
	movesFound = getMoveList( moves, board, available, player, score );

	if(movesFound == 0){
	
		dtype n_board[16][16]; dtype n_available[2][21]; int n_score[2]; int n_player;

		// Copy board data to output
		for( i = 0; i < 16; i++ )
			for( j = 0; j < 16; j++ )
				n_board[i][j] = board[i][j];
		
		// Copy piece data to output
		for( i = 0; i < 2; i++ )
			for( j = 0; j < 21; j++ )
				n_available[i][j] = available[i][j];
			
		// Copy piece data to output array
		for( i = 0; i < 2; i++ ) n_score[i] = score[i];

		n_player = 3-player;
		
		float newUtility = minimax( n_board, n_available, n_score, n_player, depth-1, alpha, beta );
		
		if( player == player_max ) { 
			if( newUtility > alpha ) {
				alpha = newUtility; 
			}
		}
		else if( newUtility < beta ) { 
			beta = newUtility;
		}
		
		return (player==player_max) ? alpha : beta;		
		
	}

	for( i = 0; i < movesFound; i++ ){

		dtype n_board[16][16]; dtype n_available[2][21]; int n_score[2]; int n_player;
		
		simulateMove( moves[i], board, available, score, player, 
						n_board, n_available, n_score, &n_player );

		float newUtility = minimax( n_board, n_available, n_score, n_player, depth-1, alpha, beta );
		
		if( player == player_max ) { 
			if( newUtility > alpha ) {
				alpha = newUtility; 
			}
		}
		else if( newUtility < beta ) { 
			beta = newUtility;
		}

		//prune
		if( beta <= alpha ) break;		
	}
	return (player==player_max) ? alpha : beta;	
}

int getMoveList( move moves[], const dtype board[16][16], const dtype available[2][21], const int player, const int score[2] ){
	
	move m;
	int i = 0, piece, piece_min;

  	liberty_mask = (player<<EX_GRID_LBTY);
	notsafe_mask = (player<<EX_GRID_NOT_SAFE);
	
	if(score[player-1] < 30) piece_min = 9;
	else piece_min = 0;
	
	for (piece=20; piece>=piece_min; piece--){
		
		/*nilim hw changes*/
		//m.piece = piece_priority[piece];
		m.piece = piece;

		// Check availability
		if(available[player-1][m.piece] == 0){
	  		continue;
		}
		
		for(m.rotate=0; m.rotate<8; m.rotate++){
			for (m.y=1; m.y<15; m.y++){
				for (m.x=1; m.x<15; m.x++){
					if(Isvalidmove(m, board, available, player)){
						moves[i] = m;
						i++;
					}
				}
			}
		}
	}
	return i;
}

int Isvalidmove(const move m, const dtype board[16][16], const dtype available[2][21], const int player){
	
	int xoff, yoff, size;
	dtype board_t;

	int lib_check = FALSE;	
	size = piece_sizes[m.piece]-1;							
	yoff = m.y;
	xoff = m.x;

	do{
		board_t = board[yoff][xoff];
		if( (board_t & notsafe_mask) ||	(yoff < 1) || (14 < yoff) || (xoff < 1) || (14 < xoff) ) return FALSE;
		if(board_t & liberty_mask)	lib_check = TRUE;
		--size;
		if(size<0) break;
		yoff = m.y + piece_y[m.piece][m.rotate][size];
		xoff = m.x + piece_x[m.piece][m.rotate][size]; 
	}while(1);
	
	return lib_check;
}

void simulateMove( const move m, const dtype board[16][16], const dtype available[2][21], const int score[2], const int player, 
						dtype n_board[16][16], dtype n_available[2][21], int n_score[2], int* n_player )
{
	int i,j;
	// Copy board data to output
	for( i = 0; i < 16; i++ )
		for( j = 0; j < 16; j++ )
			n_board[i][j] = board[i][j];

	// Copy piece data to output
	for( i = 0; i < 2; i++ )
		for( j = 0; j < 21; j++ )
			n_available[i][j] = available[i][j];

	// Update piece registry
	n_available[player-1][m.piece] = 0;

	// Copy piece data to output array
	for( i = 0; i < 2; i++ ) n_score[i] = score[i];

	// Update player score variable
	n_score[player-1] += piece_sizes[m.piece];
	
	// Update game grid
	int xoff, yoff, size;

	size = piece_sizes[m.piece]-1;
	yoff = m.y;
	xoff = m.x;
	
	do{
		n_board[yoff][xoff] = player | (0x3<<EX_GRID_NOT_SAFE);
		
		if(yoff<14) n_board[yoff+1][xoff] |= (player<<EX_GRID_NOT_SAFE);
		if(yoff>1)	n_board[yoff-1][xoff] |= (player<<EX_GRID_NOT_SAFE);
		if(xoff<14) n_board[yoff][xoff+1] |= (player<<EX_GRID_NOT_SAFE);
		if(xoff>1)	n_board[yoff][xoff-1] |= (player<<EX_GRID_NOT_SAFE);

		if((yoff>1)&&(xoff<14)) 	n_board[yoff-1][xoff+1] |= (player<<EX_GRID_LBTY);
		if((yoff<14)&&(xoff<14))	n_board[yoff+1][xoff+1] |= (player<<EX_GRID_LBTY);
		if((yoff<14)&&(xoff>1)) 	n_board[yoff+1][xoff-1] |= (player<<EX_GRID_LBTY);
		if((yoff>1)&&(xoff>1))		n_board[yoff-1][xoff-1] |= (player<<EX_GRID_LBTY);

		
		--size;
		if(size<0) break;
		yoff = m.y+piece_y[m.piece][m.rotate][size];
		xoff = m.x+piece_x[m.piece][m.rotate][size];
	}while(1);

	*n_player  = 3-player;
}

int isMoveAvailable( const dtype board[16][16], const dtype available[2][21], const int player ){
	
	move m;
	
	liberty_mask = (player<<EX_GRID_LBTY);
	notsafe_mask = (player<<EX_GRID_NOT_SAFE);
	
	for (m.piece=20; m.piece>=0; m.piece--){		
		if(available[player-1][m.piece] == 0){
	  		continue;
		}
		for(m.rotate=0; m.rotate<8; m.rotate++){
			for (m.y=1; m.y<15; m.y++){
				for (m.x=1; m.x<15; m.x++){
					if(Isvalidmove(m, board, available, player)){
						return TRUE;
					}
				}
			}
		}
	}
	return FALSE;
}

float evalfunc ( const dtype board[16][16], const dtype available[2][21], const int score[2], const int player){
	//Consider the Liberties, i.e. the squares where each player can play
	float lib_MAX=0;
	float lib_MIN=0;
	/*nilim hw changes*/
	float weight_Lib = 3; //0;     //specifier for how many placed pieces an entirely free liberty makes up

	int x, y, self, opp;
	self = player;
	opp = 3 - player;
	
	for (x=1; x<15; x++){
		for (y=1; y<15; y++){
			
			if( (!(board[x][y] & self<<EX_GRID_NOT_SAFE)) && ( (board[x][y])&(self<<EX_GRID_LBTY) ) ){         
				//check whether it's a liberty for the current player given the board position (x,y)
				lib_MAX += libValue_uni(board, x, y, player-1);     //(player -1) is being passed so as not to change the heuristic function
			}
			
			if( (!(board[x][y] & opp<<EX_GRID_NOT_SAFE)) && ( (board[x][y])&(opp<<EX_GRID_LBTY) ) ){		 
				//check whether it's a liberty for the opposing player given the board position (x,y)
				lib_MIN += libValue_uni(board, x, y, 2-player);  //((3-player) -1) is being passed so as not to change the heuristic function
			}
		}
	}
	
	//Calculate the heuristic value out of the current score and the amount of space around liberties
	float BoardScore=0;
	BoardScore = (score[player-1] - score[2-player])*68 + weight_Lib * (lib_MAX - lib_MIN);
	
	#ifdef ANN_DATA
		int score_diff  = score[player-1] - score[2-player];
		store_ann_data(board, available, score_diff, player, lib_MAX);
		store_ann_data(board, available, score_diff, 3-player, lib_MIN);
	#endif
	
	return BoardScore;
}

float libValue_uni(const dtype grid[16][16], const int x, const int y, const int player){ //changed board to grid so not to change function

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

	if ( (x<14) && !(grid[x+1][y]&(1<<(player+2))) ){//depth 1, first branch (x)
		value++;
		if ( (x<13) && !(grid[x+2][y]&(1<<(player+2))) ){//depth 2, first branch (xx)
			value++;
			if ( (x<12) && !(grid[x+3][y]&(1<<(player+2))) ){//depth 3, first branch (xxx)
				value++;
				if ( (x<11) && !(grid[x+4][y]&(1<<(player+2))) ){//(xxxx)
					value++;
				}
				if ( (y<14) && !(grid[x+3][y+1]&(1<<(player+2))) ){//(xxxy)
					value++;
				}
				if ( (y>1) && !(grid[x+3][y-1]&(1<<(player+2))) ){//(xxx-y)
					value++;
				}
			}
			if ( (y<14) && !(grid[x+2][y+1]&(1<<(player+2))) ){//depth 3, second branch (xxy)
				value++;
				if ( (x<12) && !(grid[x+3][y+1]&(1<<(player+2))) ){//(xxyx)
					value++;
				}
				if ( (y<13) && !(grid[x+2][y+2]&(1<<(player+2))) ){//(xxyy)
					value++;
				}
				if ( !(grid[x+1][y+1]&(1<<(player+2))) ){//(xxy-x)
					value++;
				}
			}
			if ( (y>1) && !(grid[x+2][y-1]&(1<<(player+2))) ){//depth 3, third branch (xx-y)
				value++;
				if ( (x<12) && !(grid[x+3][y-1]&(1<<(player+2))) ){//(xx-yx)
					value++;
				}
				if ( (y>2) && !(grid[x+2][y-2]&(1<<(player+2))) ){//(xx-y-y)
					value++;
				}
				if ( !(grid[x+1][y-1]&(1<<(player+2))) ){//(xx-y-x)
					value++;
				}
			}
		}
		if ( (y<14) && !(grid[x+1][y+1]&(1<<(player+2))) ){//depth 2, second branch (xy)
			value++;
			if ( (x<13) && !(grid[x+2][y+1]&(1<<(player+2))) ){//depth 3, first branch (xyx)
				value++;
				if ( (x<12) && !(grid[x+3][y+1]&(1<<(player+2))) ){//(xyxx)
					value++;
				}
				if ( (y<13) && !(grid[x+2][y+2]&(1<<(player+2))) ){//(xyxy)
					value++;
				}
				if ( !(grid[x+2][y]&(1<<(player+2))) ){//(xyx-y)
					value++;
				}
			}
			if ( (y<13) && !(grid[x+1][y+2]&(1<<(player+2))) ){//depth 3, second branch (xyy)
				value++;
				if ( (x<13) && !(grid[x+2][y+2]&(1<<(player+2))) ){//(xyyx)
					value++;
				}
				if ( !(grid[x][y+2]&(1<<(player+2))) ){//(xyy-x)
					value++;
				}
				if ( (y<12) && !(grid[x+1][y+3]&(1<<(player+2))) ){//(xyyy)
					value++;
				}
			}
			if ( !(grid[x][y+1]&(1<<(player+2))) ){//depth 3, third branch (xy-x)
				value++;
				if ( (x>1) && !(grid[x-1][y+1]&(1<<(player+2))) ){//(xy-x-x)
					value++;
				}
				if ( (y<13) && !(grid[x][y+2]&(1<<(player+2))) ){//(xy-xy)
					value++;
				}
				//(xy-x-y) would be return to initial liberty - trivially available
			}
		}
		if ( (y>1) && !(grid[x+1][y-1]&(1<<(player+2))) ){//depth 2, third branch (x-y)
			value++;
			if ( (x<13) && !(grid[x+2][y-1]&(1<<(player+2))) ){//depth 3, first branch (x-yx)
				value++;
				if ( (x<12) && !(grid[x+3][y-1]&(1<<(player+2))) ){//(x-yxx)
					value++;
				}
				if ( (y>2) && !(grid[x+2][y-2]&(1<<(player+2))) ){//(x-yx-y)
					value++;
				}
				if ( !(grid[x+2][y]&(1<<(player+2))) ){//(x-yxy)
					value++;
				}
			}
			if ( (y>2) && !(grid[x+1][y-2]&(1<<(player+2))) ){//depth 3, second branch (x-y-y)
				value++;
				if ( (x<13) && !(grid[x+2][y-2]&(1<<(player+2))) ){//(x-y-yx)
					value++;
				}
				if ( !(grid[x][y-2]&(1<<(player+2))) ){//(x-y-y-x)
					value++;
				}
				if ( (y>3) && !(grid[x+1][y-3]&(1<<(player+2))) ){//(x-y-y-y)
					value++;
				}
			}
			if ( !(grid[x][y-1]&(1<<(player+2))) ){//depth 3, third branch (x-y-x)
				value++;
				if ( (x>1) && !(grid[x-1][y-1]&(1<<(player+2))) ){//(x-y-x-x)
					value++;
				}
				if ( (y>2) && !(grid[x][y-2]&(1<<(player+2))) ){//(x-y-x-y)
					value++;
				}
				//(x-y-xy) would be return to initial liberty - trivially available
			}
		}
	}
	
	if ( (x>1) && !(grid[x-1][y]&(1<<(player+2))) ){//depth 1, second branch (-x)
		value++;
		if ( (x>2) && !(grid[x-2][y]&(1<<(player+2))) ){//depth 2, first branch (-x-x)
			value++;
			if ( (x>3) && !(grid[x-3][y]&(1<<(player+2))) ){//depth 3, first branch (-x-x-x)
				value++;
				if ( (x>4) && !(grid[x-4][y]&(1<<(player+2))) ){//(-x-x-x-x)
					value++;
				}
				if ( (y<14) && !(grid[x-3][y+1]&(1<<(player+2))) ){//(-x-x-xy)
					value++;
				}
				if ( (y>1) && !(grid[x-3][y-1]&(1<<(player+2))) ){//(-x-x-x-y)
					value++;
				}
			}
			if ( (y<14) && !(grid[x-2][y+1]&(1<<(player+2))) ){//depth 3, second branch (-x-xy)
				value++;
				if ( (x>3) && !(grid[x-3][y+1]&(1<<(player+2))) ){//(-x-xy-x)
					value++;
				}
				if ( (y<13) && !(grid[x-2][y+2]&(1<<(player+2))) ){//(-x-xyy)
					value++;
				}
				if ( !(grid[x-1][y+1]&(1<<(player+2))) ){//(-x-xyx)
					value++;
				}
			}
			if ( (y>1) && !(grid[x-2][y-1]&(1<<(player+2))) ){//depth 3, third branch (-x-x-y)
				value++;
				if ( (x>3) && !(grid[x-3][y-1]&(1<<(player+2))) ){//(-x-x-y-x)
					value++;
				}
				if ( (y>2) && !(grid[x-2][y-2]&(1<<(player+2))) ){//(-x-x-y-y)
					value++;
				}
				if ( !(grid[x-1][y-1]&(1<<(player+2))) ){//(-x-x-yx)
					value++;
				}
			}
		}
		if ( (y<14) && !(grid[x-1][y+1]&(1<<(player+2))) ){//depth 2, second branch (-xy)
			value++;
			if ( (x>2) && !(grid[x-2][y+1]&(1<<(player+2))) ){//depth 3, first branch (-xy-x)
				value++;
				if ( (x>3) && !(grid[x-3][y+1]&(1<<(player+2))) ){//(-xy-x-x)
					value++;
				}
				if ( (y<13) && !(grid[x-2][y+2]&(1<<(player+2))) ){//(-xy-xy)
					value++;
				}
				if ( !(grid[x-2][y]&(1<<(player+2))) ){//(-xy-x-y)
					value++;
				}
			}
			if ( (y<13) && !(grid[x-1][y+2]&(1<<(player+2))) ){//depth 3, second branch (-xyy)
				value++;
				if ( (x>2) && !(grid[x-2][y+2]&(1<<(player+2))) ){//(-xyy-x)
					value++;
				}
				if ( !(grid[x][y+2]&(1<<(player+2))) ){//(-xyyx)
					value++;
				}
				if ( (y<12) && !(grid[x-1][y+3]&(1<<(player+2))) ){//(-xyyy)
					value++;
				}
			}
			if ( !(grid[x][y+1]&(1<<(player+2))) ){//depth 3, third branch (-xyx)
				value++;
				if ( (x<14) && !(grid[x+1][y+1]&(1<<(player+2))) ){//(-xyxx)
					value++;
				}
				if ( (y<13) && !(grid[x][y+2]&(1<<(player+2))) ){//(-xyxy)
					value++;
				}
				//(-xyx-y) would be return to initial liberty - trivially available
			}
		}
		if ( (y>1) && !(grid[x-1][y-1]&(1<<(player+2))) ){//depth 2, third branch (-x-y)
			value++;
			if ( (x>2) && !(grid[x-2][y-1]&(1<<(player+2))) ){//depth 3, first branch (-x-y-x)
				value++;
				if ( (x>3) && !(grid[x-3][y-1]&(1<<(player+2))) ){//(-x-y-x-x)
					value++;
				}
				if ( (y>2) && !(grid[x-2][y-2]&(1<<(player+2))) ){//(-x-y-x-y)
					value++;
				}
				if ( !(grid[x-2][y]&(1<<(player+2))) ){//(-x-y-xy)
					value++;
				}
			}
			if ( (y>2) && !(grid[x-1][y-2]&(1<<(player+2))) ){//depth 3, second branch (-x-y-y)
				value++;
				if ( (x>2) && !(grid[x-2][y-2]&(1<<(player+2))) ){//(-x-y-y-x)
					value++;
				}
				if ( !(grid[x][y-2]&(1<<(player+2))) ){//(-x-y-yx)
					value++;
				}
				if ( (y>3) && !(grid[x-1][y-3]&(1<<(player+2))) ){//(-x-y-y-y)
					value++;
				}
			}
			if ( !(grid[x][y-1]&(1<<(player+2))) ){//depth 3, third branch (-x-yx)
				value++;
				if ( (x<14) && !(grid[x+1][y-1]&(1<<(player+2))) ){//(-x-yxx)
					value++;
				}
				if ( (y>2) && !(grid[x][y-2]&(1<<(player+2))) ){//(-x-yx-y)
					value++;
				}
				//(-x-yxy) would be return to initial liberty - trivially available
			}
		}
	}

	if ( (y<14) && !(grid[x][y+1]&(1<<(player+2))) ){//depth 1, third branch (y)
		value++;
		if ( (y<13) && !(grid[x][y+2]&(1<<(player+2))) ){//depth 2, first branch (yy)
			value++;
			if ( (y<12) && !(grid[x][y+3]&(1<<(player+2))) ){//depth 3, first branch (yyy)
				value++;
				if ( (y<11) && !(grid[x][y+4]&(1<<(player+2))) ){//(yyyy)
					value++;
				}
				if ( (x<14) && !(grid[x+1][y+3]&(1<<(player+2))) ){//(yyyx)
					value++;
				}
				if ( (x>1) && !(grid[x-1][y+3]&(1<<(player+2))) ){//(yyy-x)
					value++;
				}
			}
			if ( (x<14) && !(grid[x+1][y+2]&(1<<(player+2))) ){//depth 3, second branch (yyx)
				value++;
				if ( (y<12) && !(grid[x+1][y+3]&(1<<(player+2))) ){//(yyxy)
					value++;
				}
				if ( (x<13) && !(grid[x+2][y+2]&(1<<(player+2))) ){//(yyxx)
					value++;
				}
				if ( !(grid[x+1][y+1]&(1<<(player+2))) ){//(yyx-y)
					value++;
				}
			}
			if ( (x>1) && !(grid[x-1][y+2]&(1<<(player+2))) ){//depth 3, third branch (yy-x)
				value++;
				if ( (y<12) && !(grid[x-1][y+3]&(1<<(player+2))) ){//(yy-xy)
					value++;
				}
				if ( (x>2) && !(grid[x-2][y+2]&(1<<(player+2))) ){//(yy-x-x)
					value++;
				}
				if ( !(grid[x-1][y+1]&(1<<(player+2))) ){//(yy-x-y)
					value++;
				}
			}
		}
		if ( (x<14) && !(grid[x+1][y+1]&(1<<(player+2))) ){//depth 2, second branch (yx)
			value++;
			if ( (y<13) && !(grid[x+1][y+2]&(1<<(player+2))) ){//depth 3, first branch (yxy)
				value++;
				if ( (y<12) && !(grid[x+1][y+3]&(1<<(player+2))) ){//(yxyy)
					value++;
				}
				if ( (x<13) && !(grid[x+2][y+2]&(1<<(player+2))) ){//(yxyx)
					value++;
				}
				if ( !(grid[x][y+2]&(1<<(player+2))) ){//(yxy-x)
					value++;
				}
			}
			if ( (x<13) && !(grid[x+2][y+1]&(1<<(player+2))) ){//depth 3, second branch (yxx)
				value++;
				if ( (y<13) && !(grid[x+2][y+2]&(1<<(player+2))) ){//(yxxy)
					value++;
				}
				if ( !(grid[x+2][y]&(1<<(player+2))) ){//(yxx-y)
					value++;
				}
				if ( (x<12) && !(grid[x+3][y+1]&(1<<(player+2))) ){//(yxxx)
					value++;
				}
			}
			if ( !(grid[x+1][y]&(1<<(player+2))) ){//depth 3, third branch (yx-y)
				value++;
				if ( (y>1) && !(grid[x+1][y-1]&(1<<(player+2))) ){//(yx-y-y)
					value++;
				}
				if ( (x<13) && !(grid[x+2][y]&(1<<(player+2))) ){//(yx-yx)
					value++;
				}
				//(yx-y-x) would be return to initial liberty - trivially available
			}
		}
		if ( (x>1) && !(grid[x-1][y+1]&(1<<(player+2))) ){//depth 2, third branch (y-x)
			value++;
			if ( (y<13) && !(grid[x-1][y+2]&(1<<(player+2))) ){//depth 3, first branch (y-xy)
				value++;
				if ( (y<12) && !(grid[x-1][y+3]&(1<<(player+2))) ){//(y-xyy)
					value++;
				}
				if ( (x>2) && !(grid[x-2][y+2]&(1<<(player+2))) ){//(y-xy-x)
					value++;
				}
				if ( !(grid[x][y+2]&(1<<(player+2))) ){//(y-xyx)
					value++;
				}
			}
			if ( (x>2) && !(grid[x-2][y+1]&(1<<(player+2))) ){//depth 3, second branch (y-x-x)
				value++;
				if ( (y<13) && !(grid[x-2][y+2]&(1<<(player+2))) ){//(y-x-xy)
					value++;
				}
				if ( !(grid[x-2][y]&(1<<(player+2))) ){//(y-x-x-y)
					value++;
				}
				if ( (x>3) && !(grid[x-3][y+1]&(1<<(player+2))) ){//(y-x-x-x)
					value++;
				}
			}
			if ( !(grid[x-1][y]&(1<<(player+2))) ){//depth 3, third branch (y-x-y)
				value++;
				if ( (y>1) && !(grid[x-1][y-1]&(1<<(player+2))) ){//(y-x-y-y)
					value++;
				}
				if ( (x>2) && !(grid[x-2][y]&(1<<(player+2))) ){//(y-x-y-x)
					value++;
				}
				//(y-x-yx) would be return to initial liberty - trivially available
			}
		}
	}
	
	if ( (y>1) && !(grid[x][y-1]&(1<<(player+2))) ){//depth 1, fourth branch (-y)
		value++;
		if ( (y>2) && !(grid[x][y-2]&(1<<(player+2))) ){//depth 2, first branch (-y-y)
			value++;
			if ( (y>3) && !(grid[x][y-3]&(1<<(player+2))) ){//depth 3, first branch (-y-y-y)
				value++;
				if ( (y>4) && !(grid[x][y-4]&(1<<(player+2))) ){//(-y-y-y-y)
					value++;
				}
				if ( (x<14) && !(grid[x+1][y-3]&(1<<(player+2))) ){//(-y-y-yx)
					value++;
				}
				if ( (x>1) && !(grid[x-1][y-3]&(1<<(player+2))) ){//(-y-y-y-x)
					value++;
				}
			}
			if ( (x<14) && !(grid[x+1][y-2]&(1<<(player+2))) ){//depth 3, second branch (-y-yx)
				value++;
				if ( (y>3) && !(grid[x+1][y-3]&(1<<(player+2))) ){//(-y-yx-y)
					value++;
				}
				if ( (x<13) && !(grid[x+2][y-2]&(1<<(player+2))) ){//(-y-yxx)
					value++;
				}
				if ( !(grid[x+1][y-1]&(1<<(player+2))) ){//(-y-yxy)
					value++;
				}
			}
			if ( (x>1) && !(grid[x-1][y-2]&(1<<(player+2))) ){//depth 3, third branch (-y-y-x)
				value++;
				if ( (y>3) && !(grid[x-1][y-3]&(1<<(player+2))) ){//(-y-y-x-y)
					value++;
				}
				if ( (x>2) && !(grid[x-2][y-2]&(1<<(player+2))) ){//(-y-y-x-x)
					value++;
				}
				if ( !(grid[x-1][y-1]&(1<<(player+2))) ){//(-y-y-xy)
					value++;
				}
			}
		}
		if ( (x<14) && !(grid[x+1][y-1]&(1<<(player+2))) ){//depth 2, second branch (-yx)
			value++;
			if ( (y>2) && !(grid[x+1][y-2]&(1<<(player+2))) ){//depth 3, first branch (-yx-y)
				value++;
				if ( (y>3) && !(grid[x+1][y-3]&(1<<(player+2))) ){//(-yx-y-y)
					value++;
				}
				if ( (x<13) && !(grid[x+2][y-2]&(1<<(player+2))) ){//(-yx-yx)
					value++;
				}
				if ( !(grid[x][y-2]&(1<<(player+2))) ){//(-yx-y-x)
					value++;
				}
			}
			if ( (x<13) && !(grid[x+2][y-1]&(1<<(player+2))) ){//depth 3, second branch (-yxx)
				value++;
				if ( (y>2) && !(grid[x+2][y-2]&(1<<(player+2))) ){//(-yxx-y)
					value++;
				}
				if ( !(grid[x+2][y]&(1<<(player+2))) ){//(-yxxy)
					value++;
				}
				if ( (x<12) && !(grid[x+3][y-1]&(1<<(player+2))) ){//(-yxxx)
					value++;
				}
			}
			if ( !(grid[x+1][y]&(1<<(player+2))) ){//depth 3, third branch (-yxy)
				value++;
				if ( (y<14) && !(grid[x+1][y+1]&(1<<(player+2))) ){//(-yxyy)
					value++;
				}
				if ( (x<13) && !(grid[x+2][y]&(1<<(player+2))) ){//(-yxyx)
					value++;
				}
				//(-yxy-x) would be return to initial libertx - triviallx available
			}
		}
		if ( (x>1) && !(grid[x-1][y-1]&(1<<(player+2))) ){//depth 2, third branch (-y-x)
			value++;
			if ( (y>2) && !(grid[x-1][y-2]&(1<<(player+2))) ){//depth 3, first branch (-y-x-y)
				value++;
				if ( (y>3) && !(grid[x-1][y-3]&(1<<(player+2))) ){//(-y-x-y-y)
					value++;
				}
				if ( (x>2) && !(grid[x-2][y-2]&(1<<(player+2))) ){//(-y-x-y-x)
					value++;
				}
				if ( !(grid[x][y-2]&(1<<(player+2))) ){//(-y-x-yx)
					value++;
				}
			}
			if ( (x>2) && !(grid[x-2][y-1]&(1<<(player+2))) ){//depth 3, second branch (-y-x-x)
				value++;
				if ( (y>2) && !(grid[x-2][y-2]&(1<<(player+2))) ){//(-y-x-x-y)
					value++;
				}
				if ( !(grid[x-2][y]&(1<<(player+2))) ){//(-y-x-xy)
					value++;
				}
				if ( (x>3) && !(grid[x-3][y-1]&(1<<(player+2))) ){//(-y-x-x-x)
					value++;
				}
			}
			if ( !(grid[x-1][y]&(1<<(player+2))) ){//depth 3, third branch (-y-xy)
				value++;
				if ( (y<14) && !(grid[x-1][y+1]&(1<<(player+2))) ){//(-y-xyy)
					value++;
				}
				if ( (x>2) && !(grid[x-2][y]&(1<<(player+2))) ){//(-y-xy-x)
					value++;
				}
				//(-y-xyx) would be return to initial liberty - trivially available
			}
		}
	}
	//float result = ((float) value)/68;
	float result = ((float) value);
	return result;
}

#ifdef ANN_DATA
void store_ann_data(const dtype board[16][16], const dtype available[2][21], const int score_diff, const int player, const float BoardScore){

	ann_struct ann_data;
	int i,j;
	for(i = 1; i<15; i++){
		for(j = 1; j<15; j++){
			ann_data.board[i-1][j-1] = board[i][j];
		}
	}

	int p;

	for( p = 0; p < 2; p++ ){
		for( i = 0; i < 3; i++ ){
		ann_data.available[p][i] = 0;
		}
	}
	
	for( p = 0; p < 2; p++ ) {
		for( i = 0; i < 3; i++ ){
			for( j = 0; j < 8; j++ ){
				if( 8*i+j >= 21 ) continue;
				else ann_data.available[p][i] |= (available[p][8*i+j] << j);
			}
		}
	}

	ann_data.score_diff = score_diff;
	ann_data.player = player;
	ann_data.BoardScore = BoardScore;
	ann_data.check = -1;
	if((fwrite(&ann_data, sizeof(ann_struct), 1, fp)) != 1){
		printf("\nCould not write ann struct\n");
		}
}
#endif

