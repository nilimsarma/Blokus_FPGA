/* ============================================================================

    Project: Blokus AI - Neural Network Trainer
    
    Provides a set of functions to load saved game files with evaluation
    scores and train the neural network with those values.
    
    File: NetworkTrainer.h
    Author: David Gloe
    Date: December 02, 2011
    
============================================================================= */

#ifndef NETWORK_TRAINER_H
#define NETWORK_TRAINER_H

#include "NeuralNetwork.h"
#include "PieceSet.h"
#include "Types.h"

struct VerifyResult {
    double networkOutput1; // Result from the first output neuron
    double networkOutput2; // Result from the second output neuron
    double desiredOutput;  // Score from the file
};

class NetworkTrainer {
    public:
        // ---------------------------------------------------------------------
        // Creates a new neural network with the specified properties.
        // learningRate: How much weight is given to new knowledge (>= 0)
        // momentum: Let a change in weight persist longer (0, 1)
        // linearOutput: if false, outputs 1/(1+e^(-x)) instead of x
        // ---------------------------------------------------------------------
        NetworkTrainer(double learningRate = 1.0, double momentum = 0.0,
            bool linearOutput = false);
            
        // ---------------------------------------------------------------------
        // Loads a neural network from a previously saved file for further
        // training.
        // Throws an int exception if the file could not be loaded.
        // ---------------------------------------------------------------------
        NetworkTrainer(const char *filename);
        
        // ---------------------------------------------------------------------
        // Destructor
        // ---------------------------------------------------------------------
        ~NetworkTrainer();
        
        // ---------------------------------------------------------------------
        // Loads the given save game from disk and determines the final board
        // state and score. It then feeds this information to the neural 
        // network.
        // ---------------------------------------------------------------------
        bool train(const char *filename);
        
        // ---------------------------------------------------------------------
        // Saves the trained neural network to the given filename
        // ---------------------------------------------------------------------
        void save(const char *filename);
        
        // ---------------------------------------------------------------------
        // Reads the saved game file given by the file, and obtains the 
        // neural network output. Returns a pair of floats representing the
        // neural network output and the score given in the file.
        // ---------------------------------------------------------------------        
        VerifyResult verify(const char *filename);
    
    private:
        bool loadFile(const char *filename);
        void makeMove(Move move); 
    
        char m_board[14][14];
        double m_score;
        bool m_pieces[2][21];
        int m_currentPlayer;
        int m_scores[2];
        int m_currentPly;
        int m_numberOfPlayers;
        Move m_moveHistory[42];
	    PieceSet* m_gamePieceLayouts;
        NeuralNetwork m_network;
};

#endif /* NETWORK_TRAINER_H */

