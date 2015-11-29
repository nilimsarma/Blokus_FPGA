/*
    ICFPT 2013 Design Competition test (and will be the host) program 

    Platform: 
      - Developed on FreeBSD 8.3 (amd64).
      - On other platforms, 
          - modify /dev entries in serial_dev[]
          - init_serial() may be have to be modified
      - PATCHES FOR OTHER PLATFORM ARE WELCOME!

    Usage:
      See http://lut.eee.u-ryukyu.ac.jp/dc13/tools.html .
    
      To interrupt this program in interactive mode, press Ctrl+D (not Ctrl+C).
      This will transmit "9" to the serial port to make the game over.

    License:
      - Yasunori Osana <osana@eee.u-ryukyu.ac.jp> wrote this file.
      - This file is provided "AS IS" in the beerware license rev 42.
        (see http://people.freebsd.org/~phk/)

    Acknowledgements:
      - Cygwin compatibility provided by Prof. Akira Kojima 
        at Hiroshima city university
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "pieces.h"
#include "rotate.h"

#define BORDER 3

#define TRUE (1==1)
#define FALSE (!TRUE)

#define PIECE_ALREADY_PLACED -1
#define GRID_ALREADY_OCCUPIED -2
#define SHARES_EDGE -3
#define MUST_SHARE_VERTEX -4
#define MUST_COVER_FIRST_PLACE -5
#define MUST_COVER_SECOND_PLACE -6

#define TERMINATE_NORMAL -1
#define TERMINATE_WRONG -2

#ifdef __CYGWIN__
const char* serial_dev[2] = { "/dev/com1", "/dev/com2" }; // cygwin
#else
const char* serial_dev[2] = { "/dev/ttyS0", "/dev/ttyS1" };
#endif

/*nilim*/
#define PLAY

int serial_fd[2];
struct termios oldtio[2], tio[2];
char team_ids[2][3] = { "01", "02" };

int board[16][16];
int available[2][21];

char p1move1[5];

int show_board_status = TRUE;         // -b
int show_placed_tile = FALSE;         // -p
int show_available_tiles = TRUE;      // -t
int show_hint = FALSE;                // -h
int on_serial = 0;                    // -1, -2, -3
int use_tcp = FALSE;
int first_player = 1;
int move_timeout = 1;
//nilim
int first_turn_p1 = 20;
int first_turn_p2 = 20;

typedef struct {
  int x;
  int y;
  int piece;
  int rotate;
} move;

#ifdef PLAY /*nilim*/
extern void make_move(int player, int turn, char* code, int board[16][16], int available[2][21]);
#endif

int timeval_subtract(struct timeval *a, struct timeval *b){
  // return a-b in milliseconds
  
  return (a->tv_usec - b->tv_usec)/1000 + (a->tv_sec - b->tv_sec)*1000;
}

int read_all(int fd, int len, char* buf){
  // read LEN bytes from FD, but without CR and LF.
  int got = 0;
  struct timeval start, timelimit, stop;
  fd_set read_fds, write_fds, except_fds;

  FD_ZERO(&write_fds);
  FD_ZERO(&except_fds);

  gettimeofday(&start, NULL);
  timelimit = start;
  timelimit.tv_sec = timelimit.tv_sec + move_timeout;

  do {
    struct timeval now, timeout;
    int timeout_ms;

    gettimeofday(&now, NULL);
    timeout_ms = timeval_subtract(&timelimit, &now);
    if (timeout_ms <= 0) break; // no remaining time
#ifdef DEBUG
    printf("remaining time: %d\n", timeout_ms);
#endif

    timeout.tv_sec = timeout_ms / 1000;
    timeout.tv_usec = (timeout_ms % 1000) * 1000;

    FD_ZERO(&read_fds);
    FD_SET(fd, &read_fds);
    if (select(fd+1, &read_fds, &write_fds, &except_fds, &timeout) == 1){
      read(fd, &buf[got], 1);
    } else {
      printf("timeout!\n");
      got = 0;
      break; // timeout
    }

    if(buf[got] != 0x0d && buf[got] != 0x0a) got++;
  } while(got < len);

  gettimeofday(&stop, NULL);
#if 1 //def DEBUG
  printf("read %d bytes in %d msec.\n", got, timeval_subtract(&stop, &start));
#endif
  return got;
}

void show_board(){
  int i, x, y;
  if (show_available_tiles){
    printf("----------------- Available Tiles -----------------\n");
    printf("         ");
    for(i=0; i<21; i++) printf(" %c", 'a'+i); printf("\n");
    printf("Player 1:");
    for(i=0; i<21; i++) printf(" %d", available[0][i]); printf("\n");
    printf("Player 2:");
    for(i=0; i<21; i++) printf(" %d", available[1][i]); printf("\n");
  }

  if (show_board_status){
    printf("---------------------- Board ----------------------\n  ");
    for(x=0; x<16; x++)
      printf(" %c", ((x<10) ? x+'0' : x+'a'-10));
    printf("\n");
    for(y=0; y<16; y++){
      printf(" %c", ((y<10) ? y+'0' : y+'a'-10));
      for(x=0; x<16; x++){
        printf(" %c", ( board[y][x] == 0 ? ' ' : board[y][x] < 3 ? board[y][x]+'0' : '+'));
      }
      printf("\n");
    }
  }
  printf("---------------------------------------------------\n");
  fflush(stdout);
}

int check_code(char* code){
  char c;

  // pass is a valid code!
  if(code[0]=='0' && code[1]=='0' && code[2]=='0' && code[3]=='0') return TRUE;

  c=code[0];
  if(! (('0'<=c && c<='9') || ('a'<= c && c<='e')) ) return FALSE;

  c=code[1];
  if(! (('0'<=c && c<='9') || ('a'<= c && c<='e')) ) return FALSE;

  c=code[2];
  if(! ('a'<= c && c<='u') ) return FALSE;

  c=code[3];
  if(! ('0'<= c && c<='7') ) return FALSE;

  return TRUE;
}

move decode_code(char* code){
  // code must be valid!
  move m;
  char c;

  c=code[0];
  if('0'<=c && c<='9') m.x = c-'0';
  if('a'<=c && c<='e') m.x = c-'a'+10;

  c=code[1];
  if('0'<=c && c<='9') m.y = c-'0';
  if('a'<=c && c<='e') m.y = c-'a'+10;

  c=code[2];
  m.piece = c-'a';

  c=code[3];
  m.rotate = c-'0';

  if (show_placed_tile){
    int x,y;
    printf("------------------ Tile Pattern -------------------\n");

    printf("x: %d, y: %d, piece: %d, rotate: %d\n", m.x, m.y, m.piece, m.rotate);

    for(y=0; y<5; y++){
      for(x=0; x<5; x++){
        printf("%d ", pieces[m.piece][0][rotate[m.rotate][y][x]]);
      }
      printf("\n");
    }
  }

  return m;
}

int interactive(int p){
  return (on_serial == 0) ||
    (on_serial == 1 && p == 2) ||
    (on_serial == 2 && p == 1);
}

char* prompt(int p, char* code, char* prev_code, int must_pass, int turn){
  switch (turn+1){
  case 1:
    if (p==1) printf("turn %d: must cover (5,5)\n", turn);
    else      printf("turn %d: must cover (a,a)\n", turn);
    break;
    
  default:
    printf("turn %d\n", turn);
    break;
  }
    
  if ( interactive(p) ){ 
    // interactive
    if (must_pass){
      printf("(not asking move on console)\n");
      strcpy(code, "0000");
      return code;
    } else {
      printf("Player %d:", p);
      fflush(stdout);
#ifndef PLAY	  /*nilim*/	
      fgets(code, 6, stdin);
      if(feof(stdin)){
	code[0] = 0;
	return NULL;
      }

      code[strlen(code)-1] = 0;
#else
	make_move(p, turn, code, board, available);
#endif
      return code;
    }
  } else {
    // serial
    char prompt_buf[10];
    char code_buf[10];
    int port;

    if(prev_code[0] == 0) printf("no prev code\n");
    else printf("prev code = %s\n", prev_code);

    port = (on_serial == 3) ? p-1 : 0;

    switch(turn+1){
    case 1: 
    case 2: 
      if (p==1) strcpy(prompt_buf, "25"); 
      else      strcpy(prompt_buf, "2A"); 
      break;
    case 4: snprintf(prompt_buf, 10, "4%s%s", p1move1, prev_code); break;
    default: snprintf(prompt_buf, 6, "3%s", prev_code);
    }

    printf("serial prompt string: %s (%d bytes)\n", 
	   prompt_buf, (int)strlen(prompt_buf));

    write(serial_fd[port], prompt_buf, strlen(prompt_buf));

    if(read_all(serial_fd[port], 4, code_buf) != 4) return NULL;
    code_buf[4] = 0;
    strcpy(code, code_buf);
    printf("(got from serial %d: %s)\n", port, code);
    return code;
  }

  // should not come here
  return NULL;
}

int check_move(int player, int turn, move m){
  //  printf("check_move: player %d, turn %d\n", player, turn);
  int c, r, x, y, x_offset, y_offset;
    
  c = m.piece;
  r = m.rotate;
  x_offset = m.x-2;
  y_offset = m.y-2;
    
  // Check availability
  if(available[player-1][c] == 0){
    return PIECE_ALREADY_PLACED;
  }

  // No piece on already occupied grid
  for(y=0; y<5; y++){
    for(x=0; x<5; x++){
      int b;
      b = pieces[c][0][rotate[r][y][x]];
      if (b==1){
        if (board[y_offset+y][x_offset+x] != 0 ||
            y_offset+y < 0 || 15 < y_offset+y ||
            x_offset+x < 0 || 15 < x_offset+x
            ){
          return GRID_ALREADY_OCCUPIED;
        }
      }
    }
  }

  // New piece can't share the edge with own tile
  for(y=0; y<5; y++){
    for(x=0; x<5; x++){
      int b;
      b = pieces[c][0][rotate[r][y][x]];
      if (b==1){
        int xx, yy;
        xx = x_offset+x;
        yy = y_offset+y;
        if (board[yy][xx-1] == player || board[yy][xx+1] == player ||
            board[yy-1][xx] == player || board[yy+1][xx] == player ){
          return SHARES_EDGE;
        }
      }
    }
  }

  // must share the vertex with own tile
  if(turn >= 2){
    int got_it = FALSE;
    for(y=0; y<5; y++){
      for(x=0; x<5; x++){
        int b;
        b = pieces[c][0][rotate[r][y][x]];
        if (b==1){
          int xx, yy;
          xx = x_offset+x;
          yy = y_offset+y;
          if (board[yy-1][xx-1] == player || board[yy+1][xx-1] == player ||
              board[yy-1][xx+1] == player || board[yy+1][xx+1] == player){
            got_it = TRUE;
          }
        }
      }
    }
    if(!got_it){
      return MUST_SHARE_VERTEX;
    }
  } else {
    // player 1's first must cover (5,5)
    if(turn < 2){
      if (player == 1 && !(3<= m.x && m.x <= 7 && 3 <= m.y && m.y <= 7 &&
			   pieces[c][0][rotate[r][ 5-y_offset][ 5-x_offset]]==1 )) {
	return MUST_COVER_FIRST_PLACE;
      }
      // player 2's first move must cover (a,a)
      if (player == 2 && !(8<= m.x && m.x <= 12 && 8 <= m.y && m.y <= 12 &&
			   pieces[c][0][rotate[r][10-y_offset][10-x_offset]]==1 )) {
	return MUST_COVER_SECOND_PLACE;
      }
    }
  }

  return 0;
}

void show_error(int e){
  switch(e){
  case PIECE_ALREADY_PLACED:
    printf("INVALID MOVE! (the piece is already placed)\n");
    break;
  case GRID_ALREADY_OCCUPIED:
    printf("INVALID MOVE! (move on where already occupied)\n");
    break;
  case SHARES_EDGE:
    printf("INVALID MOVE! (shares edge)\n");
    break;
  case MUST_SHARE_VERTEX:
    printf("INVALID MOVE! (must share vertex)\n");
    break;
  case MUST_COVER_FIRST_PLACE:
    printf("INVALID MOVE! (player 1's first move must cover (5,5)\n");
    break;
  case MUST_COVER_SECOND_PLACE:
    printf("INVALID MOVE! (player 2's first move must cover (a,a))\n");
    break;
  }
}

int check_possibility(int player, int turn){
  move m;

  for (m.y=1; m.y<15; m.y++){
    for (m.x=1; m.x<15; m.x++){
      for (m.piece=0; m.piece<21; m.piece++){
        for(m.rotate=0; m.rotate<8; m.rotate++){
          if(check_move(player, turn, m) == 0){
            if (show_hint){
              printf("First-found possible move: %c%c%c%c\n", 
                     ((m.x<10) ? m.x+'0' : m.x+'a'-10),
                     ((m.y<10) ? m.y+'0' : m.y+'a'-10),
                     m.piece+'a', m.rotate+'0');
            }
            return TRUE;
          }
        }
      }
    }
  }
  return FALSE;
}

int next_player(int player){
  if(player == 1) return 2;
  return 1;
}

int remaining_size(int player){ // player 1 or 2 (not 0 or 1)
  int i, a=0;
  for(i=0; i<21; i++)
    a += available[player-1][i] * piece_sizes[i];

  return a;
}

void init_serial(){
  int p;
  int ports = 1;

  //  const char flush_code[11] = "9999999999";
  const char init_code[2] = "0";
  char team_id[4];

  int temp_timeout;

  if (on_serial == 0) return;
  if (on_serial == 3) ports = 2;
  
  for (p=0; p<ports; p++){
    if(!use_tcp){ // init serial

      serial_fd[p] = open(serial_dev[p], O_RDWR | O_NOCTTY);
      if (serial_fd[p] < 0 ) {
	perror("open serial failed! ");
	exit(-1);
      }
    
      tcgetattr(serial_fd[p], &oldtio[p]); // backup port settings
      tcflush(serial_fd[p], TCIOFLUSH);
  
      memset(&tio[p], 0, sizeof(tio[p]));
      cfsetispeed(&tio[p], B115200);
      cfsetospeed(&tio[p], B115200);
      tio[p].c_cflag |= CS8; // 8N1
      tio[p].c_cflag |= CLOCAL; // local connection, no modem control
      tio[p].c_cflag |= CREAD;

      tcsetattr(serial_fd[p], TCSANOW, &tio[p]);
      tcflush(serial_fd[p], TCIOFLUSH);
    } else { // init tcp
      int sock, on;
      struct sockaddr_in s_addr;

      on = 1;
      sock = socket(AF_INET, SOCK_STREAM, 0);
      if (sock<0){
	perror("opening stream socket failed! ");
	exit(-1);
      }
      on = 1;
      setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on));
#if defined (__FreeBSD__)  || (__APPLE__)
      setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, (char*)&on, sizeof(on));
#endif
      
      bzero((void*)&s_addr, sizeof(s_addr));

      s_addr.sin_family = AF_INET;
      s_addr.sin_addr.s_addr = INADDR_ANY;
      s_addr.sin_port = htons(10000 + p);

      if(bind(sock, (struct sockaddr *)&s_addr, sizeof(s_addr)) < 0){
	perror("bind socket failed! ");
	exit(-1);
      }

      listen(sock, 0); // only 1 client per port
      printf("Waiting on TCP port %d...\n", 10000+p);

      serial_fd[p] = accept(sock, NULL, NULL);
      if(serial_fd[p] == -1){
	perror("accept() failed! ");
	exit(-1);
      }
    }

    // flush 
    /*
      write(serial_fd[p], flush_code, strlen(flush_code));
    */

    write(serial_fd[p], init_code, strlen(init_code));

    // at least 30s timeout for game initialization
    temp_timeout = move_timeout;
    if (move_timeout < 30) move_timeout = 30;

    if (read_all(serial_fd[p], 3, team_id) != 3){
      printf("Timeout while waiting team code on serial port %d!\n", p);
      exit(-1);
    }
    team_id[3] = 0;
    printf("Team code on serial %d: %s\n", p, &team_id[1]);
    strncpy(team_ids[p], &team_id[1], 2);

    move_timeout = temp_timeout; // restore
  }
}

void close_serial(){
  int p;
  int ports = 1;

  const char close_code[2] = "9";

  if (on_serial == 0) return;
  if (on_serial == 3) ports = 2;
  
  printf("sending termination code to serial port(s).\n");
  
  for (p=0; p<ports; p++){
    write(serial_fd[p], close_code, 1);
    if(!use_tcp){
      tcdrain(serial_fd[p]);
      tcsetattr(serial_fd[p], TCSANOW, &oldtio[p]);
    }
    close(serial_fd[p]);
  }
}

void usage(){
  printf("Command line options: \n" \
         "   -b: Hide board status\n" \
         "   -p: Show placed tile on move\n"\
         "   -t: Don't show available tiles\n"\
         "   -h: Show hint (for quick testplay)\n"\
	 "   -r: Player 2 moves first\n"\
         "   -1: First move player on serial port 0\n"\
         "   -2: Second move player on serial port 0\n"\
         "   -3: Players on serial port 0 and 1. (still does NOT work)\n"\
         "   -T: tcp port 10000+10001 instead of serial port\n"\
	 "   -o XX: set timeout to XX seconds (default: 1)\n"\
         ""
         );
}

int do_game(){
  int c, x, y, r, player, turn;
  char code[6], prev_code[6];

  // ------------------------------------------------------------
  // start!
   
  show_board();

  player = first_player;
  turn = 0;
  prev_code[0] = 0;

  while(prompt(player, code, prev_code, 
	       !check_possibility(player, turn), turn)){
    move m;
    int e, x_offset, y_offset;
    if(code[0] == 0) return TERMINATE_WRONG;  // ctrl+d

    // retry if invalid code
    while(!check_code(code)){
      if(interactive(player)){
	prompt(player, code, prev_code, FALSE, turn);
      } else {
	printf("Invalid move on serial port.\n");
	return player;
      }
      if(code[0] == 0) return TERMINATE_WRONG;
    }
    if(code[0] == 0) return TERMINATE_WRONG;  // ctrl+d
    if(prev_code[0] == 0) strcpy(p1move1, code);
    if(strcmp(prev_code, "0000") == 0 &&
       strcmp(code,      "0000") == 0){
      printf("Both pass!\n");

      show_hint = FALSE;

      if(! (check_possibility(player, turn) ||
	    check_possibility(next_player(player), turn-1))){
	return TERMINATE_NORMAL;
      }
      return TERMINATE_NORMAL; // is this OK?
    }

    strcpy(prev_code, code);

    // pass
    if(strcmp(code, "0000") == 0){
      if(turn >= 2){
	player = next_player(player);
	turn++;
	continue;
      } else {
	printf("First move must not be a pass.\n");
	return player;
      }
    }

    m = decode_code(code);

    c = m.piece;
    r = m.rotate;
    x_offset = m.x-2;
    y_offset = m.y-2;
    

    if((e = check_move(player, turn, m)) != 0){
      show_error(e);
      return player; 
    }
  
    // OK, now place the move
    for(y=0; y<5; y++){
      for(x=0; x<5; x++){
        int b;
        b = pieces[c][0][rotate[r][y][x]];
        if (b==1)
          board[y_offset+y][x_offset+x] = player;
      }
    }
    available[player-1][c] = 0;

    if(remaining_size(player)==0){
      printf("Player %d won the game, since the player has no more pieces.\n",
	     player);
      return TERMINATE_NORMAL;
    }
  
    // show the board & next player
    show_board();
    player = next_player(player);
    turn++;
  }

  printf("Player %d timed out.\n", player);
  return player;
}


int main(int argc, char* argv[]){
  int x, y, result;

  int ch;
  //while ((ch = getopt(argc, argv, "bpthro:123T?")) != -1) {
  while ((ch = getopt(argc, argv, "bpthro:x:y:123T?")) != -1) {
    switch (ch) {
    case 'b': show_board_status = FALSE;  break;
    case 'p': show_placed_tile = TRUE; break;
    case 't': show_available_tiles = FALSE; break;
    case 'h': show_hint = TRUE; break;
    case 'r': first_player = 2; break; // player 2 moves first
    case '1': on_serial = 1; break; // player 1 on serial
    case '2': on_serial = 2; break; // player 2 on serial
    case '3': on_serial = 3; break; // both player on serial
    case 'T': use_tcp = TRUE; break; // use TCP
    case 'o': move_timeout = atoi(optarg); break; // set timeout
   	case 'x': first_turn_p1 = atoi(optarg); break; // player 1 first move
	case 'y': first_turn_p2 = atoi(optarg); break; // palyer 2 first move
    case '?': break;
    default:
      usage();
      return 0;
    }
  }

  /*++nilim++*/	
  srand( (unsigned)time( NULL ) );
  /*--nilim--*/
  init_serial();

  printf(">> %s %s vs %s %s \n", 
	 team_ids[0], ((first_player==1) ? "*" : ""),
	 team_ids[1], ((first_player==2) ? "*" : ""));

  // ------------------------------
  // clear board & available pieces

  for(y=0; y<16; y++)
    for(x=0; x<16; x++)
      board[y][x] = 0;

  for(y=0; y<2; y++)
    for(x=0; x<21; x++)
      available[y][x] = 1;

  // setup board: border is already filled
  for(x=0; x<16; x++){
    board[ 0][ x] = BORDER;
    board[15][ x] = BORDER;
    board[ x][ 0] = BORDER;
    board[ x][15] = BORDER;
  }

#ifdef DEBUG_FILL
  // for test (some grids already occupied on start)
  for(y=7; y<=14; y++)
    for(x=12; x<=14; x++)
      board[y][x] = 2;

  for(y=12; y<=14; y++)
    for(x=7; x<=14; x++)
      board[y][x] = 2;
#endif

#ifdef DEBUG_LESS_TILES
  for(x=5; x<21; x++){
    available[0][x] = 0;
    available[1][x] = 0;
  }
#endif


  switch(result = do_game()){
    int a1, a2;
    
  case TERMINATE_NORMAL:
    a1 = remaining_size(1);
    a2 = remaining_size(2);
    
    printf("** Total remaining size: %d / %d. ", a1, a2);
    if(a1 != a2)
      printf("Player %d won the game!\n", ( (a1<a2) ? 1 : 2 ) );
    else
      printf("Draw game.\n");
    break;

  case 1:
  case 2:
    printf("** Player %d lost the game by violation.\n", result);
    break;

  default:
  case TERMINATE_WRONG:
    printf("** Game terminated unexpectedly.\n");
    break;
  }
  
  close_serial();
  return 0;
}
