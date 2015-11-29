#include "co.h"

#define SELF 1
#define OPP 2
#define BORDER 3

#define EX_GRID_NOT_SAFE 2
#define EX_GRID_LBTY 4

#define NOT_SAFE_MASK_SELF SELF<<EX_GRID_NOT_SAFE
#define NOT_SAFE_MASK_OPP  OPP<<EX_GRID_NOT_SAFE
#define LIBERTY_MASK_SELF  SELF<<EX_GRID_LBTY
#define LIBERTY_MASK_OPP   OPP<<EX_GRID_LBTY

#define FALSE 0
#define TRUE 1

#define X_OFF 0
#define Y_OFF 4
#define PIECE_OFF 8
#define ROTATE_OFF 13
#define UTILITY_OFF 16
#define TYPE_OFF 24

#define X_MASK 0x0F
#define Y_MASK 0x0F
#define PIECE_MASK 0x1F
#define ROTATE_MASK 0x07
#define UTILITY_MASK 0xFFFF
#define TYPE_MASK 0xFF

//TYPE_OFF data
#define PERM_MOVE_SELF 				1
#define PERM_MOVE_OPP 				2
#define MINIMAX_MOVE_SELF 			3
#define MINIMAX_MOVE_OPP 			4
#define MINIMAX_MOVE_SELF2 			5
#define MINIMAX_MOVE_EVAL 			6
#define MINIMAX_MOVE_SELF_DONE 		7
#define MINIMAX_MOVE_OPP_DONE 		8
#define MINIMAX_MOVE_SELF2_DONE 	9
#define GAME_RESET 					10

#define WEIGHT_LIB 3

//DIV = 4
//0.8 * 50,000,000 / 4 = 10000000
#define TIMER_COUNT_MAX 10000000
#define TIMER_RUN 1
#define TIMER_STOP 2

#define SEARCH_EXTRA_LIMIT 9

extern void consumer_func(co_stream input_stream);
extern void producer_func(co_stream output_stream);

void make_move_func(co_stream input_stream, co_stream output_stream, 
							co_stream stream1_move_output, co_stream stream1_move_input,
							co_stream stream2_move_output, co_stream stream2_move_input,
							co_stream timer_output_stream, co_stream timer_input_stream
							)
{
	co_int8 piece_sizes[21] = { 1, 2, 3, 3, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5 };
	  
	co_int8 piece_y[21][8][4] = {
	  { { 0, 0, 0, 0 },  { 0, 0, 0, 0 },  { 0, 0, 0, 0 },  { 0, 0, 0, 0 },	{ 0, 0, 0, 0 },  { 0, 0, 0, 0 },  { 0, 0, 0, 0 },  { 0, 0, 0, 0 },	},
	  { { 1, 0, 0, 0 },  { 1, 0, 0, 0 },  { 0, 0, 0, 0 },  { 0, 0, 0, 0 },	{ -1, 0, 0, 0 }, { -1, 0, 0, 0 }, { 0, 0, 0, 0 },  { 0, 0, 0, 0 },	},
	  { { -1, 1, 0, 0 }, { -1, 1, 0, 0 }, { 0, 0, 0, 0 },  { 0, 0, 0, 0 },	{ -1, 1, 0, 0 }, { -1, 1, 0, 0 }, { 0, 0, 0, 0 },  { 0, 0, 0, 0 },	},
	  { { -1, 0, 0, 0 }, { -1, 0, 0, 0 }, { 0, 1, 0, 0 },  { 0, 1, 0, 0 },	{ 0, 1, 0, 0 },  { 0, 1, 0, 0 },  { -1, 0, 0, 0 }, { -1, 0, 0, 0 }, },
	  { { -1, 1, 2, 0 }, { -1, 1, 2, 0 }, { 0, 0, 0, 0 },  { 0, 0, 0, 0 },	{ -2, -1, 1, 0 },{ -2, -1, 1, 0 },{ 0, 0, 0, 0 },  { 0, 0, 0, 0 },	},
	  { { -1, 1, 1, 0 }, { -1, 1, 1, 0 }, { -1, 0, 0, 0 }, { -1, 0, 0, 0 }, { -1, -1, 1, 0 },{ -1, -1, 1, 0 },{ 0, 0, 1, 0 },  { 0, 0, 1, 0 },	},
	  { { -1, 0, 1, 0 }, { -1, 0, 1, 0 }, { 0, 0, 1, 0 },  { 0, 0, 1, 0 },	{ -1, 0, 1, 0 }, { -1, 0, 1, 0 }, { -1, 0, 0, 0 }, { -1, 0, 0, 0 }, },
	  { { 0, 1, 1, 0 },  { 0, 1, 1, 0 },  { 0, 1, 1, 0 },  { 0, 1, 1, 0 },	{ -1, -1, 0, 0 },{ -1, -1, 0, 0 },{ -1, -1, 0, 0 },{ -1, -1, 0, 0 },},
	  { { 0, 1, 1, 0 },  { 0, 1, 1, 0 },  { -1, 0, 1, 0 }, { -1, 0, 1, 0 }, { -1, -1, 0, 0 },{ -1, -1, 0, 0 },{ -1, 0, 1, 0 }, { -1, 0, 1, 0 }, },
	  { { -2, -1, 1, 2 },{ -2, -1, 1, 2 },{ 0, 0, 0, 0 },  { 0, 0, 0, 0 },	{ -2, -1, 1, 2 },{ -2, -1, 1, 2 },{ 0, 0, 0, 0 },  { 0, 0, 0, 0 },	},
	  { { -2, -1, 1, 1 },{ -2, -1, 1, 1 },{ -1, 0, 0, 0 }, { -1, 0, 0, 0 }, { -1, -1, 1, 2 },{ -1, -1, 1, 2 },{ 0, 0, 0, 1 },  { 0, 0, 0, 1 },	},
	  { { -2, -1, 0, 1 },{ -2, -1, 0, 1 },{ -1, -1, 0, 0 },{ -1, -1, 0, 0 },{ -1, 0, 1, 2 }, { -1, 0, 1, 2 }, { 0, 0, 1, 1 },  { 0, 0, 1, 1 },	},
	  { { -1, 0, 1, 1 }, { -1, 0, 1, 1 }, { -1, -1, 0, 0 },{ -1, -1, 0, 0 },{ -1, -1, 0, 1 },{ -1, -1, 0, 1 },{ 0, 0, 1, 1 },  { 0, 0, 1, 1 },	},
	  { { -1, -1, 1, 1 },{ -1, -1, 1, 1 },{ -1, -1, 0, 0 },{ -1, -1, 0, 0 },{ -1, -1, 1, 1 },{ -1, -1, 1, 1 },{ 0, 0, 1, 1 },  { 0, 0, 1, 1 },	},
	  { { -1, 0, 1, 2 }, { -1, 0, 1, 2 }, { 0, 0, 0, 1 },  { 0, 0, 0, 1 },	{ -2, -1, 0, 1 },{ -2, -1, 0, 1 },{ -1, 0, 0, 0 }, { -1, 0, 0, 0 }, },
	  { { -1, 1, 1, 1 }, { -1, 1, 1, 1 }, { -1, 0, 0, 1 }, { -1, 0, 0, 1 }, { -1, -1, -1, 1},{ -1, -1, -1, 1},{ -1, 0, 0, 1 }, { -1, 0, 0, 1 }, },
	  { { -2, -1, 0, 0 },{ -2, -1, 0, 0 },{ 0, 0, 1, 2 },  { 0, 0, 1, 2 },	{ 0, 0, 1, 2 },  { 0, 0, 1, 2 },  { -2, -1, 0, 0 },{ -2, -1, 0, 0 },},
	  { { -1, -1, 0, 1 },{ -1, -1, 0, 1 },{ -1, 0, 1, 1 }, { -1, 0, 1, 1 }, { -1, 0, 1, 1 }, { -1, 0, 1, 1 }, { -1, -1, 0, 1 },{ -1, -1, 0, 1 },},
	  { { -1, 0, 0, 1 }, { -1, 0, 0, 1 }, { -1, -1, 1, 1 },{ -1, -1, 1, 1 },{ -1, 0, 0, 1 }, { -1, 0, 0, 1 }, { -1, -1, 1, 1 },{ -1, -1, 1, 1 },},
	  { { -1, 0, 0, 1 }, { -1, 0, 0, 1 }, { -1, -1, 0, 1 },{ -1, -1, 0, 1 },{ -1, 0, 0, 1 }, { -1, 0, 0, 1 }, { -1, 0, 1, 1 }, { -1, 0, 1, 1 }, },
	  { { -1, 0, 0, 1 }, { -1, 0, 0, 1 }, { -1, 0, 0, 1 }, { -1, 0, 0, 1 }, { -1, 0, 0, 1 }, { -1, 0, 0, 1 }, { -1, 0, 0, 1 }, { -1, 0, 0, 1 }, },
	};
	
	co_int8 piece_x[21][8][4] = {
	  { { 0, 0, 0, 0 },  { 0, 0, 0, 0 },  { 0, 0, 0, 0 },  { 0, 0, 0, 0 },	{ 0, 0, 0, 0 },  { 0, 0, 0, 0 },	{ 0, 0, 0, 0 },  { 0, 0, 0, 0 },   },
	  { { 0, 0, 0, 0 },  { 0, 0, 0, 0 },  { -1, 0, 0, 0 }, { 1, 0, 0, 0 },	{ 0, 0, 0, 0 },  { 0, 0, 0, 0 },	{ 1, 0, 0, 0 },  { -1, 0, 0, 0 },  },
	  { { 0, 0, 0, 0 },  { 0, 0, 0, 0 },  { -1, 1, 0, 0 }, { -1, 1, 0, 0 }, { 0, 0, 0, 0 },  { 0, 0, 0, 0 },	{ -1, 1, 0, 0 }, { -1, 1, 0, 0 },  },
	  { { 0, 1, 0, 0 },  { 0, -1, 0, 0 }, { 1, 0, 0, 0 },  { -1, 0, 0, 0 }, { -1, 0, 0, 0 }, { 1, 0, 0, 0 },	{ 0, -1, 0, 0 }, { 0, 1, 0, 0 },   },
	  { { 0, 0, 0, 0 },  { 0, 0, 0, 0 },  { -2, -1, 1, 0 },{ -1, 1, 2, 0 }, { 0, 0, 0, 0 },  { 0, 0, 0, 0 },	{ -1, 1, 2, 0 }, { -2, -1, 1, 0 }, },
	  { { 0, -1, 0, 0 }, { 0, 0, 1, 0 },  { -1, -1, 1, 0 },{ 1, -1, 1, 0 }, { 0, 1, 0, 0 },  { -1, 0, 0, 0 },	{ -1, 1, 1, 0 }, { -1, 1, -1, 0 }, },
	  { { 0, 1, 0, 0 },  { 0, -1, 0, 0 }, { -1, 1, 0, 0 }, { -1, 1, 0, 0 }, { 0, -1, 0, 0 }, { 0, 1, 0, 0 },	{ 0, -1, 1, 0 }, { 0, -1, 1, 0 },  },
	  { { 1, 0, 1, 0 },  { -1, -1, 0, 0 },{ -1, -1, 0, 0 },{ 1, 0, 1, 0 },	{ -1, 0, -1, 0 },{ 0, 1, 1, 0 },	{ 0, 1, 1, 0 },  { -1, 0, -1, 0 }, },
	  { { -1, 0, 1, 0 }, { 1, -1, 0, 0 }, { 0, -1, -1, 0 },{ 0, 1, 1, 0 },	{ -1, 0, 1, 0 }, { 0, 1, -1, 0 },	{ 1, 1, 0, 0 },  { -1, -1, 0, 0 }, },
	  { { 0, 0, 0, 0 },  { 0, 0, 0, 0 },  { -2, -1, 1, 2 },{ -2, -1, 1, 2 },{ 0, 0, 0, 0 },  { 0, 0, 0, 0 },	{ -2, -1, 1, 2 },{ -2, -1, 1, 2 }, },
	  { { 0, 0, -1, 0 }, { 0, 0, 0, 1 },  { -1, -1, 1, 2 },{ 1, -2, -1, 1 },{ 0, 1, 0, 0 },  { -1, 0, 0, 0 },	{ -2, -1, 1, 1 },{ -1, 1, 2, -1 }, },
	  { { 0, 0, -1, -1 },{ 0, 0, 1, 1 },  { -1, 0, 1, 2 }, { 0, 1, -2, -1 },{ 1, 1, 0, 0 },  { -1, -1, 0, 0 },	{ -2, -1, 0, 1 },{ 1, 2, -1, 0 },  },
	  { { 0, -1, -1, 0 },{ 0, 1, 0, 1 },  { -1, 0, -1, 1 },{ 0, 1, -1, 1 }, { 0, 1, 1, 0 },  { -1, 0, -1, 0 },	{ -1, 1, 0, 1 }, { -1, 1, -1, 0 }, },
	  { { -1, 0, -1, 0 },{ 0, 1, 0, 1 },  { -1, 1, -1, 1 },{ -1, 1, -1, 1 },{ 0, 1, 0, 1 },  { -1, 0, -1, 0 },	{ -1, 1, -1, 1 },{ -1, 1, -1, 1 }, },
	  { { 0, 1, 0, 0 },  { 0, -1, 0, 0 }, { -2, -1, 1, 0 },{ -1, 1, 2, 0 }, { 0, 0, -1, 0 }, { 0, 0, 1, 0 },	{ 0, -1, 1, 2 }, { 0, -2, -1, 1 }, },
	  { { 0, -1, 0, 1 }, { 0, -1, 0, 1 }, { -1, -1, 1, -1},{ 1, -1, 1, 1 }, { -1, 0, 1, 0 }, { -1, 0, 1, 0 },	{ 1, -1, 1, 1 }, { -1, -1, 1, -1 },},
	  { { 0, 0, 1, 2 },  { 0, 0, -2, -1 },{ 1, 2, 0, 0 },  { -2, -1, 0, 0 },{ -2, -1, 0, 0 },{ 1, 2, 0, 0 },	{ 0, 0, -2, -1 },{ 0, 0, 1, 2 },   },
	  { { -1, 0, 1, 1 }, { 0, 1, -1, -1 },{ 1, 1, -1, 0 }, { -1, -1, 0, 1 },{ -1, -1, 0, 1 },{ 1, 1, -1, 0 },	{ 0, 1, -1, -1 },{ -1, 0, 1, 1 },  },
	  { { -1, -1, 1, 1 },{ 1, -1, 1, -1 },{ 0, 1, -1, 0 }, { -1, 0, 0, 1 }, { -1, -1, 1, 1 },{ 1, -1, 1, -1 },	{ 0, 1, -1, 0 }, { -1, 0, 0, 1 },  },
	  { { -1, -1, 1, 0 },{ 1, -1, 1, 0 }, { 0, 1, -1, 0 }, { -1, 0, 1, 0 }, { 0, -1, 1, 1 }, { 0, -1, 1, -1 },	{ 0, 1, -1, 0 }, { 0, -1, 0, 1 },  },
	  { { 0, -1, 1, 0 }, { 0, -1, 1, 0 }, { 0, -1, 1, 0 }, { 0, -1, 1, 0 }, { 0, -1, 1, 0 }, { 0, -1, 1, 0 },	{ 0, -1, 1, 0 }, { 0, -1, 1, 0 },  },
	};
	  

	co_int8 board[32][32];
	co_int24 avail_self, avail_opp;
	co_int8 score_self, score_opp;
	co_int8 input_code_0, input_code_1, input_code_2, input_code_3, temp_code;
	co_int8 response_code_0, response_code_1, response_code_2, response_code_3;
  	co_int8 m_x, m_y, m_piece, m_rotate, best_move_x, best_move_y, best_move_piece, best_move_rotate;
	co_int8 x, y, z, xoff, yoff, board_t, size, temp, m_piece_min, count, timer_t, next_stream;
	co_int8 x1, y1, board_t1;
	co_int2 moves_found, move_valid, lib_check, state, avail, time_rem;
	co_int32 m, m1, m_recv;
	co_int16 utility, alpha;

	co_array_config(board, co_kind, "dualsync"); 

	//open streams
	co_stream_open(stream1_move_output, O_WRONLY, INT_TYPE(32));	co_stream_open(stream1_move_input, O_RDONLY, INT_TYPE(32));
	co_stream_open(stream2_move_output, O_WRONLY, INT_TYPE(32));	co_stream_open(stream2_move_input, O_RDONLY, INT_TYPE(32));
	co_stream_open(timer_output_stream, O_WRONLY, INT_TYPE(8));		co_stream_open(timer_input_stream, O_RDONLY, INT_TYPE(8)); 

	//open UART streams
	co_stream_open(input_stream, O_RDONLY, INT_TYPE(8));
	co_stream_open(output_stream, O_WRONLY, INT_TYPE(8));
	
	//init board
	for(y = 0; y < 32; y++){
		for(x = 0; x < 32; x++){
			if((x < 4) || (y < 4) || (x > 17) || (y > 17)){
				board[y][x] = (BORDER | (0x3<<EX_GRID_NOT_SAFE));
			}
			else {
				board[y][x] = 0;
			}	
		}
	}

	//init available pieces
	avail_self = 0x7FFFFF;
	avail_opp = 0x7FFFFF;
	
	//init scores
	score_self = 0;
	score_opp = 0;

	temp_code = -1;
	
while(1){

	//read first character if input code
	co_stream_read(input_stream, &input_code_0, sizeof(co_int8));

	//
	if(input_code_0 == 	'4')	{
		temp_code = '4';
		input_code_0 = '3';		
	}
	else {
		temp_code = -1;
	}
	
	// Do something with the data stream here
	
	switch (input_code_0){
		
		//initial turn. send team number
		case '0':
			
			response_code_0 ='1';
			response_code_1 ='C';
			response_code_2 ='L';

			co_stream_write(output_stream,&response_code_0,sizeof(co_int8));
			co_stream_write(output_stream,&response_code_1,sizeof(co_int8));
			co_stream_write(output_stream,&response_code_2,sizeof(co_int8));

			break;
			
		case '2':
			
			//first move
			co_stream_read(input_stream, &input_code_0, sizeof(co_int8));

			//first move coordinates
			//Player 1
			if(input_code_0 == '5'){
		
				response_code_0='5';
				response_code_1='5';
				response_code_2='u';
				response_code_3='0';
				}
			//Player 2
			else if (input_code_0 == 'A'){

				response_code_0='a';
				response_code_1='a';
				response_code_2='u';
				response_code_3='0';
				}

			co_stream_write(output_stream,&response_code_0,sizeof(co_int8));
			co_stream_write(output_stream,&response_code_1,sizeof(co_int8));
			co_stream_write(output_stream,&response_code_2,sizeof(co_int8));
			co_stream_write(output_stream,&response_code_3,sizeof(co_int8));
			
			//decode move
			
			if('0'<=response_code_0 && response_code_0<='9') m_x = response_code_0-'0';			
			else if('a'<=response_code_0 && response_code_0<='e') m_x = response_code_0-'a'+10;
			
			if('0'<=response_code_1 && response_code_1<='9') m_y = response_code_1-'0';			
			else if('a'<=response_code_1 && response_code_1<='e') m_y = response_code_1-'a'+10;
			
			m_piece = response_code_2-'a';
			m_rotate = response_code_3-'0';

			//forward move
			m = m_piece<<PIECE_OFF|m_rotate<<ROTATE_OFF|m_x<<X_OFF|m_y<<Y_OFF|PERM_MOVE_SELF<<TYPE_OFF;
			co_stream_write(stream1_move_output, &m, sizeof(co_int32));
			co_stream_write(stream2_move_output, &m, sizeof(co_int32));
			
			//update board self
			m_y += 3;
			m_x += 3;
			
			size = piece_sizes[m_piece];
			avail_self &= ~(1<<m_piece);
			score_self += size;
			--size;
			yoff = m_y;
			xoff = m_x;
			do{
				board[yoff][xoff] = SELF | (0x3<<EX_GRID_NOT_SAFE);
				
				y = yoff+1; x = xoff; y1 = yoff+1; x1 = xoff+1; board_t = board[y][x]; board_t1 = board[y1][x1]; 
				board[y][x] = board_t|(SELF<<EX_GRID_NOT_SAFE);	board[y1][x1] = board_t1|(SELF<<EX_GRID_LBTY);
				
				y = yoff-1; x = xoff; y1 = yoff-1; x1 = xoff-1;	board_t = board[y][x]; board_t1 = board[y1][x1]; 
				board[y][x] = board_t|(SELF<<EX_GRID_NOT_SAFE); board[y1][x1] = board_t1|(SELF<<EX_GRID_LBTY);			
				
				y = yoff; x = xoff+1; y1 = yoff-1; x1 = xoff+1;	board_t = board[y][x]; board_t1 = board[y1][x1];
				board[y][x] = board_t|(SELF<<EX_GRID_NOT_SAFE); board[y1][x1] = board_t1|(SELF<<EX_GRID_LBTY);
				
				y = yoff; x = xoff-1; y1 = yoff+1; x1 = xoff-1;	board_t = board[y][x]; board_t1 = board[y1][x1];
				board[y][x] = board_t|(SELF<<EX_GRID_NOT_SAFE); board[y1][x1] = board_t1|(SELF<<EX_GRID_LBTY);
			
				--size;
				if(size<0) break;
				yoff = m_y+piece_y[m_piece][m_rotate][size];
				xoff = m_x+piece_x[m_piece][m_rotate][size];
				}while(1);

			break;
			
		case '3':
			
			//turn > 1
			//start timer
			time_rem = TRUE;	timer_t = TIMER_RUN;
			co_stream_write(timer_output_stream,&timer_t,sizeof(co_int8));
			//delay to consume any incoming message
			for(z=0; z<5; z++){
				co_stream_read_nb(timer_input_stream,&timer_t,sizeof(co_int8));
			}	

			//receive opp move
			co_stream_read(input_stream, &input_code_0, sizeof(co_int8));
			co_stream_read(input_stream, &input_code_1, sizeof(co_int8));
			co_stream_read(input_stream, &input_code_2, sizeof(co_int8));
			co_stream_read(input_stream, &input_code_3, sizeof(co_int8));

			//decode opp move
			
			if('0'<=input_code_0 && input_code_0<='9') m_x = input_code_0-'0';
			else if('a'<=input_code_0 && input_code_0<='e') m_x = input_code_0-'a'+10;
			
			if('0'<=input_code_1 && input_code_1<='9') m_y = input_code_1-'0';
			else if('a'<=input_code_1 && input_code_1<='e') m_y = input_code_1-'a'+10;
			
			m_piece = input_code_2-'a';
			m_rotate = input_code_3-'0';

			if(m_x != 0){
				
				//forward move
				m = m_piece<<PIECE_OFF|m_rotate<<ROTATE_OFF|m_x<<X_OFF|m_y<<Y_OFF|PERM_MOVE_OPP<<TYPE_OFF;
				co_stream_write(stream1_move_output, &m, sizeof(co_int32));
				co_stream_write(stream2_move_output, &m, sizeof(co_int32));
				
				//update board opp
				m_y+=3;
				m_x+=3;
				
				size = piece_sizes[m_piece];
				avail_opp &= ~(1<<m_piece);
				score_opp += size;

				--size;
				yoff = m_y;
				xoff = m_x;
										
				do{
					board[yoff][xoff] = OPP | (0x3<<EX_GRID_NOT_SAFE);
					
					y = yoff+1; x = xoff; y1 = yoff+1; x1 = xoff+1; board_t = board[y][x]; board_t1 = board[y1][x1]; 
					board[y][x] = board_t|(OPP<<EX_GRID_NOT_SAFE);	board[y1][x1] = board_t1|(OPP<<EX_GRID_LBTY);
					
					y = yoff-1; x = xoff; y1 = yoff-1; x1 = xoff-1;	board_t = board[y][x]; board_t1 = board[y1][x1]; 
					board[y][x] = board_t|(OPP<<EX_GRID_NOT_SAFE); board[y1][x1] = board_t1|(OPP<<EX_GRID_LBTY);			
					
					y = yoff; x = xoff+1; y1 = yoff-1; x1 = xoff+1;	board_t = board[y][x]; board_t1 = board[y1][x1];
					board[y][x] = board_t|(OPP<<EX_GRID_NOT_SAFE); board[y1][x1] = board_t1|(OPP<<EX_GRID_LBTY);
					
					y = yoff; x = xoff-1; y1 = yoff+1; x1 = xoff-1; board_t = board[y][x]; board_t1 = board[y1][x1];
					board[y][x] = board_t|(OPP<<EX_GRID_NOT_SAFE); board[y1][x1] = board_t1|(OPP<<EX_GRID_LBTY);
					--size;
					
					if(size<0) break;
					yoff = m_y+piece_y[m_piece][m_rotate][size];
					xoff = m_x+piece_x[m_piece][m_rotate][size];					
				}while(1);
			}

			if(temp_code == '4'){
				
				co_stream_read(input_stream, &input_code_0, sizeof(co_int8));
				co_stream_read(input_stream, &input_code_1, sizeof(co_int8));
				co_stream_read(input_stream, &input_code_2, sizeof(co_int8));
				co_stream_read(input_stream, &input_code_3, sizeof(co_int8));
				
				//decode opp move
				
				if('0'<=input_code_0 && input_code_0<='9') m_x = input_code_0-'0';
				else if('a'<=input_code_0 && input_code_0<='e') m_x = input_code_0-'a'+10;
				
				if('0'<=input_code_1 && input_code_1<='9') m_y = input_code_1-'0';
				else if('a'<=input_code_1 && input_code_1<='e') m_y = input_code_1-'a'+10;
				
				m_piece = input_code_2-'a';
				m_rotate = input_code_3-'0';
				
				if(m_x != 0){
					
					//forward move
					m = m_piece<<PIECE_OFF|m_rotate<<ROTATE_OFF|m_x<<X_OFF|m_y<<Y_OFF|PERM_MOVE_OPP<<TYPE_OFF;
					co_stream_write(stream1_move_output, &m, sizeof(co_int32));
					co_stream_write(stream2_move_output, &m, sizeof(co_int32));					
					
					//update board opp
					m_y+=3;
					m_x+=3;
					
					size = piece_sizes[m_piece];
					avail_opp &= ~(1<<m_piece);
					score_opp += size;
				
					--size;
					yoff = m_y;
					xoff = m_x;
											
					do{
						board[yoff][xoff] = OPP | (0x3<<EX_GRID_NOT_SAFE);
						
						y = yoff+1; x = xoff; y1 = yoff+1; x1 = xoff+1; board_t = board[y][x]; board_t1 = board[y1][x1]; 
						board[y][x] = board_t|(OPP<<EX_GRID_NOT_SAFE);	board[y1][x1] = board_t1|(OPP<<EX_GRID_LBTY);
						
						y = yoff-1; x = xoff; y1 = yoff-1; x1 = xoff-1;	board_t = board[y][x]; board_t1 = board[y1][x1]; 
						board[y][x] = board_t|(OPP<<EX_GRID_NOT_SAFE); board[y1][x1] = board_t1|(OPP<<EX_GRID_LBTY);			
						
						y = yoff; x = xoff+1; y1 = yoff-1; x1 = xoff+1;	board_t = board[y][x]; board_t1 = board[y1][x1];
						board[y][x] = board_t|(OPP<<EX_GRID_NOT_SAFE); board[y1][x1] = board_t1|(OPP<<EX_GRID_LBTY);
						
						y = yoff; x = xoff-1; y1 = yoff+1; x1 = xoff-1; board_t = board[y][x]; board_t1 = board[y1][x1];
						board[y][x] = board_t|(OPP<<EX_GRID_NOT_SAFE); board[y1][x1] = board_t1|(OPP<<EX_GRID_LBTY);
						--size;
						
						if(size<0) break;
						yoff = m_y+piece_y[m_piece][m_rotate][size];
						xoff = m_x+piece_x[m_piece][m_rotate][size];
					}while(1);
				}
			}
			
			//find move
			moves_found  = FALSE;
			alpha = -32768;
			m_piece_min = 0; if(score_self<25) m_piece_min=9;
			state  = 0; count  = 0;	next_stream = 1;
			
			for(m_piece=20; (m_piece>=m_piece_min) & time_rem; m_piece--){
				avail = ((avail_self&(1<<m_piece))!=0);
				for(m_rotate=0; (m_rotate<8) & avail & time_rem; m_rotate++){
					for (m_y=4; (m_y<18) & time_rem; m_y++){
						for (m_x=4; (m_x<18) & time_rem; m_x++){
							 
							lib_check = FALSE;	
							size = piece_sizes[m_piece]-1;							
							yoff = m_y;
							xoff = m_x;
							do{
								board_t = board[yoff][xoff];
								move_valid = ((board_t & NOT_SAFE_MASK_SELF)==0);
								lib_check |= ((board_t & LIBERTY_MASK_SELF)!=0);
								--size;
								yoff = m_y + piece_y[m_piece][m_rotate][size];
								xoff = m_x + piece_x[m_piece][m_rotate][size]; 									
							}while((move_valid == TRUE) & (size>=0));
							
							if(move_valid & lib_check){ 

								//if valid move found
								moves_found = TRUE;
								m = m_piece<<PIECE_OFF|m_rotate<<ROTATE_OFF|(m_x-3)<<X_OFF|(m_y-3)<<Y_OFF|MINIMAX_MOVE_SELF<<TYPE_OFF;
								m1 = 0|(alpha<<UTILITY_OFF);
								if(state == 0){
									
									//forward moves, fill the pipeline
									if(next_stream == 1) {										
										co_stream_write(stream1_move_output, &m, sizeof(co_int32));
										co_stream_write(stream1_move_output, &m1, sizeof(co_int32));
										count++;	next_stream = 2;										
									}	
									else if(next_stream == 2) {										
										co_stream_write(stream2_move_output, &m, sizeof(co_int32));
										co_stream_write(stream2_move_output, &m1, sizeof(co_int32));
										count++;  next_stream = 1;
										if(count ==  4) state = 1;
									}
								}
								
								else{
									//wait for stream to be empty, then forward next move
									while(1){
										if( co_stream_read_nb(stream1_move_input, &m_recv, sizeof(co_int32)) ){												
											co_stream_write(stream1_move_output, &m, sizeof(co_int32) );
											co_stream_write(stream1_move_output, &m1, sizeof(co_int32));
											break;
										}
										if( co_stream_read_nb(stream2_move_input, &m_recv, sizeof(co_int32)) ){
											co_stream_write(stream2_move_output, &m, sizeof(co_int32) );
											co_stream_write(stream2_move_output, &m1, sizeof(co_int32));
											break;
										}
										if( co_stream_read_nb(timer_input_stream, &timer_t, sizeof(co_int8)) ){
											//timer expired
											time_rem = FALSE;	
											timer_t = TIMER_STOP;
											co_stream_write(timer_output_stream, &timer_t, sizeof(co_int8) );
											break;
										}
									}

									utility = (m_recv>>UTILITY_OFF)&UTILITY_MASK;									
									if((utility > alpha) & time_rem){
										//found better move
										best_move_x = (m_recv>>X_OFF)&X_MASK;
										best_move_y = (m_recv>>Y_OFF)&Y_MASK;
										best_move_piece = (m_recv>>PIECE_OFF)&PIECE_MASK;
										best_move_rotate = (m_recv>>ROTATE_OFF)&ROTATE_MASK;
										alpha = utility;
									}
								}
							}
						}
					}
				}
			}
			
			while(count > 0){
				//empty pipeline
				while(1){
					if(co_stream_read_nb(stream1_move_input, &m_recv, sizeof(co_int32)) ) {--count; break;}
					if(co_stream_read_nb(stream2_move_input, &m_recv, sizeof(co_int32)) ) {--count; break;}
					}
				utility = (m_recv>>UTILITY_OFF)&UTILITY_MASK;
				if(utility > alpha){
					best_move_x = (m_recv>>X_OFF)&X_MASK;
					best_move_y = (m_recv>>Y_OFF)&Y_MASK;
					best_move_piece = (m_recv>>PIECE_OFF)&PIECE_MASK;
					best_move_rotate = (m_recv>>ROTATE_OFF)&ROTATE_MASK;
					alpha = utility;
				}
			}
			
			if(moves_found){				
				//valid move found
				response_code_0 = best_move_x+'a'-10;
				response_code_1 = best_move_y+'a'-10;
				response_code_2 =  best_move_piece+'a';
				response_code_3 =  best_move_rotate+'0';
				if(best_move_x<10) response_code_0 = best_move_x+'0';
				if(best_move_y<10) response_code_1 = best_move_y+'0';
			}
			else{
				//no valid move found
				response_code_0 = '0';
				response_code_1 = '0';
				response_code_2 = '0';
				response_code_3 = '0';
			}
			
			co_stream_write(output_stream,&response_code_0,sizeof(co_int8));
			co_stream_write(output_stream,&response_code_1,sizeof(co_int8));
			co_stream_write(output_stream,&response_code_2,sizeof(co_int8));
			co_stream_write(output_stream,&response_code_3,sizeof(co_int8));			

			//stop the timer
			timer_t = TIMER_STOP;
			co_stream_write(timer_output_stream,&timer_t,sizeof(co_int8));
			//delay to consume any incoming message
			for(z=0; z<5; z++){
				co_stream_read_nb(timer_input_stream,&timer_t,sizeof(co_int8));
			}
			
			//update table
			if(moves_found)
			{
				//forward move
				m = best_move_piece<<PIECE_OFF|best_move_rotate<<ROTATE_OFF|best_move_x<<X_OFF|best_move_y<<Y_OFF|PERM_MOVE_SELF<<TYPE_OFF;
				co_stream_write(stream1_move_output, &m, sizeof(co_int32));
				co_stream_write(stream2_move_output, &m, sizeof(co_int32));
			
				//update board self
				best_move_y+=3;
				best_move_x+=3;
				
				size = piece_sizes[best_move_piece];
				avail_self &= ~(1<<best_move_piece);
				score_self += size;

				--size;
				yoff = best_move_y;
				xoff = best_move_x;
										
				do{
					board[yoff][xoff] = SELF | (0x3<<EX_GRID_NOT_SAFE);
					
					y = yoff+1; x = xoff; y1 = yoff+1; x1 = xoff+1; board_t = board[y][x]; board_t1 = board[y1][x1]; 
					board[y][x] = board_t|(SELF<<EX_GRID_NOT_SAFE);	board[y1][x1] = board_t1|(SELF<<EX_GRID_LBTY);
					
					y = yoff-1; x = xoff; y1 = yoff-1; x1 = xoff-1;	board_t = board[y][x]; board_t1 = board[y1][x1]; 
					board[y][x] = board_t|(SELF<<EX_GRID_NOT_SAFE); board[y1][x1] = board_t1|(SELF<<EX_GRID_LBTY);			
					
					y = yoff; x = xoff+1; y1 = yoff-1; x1 = xoff+1;	board_t = board[y][x]; board_t1 = board[y1][x1];
					board[y][x] = board_t|(SELF<<EX_GRID_NOT_SAFE); board[y1][x1] = board_t1|(SELF<<EX_GRID_LBTY);
					
					y = yoff; x = xoff-1; y1 = yoff+1; x1 = xoff-1;	board_t = board[y][x]; board_t1 = board[y1][x1];
					board[y][x] = board_t|(SELF<<EX_GRID_NOT_SAFE); board[y1][x1] = board_t1|(SELF<<EX_GRID_LBTY);
				
					--size;
					if(size<0) break;
					yoff = best_move_y+piece_y[best_move_piece][best_move_rotate][size];
					xoff = best_move_x+piece_x[best_move_piece][best_move_rotate][size];
				}while(1);
						
			}			
			break;
			
		case '9':
			//reset game
			m = 0|GAME_RESET<<TYPE_OFF;
			
			//forward reset command
			co_stream_write(stream1_move_output, &m, sizeof(co_int32));
			co_stream_write(stream2_move_output, &m, sizeof(co_int32));
			
			//init board
			for(y = 0; y < 32; y++){
				for(x = 0; x < 32; x++){
					if((x < 4) || (y < 4) || (x > 17) || (y > 17)){
						board[y][x] = (BORDER | (0x3<<EX_GRID_NOT_SAFE));
					}
					else {
						board[y][x] = 0;
					}	
				}
			}
			
			//init available pieces
			avail_self = 0x7FFFFF;
			avail_opp = 0x7FFFFF;
			
			//init scores
			score_self = 0;
			score_opp = 0;

			//stop timer
			timer_t = TIMER_STOP;
			co_stream_write(timer_output_stream,&timer_t,sizeof(co_int8));
			//delay to consume any incoming message
			for(z=0; z<5; z++){
				co_stream_read_nb(timer_input_stream,&timer_t,sizeof(co_int8));
			}
			
			break;

		default:
			//
			break;
		}
	}
}

// 4 cycles between each counter increment
void timer_func(co_stream stream0_input, co_stream stream0_output 	){	
	
	co_int8 temp, state;	
	co_int32 counter;		

	co_stream_open(stream0_input, O_RDONLY, INT_TYPE(8));		
	co_stream_open(stream0_output, O_WRONLY, INT_TYPE(8));		
	
	counter = 0;	state = TIMER_STOP;
	while(1){		
		switch(state){
			
			case TIMER_RUN:
				//timer running
				counter++;
				//check for incoming message
				if( co_stream_read_nb(stream0_input, &temp, sizeof(co_int8)) ){		
					state = temp;
					counter  = 0;			
				}
				else if(counter >= TIMER_COUNT_MAX){			
					//timer expires
					co_stream_write(stream0_output, &temp, sizeof(co_int8));			
					counter = 0;			
					state = TIMER_STOP;
				}
				break;
				
			case TIMER_STOP:
				//stop the timer
				counter = 0;
				co_stream_read(stream0_input, &temp, sizeof(co_int8));
				state = temp;			
				break;
				
			default:
				state = TIMER_STOP;
				break;		
		}
	}
}

void minimax_func(co_stream stream0_move_input, co_stream stream0_move_output, 
						co_stream stream_minimax_ex1_output, co_stream stream_minimax_ex1_input,
						co_stream stream_minimax_ex2_output, co_stream stream_minimax_ex2_input,
						co_stream stream_minimax_ex3_output, co_stream stream_minimax_ex3_input,
						co_stream stream_minimax_ex4_output, co_stream stream_minimax_ex4_input
						)
{
	co_int8 piece_sizes[21] = { 1, 2, 3, 3, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5 };
	  
	co_int8 piece_y[21][8][4] = {
	  { { 0, 0, 0, 0 },  { 0, 0, 0, 0 },  { 0, 0, 0, 0 },  { 0, 0, 0, 0 },	{ 0, 0, 0, 0 },  { 0, 0, 0, 0 },  { 0, 0, 0, 0 },  { 0, 0, 0, 0 },	},
	  { { 1, 0, 0, 0 },  { 1, 0, 0, 0 },  { 0, 0, 0, 0 },  { 0, 0, 0, 0 },	{ -1, 0, 0, 0 }, { -1, 0, 0, 0 }, { 0, 0, 0, 0 },  { 0, 0, 0, 0 },	},
	  { { -1, 1, 0, 0 }, { -1, 1, 0, 0 }, { 0, 0, 0, 0 },  { 0, 0, 0, 0 },	{ -1, 1, 0, 0 }, { -1, 1, 0, 0 }, { 0, 0, 0, 0 },  { 0, 0, 0, 0 },	},
	  { { -1, 0, 0, 0 }, { -1, 0, 0, 0 }, { 0, 1, 0, 0 },  { 0, 1, 0, 0 },	{ 0, 1, 0, 0 },  { 0, 1, 0, 0 },  { -1, 0, 0, 0 }, { -1, 0, 0, 0 }, },
	  { { -1, 1, 2, 0 }, { -1, 1, 2, 0 }, { 0, 0, 0, 0 },  { 0, 0, 0, 0 },	{ -2, -1, 1, 0 },{ -2, -1, 1, 0 },{ 0, 0, 0, 0 },  { 0, 0, 0, 0 },	},
	  { { -1, 1, 1, 0 }, { -1, 1, 1, 0 }, { -1, 0, 0, 0 }, { -1, 0, 0, 0 }, { -1, -1, 1, 0 },{ -1, -1, 1, 0 },{ 0, 0, 1, 0 },  { 0, 0, 1, 0 },	},
	  { { -1, 0, 1, 0 }, { -1, 0, 1, 0 }, { 0, 0, 1, 0 },  { 0, 0, 1, 0 },	{ -1, 0, 1, 0 }, { -1, 0, 1, 0 }, { -1, 0, 0, 0 }, { -1, 0, 0, 0 }, },
	  { { 0, 1, 1, 0 },  { 0, 1, 1, 0 },  { 0, 1, 1, 0 },  { 0, 1, 1, 0 },	{ -1, -1, 0, 0 },{ -1, -1, 0, 0 },{ -1, -1, 0, 0 },{ -1, -1, 0, 0 },},
	  { { 0, 1, 1, 0 },  { 0, 1, 1, 0 },  { -1, 0, 1, 0 }, { -1, 0, 1, 0 }, { -1, -1, 0, 0 },{ -1, -1, 0, 0 },{ -1, 0, 1, 0 }, { -1, 0, 1, 0 }, },
	  { { -2, -1, 1, 2 },{ -2, -1, 1, 2 },{ 0, 0, 0, 0 },  { 0, 0, 0, 0 },	{ -2, -1, 1, 2 },{ -2, -1, 1, 2 },{ 0, 0, 0, 0 },  { 0, 0, 0, 0 },	},
	  { { -2, -1, 1, 1 },{ -2, -1, 1, 1 },{ -1, 0, 0, 0 }, { -1, 0, 0, 0 }, { -1, -1, 1, 2 },{ -1, -1, 1, 2 },{ 0, 0, 0, 1 },  { 0, 0, 0, 1 },	},
	  { { -2, -1, 0, 1 },{ -2, -1, 0, 1 },{ -1, -1, 0, 0 },{ -1, -1, 0, 0 },{ -1, 0, 1, 2 }, { -1, 0, 1, 2 }, { 0, 0, 1, 1 },  { 0, 0, 1, 1 },	},
	  { { -1, 0, 1, 1 }, { -1, 0, 1, 1 }, { -1, -1, 0, 0 },{ -1, -1, 0, 0 },{ -1, -1, 0, 1 },{ -1, -1, 0, 1 },{ 0, 0, 1, 1 },  { 0, 0, 1, 1 },	},
	  { { -1, -1, 1, 1 },{ -1, -1, 1, 1 },{ -1, -1, 0, 0 },{ -1, -1, 0, 0 },{ -1, -1, 1, 1 },{ -1, -1, 1, 1 },{ 0, 0, 1, 1 },  { 0, 0, 1, 1 },	},
	  { { -1, 0, 1, 2 }, { -1, 0, 1, 2 }, { 0, 0, 0, 1 },  { 0, 0, 0, 1 },	{ -2, -1, 0, 1 },{ -2, -1, 0, 1 },{ -1, 0, 0, 0 }, { -1, 0, 0, 0 }, },
	  { { -1, 1, 1, 1 }, { -1, 1, 1, 1 }, { -1, 0, 0, 1 }, { -1, 0, 0, 1 }, { -1, -1, -1, 1},{ -1, -1, -1, 1},{ -1, 0, 0, 1 }, { -1, 0, 0, 1 }, },
	  { { -2, -1, 0, 0 },{ -2, -1, 0, 0 },{ 0, 0, 1, 2 },  { 0, 0, 1, 2 },	{ 0, 0, 1, 2 },  { 0, 0, 1, 2 },  { -2, -1, 0, 0 },{ -2, -1, 0, 0 },},
	  { { -1, -1, 0, 1 },{ -1, -1, 0, 1 },{ -1, 0, 1, 1 }, { -1, 0, 1, 1 }, { -1, 0, 1, 1 }, { -1, 0, 1, 1 }, { -1, -1, 0, 1 },{ -1, -1, 0, 1 },},
	  { { -1, 0, 0, 1 }, { -1, 0, 0, 1 }, { -1, -1, 1, 1 },{ -1, -1, 1, 1 },{ -1, 0, 0, 1 }, { -1, 0, 0, 1 }, { -1, -1, 1, 1 },{ -1, -1, 1, 1 },},
	  { { -1, 0, 0, 1 }, { -1, 0, 0, 1 }, { -1, -1, 0, 1 },{ -1, -1, 0, 1 },{ -1, 0, 0, 1 }, { -1, 0, 0, 1 }, { -1, 0, 1, 1 }, { -1, 0, 1, 1 }, },
	  { { -1, 0, 0, 1 }, { -1, 0, 0, 1 }, { -1, 0, 0, 1 }, { -1, 0, 0, 1 }, { -1, 0, 0, 1 }, { -1, 0, 0, 1 }, { -1, 0, 0, 1 }, { -1, 0, 0, 1 }, },
	};
	
	co_int8 piece_x[21][8][4] = {
	  { { 0, 0, 0, 0 },  { 0, 0, 0, 0 },  { 0, 0, 0, 0 },  { 0, 0, 0, 0 },	{ 0, 0, 0, 0 },  { 0, 0, 0, 0 },	{ 0, 0, 0, 0 },  { 0, 0, 0, 0 },   },
	  { { 0, 0, 0, 0 },  { 0, 0, 0, 0 },  { -1, 0, 0, 0 }, { 1, 0, 0, 0 },	{ 0, 0, 0, 0 },  { 0, 0, 0, 0 },	{ 1, 0, 0, 0 },  { -1, 0, 0, 0 },  },
	  { { 0, 0, 0, 0 },  { 0, 0, 0, 0 },  { -1, 1, 0, 0 }, { -1, 1, 0, 0 }, { 0, 0, 0, 0 },  { 0, 0, 0, 0 },	{ -1, 1, 0, 0 }, { -1, 1, 0, 0 },  },
	  { { 0, 1, 0, 0 },  { 0, -1, 0, 0 }, { 1, 0, 0, 0 },  { -1, 0, 0, 0 }, { -1, 0, 0, 0 }, { 1, 0, 0, 0 },	{ 0, -1, 0, 0 }, { 0, 1, 0, 0 },   },
	  { { 0, 0, 0, 0 },  { 0, 0, 0, 0 },  { -2, -1, 1, 0 },{ -1, 1, 2, 0 }, { 0, 0, 0, 0 },  { 0, 0, 0, 0 },	{ -1, 1, 2, 0 }, { -2, -1, 1, 0 }, },
	  { { 0, -1, 0, 0 }, { 0, 0, 1, 0 },  { -1, -1, 1, 0 },{ 1, -1, 1, 0 }, { 0, 1, 0, 0 },  { -1, 0, 0, 0 },	{ -1, 1, 1, 0 }, { -1, 1, -1, 0 }, },
	  { { 0, 1, 0, 0 },  { 0, -1, 0, 0 }, { -1, 1, 0, 0 }, { -1, 1, 0, 0 }, { 0, -1, 0, 0 }, { 0, 1, 0, 0 },	{ 0, -1, 1, 0 }, { 0, -1, 1, 0 },  },
	  { { 1, 0, 1, 0 },  { -1, -1, 0, 0 },{ -1, -1, 0, 0 },{ 1, 0, 1, 0 },	{ -1, 0, -1, 0 },{ 0, 1, 1, 0 },	{ 0, 1, 1, 0 },  { -1, 0, -1, 0 }, },
	  { { -1, 0, 1, 0 }, { 1, -1, 0, 0 }, { 0, -1, -1, 0 },{ 0, 1, 1, 0 },	{ -1, 0, 1, 0 }, { 0, 1, -1, 0 },	{ 1, 1, 0, 0 },  { -1, -1, 0, 0 }, },
	  { { 0, 0, 0, 0 },  { 0, 0, 0, 0 },  { -2, -1, 1, 2 },{ -2, -1, 1, 2 },{ 0, 0, 0, 0 },  { 0, 0, 0, 0 },	{ -2, -1, 1, 2 },{ -2, -1, 1, 2 }, },
	  { { 0, 0, -1, 0 }, { 0, 0, 0, 1 },  { -1, -1, 1, 2 },{ 1, -2, -1, 1 },{ 0, 1, 0, 0 },  { -1, 0, 0, 0 },	{ -2, -1, 1, 1 },{ -1, 1, 2, -1 }, },
	  { { 0, 0, -1, -1 },{ 0, 0, 1, 1 },  { -1, 0, 1, 2 }, { 0, 1, -2, -1 },{ 1, 1, 0, 0 },  { -1, -1, 0, 0 },	{ -2, -1, 0, 1 },{ 1, 2, -1, 0 },  },
	  { { 0, -1, -1, 0 },{ 0, 1, 0, 1 },  { -1, 0, -1, 1 },{ 0, 1, -1, 1 }, { 0, 1, 1, 0 },  { -1, 0, -1, 0 },	{ -1, 1, 0, 1 }, { -1, 1, -1, 0 }, },
	  { { -1, 0, -1, 0 },{ 0, 1, 0, 1 },  { -1, 1, -1, 1 },{ -1, 1, -1, 1 },{ 0, 1, 0, 1 },  { -1, 0, -1, 0 },	{ -1, 1, -1, 1 },{ -1, 1, -1, 1 }, },
	  { { 0, 1, 0, 0 },  { 0, -1, 0, 0 }, { -2, -1, 1, 0 },{ -1, 1, 2, 0 }, { 0, 0, -1, 0 }, { 0, 0, 1, 0 },	{ 0, -1, 1, 2 }, { 0, -2, -1, 1 }, },
	  { { 0, -1, 0, 1 }, { 0, -1, 0, 1 }, { -1, -1, 1, -1},{ 1, -1, 1, 1 }, { -1, 0, 1, 0 }, { -1, 0, 1, 0 },	{ 1, -1, 1, 1 }, { -1, -1, 1, -1 },},
	  { { 0, 0, 1, 2 },  { 0, 0, -2, -1 },{ 1, 2, 0, 0 },  { -2, -1, 0, 0 },{ -2, -1, 0, 0 },{ 1, 2, 0, 0 },	{ 0, 0, -2, -1 },{ 0, 0, 1, 2 },   },
	  { { -1, 0, 1, 1 }, { 0, 1, -1, -1 },{ 1, 1, -1, 0 }, { -1, -1, 0, 1 },{ -1, -1, 0, 1 },{ 1, 1, -1, 0 },	{ 0, 1, -1, -1 },{ -1, 0, 1, 1 },  },
	  { { -1, -1, 1, 1 },{ 1, -1, 1, -1 },{ 0, 1, -1, 0 }, { -1, 0, 0, 1 }, { -1, -1, 1, 1 },{ 1, -1, 1, -1 },	{ 0, 1, -1, 0 }, { -1, 0, 0, 1 },  },
	  { { -1, -1, 1, 0 },{ 1, -1, 1, 0 }, { 0, 1, -1, 0 }, { -1, 0, 1, 0 }, { 0, -1, 1, 1 }, { 0, -1, 1, -1 },	{ 0, 1, -1, 0 }, { 0, -1, 0, 1 },  },
	  { { 0, -1, 1, 0 }, { 0, -1, 1, 0 }, { 0, -1, 1, 0 }, { 0, -1, 1, 0 }, { 0, -1, 1, 0 }, { 0, -1, 1, 0 },	{ 0, -1, 1, 0 }, { 0, -1, 1, 0 },  },
	};
	
	co_int8 board[32][32];
	co_int8 score_self, score_opp;
	co_int24 avail_self, avail_opp;
	co_int8 x,y,z, xoff, yoff, board_t, m_piece_min, size, temp, move_type, count, next_stream;
	co_int8 x1, y1, board_t1;
	co_int8 m_x, m_y, m_piece, m_rotate, best_move_x, best_move_y, best_move_piece, best_move_rotate;
	co_int2 moves_found, lib_check, move_valid, avail, state, not_prune;
	co_int32 move, m, m1, m2, m_recv;

	//backup data
	co_int8 x_bak[50], y_bak[50], board_bak[50], count_bak;
	co_int8	score_self_bak, score_opp_bak;
	co_int24 avail_self_bak, avail_opp_bak;

	co_int16 alpha, beta, utility;

	co_array_config(board, co_kind, "dualsync"); 	
	
	//open streams
	co_stream_open(stream0_move_input, O_RDONLY, INT_TYPE(32));	co_stream_open(stream0_move_output, O_WRONLY, INT_TYPE(32));
	co_stream_open(stream_minimax_ex1_input, O_RDONLY, INT_TYPE(32));	co_stream_open(stream_minimax_ex1_output, O_WRONLY, INT_TYPE(32));	
	co_stream_open(stream_minimax_ex2_input, O_RDONLY, INT_TYPE(32));	co_stream_open(stream_minimax_ex2_output, O_WRONLY, INT_TYPE(32));	
	co_stream_open(stream_minimax_ex3_input, O_RDONLY, INT_TYPE(32));	co_stream_open(stream_minimax_ex3_output, O_WRONLY, INT_TYPE(32));	
	co_stream_open(stream_minimax_ex4_input, O_RDONLY, INT_TYPE(32));	co_stream_open(stream_minimax_ex4_output, O_WRONLY, INT_TYPE(32));	

	//init board
	for(y = 0; y < 32; y++){
		for(x = 0; x < 32; x++){
			if((x < 4) || (y < 4) || (x > 17) || (y > 17)){
				board[y][x] = (BORDER | (0x3<<EX_GRID_NOT_SAFE));
			}
			else {
				board[y][x] = 0;
			}	
		}
	}

	//init available pieces
	avail_self = 0x7FFFFF;	
	avail_opp = 0x7FFFFF;
	
	//init scores
	score_self = 0; 
	score_opp = 0;
	
	while(1){		
		//read incoming message
		co_stream_read(stream0_move_input, &move, sizeof(co_int32));			
		move_type = (move>>TYPE_OFF)&TYPE_MASK;
		m_x = ((move>>X_OFF)&X_MASK) + 3;
		m_y = ((move>>Y_OFF)&Y_MASK) + 3;
		m_piece = (move>>PIECE_OFF)&PIECE_MASK;
		m_rotate = (move>>ROTATE_OFF)&ROTATE_MASK;

		switch (move_type){
			
			case MINIMAX_MOVE_SELF:
				co_stream_read(stream0_move_input, &m, sizeof(co_int32));
				alpha = (m>>UTILITY_OFF)&UTILITY_MASK;
				
				//forward
				co_stream_write(stream_minimax_ex1_output, &move, sizeof(co_int32));
				co_stream_write(stream_minimax_ex2_output, &move, sizeof(co_int32));
				co_stream_write(stream_minimax_ex3_output, &move, sizeof(co_int32));
				co_stream_write(stream_minimax_ex4_output, &move, sizeof(co_int32));

				//update board self
				avail_self_bak = avail_self;
				score_self_bak = score_self;
				
				size = piece_sizes[m_piece];
				avail_self &= ~(1<<m_piece);
				score_self += size;
				--size;
				yoff = m_y;
				xoff = m_x;
				count_bak = 0;
				do{
					//backup data
					x_bak[count_bak] = xoff; y_bak[count_bak] = yoff; board_bak[count_bak] = board[yoff][xoff]; ++count_bak;
					board[yoff][xoff] = SELF | (0x3<<EX_GRID_NOT_SAFE);
					
					y = yoff+1; x = xoff; y1 = yoff+1; x1 = xoff+1; board_t = board[y][x]; board_t1 = board[y1][x1]; 
					board[y][x] = board_t|(SELF<<EX_GRID_NOT_SAFE); board[y1][x1] = board_t1|(SELF<<EX_GRID_LBTY);
					x_bak[count_bak] = x; y_bak[count_bak] = y; board_bak[count_bak] = board_t; ++count_bak;					
					x_bak[count_bak] = x1; y_bak[count_bak] = y1; board_bak[count_bak] = board_t1; ++count_bak;
					
					y = yoff-1; x = xoff; y1 = yoff-1; x1 = xoff-1; board_t = board[y][x]; board_t1 = board[y1][x1];
					board[y][x] = board_t|(SELF<<EX_GRID_NOT_SAFE); board[y1][x1] = board_t1|(SELF<<EX_GRID_LBTY);
					x_bak[count_bak] = x; y_bak[count_bak] = y; board_bak[count_bak] = board_t; ++count_bak;
					x_bak[count_bak] = x1; y_bak[count_bak] = y1; board_bak[count_bak] = board_t1; ++count_bak;

					y = yoff; x = xoff+1; y1 = yoff-1; x1 = xoff+1;	board_t = board[y][x]; board_t1 = board[y1][x1];
					board[y][x] = board_t|(SELF<<EX_GRID_NOT_SAFE); board[y1][x1] = board_t1|(SELF<<EX_GRID_LBTY);
					x_bak[count_bak] = x; y_bak[count_bak] = y; board_bak[count_bak] = board_t; ++count_bak;
					x_bak[count_bak] = x1; y_bak[count_bak] = y1; board_bak[count_bak] = board_t1; ++count_bak;

					y = yoff; x = xoff-1; y1 = yoff+1; x1 = xoff-1;	board_t = board[y][x]; board_t1 = board[y1][x1]; 
					board[y][x] = board_t|(SELF<<EX_GRID_NOT_SAFE);board[y1][x1] = board_t1|(SELF<<EX_GRID_LBTY);
					x_bak[count_bak] = x; y_bak[count_bak] = y; board_bak[count_bak] = board_t; ++count_bak;
					x_bak[count_bak] = x1; y_bak[count_bak] = y1; board_bak[count_bak] = board_t1; ++count_bak;
				
					--size;
					if(size<0) break;
					yoff = m_y+piece_y[m_piece][m_rotate][size];
					xoff = m_x+piece_x[m_piece][m_rotate][size];
				}while(1);				
				
				//find move opp for minimax search
				moves_found  = FALSE;
				beta = 32767;
				m_piece_min = 0; if(score_opp<25) m_piece_min=9;
				state  = 0; count  = 0; next_stream = 1;
				not_prune = TRUE;
								
				for(m_piece=20; (m_piece>=m_piece_min) & not_prune; m_piece--){
					avail = ((avail_opp&(1<<m_piece))!=0);
					for(m_rotate=0; (m_rotate<8) & avail & not_prune; m_rotate++){
						for (m_y=4; (m_y<18) & not_prune; m_y++){
							for (m_x=4; (m_x<18) & not_prune; m_x++){
								 
								lib_check = FALSE;	
								size = piece_sizes[m_piece]-1;							
								yoff = m_y;
								xoff = m_x;

								do{	
									board_t = board[yoff][xoff];
									move_valid = ((board_t & NOT_SAFE_MASK_OPP)==0);
									lib_check |= ((board_t & LIBERTY_MASK_OPP)!=0);
									--size;
									yoff = m_y + piece_y[m_piece][m_rotate][size];
									xoff = m_x + piece_x[m_piece][m_rotate][size]; 									
								}while((move_valid == TRUE) & (size>=0));
								
								if(move_valid & lib_check){ 

									//valid move
									moves_found = TRUE;
									m = m_piece<<PIECE_OFF|m_rotate<<ROTATE_OFF|(m_x-3)<<X_OFF|(m_y-3)<<Y_OFF|MINIMAX_MOVE_OPP<<TYPE_OFF;
									m1 = 0|(alpha<<UTILITY_OFF);
									m2 = 0|(beta<<UTILITY_OFF);	
									
									if(state == 0){
										if(next_stream == 1){
											co_stream_write(stream_minimax_ex1_output, &m, sizeof(co_int32) );
											co_stream_write(stream_minimax_ex1_output, &m1, sizeof(co_int32) );
											co_stream_write(stream_minimax_ex1_output, &m2, sizeof(co_int32) );
											++count; next_stream = 2;
										}
										if(next_stream == 2){
											co_stream_write(stream_minimax_ex2_output, &m, sizeof(co_int32) );
											co_stream_write(stream_minimax_ex2_output, &m1, sizeof(co_int32) );
											co_stream_write(stream_minimax_ex2_output, &m2, sizeof(co_int32) );
											++count; next_stream = 3;
										}
										if(next_stream == 3){
											co_stream_write(stream_minimax_ex3_output, &m, sizeof(co_int32) );
											co_stream_write(stream_minimax_ex3_output, &m1, sizeof(co_int32) );
											co_stream_write(stream_minimax_ex3_output, &m2, sizeof(co_int32) );
											++count; next_stream = 4;
										}
										if(next_stream == 4){
											co_stream_write(stream_minimax_ex4_output, &m, sizeof(co_int32) );
											co_stream_write(stream_minimax_ex4_output, &m1, sizeof(co_int32) );
											co_stream_write(stream_minimax_ex4_output, &m2, sizeof(co_int32) );
											++count; next_stream = 1;
											if(count == 8) state = 1;
										}
									}
									else{
										while(1){
											if(co_stream_read_nb(stream_minimax_ex1_input, &m_recv, sizeof(co_int32))){
												co_stream_write(stream_minimax_ex1_output, &m, sizeof(co_int32) );
												co_stream_write(stream_minimax_ex1_output, &m1, sizeof(co_int32) );
												co_stream_write(stream_minimax_ex1_output, &m2, sizeof(co_int32) );
												break;
											}
											if(co_stream_read_nb(stream_minimax_ex2_input, &m_recv, sizeof(co_int32))){
												co_stream_write(stream_minimax_ex2_output, &m, sizeof(co_int32) );
												co_stream_write(stream_minimax_ex2_output, &m1, sizeof(co_int32) );
												co_stream_write(stream_minimax_ex2_output, &m2, sizeof(co_int32) );
												break;
											}
											if(co_stream_read_nb(stream_minimax_ex3_input, &m_recv, sizeof(co_int32))){
												co_stream_write(stream_minimax_ex3_output, &m, sizeof(co_int32) );
												co_stream_write(stream_minimax_ex3_output, &m1, sizeof(co_int32) );
												co_stream_write(stream_minimax_ex3_output, &m2, sizeof(co_int32) );
												break;
											}
											if(co_stream_read_nb(stream_minimax_ex4_input, &m_recv, sizeof(co_int32))){
												co_stream_write(stream_minimax_ex4_output, &m, sizeof(co_int32) );
												co_stream_write(stream_minimax_ex4_output, &m1, sizeof(co_int32) );
												co_stream_write(stream_minimax_ex4_output, &m2, sizeof(co_int32) );
												break;
											}
										}
										utility = (m_recv>>UTILITY_OFF)&UTILITY_MASK;
										if(utility < beta)	{
											beta = utility;	
											if(beta<=alpha) not_prune=FALSE; 
										}
									}
								}
							}
						}
					}
				}
					
				while(count > 0){
					if(co_stream_read_nb(stream_minimax_ex1_input, &m_recv, sizeof(co_int32))){
						count--;
						utility = (m_recv>>UTILITY_OFF)&UTILITY_MASK;
						if(utility < beta)	beta = utility;
					}					
					if(co_stream_read_nb(stream_minimax_ex2_input, &m_recv, sizeof(co_int32))){
						count--;
						utility = (m_recv>>UTILITY_OFF)&UTILITY_MASK;
						if(utility < beta)	beta = utility;
					}
					if(co_stream_read_nb(stream_minimax_ex3_input, &m_recv, sizeof(co_int32))){
						count--;
						utility = (m_recv>>UTILITY_OFF)&UTILITY_MASK;
						if(utility < beta)	beta = utility;
					}
					if(co_stream_read_nb(stream_minimax_ex4_input, &m_recv, sizeof(co_int32))){
						count--;
						utility = (m_recv>>UTILITY_OFF)&UTILITY_MASK;
						if(utility < beta)	beta = utility;
					}
				}

				if(moves_found == FALSE){
					//no valid move found 
					m = 0|MINIMAX_MOVE_OPP<<TYPE_OFF;
					m1 = 0|(alpha<<UTILITY_OFF);
					m2 = 0|(beta<<UTILITY_OFF);	
					co_stream_write(stream_minimax_ex1_output, &m, sizeof(co_int32));
					co_stream_write(stream_minimax_ex1_output, &m1, sizeof(co_int32) );
					co_stream_write(stream_minimax_ex1_output, &m2, sizeof(co_int32) );
					co_par_break();
					co_stream_read(stream_minimax_ex1_input, &m_recv, sizeof(co_int32)); 										
					
					utility = (m_recv>>UTILITY_OFF)&UTILITY_MASK;									
					if(utility < beta)	beta = utility;
				}


				//return minimax utility
				move &= 0x0000FFFF;				
				move |= beta<<UTILITY_OFF;				
				co_stream_write(stream0_move_output, &move, sizeof(co_int32));

				m = 0|MINIMAX_MOVE_SELF_DONE<<TYPE_OFF;
				co_stream_write(stream_minimax_ex1_output, &m, sizeof(co_int32));
				co_stream_write(stream_minimax_ex2_output, &m, sizeof(co_int32));
				co_stream_write(stream_minimax_ex3_output, &m, sizeof(co_int32));
				co_stream_write(stream_minimax_ex4_output, &m, sizeof(co_int32));
				
				//restore backup values
				--count_bak;
				do{
					#pragma CO PIPELINE
					xoff = x_bak[count_bak];
					yoff = y_bak[count_bak];
					board[yoff][xoff] = board_bak[count_bak];
					--count_bak;
					}while(count_bak>=0);				
				score_self = score_self_bak;
				avail_self = avail_self_bak;
				
				break;
				
			case PERM_MOVE_SELF:
				//forward move
				co_stream_write(stream_minimax_ex1_output, &move, sizeof(co_int32));
				co_stream_write(stream_minimax_ex2_output, &move, sizeof(co_int32));
				co_stream_write(stream_minimax_ex3_output, &move, sizeof(co_int32));
				co_stream_write(stream_minimax_ex4_output, &move, sizeof(co_int32));
				

				//update board self				
				size = piece_sizes[m_piece];
				avail_self &= ~(1<<m_piece);
				score_self += size;
				--size;
				yoff = m_y;
				xoff = m_x;
				
				do{
					board[yoff][xoff] = SELF | (0x3<<EX_GRID_NOT_SAFE);
					
					y = yoff+1; x = xoff; y1 = yoff+1; x1 = xoff+1; board_t = board[y][x]; board_t1 = board[y1][x1]; 
					board[y][x] = board_t|(SELF<<EX_GRID_NOT_SAFE);	board[y1][x1] = board_t1|(SELF<<EX_GRID_LBTY);
					
					y = yoff-1; x = xoff; y1 = yoff-1; x1 = xoff-1;	board_t = board[y][x]; board_t1 = board[y1][x1]; 
					board[y][x] = board_t|(SELF<<EX_GRID_NOT_SAFE); board[y1][x1] = board_t1|(SELF<<EX_GRID_LBTY);			
					
					y = yoff; x = xoff+1; y1 = yoff-1; x1 = xoff+1;	board_t = board[y][x]; board_t1 = board[y1][x1];
					board[y][x] = board_t|(SELF<<EX_GRID_NOT_SAFE); board[y1][x1] = board_t1|(SELF<<EX_GRID_LBTY);
					
					y = yoff; x = xoff-1; y1 = yoff+1; x1 = xoff-1;	board_t = board[y][x]; board_t1 = board[y1][x1];
					board[y][x] = board_t|(SELF<<EX_GRID_NOT_SAFE); board[y1][x1] = board_t1|(SELF<<EX_GRID_LBTY);
				
					--size;
					if(size<0) break;
					yoff = m_y+piece_y[m_piece][m_rotate][size];
					xoff = m_x+piece_x[m_piece][m_rotate][size];
				}while(1);				
				break;
				
			case PERM_MOVE_OPP:

				//forward move
				co_stream_write(stream_minimax_ex1_output, &move, sizeof(co_int32));
				co_stream_write(stream_minimax_ex2_output, &move, sizeof(co_int32));
				co_stream_write(stream_minimax_ex3_output, &move, sizeof(co_int32));
				co_stream_write(stream_minimax_ex4_output, &move, sizeof(co_int32));
				
				//update board opp
				
				size = piece_sizes[m_piece];
				avail_opp &= ~(1<<m_piece); 
				score_opp += size;
				
				--size;
				yoff = m_y;
				xoff = m_x;
										
				do{
					board[yoff][xoff] = OPP | (0x3<<EX_GRID_NOT_SAFE);
					
					y = yoff+1; x = xoff; y1 = yoff+1; x1 = xoff+1; board_t = board[y][x]; board_t1 = board[y1][x1]; 
					board[y][x] = board_t|(OPP<<EX_GRID_NOT_SAFE);	board[y1][x1] = board_t1|(OPP<<EX_GRID_LBTY);
					
					y = yoff-1; x = xoff; y1 = yoff-1; x1 = xoff-1;	board_t = board[y][x]; board_t1 = board[y1][x1]; 
					board[y][x] = board_t|(OPP<<EX_GRID_NOT_SAFE); board[y1][x1] = board_t1|(OPP<<EX_GRID_LBTY);			
					
					y = yoff; x = xoff+1; y1 = yoff-1; x1 = xoff+1;	board_t = board[y][x]; board_t1 = board[y1][x1];
					board[y][x] = board_t|(OPP<<EX_GRID_NOT_SAFE); board[y1][x1] = board_t1|(OPP<<EX_GRID_LBTY);
					
					y = yoff; x = xoff-1; y1 = yoff+1; x1 = xoff-1; board_t = board[y][x]; board_t1 = board[y1][x1];
					board[y][x] = board_t|(OPP<<EX_GRID_NOT_SAFE); board[y1][x1] = board_t1|(OPP<<EX_GRID_LBTY);
					--size;
					
					if(size<0) break;
					yoff = m_y+piece_y[m_piece][m_rotate][size];
					xoff = m_x+piece_x[m_piece][m_rotate][size];
					
				}while(1);
				break;
			case GAME_RESET:

				//reset game
				co_stream_write(stream_minimax_ex1_output, &move, sizeof(co_int32));
				co_stream_write(stream_minimax_ex2_output, &move, sizeof(co_int32));
				co_stream_write(stream_minimax_ex3_output, &move, sizeof(co_int32));
				co_stream_write(stream_minimax_ex4_output, &move, sizeof(co_int32));
				
				//init board
				for(y = 0; y < 32; y++){
					for(x = 0; x < 32; x++){
						if((x < 4) || (y < 4) || (x > 17) || (y > 17)){
							board[y][x] = (BORDER | (0x3<<EX_GRID_NOT_SAFE));
						}
						else {
							board[y][x] = 0;
						}	
					}
				}
				
				//init available pieces
				avail_self = 0x7FFFFF;
				avail_opp = 0x7FFFFF;
				
				//init scores
				score_self = 0;
				score_opp = 0;
				
				break;
				
			default:
				break;
		}
	}
		
}

void minimax_ex_func(co_stream stream0_move_input, co_stream stream0_move_output, 
						co_stream stream_eval1_output, co_stream stream_eval1_input,
						co_stream stream_eval2_output, co_stream stream_eval2_input
						)
{
	co_int8 piece_sizes[21] = { 1, 2, 3, 3, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5 };
	  
	co_int8 piece_y[21][8][4] = {
	  { { 0, 0, 0, 0 },  { 0, 0, 0, 0 },  { 0, 0, 0, 0 },  { 0, 0, 0, 0 },	{ 0, 0, 0, 0 },  { 0, 0, 0, 0 },  { 0, 0, 0, 0 },  { 0, 0, 0, 0 },	},
	  { { 1, 0, 0, 0 },  { 1, 0, 0, 0 },  { 0, 0, 0, 0 },  { 0, 0, 0, 0 },	{ -1, 0, 0, 0 }, { -1, 0, 0, 0 }, { 0, 0, 0, 0 },  { 0, 0, 0, 0 },	},
	  { { -1, 1, 0, 0 }, { -1, 1, 0, 0 }, { 0, 0, 0, 0 },  { 0, 0, 0, 0 },	{ -1, 1, 0, 0 }, { -1, 1, 0, 0 }, { 0, 0, 0, 0 },  { 0, 0, 0, 0 },	},
	  { { -1, 0, 0, 0 }, { -1, 0, 0, 0 }, { 0, 1, 0, 0 },  { 0, 1, 0, 0 },	{ 0, 1, 0, 0 },  { 0, 1, 0, 0 },  { -1, 0, 0, 0 }, { -1, 0, 0, 0 }, },
	  { { -1, 1, 2, 0 }, { -1, 1, 2, 0 }, { 0, 0, 0, 0 },  { 0, 0, 0, 0 },	{ -2, -1, 1, 0 },{ -2, -1, 1, 0 },{ 0, 0, 0, 0 },  { 0, 0, 0, 0 },	},
	  { { -1, 1, 1, 0 }, { -1, 1, 1, 0 }, { -1, 0, 0, 0 }, { -1, 0, 0, 0 }, { -1, -1, 1, 0 },{ -1, -1, 1, 0 },{ 0, 0, 1, 0 },  { 0, 0, 1, 0 },	},
	  { { -1, 0, 1, 0 }, { -1, 0, 1, 0 }, { 0, 0, 1, 0 },  { 0, 0, 1, 0 },	{ -1, 0, 1, 0 }, { -1, 0, 1, 0 }, { -1, 0, 0, 0 }, { -1, 0, 0, 0 }, },
	  { { 0, 1, 1, 0 },  { 0, 1, 1, 0 },  { 0, 1, 1, 0 },  { 0, 1, 1, 0 },	{ -1, -1, 0, 0 },{ -1, -1, 0, 0 },{ -1, -1, 0, 0 },{ -1, -1, 0, 0 },},
	  { { 0, 1, 1, 0 },  { 0, 1, 1, 0 },  { -1, 0, 1, 0 }, { -1, 0, 1, 0 }, { -1, -1, 0, 0 },{ -1, -1, 0, 0 },{ -1, 0, 1, 0 }, { -1, 0, 1, 0 }, },
	  { { -2, -1, 1, 2 },{ -2, -1, 1, 2 },{ 0, 0, 0, 0 },  { 0, 0, 0, 0 },	{ -2, -1, 1, 2 },{ -2, -1, 1, 2 },{ 0, 0, 0, 0 },  { 0, 0, 0, 0 },	},
	  { { -2, -1, 1, 1 },{ -2, -1, 1, 1 },{ -1, 0, 0, 0 }, { -1, 0, 0, 0 }, { -1, -1, 1, 2 },{ -1, -1, 1, 2 },{ 0, 0, 0, 1 },  { 0, 0, 0, 1 },	},
	  { { -2, -1, 0, 1 },{ -2, -1, 0, 1 },{ -1, -1, 0, 0 },{ -1, -1, 0, 0 },{ -1, 0, 1, 2 }, { -1, 0, 1, 2 }, { 0, 0, 1, 1 },  { 0, 0, 1, 1 },	},
	  { { -1, 0, 1, 1 }, { -1, 0, 1, 1 }, { -1, -1, 0, 0 },{ -1, -1, 0, 0 },{ -1, -1, 0, 1 },{ -1, -1, 0, 1 },{ 0, 0, 1, 1 },  { 0, 0, 1, 1 },	},
	  { { -1, -1, 1, 1 },{ -1, -1, 1, 1 },{ -1, -1, 0, 0 },{ -1, -1, 0, 0 },{ -1, -1, 1, 1 },{ -1, -1, 1, 1 },{ 0, 0, 1, 1 },  { 0, 0, 1, 1 },	},
	  { { -1, 0, 1, 2 }, { -1, 0, 1, 2 }, { 0, 0, 0, 1 },  { 0, 0, 0, 1 },	{ -2, -1, 0, 1 },{ -2, -1, 0, 1 },{ -1, 0, 0, 0 }, { -1, 0, 0, 0 }, },
	  { { -1, 1, 1, 1 }, { -1, 1, 1, 1 }, { -1, 0, 0, 1 }, { -1, 0, 0, 1 }, { -1, -1, -1, 1},{ -1, -1, -1, 1},{ -1, 0, 0, 1 }, { -1, 0, 0, 1 }, },
	  { { -2, -1, 0, 0 },{ -2, -1, 0, 0 },{ 0, 0, 1, 2 },  { 0, 0, 1, 2 },	{ 0, 0, 1, 2 },  { 0, 0, 1, 2 },  { -2, -1, 0, 0 },{ -2, -1, 0, 0 },},
	  { { -1, -1, 0, 1 },{ -1, -1, 0, 1 },{ -1, 0, 1, 1 }, { -1, 0, 1, 1 }, { -1, 0, 1, 1 }, { -1, 0, 1, 1 }, { -1, -1, 0, 1 },{ -1, -1, 0, 1 },},
	  { { -1, 0, 0, 1 }, { -1, 0, 0, 1 }, { -1, -1, 1, 1 },{ -1, -1, 1, 1 },{ -1, 0, 0, 1 }, { -1, 0, 0, 1 }, { -1, -1, 1, 1 },{ -1, -1, 1, 1 },},
	  { { -1, 0, 0, 1 }, { -1, 0, 0, 1 }, { -1, -1, 0, 1 },{ -1, -1, 0, 1 },{ -1, 0, 0, 1 }, { -1, 0, 0, 1 }, { -1, 0, 1, 1 }, { -1, 0, 1, 1 }, },
	  { { -1, 0, 0, 1 }, { -1, 0, 0, 1 }, { -1, 0, 0, 1 }, { -1, 0, 0, 1 }, { -1, 0, 0, 1 }, { -1, 0, 0, 1 }, { -1, 0, 0, 1 }, { -1, 0, 0, 1 }, },
	};
	
	co_int8 piece_x[21][8][4] = {
	  { { 0, 0, 0, 0 },  { 0, 0, 0, 0 },  { 0, 0, 0, 0 },  { 0, 0, 0, 0 },	{ 0, 0, 0, 0 },  { 0, 0, 0, 0 },	{ 0, 0, 0, 0 },  { 0, 0, 0, 0 },   },
	  { { 0, 0, 0, 0 },  { 0, 0, 0, 0 },  { -1, 0, 0, 0 }, { 1, 0, 0, 0 },	{ 0, 0, 0, 0 },  { 0, 0, 0, 0 },	{ 1, 0, 0, 0 },  { -1, 0, 0, 0 },  },
	  { { 0, 0, 0, 0 },  { 0, 0, 0, 0 },  { -1, 1, 0, 0 }, { -1, 1, 0, 0 }, { 0, 0, 0, 0 },  { 0, 0, 0, 0 },	{ -1, 1, 0, 0 }, { -1, 1, 0, 0 },  },
	  { { 0, 1, 0, 0 },  { 0, -1, 0, 0 }, { 1, 0, 0, 0 },  { -1, 0, 0, 0 }, { -1, 0, 0, 0 }, { 1, 0, 0, 0 },	{ 0, -1, 0, 0 }, { 0, 1, 0, 0 },   },
	  { { 0, 0, 0, 0 },  { 0, 0, 0, 0 },  { -2, -1, 1, 0 },{ -1, 1, 2, 0 }, { 0, 0, 0, 0 },  { 0, 0, 0, 0 },	{ -1, 1, 2, 0 }, { -2, -1, 1, 0 }, },
	  { { 0, -1, 0, 0 }, { 0, 0, 1, 0 },  { -1, -1, 1, 0 },{ 1, -1, 1, 0 }, { 0, 1, 0, 0 },  { -1, 0, 0, 0 },	{ -1, 1, 1, 0 }, { -1, 1, -1, 0 }, },
	  { { 0, 1, 0, 0 },  { 0, -1, 0, 0 }, { -1, 1, 0, 0 }, { -1, 1, 0, 0 }, { 0, -1, 0, 0 }, { 0, 1, 0, 0 },	{ 0, -1, 1, 0 }, { 0, -1, 1, 0 },  },
	  { { 1, 0, 1, 0 },  { -1, -1, 0, 0 },{ -1, -1, 0, 0 },{ 1, 0, 1, 0 },	{ -1, 0, -1, 0 },{ 0, 1, 1, 0 },	{ 0, 1, 1, 0 },  { -1, 0, -1, 0 }, },
	  { { -1, 0, 1, 0 }, { 1, -1, 0, 0 }, { 0, -1, -1, 0 },{ 0, 1, 1, 0 },	{ -1, 0, 1, 0 }, { 0, 1, -1, 0 },	{ 1, 1, 0, 0 },  { -1, -1, 0, 0 }, },
	  { { 0, 0, 0, 0 },  { 0, 0, 0, 0 },  { -2, -1, 1, 2 },{ -2, -1, 1, 2 },{ 0, 0, 0, 0 },  { 0, 0, 0, 0 },	{ -2, -1, 1, 2 },{ -2, -1, 1, 2 }, },
	  { { 0, 0, -1, 0 }, { 0, 0, 0, 1 },  { -1, -1, 1, 2 },{ 1, -2, -1, 1 },{ 0, 1, 0, 0 },  { -1, 0, 0, 0 },	{ -2, -1, 1, 1 },{ -1, 1, 2, -1 }, },
	  { { 0, 0, -1, -1 },{ 0, 0, 1, 1 },  { -1, 0, 1, 2 }, { 0, 1, -2, -1 },{ 1, 1, 0, 0 },  { -1, -1, 0, 0 },	{ -2, -1, 0, 1 },{ 1, 2, -1, 0 },  },
	  { { 0, -1, -1, 0 },{ 0, 1, 0, 1 },  { -1, 0, -1, 1 },{ 0, 1, -1, 1 }, { 0, 1, 1, 0 },  { -1, 0, -1, 0 },	{ -1, 1, 0, 1 }, { -1, 1, -1, 0 }, },
	  { { -1, 0, -1, 0 },{ 0, 1, 0, 1 },  { -1, 1, -1, 1 },{ -1, 1, -1, 1 },{ 0, 1, 0, 1 },  { -1, 0, -1, 0 },	{ -1, 1, -1, 1 },{ -1, 1, -1, 1 }, },
	  { { 0, 1, 0, 0 },  { 0, -1, 0, 0 }, { -2, -1, 1, 0 },{ -1, 1, 2, 0 }, { 0, 0, -1, 0 }, { 0, 0, 1, 0 },	{ 0, -1, 1, 2 }, { 0, -2, -1, 1 }, },
	  { { 0, -1, 0, 1 }, { 0, -1, 0, 1 }, { -1, -1, 1, -1},{ 1, -1, 1, 1 }, { -1, 0, 1, 0 }, { -1, 0, 1, 0 },	{ 1, -1, 1, 1 }, { -1, -1, 1, -1 },},
	  { { 0, 0, 1, 2 },  { 0, 0, -2, -1 },{ 1, 2, 0, 0 },  { -2, -1, 0, 0 },{ -2, -1, 0, 0 },{ 1, 2, 0, 0 },	{ 0, 0, -2, -1 },{ 0, 0, 1, 2 },   },
	  { { -1, 0, 1, 1 }, { 0, 1, -1, -1 },{ 1, 1, -1, 0 }, { -1, -1, 0, 1 },{ -1, -1, 0, 1 },{ 1, 1, -1, 0 },	{ 0, 1, -1, -1 },{ -1, 0, 1, 1 },  },
	  { { -1, -1, 1, 1 },{ 1, -1, 1, -1 },{ 0, 1, -1, 0 }, { -1, 0, 0, 1 }, { -1, -1, 1, 1 },{ 1, -1, 1, -1 },	{ 0, 1, -1, 0 }, { -1, 0, 0, 1 },  },
	  { { -1, -1, 1, 0 },{ 1, -1, 1, 0 }, { 0, 1, -1, 0 }, { -1, 0, 1, 0 }, { 0, -1, 1, 1 }, { 0, -1, 1, -1 },	{ 0, 1, -1, 0 }, { 0, -1, 0, 1 },  },
	  { { 0, -1, 1, 0 }, { 0, -1, 1, 0 }, { 0, -1, 1, 0 }, { 0, -1, 1, 0 }, { 0, -1, 1, 0 }, { 0, -1, 1, 0 },	{ 0, -1, 1, 0 }, { 0, -1, 1, 0 },  },
	};
	
	co_int8 board[32][32];
	co_int8 score_self, score_opp, num_avail_self, num_avail_opp;
	co_int24 avail_self, avail_opp;
	
	co_int8 x,y,z, xoff, yoff, board_t, m_piece_min, size, temp, move_type, count, next_stream;
	co_int8 x1, y1, board_t1, mx_backup;
	co_int8 m_x, m_y, m_piece, m_rotate, best_move_x, best_move_y, best_move_piece, best_move_rotate;
	co_int2 moves_found, lib_check, move_valid, avail, state, not_prune, search_extra;
	co_int32 move, m, m1, m2, m_recv;

	//backup data
	co_int8 x_bak_s[50], y_bak_s[50], board_bak_s[50], count_bak_s;
	co_int8 x_bak_op[50], y_bak_op[50], board_bak_op[50], count_bak_op;
	co_int8	score_self_bak, score_opp_bak;
	co_int24 avail_self_bak, avail_opp_bak;

	co_int16 alpha, beta, utility;

	co_array_config(board, co_kind, "dualsync"); 	
	
	//open streams
	co_stream_open(stream0_move_input, O_RDONLY, INT_TYPE(32));	co_stream_open(stream0_move_output, O_WRONLY, INT_TYPE(32));
	co_stream_open(stream_eval1_input, O_RDONLY, INT_TYPE(32)); co_stream_open(stream_eval1_output, O_WRONLY, INT_TYPE(32));
	co_stream_open(stream_eval2_input, O_RDONLY, INT_TYPE(32));	co_stream_open(stream_eval2_output, O_WRONLY, INT_TYPE(32));

	//init board
	for(y = 0; y < 32; y++){
		for(x = 0; x < 32; x++){
			if((x < 4) || (y < 4) || (x > 17) || (y > 17)){
				board[y][x] = (BORDER | (0x3<<EX_GRID_NOT_SAFE));
			}
			else {
				board[y][x] = 0;
			}	
		}
	}

	//init available pieces
	avail_self = 0x7FFFFF;	
	avail_opp = 0x7FFFFF;
	
	//init scores
	score_self = 0; 
	score_opp = 0;

	num_avail_self = 21;
	num_avail_opp = 21;
	search_extra = FALSE;
	
	while(1){		
		//read incoming message
		co_stream_read(stream0_move_input, &move, sizeof(co_int32));			
		move_type = (move>>TYPE_OFF)&TYPE_MASK;
		m_x = ((move>>X_OFF)&X_MASK) + 3;
		m_y = ((move>>Y_OFF)&Y_MASK) + 3;
		m_piece = (move>>PIECE_OFF)&PIECE_MASK;
		m_rotate = (move>>ROTATE_OFF)&ROTATE_MASK;

		switch (move_type){
			
			case MINIMAX_MOVE_OPP:
				mx_backup = m_x;
				
				co_stream_read(stream0_move_input, &m1, sizeof(co_int32)); alpha = (m1>>UTILITY_OFF)&UTILITY_MASK;
				co_stream_read(stream0_move_input, &m2, sizeof(co_int32)); beta = (m2>>UTILITY_OFF)&UTILITY_MASK;
				
				if(mx_backup>3){
										
					//forward
					co_stream_write(stream_eval1_output, &move, sizeof(co_int32));
					co_stream_write(stream_eval2_output, &move, sizeof(co_int32));

					//update board opp
					avail_opp_bak = avail_opp;
					score_opp_bak = score_opp;
					
					size = piece_sizes[m_piece];
					avail_opp &= ~(1<<m_piece);
					score_opp += size;
					--size;
					yoff = m_y;
					xoff = m_x;
					count_bak_op = 0;
					do{
						//backup data
						x_bak_op[count_bak_op] = xoff; y_bak_op[count_bak_op] = yoff; board_bak_op[count_bak_op] = board[yoff][xoff]; ++count_bak_op;
						board[yoff][xoff] = OPP | (0x3<<EX_GRID_NOT_SAFE);
						
						y = yoff+1; x = xoff; y1 = yoff+1; x1 = xoff+1; board_t = board[y][x]; board_t1 = board[y1][x1]; 
						board[y][x] = board_t|(OPP<<EX_GRID_NOT_SAFE); board[y1][x1] = board_t1|(OPP<<EX_GRID_LBTY);
						x_bak_op[count_bak_op] = x; y_bak_op[count_bak_op] = y; board_bak_op[count_bak_op] = board_t; ++count_bak_op;										
						x_bak_op[count_bak_op] = x1; y_bak_op[count_bak_op] = y1; board_bak_op[count_bak_op] = board_t1; ++count_bak_op;

						y = yoff-1; x = xoff; y1 = yoff-1; x1 = xoff-1; board_t = board[y][x]; board_t1 = board[y1][x1]; 
						board[y][x] = board_t|(OPP<<EX_GRID_NOT_SAFE); board[y1][x1] = board_t1|(OPP<<EX_GRID_LBTY);
						x_bak_op[count_bak_op] = x; y_bak_op[count_bak_op] = y; board_bak_op[count_bak_op] = board_t; ++count_bak_op;										
						x_bak_op[count_bak_op] = x1; y_bak_op[count_bak_op] = y1; board_bak_op[count_bak_op] = board_t1; ++count_bak_op;

						y = yoff; x = xoff+1; y1 = yoff-1; x1 = xoff+1; board_t = board[y][x]; board_t1 = board[y1][x1]; 
						board[y][x] = board_t|(OPP<<EX_GRID_NOT_SAFE); board[y1][x1] = board_t1|(OPP<<EX_GRID_LBTY);
						x_bak_op[count_bak_op] = x; y_bak_op[count_bak_op] = y; board_bak_op[count_bak_op] = board_t; ++count_bak_op;										
						x_bak_op[count_bak_op] = x1; y_bak_op[count_bak_op] = y1; board_bak_op[count_bak_op] = board_t1; ++count_bak_op;

						y = yoff; x = xoff-1; y1 = yoff+1; x1 = xoff-1; board_t = board[y][x]; board_t1 = board[y1][x1]; 
						board[y][x] = board_t|(OPP<<EX_GRID_NOT_SAFE); board[y1][x1] = board_t1|(OPP<<EX_GRID_LBTY);
						x_bak_op[count_bak_op] = x; y_bak_op[count_bak_op] = y; board_bak_op[count_bak_op] = board_t; ++count_bak_op;										
						x_bak_op[count_bak_op] = x1; y_bak_op[count_bak_op] = y1; board_bak_op[count_bak_op] = board_t1; ++count_bak_op;
						
						--size;
						if(size<0) break;
						yoff = m_y+piece_y[m_piece][m_rotate][size];
						xoff = m_x+piece_x[m_piece][m_rotate][size];
					}while(1);				
				}
				
				//find move self for minimax search
			moves_found  = FALSE;
			if(search_extra == TRUE){

				not_prune = TRUE;
				m_piece_min = 0;
				state  = 0; count  = 0; next_stream = 1;
				
				for(m_piece=20; (m_piece>=m_piece_min) & not_prune; m_piece--){
					avail = ((avail_self&(1<<m_piece))!=0);
					for(m_rotate=0; (m_rotate<8) & avail & not_prune; m_rotate++){
						for (m_y=4; (m_y<18) & not_prune; m_y++){
							for (m_x=4; (m_x<18) & not_prune; m_x++){
								 
								lib_check = FALSE;	
								size = piece_sizes[m_piece]-1;							
								yoff = m_y;
								xoff = m_x;

								do{	
									board_t = board[yoff][xoff];
									move_valid = ((board_t & NOT_SAFE_MASK_SELF)==0);
									lib_check |= ((board_t & LIBERTY_MASK_SELF)!=0);
									--size;
									yoff = m_y + piece_y[m_piece][m_rotate][size];
									xoff = m_x + piece_x[m_piece][m_rotate][size]; 									
								}while((move_valid == TRUE) & (size>=0));
								
								if(move_valid & lib_check){ 
									
									//valid move
									moves_found = TRUE;
									m = m_piece<<PIECE_OFF|m_rotate<<ROTATE_OFF|(m_x-3)<<X_OFF|(m_y-3)<<Y_OFF|MINIMAX_MOVE_SELF2<<TYPE_OFF;
									m1 = 0|MINIMAX_MOVE_EVAL<<TYPE_OFF;
									m2 = 0|MINIMAX_MOVE_SELF2_DONE<<TYPE_OFF;
									if(state == 0){
										//forward move, fill pipeline
										if(next_stream == 1) {										
											co_stream_write(stream_eval1_output, &m, sizeof(co_int32));
											co_stream_write(stream_eval1_output, &m1, sizeof(co_int32));
											co_stream_write(stream_eval1_output, &m2, sizeof(co_int32));
											count++; next_stream = 2;																						
										}
										else if(next_stream == 2) {										
											co_stream_write(stream_eval2_output, &m, sizeof(co_int32));
											co_stream_write(stream_eval2_output, &m1, sizeof(co_int32));
											co_stream_write(stream_eval2_output, &m2, sizeof(co_int32));
											count++; next_stream = 1;											
											if(count == 6) state = 1;
										}
									}
									
									else{
										while(1){
											//wait for empty stream, forward move
											if( co_stream_read_nb(stream_eval1_input, &m_recv, sizeof(co_int32)) ){
												co_stream_write(stream_eval1_output, &m, sizeof(co_int32) );
												co_stream_write(stream_eval1_output, &m1, sizeof(co_int32));
												co_stream_write(stream_eval1_output, &m2, sizeof(co_int32));
												break;
											}
											if( co_stream_read_nb(stream_eval2_input, &m_recv, sizeof(co_int32)) ){
												co_stream_write(stream_eval2_output, &m, sizeof(co_int32) );
												co_stream_write(stream_eval2_output, &m1, sizeof(co_int32) );
												co_stream_write(stream_eval2_output, &m2, sizeof(co_int32) );
												break;
											}
										}								
										utility = (m_recv>>UTILITY_OFF)&UTILITY_MASK;
										//better move found
										if(utility > alpha)	{
											alpha = utility; 
											if(beta<=alpha) not_prune = FALSE;
										}
									}
								}
							}
						}
					}
				}

				while(count > 0){
					//empty pipeline
					if(co_stream_read_nb(stream_eval1_input, &m_recv, sizeof(co_int32)) ) {
						--count;
						utility = (m_recv>>UTILITY_OFF)&UTILITY_MASK;
						if(utility > alpha)	alpha = utility;
					}
					if(co_stream_read_nb(stream_eval2_input, &m_recv, sizeof(co_int32)) ) {
						--count;
						utility = (m_recv>>UTILITY_OFF)&UTILITY_MASK;
						if(utility > alpha)	alpha = utility;
					}					
				}				
			}	
				if(moves_found == FALSE){
					//no valid move found 
					m = 0|MINIMAX_MOVE_EVAL<<TYPE_OFF;
					co_stream_write(stream_eval1_output, &m, sizeof(co_int32));
					co_par_break();
					co_stream_read(stream_eval1_input, &m_recv, sizeof(co_int32)); 					
					
					utility = (m_recv>>UTILITY_OFF)&UTILITY_MASK;									
					if(utility > alpha)	alpha = utility;
				}

				//return minimax utility
				move &= 0x0000FFFF;
				move |= alpha<<UTILITY_OFF;
				co_stream_write(stream0_move_output, &move, sizeof(co_int32));
				
				//restore backup values
				if(mx_backup>3){
					
					m = 0|MINIMAX_MOVE_OPP_DONE<<TYPE_OFF;
					co_stream_write(stream_eval1_output, &m, sizeof(co_int32));
					co_stream_write(stream_eval2_output, &m, sizeof(co_int32));
					
					--count_bak_op;
					do{
						#pragma CO PIPELINE
						xoff = x_bak_op[count_bak_op];
						yoff = y_bak_op[count_bak_op];
						board[yoff][xoff] = board_bak_op[count_bak_op];
						--count_bak_op;
						}while(count_bak_op>=0);				
					score_opp = score_opp_bak;
					avail_opp = avail_opp_bak;
				}
				
				break;
							
			case MINIMAX_MOVE_SELF:
				//forward
				co_stream_write(stream_eval1_output, &move, sizeof(co_int32));
				co_stream_write(stream_eval2_output, &move, sizeof(co_int32));
				
				//update board self
				avail_self_bak = avail_self;
				score_self_bak = score_self;
				
				size = piece_sizes[m_piece];
				avail_self &= ~(1<<m_piece);
				score_self += size;
				--size;
				yoff = m_y;
				xoff = m_x;
				count_bak_s = 0;
				do{
					//backup data
					x_bak_s[count_bak_s] = xoff; y_bak_s[count_bak_s] = yoff; board_bak_s[count_bak_s] = board[yoff][xoff]; ++count_bak_s;
					board[yoff][xoff] = SELF | (0x3<<EX_GRID_NOT_SAFE);
					
					y = yoff+1; x = xoff; y1 = yoff+1; x1 = xoff+1; board_t = board[y][x]; board_t1 = board[y1][x1]; 
					board[y][x] = board_t|(SELF<<EX_GRID_NOT_SAFE); board[y1][x1] = board_t1|(SELF<<EX_GRID_LBTY);
					x_bak_s[count_bak_s] = x; y_bak_s[count_bak_s] = y; board_bak_s[count_bak_s] = board_t; ++count_bak_s;					
					x_bak_s[count_bak_s] = x1; y_bak_s[count_bak_s] = y1; board_bak_s[count_bak_s] = board_t1; ++count_bak_s;
					
					y = yoff-1; x = xoff; y1 = yoff-1; x1 = xoff-1; board_t = board[y][x]; board_t1 = board[y1][x1];
					board[y][x] = board_t|(SELF<<EX_GRID_NOT_SAFE); board[y1][x1] = board_t1|(SELF<<EX_GRID_LBTY);
					x_bak_s[count_bak_s] = x; y_bak_s[count_bak_s] = y; board_bak_s[count_bak_s] = board_t; ++count_bak_s;
					x_bak_s[count_bak_s] = x1; y_bak_s[count_bak_s] = y1; board_bak_s[count_bak_s] = board_t1; ++count_bak_s;
			
					y = yoff; x = xoff+1; y1 = yoff-1; x1 = xoff+1; board_t = board[y][x]; board_t1 = board[y1][x1];
					board[y][x] = board_t|(SELF<<EX_GRID_NOT_SAFE); board[y1][x1] = board_t1|(SELF<<EX_GRID_LBTY);
					x_bak_s[count_bak_s] = x; y_bak_s[count_bak_s] = y; board_bak_s[count_bak_s] = board_t; ++count_bak_s;
					x_bak_s[count_bak_s] = x1; y_bak_s[count_bak_s] = y1; board_bak_s[count_bak_s] = board_t1; ++count_bak_s;
			
					y = yoff; x = xoff-1; y1 = yoff+1; x1 = xoff-1; board_t = board[y][x]; board_t1 = board[y1][x1]; 
					board[y][x] = board_t|(SELF<<EX_GRID_NOT_SAFE);board[y1][x1] = board_t1|(SELF<<EX_GRID_LBTY);
					x_bak_s[count_bak_s] = x; y_bak_s[count_bak_s] = y; board_bak_s[count_bak_s] = board_t; ++count_bak_s;
					x_bak_s[count_bak_s] = x1; y_bak_s[count_bak_s] = y1; board_bak_s[count_bak_s] = board_t1; ++count_bak_s;
				
					--size;
					if(size<0) break;
					yoff = m_y+piece_y[m_piece][m_rotate][size];
					xoff = m_x+piece_x[m_piece][m_rotate][size];
				}while(1);				
				break;
			
			case MINIMAX_MOVE_SELF_DONE:
				
				//forward
				co_stream_write(stream_eval1_output, &move, sizeof(co_int32));
				co_stream_write(stream_eval2_output, &move, sizeof(co_int32));
								
				//restore backup values
				--count_bak_s;
				while(count_bak_s>=0){
					#pragma CO PIPELINE
					xoff = x_bak_s[count_bak_s];
					yoff = y_bak_s[count_bak_s];
					board[yoff][xoff] = board_bak_s[count_bak_s];
					--count_bak_s;
					};
				
				score_self = score_self_bak;
				avail_self = avail_self_bak;
				break;
				
			case PERM_MOVE_SELF:
				//forward
				co_stream_write(stream_eval1_output, &move, sizeof(co_int32));
				co_stream_write(stream_eval2_output, &move, sizeof(co_int32));

				num_avail_self--;
				if((num_avail_opp <= SEARCH_EXTRA_LIMIT) && (num_avail_self <= SEARCH_EXTRA_LIMIT))
					search_extra = TRUE;
				else
					search_extra = FALSE;
				
				//update board self
				size = piece_sizes[m_piece];
				avail_self &= ~(1<<m_piece);
				score_self += size;
				--size;
				yoff = m_y;
				xoff = m_x;
				
				do{
					board[yoff][xoff] = SELF | (0x3<<EX_GRID_NOT_SAFE);
					
					y = yoff+1; x = xoff; y1 = yoff+1; x1 = xoff+1; board_t = board[y][x]; board_t1 = board[y1][x1]; 
					board[y][x] = board_t|(SELF<<EX_GRID_NOT_SAFE); board[y1][x1] = board_t1|(SELF<<EX_GRID_LBTY);
					
					y = yoff-1; x = xoff; y1 = yoff-1; x1 = xoff-1; board_t = board[y][x]; board_t1 = board[y1][x1]; 
					board[y][x] = board_t|(SELF<<EX_GRID_NOT_SAFE); board[y1][x1] = board_t1|(SELF<<EX_GRID_LBTY);			
					
					y = yoff; x = xoff+1; y1 = yoff-1; x1 = xoff+1; board_t = board[y][x]; board_t1 = board[y1][x1];
					board[y][x] = board_t|(SELF<<EX_GRID_NOT_SAFE); board[y1][x1] = board_t1|(SELF<<EX_GRID_LBTY);
					
					y = yoff; x = xoff-1; y1 = yoff+1; x1 = xoff-1; board_t = board[y][x]; board_t1 = board[y1][x1];
					board[y][x] = board_t|(SELF<<EX_GRID_NOT_SAFE); board[y1][x1] = board_t1|(SELF<<EX_GRID_LBTY);
				
					--size;
					if(size<0) break;
					yoff = m_y+piece_y[m_piece][m_rotate][size];
					xoff = m_x+piece_x[m_piece][m_rotate][size];
				}while(1);				
				break;
				
			case PERM_MOVE_OPP:

				//forward
				co_stream_write(stream_eval1_output, &move, sizeof(co_int32));
				co_stream_write(stream_eval2_output, &move, sizeof(co_int32));

				num_avail_opp--;
				if((num_avail_opp <= SEARCH_EXTRA_LIMIT) && (num_avail_self <= SEARCH_EXTRA_LIMIT))
					search_extra = TRUE;
				else
					search_extra = FALSE;
				
				//update board opp
				size = piece_sizes[m_piece];
				avail_opp &= ~(1<<m_piece); 
				score_opp += size;
				
				--size;
				yoff = m_y;
				xoff = m_x;
										
				do{
					board[yoff][xoff] = OPP | (0x3<<EX_GRID_NOT_SAFE);
					
					y = yoff+1; x = xoff; y1 = yoff+1; x1 = xoff+1; board_t = board[y][x]; board_t1 = board[y1][x1]; 
					board[y][x] = board_t|(OPP<<EX_GRID_NOT_SAFE);	board[y1][x1] = board_t1|(OPP<<EX_GRID_LBTY);
					
					y = yoff-1; x = xoff; y1 = yoff-1; x1 = xoff-1; board_t = board[y][x]; board_t1 = board[y1][x1]; 
					board[y][x] = board_t|(OPP<<EX_GRID_NOT_SAFE); board[y1][x1] = board_t1|(OPP<<EX_GRID_LBTY);			
					
					y = yoff; x = xoff+1; y1 = yoff-1; x1 = xoff+1; board_t = board[y][x]; board_t1 = board[y1][x1];
					board[y][x] = board_t|(OPP<<EX_GRID_NOT_SAFE); board[y1][x1] = board_t1|(OPP<<EX_GRID_LBTY);
					
					y = yoff; x = xoff-1; y1 = yoff+1; x1 = xoff-1; board_t = board[y][x]; board_t1 = board[y1][x1];
					board[y][x] = board_t|(OPP<<EX_GRID_NOT_SAFE); board[y1][x1] = board_t1|(OPP<<EX_GRID_LBTY);
					--size;
					
					if(size<0) break;
					yoff = m_y+piece_y[m_piece][m_rotate][size];
					xoff = m_x+piece_x[m_piece][m_rotate][size];
					
				}while(1);
				break;
			
			case GAME_RESET:

				//forward
				co_stream_write(stream_eval1_output, &move, sizeof(co_int32));
				co_stream_write(stream_eval2_output, &move, sizeof(co_int32));
				
				//init board
				for(y = 0; y < 32; y++){
					for(x = 0; x < 32; x++){
						if((x < 4) || (y < 4) || (x > 17) || (y > 17)){
							board[y][x] = (BORDER | (0x3<<EX_GRID_NOT_SAFE));
						}
						else {
							board[y][x] = 0;
						}	
					}
				}
				
				//init available pieces
				avail_self = 0x7FFFFF;
				avail_opp = 0x7FFFFF;
				
				//init scores
				score_self = 0;
				score_opp = 0;
			
				num_avail_self = 21;
				num_avail_opp = 21;
				search_extra = FALSE;
				
				break;
				
			default:
				break;
		}
	}
		
}

void eval_func(co_stream stream0_move_input, co_stream stream0_move_output	)
{
	co_int8 piece_sizes[21] = { 1, 2, 3, 3, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5 };
	  
	co_int8 piece_y[21][8][4] = {
	  { { 0, 0, 0, 0 },  { 0, 0, 0, 0 },  { 0, 0, 0, 0 },  { 0, 0, 0, 0 },	{ 0, 0, 0, 0 },  { 0, 0, 0, 0 },  { 0, 0, 0, 0 },  { 0, 0, 0, 0 },	},
	  { { 1, 0, 0, 0 },  { 1, 0, 0, 0 },  { 0, 0, 0, 0 },  { 0, 0, 0, 0 },	{ -1, 0, 0, 0 }, { -1, 0, 0, 0 }, { 0, 0, 0, 0 },  { 0, 0, 0, 0 },	},
	  { { -1, 1, 0, 0 }, { -1, 1, 0, 0 }, { 0, 0, 0, 0 },  { 0, 0, 0, 0 },	{ -1, 1, 0, 0 }, { -1, 1, 0, 0 }, { 0, 0, 0, 0 },  { 0, 0, 0, 0 },	},
	  { { -1, 0, 0, 0 }, { -1, 0, 0, 0 }, { 0, 1, 0, 0 },  { 0, 1, 0, 0 },	{ 0, 1, 0, 0 },  { 0, 1, 0, 0 },  { -1, 0, 0, 0 }, { -1, 0, 0, 0 }, },
	  { { -1, 1, 2, 0 }, { -1, 1, 2, 0 }, { 0, 0, 0, 0 },  { 0, 0, 0, 0 },	{ -2, -1, 1, 0 },{ -2, -1, 1, 0 },{ 0, 0, 0, 0 },  { 0, 0, 0, 0 },	},
	  { { -1, 1, 1, 0 }, { -1, 1, 1, 0 }, { -1, 0, 0, 0 }, { -1, 0, 0, 0 }, { -1, -1, 1, 0 },{ -1, -1, 1, 0 },{ 0, 0, 1, 0 },  { 0, 0, 1, 0 },	},
	  { { -1, 0, 1, 0 }, { -1, 0, 1, 0 }, { 0, 0, 1, 0 },  { 0, 0, 1, 0 },	{ -1, 0, 1, 0 }, { -1, 0, 1, 0 }, { -1, 0, 0, 0 }, { -1, 0, 0, 0 }, },
	  { { 0, 1, 1, 0 },  { 0, 1, 1, 0 },  { 0, 1, 1, 0 },  { 0, 1, 1, 0 },	{ -1, -1, 0, 0 },{ -1, -1, 0, 0 },{ -1, -1, 0, 0 },{ -1, -1, 0, 0 },},
	  { { 0, 1, 1, 0 },  { 0, 1, 1, 0 },  { -1, 0, 1, 0 }, { -1, 0, 1, 0 }, { -1, -1, 0, 0 },{ -1, -1, 0, 0 },{ -1, 0, 1, 0 }, { -1, 0, 1, 0 }, },
	  { { -2, -1, 1, 2 },{ -2, -1, 1, 2 },{ 0, 0, 0, 0 },  { 0, 0, 0, 0 },	{ -2, -1, 1, 2 },{ -2, -1, 1, 2 },{ 0, 0, 0, 0 },  { 0, 0, 0, 0 },	},
	  { { -2, -1, 1, 1 },{ -2, -1, 1, 1 },{ -1, 0, 0, 0 }, { -1, 0, 0, 0 }, { -1, -1, 1, 2 },{ -1, -1, 1, 2 },{ 0, 0, 0, 1 },  { 0, 0, 0, 1 },	},
	  { { -2, -1, 0, 1 },{ -2, -1, 0, 1 },{ -1, -1, 0, 0 },{ -1, -1, 0, 0 },{ -1, 0, 1, 2 }, { -1, 0, 1, 2 }, { 0, 0, 1, 1 },  { 0, 0, 1, 1 },	},
	  { { -1, 0, 1, 1 }, { -1, 0, 1, 1 }, { -1, -1, 0, 0 },{ -1, -1, 0, 0 },{ -1, -1, 0, 1 },{ -1, -1, 0, 1 },{ 0, 0, 1, 1 },  { 0, 0, 1, 1 },	},
	  { { -1, -1, 1, 1 },{ -1, -1, 1, 1 },{ -1, -1, 0, 0 },{ -1, -1, 0, 0 },{ -1, -1, 1, 1 },{ -1, -1, 1, 1 },{ 0, 0, 1, 1 },  { 0, 0, 1, 1 },	},
	  { { -1, 0, 1, 2 }, { -1, 0, 1, 2 }, { 0, 0, 0, 1 },  { 0, 0, 0, 1 },	{ -2, -1, 0, 1 },{ -2, -1, 0, 1 },{ -1, 0, 0, 0 }, { -1, 0, 0, 0 }, },
	  { { -1, 1, 1, 1 }, { -1, 1, 1, 1 }, { -1, 0, 0, 1 }, { -1, 0, 0, 1 }, { -1, -1, -1, 1},{ -1, -1, -1, 1},{ -1, 0, 0, 1 }, { -1, 0, 0, 1 }, },
	  { { -2, -1, 0, 0 },{ -2, -1, 0, 0 },{ 0, 0, 1, 2 },  { 0, 0, 1, 2 },	{ 0, 0, 1, 2 },  { 0, 0, 1, 2 },  { -2, -1, 0, 0 },{ -2, -1, 0, 0 },},
	  { { -1, -1, 0, 1 },{ -1, -1, 0, 1 },{ -1, 0, 1, 1 }, { -1, 0, 1, 1 }, { -1, 0, 1, 1 }, { -1, 0, 1, 1 }, { -1, -1, 0, 1 },{ -1, -1, 0, 1 },},
	  { { -1, 0, 0, 1 }, { -1, 0, 0, 1 }, { -1, -1, 1, 1 },{ -1, -1, 1, 1 },{ -1, 0, 0, 1 }, { -1, 0, 0, 1 }, { -1, -1, 1, 1 },{ -1, -1, 1, 1 },},
	  { { -1, 0, 0, 1 }, { -1, 0, 0, 1 }, { -1, -1, 0, 1 },{ -1, -1, 0, 1 },{ -1, 0, 0, 1 }, { -1, 0, 0, 1 }, { -1, 0, 1, 1 }, { -1, 0, 1, 1 }, },
	  { { -1, 0, 0, 1 }, { -1, 0, 0, 1 }, { -1, 0, 0, 1 }, { -1, 0, 0, 1 }, { -1, 0, 0, 1 }, { -1, 0, 0, 1 }, { -1, 0, 0, 1 }, { -1, 0, 0, 1 }, },
	};
	
	co_int8 piece_x[21][8][4] = {
	  { { 0, 0, 0, 0 },  { 0, 0, 0, 0 },  { 0, 0, 0, 0 },  { 0, 0, 0, 0 },	{ 0, 0, 0, 0 },  { 0, 0, 0, 0 },	{ 0, 0, 0, 0 },  { 0, 0, 0, 0 },   },
	  { { 0, 0, 0, 0 },  { 0, 0, 0, 0 },  { -1, 0, 0, 0 }, { 1, 0, 0, 0 },	{ 0, 0, 0, 0 },  { 0, 0, 0, 0 },	{ 1, 0, 0, 0 },  { -1, 0, 0, 0 },  },
	  { { 0, 0, 0, 0 },  { 0, 0, 0, 0 },  { -1, 1, 0, 0 }, { -1, 1, 0, 0 }, { 0, 0, 0, 0 },  { 0, 0, 0, 0 },	{ -1, 1, 0, 0 }, { -1, 1, 0, 0 },  },
	  { { 0, 1, 0, 0 },  { 0, -1, 0, 0 }, { 1, 0, 0, 0 },  { -1, 0, 0, 0 }, { -1, 0, 0, 0 }, { 1, 0, 0, 0 },	{ 0, -1, 0, 0 }, { 0, 1, 0, 0 },   },
	  { { 0, 0, 0, 0 },  { 0, 0, 0, 0 },  { -2, -1, 1, 0 },{ -1, 1, 2, 0 }, { 0, 0, 0, 0 },  { 0, 0, 0, 0 },	{ -1, 1, 2, 0 }, { -2, -1, 1, 0 }, },
	  { { 0, -1, 0, 0 }, { 0, 0, 1, 0 },  { -1, -1, 1, 0 },{ 1, -1, 1, 0 }, { 0, 1, 0, 0 },  { -1, 0, 0, 0 },	{ -1, 1, 1, 0 }, { -1, 1, -1, 0 }, },
	  { { 0, 1, 0, 0 },  { 0, -1, 0, 0 }, { -1, 1, 0, 0 }, { -1, 1, 0, 0 }, { 0, -1, 0, 0 }, { 0, 1, 0, 0 },	{ 0, -1, 1, 0 }, { 0, -1, 1, 0 },  },
	  { { 1, 0, 1, 0 },  { -1, -1, 0, 0 },{ -1, -1, 0, 0 },{ 1, 0, 1, 0 },	{ -1, 0, -1, 0 },{ 0, 1, 1, 0 },	{ 0, 1, 1, 0 },  { -1, 0, -1, 0 }, },
	  { { -1, 0, 1, 0 }, { 1, -1, 0, 0 }, { 0, -1, -1, 0 },{ 0, 1, 1, 0 },	{ -1, 0, 1, 0 }, { 0, 1, -1, 0 },	{ 1, 1, 0, 0 },  { -1, -1, 0, 0 }, },
	  { { 0, 0, 0, 0 },  { 0, 0, 0, 0 },  { -2, -1, 1, 2 },{ -2, -1, 1, 2 },{ 0, 0, 0, 0 },  { 0, 0, 0, 0 },	{ -2, -1, 1, 2 },{ -2, -1, 1, 2 }, },
	  { { 0, 0, -1, 0 }, { 0, 0, 0, 1 },  { -1, -1, 1, 2 },{ 1, -2, -1, 1 },{ 0, 1, 0, 0 },  { -1, 0, 0, 0 },	{ -2, -1, 1, 1 },{ -1, 1, 2, -1 }, },
	  { { 0, 0, -1, -1 },{ 0, 0, 1, 1 },  { -1, 0, 1, 2 }, { 0, 1, -2, -1 },{ 1, 1, 0, 0 },  { -1, -1, 0, 0 },	{ -2, -1, 0, 1 },{ 1, 2, -1, 0 },  },
	  { { 0, -1, -1, 0 },{ 0, 1, 0, 1 },  { -1, 0, -1, 1 },{ 0, 1, -1, 1 }, { 0, 1, 1, 0 },  { -1, 0, -1, 0 },	{ -1, 1, 0, 1 }, { -1, 1, -1, 0 }, },
	  { { -1, 0, -1, 0 },{ 0, 1, 0, 1 },  { -1, 1, -1, 1 },{ -1, 1, -1, 1 },{ 0, 1, 0, 1 },  { -1, 0, -1, 0 },	{ -1, 1, -1, 1 },{ -1, 1, -1, 1 }, },
	  { { 0, 1, 0, 0 },  { 0, -1, 0, 0 }, { -2, -1, 1, 0 },{ -1, 1, 2, 0 }, { 0, 0, -1, 0 }, { 0, 0, 1, 0 },	{ 0, -1, 1, 2 }, { 0, -2, -1, 1 }, },
	  { { 0, -1, 0, 1 }, { 0, -1, 0, 1 }, { -1, -1, 1, -1},{ 1, -1, 1, 1 }, { -1, 0, 1, 0 }, { -1, 0, 1, 0 },	{ 1, -1, 1, 1 }, { -1, -1, 1, -1 },},
	  { { 0, 0, 1, 2 },  { 0, 0, -2, -1 },{ 1, 2, 0, 0 },  { -2, -1, 0, 0 },{ -2, -1, 0, 0 },{ 1, 2, 0, 0 },	{ 0, 0, -2, -1 },{ 0, 0, 1, 2 },   },
	  { { -1, 0, 1, 1 }, { 0, 1, -1, -1 },{ 1, 1, -1, 0 }, { -1, -1, 0, 1 },{ -1, -1, 0, 1 },{ 1, 1, -1, 0 },	{ 0, 1, -1, -1 },{ -1, 0, 1, 1 },  },
	  { { -1, -1, 1, 1 },{ 1, -1, 1, -1 },{ 0, 1, -1, 0 }, { -1, 0, 0, 1 }, { -1, -1, 1, 1 },{ 1, -1, 1, -1 },	{ 0, 1, -1, 0 }, { -1, 0, 0, 1 },  },
	  { { -1, -1, 1, 0 },{ 1, -1, 1, 0 }, { 0, 1, -1, 0 }, { -1, 0, 1, 0 }, { 0, -1, 1, 1 }, { 0, -1, 1, -1 },	{ 0, 1, -1, 0 }, { 0, -1, 0, 1 },  },
	  { { 0, -1, 1, 0 }, { 0, -1, 1, 0 }, { 0, -1, 1, 0 }, { 0, -1, 1, 0 }, { 0, -1, 1, 0 }, { 0, -1, 1, 0 },	{ 0, -1, 1, 0 }, { 0, -1, 1, 0 },  },
	};
	
	co_int8 board[32][32];
	co_int8 score_self, score_opp;
	co_int24 avail_self, avail_opp;
	
	co_int8 x,y,z, xoff, yoff, board_t, m_piece_min, size, temp, temp1, move_type, count;
	co_int8 x1, y1, board_t1;
	co_int8 m_x, m_y, m_piece, m_rotate, best_move_x, best_move_y, best_move_piece, best_move_rotate;
	co_int2 moves_found, lib_check, move_valid, avail, state;
	co_int32 move;

	//backup data
	co_int8 x_bak_s[50], y_bak_s[50], board_bak_s[50], count_bak_s;
	co_int8 x_bak_op[50], y_bak_op[50], board_bak_op[50], count_bak_op;
	co_int8 x_bak_s2[50], y_bak_s2[50], board_bak_s2[50], count_bak_s2;
	co_int8	score_self_bak, score_opp_bak, score_self2_bak;
	co_int24 avail_self_bak, avail_opp_bak, avail_self2_bak;

	//self
	co_int8 s_m4_0, s_m3_m1, s_m3_0, s_m3_1, s_m2_m2, s_m2_m1, s_m2_0, s_m2_1, s_m2_2;
	co_int8 s_m1_m3, s_m1_m2, s_m1_m1, s_m1_0, s_m1_1, s_m1_2, s_m1_3;
	co_int8 s_0_m4, s_0_m3, s_0_m2, s_0_m1, s_0_0, s_0_1, s_0_2, s_0_3, s_0_4;
	co_int8 s_1_m3, s_1_m2, s_1_m1, s_1_0, s_1_1, s_1_2, s_1_3;
	co_int8 s_2_m2, s_2_m1, s_2_0, s_2_1, s_2_2, s_3_m1, s_3_0, s_3_1, s_4_0;

	//opp
	co_int8 o_m4_0, o_m3_m1, o_m3_0, o_m3_1, o_m2_m2, o_m2_m1, o_m2_0, o_m2_1, o_m2_2;
	co_int8 o_m1_m3, o_m1_m2, o_m1_m1, o_m1_0, o_m1_1, o_m1_2, o_m1_3;
	co_int8 o_0_m4, o_0_m3, o_0_m2, o_0_m1, o_0_0, o_0_1, o_0_2, o_0_3, o_0_4;
	co_int8 o_1_m3, o_1_m2, o_1_m1, o_1_0, o_1_1, o_1_2, o_1_3;
	co_int8 o_2_m2, o_2_m1, o_2_0, o_2_1, o_2_2, o_3_m1, o_3_0, o_3_1, o_4_0;

	co_int16 net_lib, utility;
	co_int16 value_self, value_opp;
	co_int8 val;

	co_array_config(board, co_kind, "dualsync"); 
		
	//open streams
	co_stream_open(stream0_move_input, O_RDONLY, INT_TYPE(32));	co_stream_open(stream0_move_output, O_WRONLY, INT_TYPE(32));

	//init board
	for(y = 0; y < 32; y++){
		for(x = 0; x < 32; x++){
			if((x < 4) || (y < 4) || (x > 17) || (y > 17)){
				board[y][x] = (BORDER | (0x3<<EX_GRID_NOT_SAFE));
			}
			else {
				board[y][x] = 0;
			}	
		}
	}

	//init available pieces
	avail_self = 0x7FFFFF;	
	avail_opp = 0x7FFFFF;
	
	//init scores
	score_self = 0;	
	score_opp = 0;
	
	while(1){			
		//read incoming message
		co_stream_read(stream0_move_input, &move, sizeof(co_int32));			
		
		move_type = (move>>TYPE_OFF)&TYPE_MASK;
		m_x = ((move>>X_OFF)&X_MASK) + 3;
		m_y = ((move>>Y_OFF)&Y_MASK) + 3;
		m_piece = (move>>PIECE_OFF)&PIECE_MASK;
		m_rotate = (move>>ROTATE_OFF)&ROTATE_MASK;
		
		switch (move_type){

			case MINIMAX_MOVE_EVAL:
				//heuristic evaluation
				utility = ((co_int16)(score_self - score_opp))*68;
				net_lib = 0;
				
				for(y=4; y<18; y++){
					for(x=4;x<18;x++){
						temp=board[y][x]; 		
						s_0_0 = ( ((temp&NOT_SAFE_MASK_SELF)==0) & ((temp&LIBERTY_MASK_SELF)!=0) );		
						o_0_0 = ( ((temp&NOT_SAFE_MASK_OPP)==0) & ((temp & LIBERTY_MASK_OPP)!=0) );

						if(s_0_0 | o_0_0){
							value_self = 0;	value_opp = 0;
							//read in all the values
							temp=board[y-4][x];		s_m4_0 = ((temp&NOT_SAFE_MASK_SELF)==0);		o_m4_0 = ((temp&NOT_SAFE_MASK_OPP)==0);
							temp=board[y-3][x-1]; 	s_m3_m1 = ((temp&NOT_SAFE_MASK_SELF)==0);		o_m3_m1 = ((temp&NOT_SAFE_MASK_OPP)==0);
							temp=board[y-3][x]; 	s_m3_0 = ((temp&NOT_SAFE_MASK_SELF)==0);		o_m3_0 = ((temp&NOT_SAFE_MASK_OPP)==0);
							temp=board[y-3][x+1]; 	s_m3_1 = ((temp&NOT_SAFE_MASK_SELF)==0);		o_m3_1 = ((temp&NOT_SAFE_MASK_OPP)==0);
							temp=board[y-2][x-2]; 	s_m2_m2 = ((temp&NOT_SAFE_MASK_SELF)==0);		o_m2_m2 = ((temp&NOT_SAFE_MASK_OPP)==0);
							temp=board[y-2][x-1];	s_m2_m1 = ((temp&NOT_SAFE_MASK_SELF)==0);		o_m2_m1 = ((temp&NOT_SAFE_MASK_OPP)==0);
							temp=board[y-2][x];		s_m2_0 = ((temp&NOT_SAFE_MASK_SELF)==0);		o_m2_0 = ((temp&NOT_SAFE_MASK_OPP)==0);
							temp=board[y-2][x+1];	s_m2_1 = ((temp&NOT_SAFE_MASK_SELF)==0);		o_m2_1 = ((temp&NOT_SAFE_MASK_OPP)==0);
							temp=board[y-2][x+2];	s_m2_2 = ((temp&NOT_SAFE_MASK_SELF)==0);		o_m2_2 = ((temp&NOT_SAFE_MASK_OPP)==0);
							temp=board[y-1][x-3];	s_m1_m3 = ((temp&NOT_SAFE_MASK_SELF)==0);		o_m1_m3 = ((temp&NOT_SAFE_MASK_OPP)==0);
							temp=board[y-1][x-2];	s_m1_m2 = ((temp&NOT_SAFE_MASK_SELF)==0);		o_m1_m2 = ((temp&NOT_SAFE_MASK_OPP)==0);
							temp=board[y-1][x-1];	s_m1_m1 = ((temp&NOT_SAFE_MASK_SELF)==0);		o_m1_m1 = ((temp&NOT_SAFE_MASK_OPP)==0);
							temp=board[y-1][x];		s_m1_0 = ((temp&NOT_SAFE_MASK_SELF)==0);		o_m1_0 = ((temp&NOT_SAFE_MASK_OPP)==0);
							temp=board[y-1][x+1];	s_m1_1 = ((temp&NOT_SAFE_MASK_SELF)==0);		o_m1_1 = ((temp&NOT_SAFE_MASK_OPP)==0);
							temp=board[y-1][x+2];	s_m1_2 = ((temp&NOT_SAFE_MASK_SELF)==0);		o_m1_2 = ((temp&NOT_SAFE_MASK_OPP)==0);
							temp=board[y-1][x+3];	s_m1_3 = ((temp&NOT_SAFE_MASK_SELF)==0);		o_m1_3 = ((temp&NOT_SAFE_MASK_OPP)==0);
							temp=board[y][x-4];		s_0_m4 = ((temp&NOT_SAFE_MASK_SELF)==0);		o_0_m4 = ((temp&NOT_SAFE_MASK_OPP)==0);
							temp=board[y][x-3];		s_0_m3 = ((temp&NOT_SAFE_MASK_SELF)==0);		o_0_m3 = ((temp&NOT_SAFE_MASK_OPP)==0);
							temp=board[y][x-2];		s_0_m2 = ((temp&NOT_SAFE_MASK_SELF)==0);		o_0_m2 = ((temp&NOT_SAFE_MASK_OPP)==0);
							temp=board[y][x-1];		s_0_m1 = ((temp&NOT_SAFE_MASK_SELF)==0);		o_0_m1 = ((temp&NOT_SAFE_MASK_OPP)==0);
							temp=board[y][x+1];		s_0_1 = ((temp&NOT_SAFE_MASK_SELF)==0);			o_0_1 = ((temp&NOT_SAFE_MASK_OPP)==0);
							temp=board[y][x+2];		s_0_2 = ((temp&NOT_SAFE_MASK_SELF)==0);			o_0_2 = ((temp&NOT_SAFE_MASK_OPP)==0);
							temp=board[y][x+3];		s_0_3 = ((temp&NOT_SAFE_MASK_SELF)==0);			o_0_3 = ((temp&NOT_SAFE_MASK_OPP)==0);
							temp=board[y][x+4];		s_0_4 = ((temp&NOT_SAFE_MASK_SELF)==0);			o_0_4 = ((temp&NOT_SAFE_MASK_OPP)==0);
							temp=board[y+1][x-3];	s_1_m3 = ((temp&NOT_SAFE_MASK_SELF)==0);		o_1_m3 = ((temp&NOT_SAFE_MASK_OPP)==0);
							temp=board[y+1][x-2];	s_1_m2 = ((temp&NOT_SAFE_MASK_SELF)==0);		o_1_m2 = ((temp&NOT_SAFE_MASK_OPP)==0);
							temp=board[y+1][x-1];	s_1_m1 = ((temp&NOT_SAFE_MASK_SELF)==0);		o_1_m1 = ((temp&NOT_SAFE_MASK_OPP)==0);
							temp=board[y+1][x]; 	s_1_0 = ((temp&NOT_SAFE_MASK_SELF)==0);			o_1_0 = ((temp&NOT_SAFE_MASK_OPP)==0);
							temp=board[y+1][x+1];	s_1_1 = ((temp&NOT_SAFE_MASK_SELF)==0);			o_1_1 = ((temp&NOT_SAFE_MASK_OPP)==0);
							temp=board[y+1][x+2];	s_1_2 = ((temp&NOT_SAFE_MASK_SELF)==0);			o_1_2 = ((temp&NOT_SAFE_MASK_OPP)==0);
							temp=board[y+1][x+3];	s_1_3 = ((temp&NOT_SAFE_MASK_SELF)==0);			o_1_3 = ((temp&NOT_SAFE_MASK_OPP)==0);
							temp=board[y+2][x-2];	s_2_m2 = ((temp&NOT_SAFE_MASK_SELF)==0);		o_2_m2 = ((temp&NOT_SAFE_MASK_OPP)==0);
							temp=board[y+2][x-1];	s_2_m1 = ((temp&NOT_SAFE_MASK_SELF)==0);		o_2_m1 = ((temp&NOT_SAFE_MASK_OPP)==0);
							temp=board[y+2][x];		s_2_0 = ((temp&NOT_SAFE_MASK_SELF)==0);			o_2_0 = ((temp&NOT_SAFE_MASK_OPP)==0);
							temp=board[y+2][x+1];	s_2_1 = ((temp&NOT_SAFE_MASK_SELF)==0);			o_2_1 = ((temp&NOT_SAFE_MASK_OPP)==0);
							temp=board[y+2][x+2]; 	s_2_2 = ((temp&NOT_SAFE_MASK_SELF)==0);			o_2_2 = ((temp&NOT_SAFE_MASK_OPP)==0);
							temp=board[y+3][x-1];	s_3_m1 = ((temp&NOT_SAFE_MASK_SELF)==0);		o_3_m1 = ((temp&NOT_SAFE_MASK_OPP)==0);
							temp=board[y+3][x];		s_3_0 = ((temp&NOT_SAFE_MASK_SELF)==0);			o_3_0 = ((temp&NOT_SAFE_MASK_OPP)==0);
							temp=board[y+3][x+1]; 	s_3_1 = ((temp&NOT_SAFE_MASK_SELF)==0);			o_3_1 = ((temp&NOT_SAFE_MASK_OPP)==0);
							temp=board[y+4][x];		s_4_0 = ((temp&NOT_SAFE_MASK_SELF)==0);			o_4_0 = ((temp&NOT_SAFE_MASK_OPP)==0);

							//evaluate the tree
							//self
							if(s_0_0){
								if (s_1_0){
									value_self++;
									if (s_2_0){
										#pragma CO FLATTEN
										val = 1;
										if (s_3_0)	val += s_4_0 + s_3_1 + s_3_m1 + 1;
										if (s_2_1)	val += s_3_1 + s_2_2 + s_1_1 + 1;
										if (s_2_m1)	val += s_3_m1 + s_2_m2 + s_1_m1 + 1;
										value_self += val;
									}
									if (s_1_1){
										#pragma CO FLATTEN
										val = 1;
										if (s_2_1)	val+= s_3_1 + s_2_2 + s_2_0 + 1;
										if (s_1_2)	val+= s_2_2 + s_0_2 + s_1_3 + 1;
										if (s_0_1)	val+= s_m1_1 + s_0_2 + 1;
										value_self += val;
									}
									if (s_1_m1){
										#pragma CO FLATTEN
										val=1;
										if (s_2_m1)	val+=	s_3_m1 + s_2_m2 +  s_2_0 +  1;
										if (s_1_m2)	val+= s_2_m2 + s_0_m2 + s_1_m3 + 1;
										if (s_0_m1)	val+=	s_m1_m1 + s_0_m2 + 1;
										value_self += val;
									}
								}
								if (s_m1_0){
									value_self++;
									if (s_m2_0){
										#pragma CO FLATTEN
										val=1;
										if (s_m3_0)	val+= s_m4_0 + s_m3_1 + s_m3_m1 + 1;
										if (s_m2_1)	val+= s_m3_1 + s_m2_2 + s_m1_1 + 1;
										if (s_m2_m1)val+= s_m3_m1 + s_m2_m2 + s_m1_m1 + 1;
										value_self += val;
									}
									if (s_m1_1){
										#pragma CO FLATTEN
										val=1;
										if (s_m2_1)	val+= s_m3_1 + s_m2_2 + s_m2_0 + 1;
										if (s_m1_2)	val+= s_m2_2 + s_0_2 + s_m1_3 + 1;
										if (s_0_1)	val+= s_1_1 + s_0_2 + 1;
										value_self += val;
									}
									if (s_m1_m1){
										#pragma CO FLATTEN
										val=1; 
										if (s_m2_m1) val+= s_m3_m1 + s_m2_m2 + s_m2_0 + 1;
										if (s_m1_m2) val+= s_m2_m2 + s_0_m2 + s_m1_m3 + 1;
										if (s_0_m1) val+= s_1_m1 + s_0_m2 + 1;
										value_self += val;
									}
								}
								if (s_0_1){
									value_self++;
									if (s_0_2){
										#pragma CO FLATTEN
										val=1;
										if (s_0_3) val+= s_0_4 + s_1_3 + s_m1_3 + 1;
										if (s_1_2) val+= s_1_3 + s_2_2 + s_1_1 + 1;
										if (s_m1_2)val+= s_m1_3 + s_m2_2 + s_m1_1 + 1;
										value_self += val;
									}
									if (s_1_1){
										#pragma CO FLATTEN
										val=1;
										if (s_1_2) val+= s_1_3 + s_2_2 + s_0_2 + 1;
										if (s_2_1) val+= s_2_2 + s_2_0 + s_3_1 + 1;
										if (s_1_0) val+= s_1_m1 + s_2_0 + 1;
										value_self += val;
									}
									if (s_m1_1){
										#pragma CO FLATTEN
										val=1;
										if (s_m1_2) val+= s_m1_3 + s_m2_2 + s_0_2 + 1;
										if (s_m2_1) val+= s_m2_2 + s_m2_0 + s_m3_1 + 1;
										if (s_m1_0) val+= s_m1_m1 + s_m2_0 + 1;
										value_self += val;
									}
								}
								if (s_0_m1){
									value_self++;
									if (s_0_m2){
										#pragma CO FLATTEN
										val=1; 
										if (s_0_m3) val+= s_0_m4 + s_1_m3 + s_m1_m3 + 1;
										if (s_1_m2) val+= s_1_m3 + s_2_m2 + s_1_m1 + 1;
										if (s_m1_m2) val+= s_m1_m3 + s_m2_m2 + s_m1_m1 + 1;
										value_self += val;
									}
									if (s_1_m1){
										#pragma CO FLATTEN
										val=1; 
										if (s_1_m2) val+= s_1_m3 + s_2_m2 + s_0_m2 + 1;
										if (s_2_m1) val+= s_2_m2 + s_2_0 + s_3_m1 + 1;
										if (s_1_0) val+= s_1_1 + s_2_0 + 1;
										value_self += val;
									}
									if (s_m1_m1){
										#pragma CO FLATTEN
										val=1; 
										if (s_m1_m2) val+= s_m1_m3 + s_m2_m2 + s_0_m2 + 1;
										if (s_m2_m1) val+= s_m2_m2 + s_m2_0 + s_m3_m1 + 1;
										if (s_m1_0) val+= s_m1_1 + s_m2_0 + 1;
										value_self += val;
									}
								}
							}

							//opp
							if(o_0_0){
								if (o_1_0){
									value_opp++;
									if (o_2_0){
										#pragma CO FLATTEN
										val = 1;
										if (o_3_0)	val += o_4_0 + o_3_1 + o_3_m1 + 1;
										if (o_2_1)	val += o_3_1 + o_2_2 + o_1_1 + 1;
										if (o_2_m1)	val += o_3_m1 + o_2_m2 + o_1_m1 + 1;
										value_opp += val;
									}
									if (o_1_1){
										#pragma CO FLATTEN
										val = 1;
										if (o_2_1)	val+= o_3_1 + o_2_2 + o_2_0 + 1;
										if (o_1_2)	val+= o_2_2 + o_0_2 + o_1_3 + 1;
										if (o_0_1)	val+= o_m1_1 + o_0_2 + 1;
										value_opp += val;
									}
									if (o_1_m1){
										#pragma CO FLATTEN
										val=1;
										if (o_2_m1)	val+=	o_3_m1 + o_2_m2 +  o_2_0 +  1;
										if (o_1_m2)	val+= o_2_m2 + o_0_m2 + o_1_m3 + 1;
										if (o_0_m1)	val+=	o_m1_m1 + o_0_m2 + 1;
										value_opp += val;
									}
								}
								if (o_m1_0){
									value_opp++;
									if (o_m2_0){
										#pragma CO FLATTEN
										val=1;
										if (o_m3_0)	val+= o_m4_0 + o_m3_1 + o_m3_m1 + 1;
										if (o_m2_1)	val+= o_m3_1 + o_m2_2 + o_m1_1 + 1;
										if (o_m2_m1)val+= o_m3_m1 + o_m2_m2 + o_m1_m1 + 1;
										value_opp += val;
									}
									if (o_m1_1){
										#pragma CO FLATTEN
										val=1;
										if (o_m2_1)	val+= o_m3_1 + o_m2_2 + o_m2_0 + 1;
										if (o_m1_2)	val+= o_m2_2 + o_0_2 + o_m1_3 + 1;
										if (o_0_1)	val+= o_1_1 + o_0_2 + 1;
										value_opp += val;
									}
									if (o_m1_m1){
										#pragma CO FLATTEN
										val=1; 
										if (o_m2_m1) val+= o_m3_m1 + o_m2_m2 + o_m2_0 + 1;
										if (o_m1_m2) val+= o_m2_m2 + o_0_m2 + o_m1_m3 + 1;
										if (o_0_m1) val+= o_1_m1 + o_0_m2 + 1;
										value_opp += val;
									}
								}
								if (o_0_1){
									value_opp++;
									if (o_0_2){
										#pragma CO FLATTEN
										val=1;
										if (o_0_3) val+= o_0_4 + o_1_3 + o_m1_3 + 1;
										if (o_1_2) val+= o_1_3 + o_2_2 + o_1_1 + 1;
										if (o_m1_2)val+= o_m1_3 + o_m2_2 + o_m1_1 + 1;
										value_opp += val;
									}
									if (o_1_1){
										#pragma CO FLATTEN
										val=1;
										if (o_1_2) val+= o_1_3 + o_2_2 + o_0_2 + 1;
										if (o_2_1) val+= o_2_2 + o_2_0 + o_3_1 + 1;
										if (o_1_0) val+= o_1_m1 + o_2_0 + 1;
										value_opp += val;
									}
									if (o_m1_1){
										#pragma CO FLATTEN
										val=1;
										if (o_m1_2) val+= o_m1_3 + o_m2_2 + o_0_2 + 1;
										if (o_m2_1) val+= o_m2_2 + o_m2_0 + o_m3_1 + 1;
										if (o_m1_0) val+= o_m1_m1 + o_m2_0 + 1;
										value_opp += val;
									}
								}
								if (o_0_m1){
									value_opp++;
									if (o_0_m2){
										#pragma CO FLATTEN
										val=1; 
										if (o_0_m3) val+= o_0_m4 + o_1_m3 + o_m1_m3 + 1;
										if (o_1_m2) val+= o_1_m3 + o_2_m2 + o_1_m1 + 1;
										if (o_m1_m2) val+= o_m1_m3 + o_m2_m2 + o_m1_m1 + 1;
										value_opp += val;
									}
									if (o_1_m1){
										#pragma CO FLATTEN
										val=1; 
										if (o_1_m2) val+= o_1_m3 + o_2_m2 + o_0_m2 + 1;
										if (o_2_m1) val+= o_2_m2 + o_2_0 + o_3_m1 + 1;
										if (o_1_0) val+= o_1_1 + o_2_0 + 1;
										value_opp += val;
									}
									if (o_m1_m1){
										#pragma CO FLATTEN
										val=1; 
										if (o_m1_m2) val+= o_m1_m3 + o_m2_m2 + o_0_m2 + 1;
										if (o_m2_m1) val+= o_m2_m2 + o_m2_0 + o_m3_m1 + 1;
										if (o_m1_0) val+= o_m1_1 + o_m2_0 + 1;
										value_opp += val;
									}
								}
							}
							net_lib += value_self - value_opp;
						}
					}
				}
				//heuristic end

				//calculate utility
				utility+=WEIGHT_LIB*net_lib;								
				move &= 0x0000FFFF;
				move |= (utility<<UTILITY_OFF);
				co_stream_write(stream0_move_output, &move, sizeof(co_int32));				
				break;

			case MINIMAX_MOVE_SELF2:
				
				//update board self
				avail_self2_bak = avail_self;
				score_self2_bak = score_self;
				
				size = piece_sizes[m_piece];
				avail_self &= ~(1<<m_piece);
				score_self += size;
				--size;
				yoff = m_y;
				xoff = m_x;
				count_bak_s2 = 0;
				do{
					//backup data
					x_bak_s2[count_bak_s2] = xoff; y_bak_s2[count_bak_s2] = yoff; board_bak_s2[count_bak_s2] = board[yoff][xoff]; ++count_bak_s2;
					board[yoff][xoff] = SELF | (0x3<<EX_GRID_NOT_SAFE);
					
					y = yoff+1; x = xoff; y1 = yoff+1; x1 = xoff+1; board_t = board[y][x]; board_t1 = board[y1][x1]; 
					board[y][x] = board_t|(SELF<<EX_GRID_NOT_SAFE); board[y1][x1] = board_t1|(SELF<<EX_GRID_LBTY);
					x_bak_s2[count_bak_s2] = x; y_bak_s2[count_bak_s2] = y; board_bak_s2[count_bak_s2] = board_t; ++count_bak_s2;					
					x_bak_s2[count_bak_s2] = x1; y_bak_s2[count_bak_s2] = y1; board_bak_s2[count_bak_s2] = board_t1; ++count_bak_s2;
					
					y = yoff-1; x = xoff; y1 = yoff-1; x1 = xoff-1; board_t = board[y][x]; board_t1 = board[y1][x1];
					board[y][x] = board_t|(SELF<<EX_GRID_NOT_SAFE); board[y1][x1] = board_t1|(SELF<<EX_GRID_LBTY);
					x_bak_s2[count_bak_s2] = x; y_bak_s2[count_bak_s2] = y; board_bak_s2[count_bak_s2] = board_t; ++count_bak_s2;					
					x_bak_s2[count_bak_s2] = x1; y_bak_s2[count_bak_s2] = y1; board_bak_s2[count_bak_s2] = board_t1; ++count_bak_s2;

					y = yoff; x = xoff+1; y1 = yoff-1; x1 = xoff+1;	board_t = board[y][x]; board_t1 = board[y1][x1];
					board[y][x] = board_t|(SELF<<EX_GRID_NOT_SAFE); board[y1][x1] = board_t1|(SELF<<EX_GRID_LBTY);
					x_bak_s2[count_bak_s2] = x; y_bak_s2[count_bak_s2] = y; board_bak_s2[count_bak_s2] = board_t; ++count_bak_s2;					
					x_bak_s2[count_bak_s2] = x1; y_bak_s2[count_bak_s2] = y1; board_bak_s2[count_bak_s2] = board_t1; ++count_bak_s2;

					y = yoff; x = xoff-1; y1 = yoff+1; x1 = xoff-1;	board_t = board[y][x]; board_t1 = board[y1][x1]; 
					board[y][x] = board_t|(SELF<<EX_GRID_NOT_SAFE);board[y1][x1] = board_t1|(SELF<<EX_GRID_LBTY);
					x_bak_s2[count_bak_s2] = x; y_bak_s2[count_bak_s2] = y; board_bak_s2[count_bak_s2] = board_t; ++count_bak_s2;					
					x_bak_s2[count_bak_s2] = x1; y_bak_s2[count_bak_s2] = y1; board_bak_s2[count_bak_s2] = board_t1; ++count_bak_s2;
				
					--size;
					if(size<0) break;
					yoff = m_y+piece_y[m_piece][m_rotate][size];
					xoff = m_x+piece_x[m_piece][m_rotate][size];
				}while(1);				
				break;

			case MINIMAX_MOVE_SELF2_DONE:
				//restore backup values
				--count_bak_s2;
				while(count_bak_s2>=0){
					#pragma CO PIPELINE
					xoff = x_bak_s2[count_bak_s2];
					yoff = y_bak_s2[count_bak_s2];
					board[yoff][xoff] = board_bak_s2[count_bak_s2];
					--count_bak_s2;
					};
				
				score_self = score_self2_bak;
				avail_self = avail_self2_bak;
				break;
				
			case MINIMAX_MOVE_OPP:
				
				//update board opp
				avail_opp_bak = avail_opp;
				score_opp_bak = score_opp;
				
				size = piece_sizes[m_piece];
				avail_opp &= ~(1<<m_piece);
				score_opp += size;
				--size;
				yoff = m_y;
				xoff = m_x;
				count_bak_op = 0;
				do{
					//backup data
					x_bak_op[count_bak_op] = xoff; y_bak_op[count_bak_op] = yoff; board_bak_op[count_bak_op] = board[yoff][xoff]; ++count_bak_op;
					board[yoff][xoff] = OPP | (0x3<<EX_GRID_NOT_SAFE);
					
					y = yoff+1; x = xoff; y1 = yoff+1; x1 = xoff+1; board_t = board[y][x]; board_t1 = board[y1][x1]; 
					board[y][x] = board_t|(OPP<<EX_GRID_NOT_SAFE); board[y1][x1] = board_t1|(OPP<<EX_GRID_LBTY);
					x_bak_op[count_bak_op] = x; y_bak_op[count_bak_op] = y; board_bak_op[count_bak_op] = board_t; ++count_bak_op;										
					x_bak_op[count_bak_op] = x1; y_bak_op[count_bak_op] = y1; board_bak_op[count_bak_op] = board_t1; ++count_bak_op;

					y = yoff-1; x = xoff; y1 = yoff-1; x1 = xoff-1; board_t = board[y][x]; board_t1 = board[y1][x1]; 
					board[y][x] = board_t|(OPP<<EX_GRID_NOT_SAFE); board[y1][x1] = board_t1|(OPP<<EX_GRID_LBTY);
					x_bak_op[count_bak_op] = x; y_bak_op[count_bak_op] = y; board_bak_op[count_bak_op] = board_t; ++count_bak_op;										
					x_bak_op[count_bak_op] = x1; y_bak_op[count_bak_op] = y1; board_bak_op[count_bak_op] = board_t1; ++count_bak_op;

					y = yoff; x = xoff+1; y1 = yoff-1; x1 = xoff+1; board_t = board[y][x]; board_t1 = board[y1][x1]; 
					board[y][x] = board_t|(OPP<<EX_GRID_NOT_SAFE); board[y1][x1] = board_t1|(OPP<<EX_GRID_LBTY);
					x_bak_op[count_bak_op] = x; y_bak_op[count_bak_op] = y; board_bak_op[count_bak_op] = board_t; ++count_bak_op;										
					x_bak_op[count_bak_op] = x1; y_bak_op[count_bak_op] = y1; board_bak_op[count_bak_op] = board_t1; ++count_bak_op;

					y = yoff; x = xoff-1; y1 = yoff+1; x1 = xoff-1; board_t = board[y][x]; board_t1 = board[y1][x1]; 
					board[y][x] = board_t|(OPP<<EX_GRID_NOT_SAFE); board[y1][x1] = board_t1|(OPP<<EX_GRID_LBTY);
					x_bak_op[count_bak_op] = x; y_bak_op[count_bak_op] = y; board_bak_op[count_bak_op] = board_t; ++count_bak_op;										
					x_bak_op[count_bak_op] = x1; y_bak_op[count_bak_op] = y1; board_bak_op[count_bak_op] = board_t1; ++count_bak_op;
					
					--size;
					if(size<0) break;
					yoff = m_y+piece_y[m_piece][m_rotate][size];
					xoff = m_x+piece_x[m_piece][m_rotate][size];
				}while(1);				

				break;

			case MINIMAX_MOVE_OPP_DONE:
				//restore backup values
				--count_bak_op;
				do{
					#pragma CO PIPELINE
					xoff = x_bak_op[count_bak_op];
					yoff = y_bak_op[count_bak_op];
					board[yoff][xoff] = board_bak_op[count_bak_op];
					--count_bak_op;
					}while(count_bak_op>=0);
				
				score_opp = score_opp_bak;
				avail_opp = avail_opp_bak;
				break;

			case MINIMAX_MOVE_SELF:

				//update board self
				avail_self_bak = avail_self;
				score_self_bak = score_self;
				
				size = piece_sizes[m_piece];
				avail_self &= ~(1<<m_piece);
				score_self += size;
				--size;
				yoff = m_y;
				xoff = m_x;
				count_bak_s = 0;
				do{
					//backup data
					x_bak_s[count_bak_s] = xoff; y_bak_s[count_bak_s] = yoff; board_bak_s[count_bak_s] = board[yoff][xoff]; ++count_bak_s;
					board[yoff][xoff] = SELF | (0x3<<EX_GRID_NOT_SAFE);
					
					y = yoff+1; x = xoff; y1 = yoff+1; x1 = xoff+1; board_t = board[y][x]; board_t1 = board[y1][x1]; 
					board[y][x] = board_t|(SELF<<EX_GRID_NOT_SAFE); board[y1][x1] = board_t1|(SELF<<EX_GRID_LBTY);
					x_bak_s[count_bak_s] = x; y_bak_s[count_bak_s] = y; board_bak_s[count_bak_s] = board_t; ++count_bak_s;					
					x_bak_s[count_bak_s] = x1; y_bak_s[count_bak_s] = y1; board_bak_s[count_bak_s] = board_t1; ++count_bak_s;
					
					y = yoff-1; x = xoff; y1 = yoff-1; x1 = xoff-1; board_t = board[y][x]; board_t1 = board[y1][x1];
					board[y][x] = board_t|(SELF<<EX_GRID_NOT_SAFE); board[y1][x1] = board_t1|(SELF<<EX_GRID_LBTY);
					x_bak_s[count_bak_s] = x; y_bak_s[count_bak_s] = y; board_bak_s[count_bak_s] = board_t; ++count_bak_s;
					x_bak_s[count_bak_s] = x1; y_bak_s[count_bak_s] = y1; board_bak_s[count_bak_s] = board_t1; ++count_bak_s;

					y = yoff; x = xoff+1; y1 = yoff-1; x1 = xoff+1;	board_t = board[y][x]; board_t1 = board[y1][x1];
					board[y][x] = board_t|(SELF<<EX_GRID_NOT_SAFE); board[y1][x1] = board_t1|(SELF<<EX_GRID_LBTY);
					x_bak_s[count_bak_s] = x; y_bak_s[count_bak_s] = y; board_bak_s[count_bak_s] = board_t; ++count_bak_s;
					x_bak_s[count_bak_s] = x1; y_bak_s[count_bak_s] = y1; board_bak_s[count_bak_s] = board_t1; ++count_bak_s;

					y = yoff; x = xoff-1; y1 = yoff+1; x1 = xoff-1;	board_t = board[y][x]; board_t1 = board[y1][x1]; 
					board[y][x] = board_t|(SELF<<EX_GRID_NOT_SAFE);board[y1][x1] = board_t1|(SELF<<EX_GRID_LBTY);
					x_bak_s[count_bak_s] = x; y_bak_s[count_bak_s] = y; board_bak_s[count_bak_s] = board_t; ++count_bak_s;
					x_bak_s[count_bak_s] = x1; y_bak_s[count_bak_s] = y1; board_bak_s[count_bak_s] = board_t1; ++count_bak_s;
				
					--size;
					if(size<0) break;
					yoff = m_y+piece_y[m_piece][m_rotate][size];
					xoff = m_x+piece_x[m_piece][m_rotate][size];
				}while(1);				
				break;

			case MINIMAX_MOVE_SELF_DONE:
				
				//restore backup values
				--count_bak_s;
				while(count_bak_s>=0){
					#pragma CO PIPELINE
					xoff = x_bak_s[count_bak_s];
					yoff = y_bak_s[count_bak_s];
					board[yoff][xoff] = board_bak_s[count_bak_s];
					--count_bak_s;
					};
				
				score_self = score_self_bak;
				avail_self = avail_self_bak;
				break;
				
			case PERM_MOVE_SELF:
				
				//update board self
				size = piece_sizes[m_piece];
				avail_self &= ~(1<<m_piece);
				score_self += size;
				--size;
				yoff = m_y;
				xoff = m_x;
				
				do{
					board[yoff][xoff] = SELF | (0x3<<EX_GRID_NOT_SAFE);
					
					y = yoff+1; x = xoff; y1 = yoff+1; x1 = xoff+1; board_t = board[y][x]; board_t1 = board[y1][x1]; 
					board[y][x] = board_t|(SELF<<EX_GRID_NOT_SAFE);	board[y1][x1] = board_t1|(SELF<<EX_GRID_LBTY);
					
					y = yoff-1; x = xoff; y1 = yoff-1; x1 = xoff-1;	board_t = board[y][x]; board_t1 = board[y1][x1]; 
					board[y][x] = board_t|(SELF<<EX_GRID_NOT_SAFE); board[y1][x1] = board_t1|(SELF<<EX_GRID_LBTY);			
					
					y = yoff; x = xoff+1; y1 = yoff-1; x1 = xoff+1;	board_t = board[y][x]; board_t1 = board[y1][x1];
					board[y][x] = board_t|(SELF<<EX_GRID_NOT_SAFE); board[y1][x1] = board_t1|(SELF<<EX_GRID_LBTY);
					
					y = yoff; x = xoff-1; y1 = yoff+1; x1 = xoff-1;	board_t = board[y][x]; board_t1 = board[y1][x1];
					board[y][x] = board_t|(SELF<<EX_GRID_NOT_SAFE); board[y1][x1] = board_t1|(SELF<<EX_GRID_LBTY);
				
					--size;
					if(size<0) break;
					yoff = m_y+piece_y[m_piece][m_rotate][size];
					xoff = m_x+piece_x[m_piece][m_rotate][size];
				}while(1);				
				break;
				
			case PERM_MOVE_OPP:
				
				//update board opp
				size = piece_sizes[m_piece];
				avail_opp &= ~(1<<m_piece);	
				score_opp += size;
				
				--size;
				yoff = m_y;
				xoff = m_x;
										
				do{
					board[yoff][xoff] = OPP | (0x3<<EX_GRID_NOT_SAFE);
					
					y = yoff+1; x = xoff; y1 = yoff+1; x1 = xoff+1; board_t = board[y][x]; board_t1 = board[y1][x1]; 
					board[y][x] = board_t|(OPP<<EX_GRID_NOT_SAFE);	board[y1][x1] = board_t1|(OPP<<EX_GRID_LBTY);
					
					y = yoff-1; x = xoff; y1 = yoff-1; x1 = xoff-1;	board_t = board[y][x]; board_t1 = board[y1][x1]; 
					board[y][x] = board_t|(OPP<<EX_GRID_NOT_SAFE); board[y1][x1] = board_t1|(OPP<<EX_GRID_LBTY);			
					
					y = yoff; x = xoff+1; y1 = yoff-1; x1 = xoff+1;	board_t = board[y][x]; board_t1 = board[y1][x1];
					board[y][x] = board_t|(OPP<<EX_GRID_NOT_SAFE); board[y1][x1] = board_t1|(OPP<<EX_GRID_LBTY);
					
					y = yoff; x = xoff-1; y1 = yoff+1; x1 = xoff-1; board_t = board[y][x]; board_t1 = board[y1][x1];
					board[y][x] = board_t|(OPP<<EX_GRID_NOT_SAFE); board[y1][x1] = board_t1|(OPP<<EX_GRID_LBTY);
					--size;
					
					if(size<0) break;
					yoff = m_y+piece_y[m_piece][m_rotate][size];
					xoff = m_x+piece_x[m_piece][m_rotate][size];
					
				}while(1);
				break;
				
			case GAME_RESET:
				
				//init board
				for(y = 0; y < 32; y++){
					for(x = 0; x < 32; x++){
						if((x < 4) || (y < 4) || (x > 17) || (y > 17)){
							board[y][x] = (BORDER | (0x3<<EX_GRID_NOT_SAFE));
						}
						else {
							board[y][x] = 0;
						}	
					}
				}
				
				//init available pieces
				avail_self = 0x7FFFFF;
				avail_opp = 0x7FFFFF;
				
				//init scores
				score_self = 0;
				score_opp = 0;

				break;
				
			default:
				break;
		}		
	}
}
 
void config_blokus(void *arg) {  // Configuration function

	//main process, timer process, uart processes
	co_process make_move_proc, timer_proc, producer_proc, consumer_proc;

	co_process minimax1_proc, minimax2_proc;
	co_process minimax_ex1_proc, minimax_ex2_proc, minimax_ex3_proc, minimax_ex4_proc;
	co_process minimax_ex5_proc, minimax_ex6_proc, minimax_ex7_proc, minimax_ex8_proc;

	co_process	eval1_proc, eval2_proc, eval3_proc, eval4_proc, eval5_proc;
	co_process	eval6_proc, eval7_proc, eval8_proc, eval9_proc, eval10_proc;
	co_process	eval11_proc, eval12_proc, eval13_proc, eval14_proc, eval15_proc;
	co_process  eval16_proc;

	co_stream input_stream, output_stream, stream_move_timer, stream_timer_move;
	
	co_stream stream_move_minimax1, stream_minimax1_move;
	co_stream stream_move_minimax2, stream_minimax2_move;

	co_stream stream_minimax_minimax_ex1, stream_minimax_ex1_minimax;
	co_stream stream_minimax_minimax_ex2, stream_minimax_ex2_minimax;
	co_stream stream_minimax_minimax_ex3, stream_minimax_ex3_minimax;
	co_stream stream_minimax_minimax_ex4, stream_minimax_ex4_minimax;
	co_stream stream_minimax_minimax_ex5, stream_minimax_ex5_minimax;
	co_stream stream_minimax_minimax_ex6, stream_minimax_ex6_minimax;
	co_stream stream_minimax_minimax_ex7, stream_minimax_ex7_minimax;
	co_stream stream_minimax_minimax_ex8, stream_minimax_ex8_minimax;
	
	co_stream stream_minimax_ex_eval1, stream_eval1_minimax_ex;
	co_stream stream_minimax_ex_eval2, stream_eval2_minimax_ex;
	co_stream stream_minimax_ex_eval3, stream_eval3_minimax_ex;
	co_stream stream_minimax_ex_eval4, stream_eval4_minimax_ex;
	co_stream stream_minimax_ex_eval5, stream_eval5_minimax_ex;
	co_stream stream_minimax_ex_eval6, stream_eval6_minimax_ex;
	co_stream stream_minimax_ex_eval7, stream_eval7_minimax_ex;
	co_stream stream_minimax_ex_eval8, stream_eval8_minimax_ex;
	co_stream stream_minimax_ex_eval9, stream_eval9_minimax_ex;
	co_stream stream_minimax_ex_eval10, stream_eval10_minimax_ex;
	co_stream stream_minimax_ex_eval11, stream_eval11_minimax_ex;
	co_stream stream_minimax_ex_eval12, stream_eval12_minimax_ex;
	co_stream stream_minimax_ex_eval13, stream_eval13_minimax_ex;
	co_stream stream_minimax_ex_eval14, stream_eval14_minimax_ex;
	co_stream stream_minimax_ex_eval15, stream_eval15_minimax_ex;
	co_stream stream_minimax_ex_eval16, stream_eval16_minimax_ex;

	input_stream = co_stream_create("input_stream", INT_TYPE(8), 1);
	output_stream = co_stream_create("output_stream", INT_TYPE(8), 1);
	stream_move_timer = co_stream_create("stream_move_timer", INT_TYPE(8), 1);	
	stream_timer_move = co_stream_create("stream_timer_move", INT_TYPE(8), 1);	

	stream_move_minimax1=co_stream_create("stream_move_minimax1",INT_TYPE(32),8);	stream_minimax1_move=co_stream_create("stream_minimax1_move",INT_TYPE(32),8);
	stream_move_minimax2=co_stream_create("stream_move_minimax2",INT_TYPE(32),8);	stream_minimax2_move=co_stream_create("stream_minimax2_move",INT_TYPE(32),8);

	stream_minimax_minimax_ex1=co_stream_create("stream_minimax_minimax_ex1",INT_TYPE(32),24);	stream_minimax_ex1_minimax=co_stream_create("stream_minimax_ex1_minimax",INT_TYPE(32),24);
	stream_minimax_minimax_ex2=co_stream_create("stream_minimax_minimax_ex2",INT_TYPE(32),24);	stream_minimax_ex2_minimax=co_stream_create("stream_minimax_ex2_minimax",INT_TYPE(32),24);
	stream_minimax_minimax_ex3=co_stream_create("stream_minimax_minimax_ex3",INT_TYPE(32),24);	stream_minimax_ex3_minimax=co_stream_create("stream_minimax_ex3_minimax",INT_TYPE(32),24);
	stream_minimax_minimax_ex4=co_stream_create("stream_minimax_minimax_ex4",INT_TYPE(32),24);	stream_minimax_ex4_minimax=co_stream_create("stream_minimax_ex4_minimax",INT_TYPE(32),24);
	stream_minimax_minimax_ex5=co_stream_create("stream_minimax_minimax_ex5",INT_TYPE(32),24);	stream_minimax_ex5_minimax=co_stream_create("stream_minimax_ex5_minimax",INT_TYPE(32),24);
	stream_minimax_minimax_ex6=co_stream_create("stream_minimax_minimax_ex6",INT_TYPE(32),24);	stream_minimax_ex6_minimax=co_stream_create("stream_minimax_ex6_minimax",INT_TYPE(32),24);
	stream_minimax_minimax_ex7=co_stream_create("stream_minimax_minimax_ex7",INT_TYPE(32),24);	stream_minimax_ex7_minimax=co_stream_create("stream_minimax_ex7_minimax",INT_TYPE(32),24);
	stream_minimax_minimax_ex8=co_stream_create("stream_minimax_minimax_ex8",INT_TYPE(32),24);	stream_minimax_ex8_minimax=co_stream_create("stream_minimax_ex8_minimax",INT_TYPE(32),24);

	stream_minimax_ex_eval1=co_stream_create("stream_minimax_ex_eval1",INT_TYPE(32),18); stream_eval1_minimax_ex=co_stream_create("stream_eval1_minimax_ex",INT_TYPE(32),18);
	stream_minimax_ex_eval2=co_stream_create("stream_minimax_ex_eval2",INT_TYPE(32),18); stream_eval2_minimax_ex=co_stream_create("stream_eval2_minimax_ex",INT_TYPE(32),18);
	stream_minimax_ex_eval3=co_stream_create("stream_minimax_ex_eval3",INT_TYPE(32),18); stream_eval3_minimax_ex=co_stream_create("stream_eval3_minimax_ex",INT_TYPE(32),18);
	stream_minimax_ex_eval4=co_stream_create("stream_minimax_ex_eval4",INT_TYPE(32),18); stream_eval4_minimax_ex=co_stream_create("stream_eval4_minimax_ex",INT_TYPE(32),18);
	stream_minimax_ex_eval5=co_stream_create("stream_minimax_ex_eval5",INT_TYPE(32),18); stream_eval5_minimax_ex=co_stream_create("stream_eval5_minimax_ex",INT_TYPE(32),18);
	stream_minimax_ex_eval6=co_stream_create("stream_minimax_ex_eval6",INT_TYPE(32),18); stream_eval6_minimax_ex=co_stream_create("stream_eval6_minimax_ex",INT_TYPE(32),18);
	stream_minimax_ex_eval7=co_stream_create("stream_minimax_ex_eval7",INT_TYPE(32),18); stream_eval7_minimax_ex=co_stream_create("stream_eval7_minimax_ex",INT_TYPE(32),18);
	stream_minimax_ex_eval8=co_stream_create("stream_minimax_ex_eval8",INT_TYPE(32),18); stream_eval8_minimax_ex=co_stream_create("stream_eval8_minimax_ex",INT_TYPE(32),18);
	stream_minimax_ex_eval9=co_stream_create("stream_minimax_ex_eval9",INT_TYPE(32),18); stream_eval9_minimax_ex=co_stream_create("stream_eval9_minimax_ex",INT_TYPE(32),18);
	stream_minimax_ex_eval10=co_stream_create("stream_minimax_ex_eval10",INT_TYPE(32),18); stream_eval10_minimax_ex=co_stream_create("stream_eval10_minimax_ex",INT_TYPE(32),18);
	stream_minimax_ex_eval11=co_stream_create("stream_minimax_ex_eval11",INT_TYPE(32),18); stream_eval11_minimax_ex=co_stream_create("stream_eval11_minimax_ex",INT_TYPE(32),18);
	stream_minimax_ex_eval12=co_stream_create("stream_minimax_ex_eval12",INT_TYPE(32),18); stream_eval12_minimax_ex=co_stream_create("stream_eval12_minimax_ex",INT_TYPE(32),18);
	stream_minimax_ex_eval13=co_stream_create("stream_minimax_ex_eval13",INT_TYPE(32),18); stream_eval13_minimax_ex=co_stream_create("stream_eval13_minimax_ex",INT_TYPE(32),18);
	stream_minimax_ex_eval14=co_stream_create("stream_minimax_ex_eval14",INT_TYPE(32),18); stream_eval14_minimax_ex=co_stream_create("stream_eval14_minimax_ex",INT_TYPE(32),18);
	stream_minimax_ex_eval15=co_stream_create("stream_minimax_ex_eval15",INT_TYPE(32),18); stream_eval15_minimax_ex=co_stream_create("stream_eval15_minimax_ex",INT_TYPE(32),18);
	stream_minimax_ex_eval16=co_stream_create("stream_minimax_ex_eval16",INT_TYPE(32),18); stream_eval16_minimax_ex=co_stream_create("stream_eval16_minimax_ex",INT_TYPE(32),18);

	producer_proc = co_process_create("producer", (co_function) producer_func, 1, input_stream);
	consumer_proc = co_process_create("consumer", (co_function) consumer_func, 1, output_stream);
	
	make_move_proc = co_process_create("make_move", (co_function) make_move_func, 8, input_stream, output_stream, 
																					stream_move_minimax1, stream_minimax1_move,
																					stream_move_minimax2, stream_minimax2_move,
																					stream_move_timer, stream_timer_move
																					);	

	timer_proc = co_process_create("timer", (co_function) timer_func, 2, stream_move_timer, stream_timer_move);

	minimax1_proc = co_process_create("minimax1", (co_function) minimax_func, 10, stream_move_minimax1, stream_minimax1_move, 
																				stream_minimax_minimax_ex1, stream_minimax_ex1_minimax,
																				stream_minimax_minimax_ex2, stream_minimax_ex2_minimax,
																				stream_minimax_minimax_ex3, stream_minimax_ex3_minimax,
																				stream_minimax_minimax_ex4, stream_minimax_ex4_minimax
																				);
	
	minimax2_proc = co_process_create("minimax2", (co_function) minimax_func, 10, stream_move_minimax2, stream_minimax2_move, 
																				stream_minimax_minimax_ex5, stream_minimax_ex5_minimax,
																				stream_minimax_minimax_ex6, stream_minimax_ex6_minimax,
																				stream_minimax_minimax_ex7, stream_minimax_ex7_minimax,
																				stream_minimax_minimax_ex8, stream_minimax_ex8_minimax
																				);
	
	minimax_ex1_proc = co_process_create("minimax_ex1", (co_function) minimax_ex_func, 6, stream_minimax_minimax_ex1, stream_minimax_ex1_minimax, 
																				stream_minimax_ex_eval1, stream_eval1_minimax_ex,
																				stream_minimax_ex_eval2, stream_eval2_minimax_ex
																				);
	minimax_ex2_proc = co_process_create("minimax_ex2", (co_function) minimax_ex_func, 6, stream_minimax_minimax_ex2, stream_minimax_ex2_minimax, 
																				stream_minimax_ex_eval3, stream_eval3_minimax_ex,
																				stream_minimax_ex_eval4, stream_eval4_minimax_ex																			
																				);
	minimax_ex3_proc = co_process_create("minimax_ex3", (co_function) minimax_ex_func, 6, stream_minimax_minimax_ex3, stream_minimax_ex3_minimax, 
																				stream_minimax_ex_eval5, stream_eval5_minimax_ex,
																				stream_minimax_ex_eval6, stream_eval6_minimax_ex																			
																				);
	minimax_ex4_proc = co_process_create("minimax_ex4", (co_function) minimax_ex_func, 6, stream_minimax_minimax_ex4, stream_minimax_ex4_minimax, 
																				stream_minimax_ex_eval7, stream_eval7_minimax_ex,
																				stream_minimax_ex_eval8, stream_eval8_minimax_ex																			
																				);
	minimax_ex5_proc = co_process_create("minimax_ex5", (co_function) minimax_ex_func, 6, stream_minimax_minimax_ex5, stream_minimax_ex5_minimax, 
																				stream_minimax_ex_eval9, stream_eval9_minimax_ex,
																				stream_minimax_ex_eval10, stream_eval10_minimax_ex																			
																				);
	minimax_ex6_proc = co_process_create("minimax_ex6", (co_function) minimax_ex_func, 6, stream_minimax_minimax_ex6, stream_minimax_ex6_minimax, 
																				stream_minimax_ex_eval11, stream_eval11_minimax_ex,
																				stream_minimax_ex_eval12, stream_eval12_minimax_ex																			
																				);
	minimax_ex7_proc = co_process_create("minimax_ex7", (co_function) minimax_ex_func, 6, stream_minimax_minimax_ex7, stream_minimax_ex7_minimax, 
																				stream_minimax_ex_eval13, stream_eval13_minimax_ex,
																				stream_minimax_ex_eval14, stream_eval14_minimax_ex																			
																				);
	minimax_ex8_proc = co_process_create("minimax_ex8", (co_function) minimax_ex_func, 6, stream_minimax_minimax_ex8, stream_minimax_ex8_minimax, 
																				stream_minimax_ex_eval15, stream_eval15_minimax_ex,
																				stream_minimax_ex_eval16, stream_eval16_minimax_ex																			
																				);
	
	eval1_proc = co_process_create("eval1", (co_function) eval_func, 2, stream_minimax_ex_eval1, stream_eval1_minimax_ex																		);	
	eval2_proc = co_process_create("eval2", (co_function) eval_func, 2, stream_minimax_ex_eval2, stream_eval2_minimax_ex);
	eval3_proc = co_process_create("eval3", (co_function) eval_func, 2, stream_minimax_ex_eval3, stream_eval3_minimax_ex);
	eval4_proc = co_process_create("eval4", (co_function) eval_func, 2, stream_minimax_ex_eval4, stream_eval4_minimax_ex);
	eval5_proc = co_process_create("eval5", (co_function) eval_func, 2, stream_minimax_ex_eval5, stream_eval5_minimax_ex);
	eval6_proc = co_process_create("eval6", (co_function) eval_func, 2, stream_minimax_ex_eval6, stream_eval6_minimax_ex);
	eval7_proc = co_process_create("eval7", (co_function) eval_func, 2, stream_minimax_ex_eval7, stream_eval7_minimax_ex);
	eval8_proc = co_process_create("eval8", (co_function) eval_func, 2, stream_minimax_ex_eval8, stream_eval8_minimax_ex);
	eval9_proc = co_process_create("eval9", (co_function) eval_func, 2, stream_minimax_ex_eval9, stream_eval9_minimax_ex);
	eval10_proc = co_process_create("eval10", (co_function) eval_func, 2, stream_minimax_ex_eval10, stream_eval10_minimax_ex);
	eval11_proc = co_process_create("eval11", (co_function) eval_func, 2, stream_minimax_ex_eval11, stream_eval11_minimax_ex);
	eval12_proc = co_process_create("eval12", (co_function) eval_func, 2, stream_minimax_ex_eval12, stream_eval12_minimax_ex);
	eval13_proc = co_process_create("eval13", (co_function) eval_func, 2, stream_minimax_ex_eval13, stream_eval13_minimax_ex);
	eval14_proc = co_process_create("eval14", (co_function) eval_func, 2, stream_minimax_ex_eval14, stream_eval14_minimax_ex);
	eval15_proc = co_process_create("eval15", (co_function) eval_func, 2, stream_minimax_ex_eval15, stream_eval15_minimax_ex);
	eval16_proc = co_process_create("eval16", (co_function) eval_func, 2, stream_minimax_ex_eval16, stream_eval16_minimax_ex);	

	co_process_config(make_move_proc, co_loc, "PE0");  // Assign to PE0
	co_process_config(minimax1_proc, co_loc, "PE0");  // Assign to PE0
	co_process_config(minimax2_proc, co_loc, "PE0");  // Assign to PE0
	
	co_process_config(minimax_ex1_proc, co_loc, "PE0");  // Assign to PE0
	co_process_config(minimax_ex2_proc, co_loc, "PE0");  // Assign to PE0
	co_process_config(minimax_ex3_proc, co_loc, "PE0");  // Assign to PE0
	co_process_config(minimax_ex4_proc, co_loc, "PE0");  // Assign to PE0
	co_process_config(minimax_ex5_proc, co_loc, "PE0");  // Assign to PE0
	co_process_config(minimax_ex6_proc, co_loc, "PE0");  // Assign to PE0
	co_process_config(minimax_ex7_proc, co_loc, "PE0");  // Assign to PE0
	co_process_config(minimax_ex8_proc, co_loc, "PE0");  // Assign to PE0

	co_process_config(eval1_proc, co_loc, "PE0");  // Assign to PE0
	co_process_config(eval2_proc, co_loc, "PE0");  // Assign to PE0
	co_process_config(eval3_proc, co_loc, "PE0");  // Assign to PE0
	co_process_config(eval4_proc, co_loc, "PE0");  // Assign to PE0
	co_process_config(eval5_proc, co_loc, "PE0");  // Assign to PE0
	co_process_config(eval6_proc, co_loc, "PE0");  // Assign to PE0
	co_process_config(eval7_proc, co_loc, "PE0");  // Assign to PE0
	co_process_config(eval8_proc, co_loc, "PE0");  // Assign to PE0
	co_process_config(eval9_proc, co_loc, "PE0");  // Assign to PE0
	co_process_config(eval10_proc, co_loc, "PE0");  // Assign to PE0
	co_process_config(eval11_proc, co_loc, "PE0");  // Assign to PE0
	co_process_config(eval12_proc, co_loc, "PE0");  // Assign to PE0
	co_process_config(eval13_proc, co_loc, "PE0");  // Assign to PE0
	co_process_config(eval14_proc, co_loc, "PE0");  // Assign to PE0
	co_process_config(eval15_proc, co_loc, "PE0");  // Assign to PE0
	co_process_config(eval16_proc, co_loc, "PE0");  // Assign to PE0
	co_process_config(timer_proc, co_loc, "PE0");  // Assign to PE0
}

co_architecture co_initialize(int param) { 
	return(co_architecture_create("Blokus","generic", config_blokus,(void *)param));
}
