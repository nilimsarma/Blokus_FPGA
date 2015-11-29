// ---------------------------------------------------------
//   
//                           README
//
// ---------------------------------------------------------

// ---------------------------------------------------------
//                        INTRODUCTION
// ---------------------------------------------------------

This aim of this project is to develop effective AI players
for the board game Blokus. In order to do so we have also
developed a program to simulate Blokus matches between AI
players. As such, there are very specific requirements which
developers must follow when adding new players. While these
requirements are outlined below, the Random AI player located
in the Random directory provides a simple basic example to
follow.


// ---------------------------------------------------------
//                    AI PLAYER CONVENTIONS
// ---------------------------------------------------------

The AI player communicate with the match simulator using a 
memory mapped file. The memory mapped file layout is specified
by the GameData structure defined in the Types.h file located
in the Includes directory. The name of this file will be passed
to the player as the first command-line argument on execution.

The file contains three different types of variables: settings
options, interprocess communication variables, and match state
variables. 

  Settings Options - This options will be set by the simulator
    once at the beginning of player initialization immediately 
    before an AI player is launched. If the player changes the
    data within this section, it is guaranteed neither to remain 
    unchanged or be changed back to the original values.

  Match State - This variables are set by the simulator before
    the beginning of each AI players turn. They describe the
    board state, move history, score, pieces, and current ply.

  IPC - the turnReady flag will be set when it is a players 
    turn to move and the match state variables have been set.
    When an AI player has completed their turn, they should
    FIRST set the move variable to their chosen move and THEN
    set the moveReady flag to a non-zero value. The timeout
    variable is set just before the turnReady flag and will
    indicate to the AI player how many seconds they have 
    remaining for move processing this match. 

The Move struct specifies a move on the game board using a format
outlined below. This class is defined in the Types.h alongside 
the GameState struct. Any ambiguous moves presented will automatically
be reduced to a common unambiguous format if the game is saved.

  pieceNumber - The piece number, labeled in increasing order from 
      0 to 20 as they are defined in the Pieces.txt file.

  rotated - The angle of the piece. 0 specifies the standard 
      orientation and subsequent positive values specify 
      counter-clockwise rotation by 90 degree increments.

  flipped - Whether the piece is flipped. ( Flipping is performed 
      after any rotation )

  GridX,GridY - The x and y grid coordinates of the topmost,leftmost 
        tile in the piece layout. Typically y is one tile above the
        topmost solid tile in the piece and x is one tile left of
        the leftmost solid tile after rotation and flipping have
        been performed.

Note: The layout and content of the elements within this file
  subject to change. It would be in the best interests of 
  forward compatibility to design your AI program in such
  a way that the main program is not directly reliant on 
  the data in the memory mapped file. For an example of what
  this means, see the main.h file in the Random project 
  directory.

  This is especially true since the current security issues
  with the communication and launch protocols make it possible
  for AI players to violate the principles of fair play.

  In addition, DO NOT develope your AI players under the
  assumption that they will play through an entire match
  without intervention from the empty board state. This 
  will lead to errors with save/load functionality and 
  the features of the simulator program.


// ---------------------------------------------------------
//                           CREDITS
// ---------------------------------------------------------

Special Thanks:

    - Chad Vernon, 
        For his tutorials on DirectX, providing the general engine
        layout and providing code for several of the encapsulation 
        classes. Some of the ideas for the GUI also came from his 
        tutorials. Original tutorials available at 
        http://www.chadvernon.com/

    - Keith Ditchburn
        For his tutorials on 3D Game Programming, much of the 3D
        transformations and 3D maths code were based on snippets 
        from his site. Original tutorials available at 
        http://www.toymaker.info/


// ---------------------------------------------------------
//                         CONTACT INFO
// ---------------------------------------------------------

If you have any questions or comments feel free to email me
(Lucas Sherman) at LucasASherman@gmail.com

// ---------------------------------------------------------
//                       PROJECT FILES
// ---------------------------------------------------------

compilation.txt - Provides instructions for compiling the main
     program and the associated AI player programs.

license.txt - Project licensing information

opening_book.txt - Opening book readme file

Random, Beam, Minimax, etc - AI Player Project Directories, 
    each should contain its own readme files and instructions.

Includes - Common include files used by multiple AI players

Source - Source code for the main program used to simulate
    blokus matches with AI players.



