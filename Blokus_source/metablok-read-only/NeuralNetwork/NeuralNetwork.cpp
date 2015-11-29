/* ===========================================================================

	Project: Neural Network 

	Description:
	 Implements neural network using neural network layer elements.

	 Modified from neural network class and functions taken from:
		"AI for Game Developers" by David M. Bourg & Glenn Seemann
		Copyright 2004 O'ReillyMedia, Inc., 0-596-00555-5.

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

// Standard includes
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <math.h>

// Include Layers
#include "NeuralNetworkLayer.h"

// Include header
#include "NeuralNetwork.h"

// Initializes the neural network
void NeuralNetwork::Initialize( int nNodesInput, 
								int nNodesHidden, 
								int nNodesOutput )
{
	InputLayer.NumberOfNodes = nNodesInput;
	InputLayer.NumberOfChildNodes = nNodesHidden;
	InputLayer.NumberOfParentNodes = 0;
	InputLayer.Initialize( nNodesInput, NULL, &HiddenLayer );
	InputLayer.RandomizeWeights( );

	HiddenLayer.NumberOfNodes = nNodesHidden;
	HiddenLayer.NumberOfChildNodes = nNodesOutput;
	HiddenLayer.NumberOfParentNodes = nNodesInput;
	HiddenLayer.Initialize( nNodesHidden, &InputLayer, &OutputLayer );
	HiddenLayer.RandomizeWeights( );

	OutputLayer.NumberOfNodes = nNodesOutput;
	OutputLayer.NumberOfChildNodes = 0;
	OutputLayer.NumberOfParentNodes = nNodesHidden;
	OutputLayer.Initialize( nNodesOutput, &HiddenLayer, NULL );
}

// Sets input neuron values
void NeuralNetwork::SetInput( int i, double value )
{
	InputLayer.NeuronValues[i] = value;
}

// Sets the desired output values
void NeuralNetwork::SetDesiredOutput( int i, double value )
{
	OutputLayer.DesiredValues[i] = value;
}

// Returns the output neuron values
double NeuralNetwork::GetOutput( int i )
{
	return OutputLayer.NeuronValues[i];
}

// Calculates neuron values 
void NeuralNetwork::FeedForward( )
{
	InputLayer.CalculateNeuronValues( );
	HiddenLayer.CalculateNeuronValues( );
	OutputLayer.CalculateNeuronValues( );
}

// Computes error and adjusts weights
void NeuralNetwork::BackPropogate( )
{
	OutputLayer.CalculateErrors( );
	HiddenLayer.CalculateErrors( );

	HiddenLayer.AdjustWeights( );
	InputLayer.AdjustWeights( );
}

// Returns the largest output neuron
int NeuralNetwork::GetMaxOutputID( )
{
	double max = OutputLayer.NeuronValues[0];
	int id;

	for( int i = 1; i < OutputLayer.NumberOfNodes; i++ )
		if( OutputLayer.NeuronValues[i] > max )
			{ max = OutputLayer.NeuronValues[i]; id = i; }

	return id;
}

// Calculates the error assosciated with the output
double NeuralNetwork::CalculateError( )
{
	double error = 0.0;

	for( int i = 0; i < OutputLayer.NumberOfNodes; i++ )
		error += pow(OutputLayer.NeuronValues[i]-
				OutputLayer.DesiredValues[i], 2);

	return error / (double)OutputLayer.NumberOfNodes;
}

// Sets the learning rate of the network
void NeuralNetwork::SetLearningRate( double rate )
{
	InputLayer.LearningRate = rate;
	HiddenLayer.LearningRate = rate;
	OutputLayer.LearningRate = rate;
}

// Sets whether to use linear activation 
void NeuralNetwork::SetLinearOutput( bool useLinear )
{
	InputLayer.LinearOutput = useLinear;
	HiddenLayer.LinearOutput = useLinear;
	OutputLayer.LinearOutput = useLinear;
}

// Sets whether to train using momentum 
void NeuralNetwork::SetMomentum( bool useMomentum, double factor )
{
	InputLayer.UseMomentum = useMomentum;
	HiddenLayer.UseMomentum = useMomentum;
	OutputLayer.UseMomentum = useMomentum;

	InputLayer.MomentumFactor = factor;
	HiddenLayer.MomentumFactor = factor;
	OutputLayer.MomentumFactor = factor;
}

// Outputs neural network settings to file
void NeuralNetwork::DumpData( const char* name )
{
	// Open the file for rewriting, creating it if necessary
	std::fstream file(name, std::ios::in | std::ios::out | std::ios::trunc);

	// Check for failure
	if( !file.is_open( ) ) return;
	
	// Write number of nodes per layer to file
	file << "\nNumber of Input Layer Nodes\n";
	file << InputLayer.NumberOfNodes;
	file << "\nNumber of Hidden Layer Nodes\n";
	file << HiddenLayer.NumberOfNodes;
	file << "\nNumber of Output Layer Nodes\n";
	file << OutputLayer.NumberOfNodes;
	file << "\n";
	 
	// Write input header into the file
	file << "// --------------------------------\n";
	file << "//           Input Layer           \n";
	file << "// --------------------------------\n";

	// Write node weights
	file << "\nWeights:\n";
	for( int i = 0; i < InputLayer.NumberOfNodes; i++ )
	for( int j = 0; j < InputLayer.NumberOfChildNodes; j++ )
		file << InputLayer.Weights[i][j] << ' ';

	// Write bias weights
	file << "\nBias Weights:\n";
	for( int i = 0; i < InputLayer.NumberOfChildNodes; i++ ) {
		file << InputLayer.BiasWeights[i] << ' ';
	}
	file << '\n';

	// Write hidden header into the file
	file << "// --------------------------------\n";
	file << "//           Hidden Layer          \n";
	file << "// --------------------------------\n";

	// Write node weights
	file << "\nWeights:\n";
	for( int i = 0; i < HiddenLayer.NumberOfNodes; i++ ) {
	    for( int j = 0; j < HiddenLayer.NumberOfChildNodes; j++ ) {
		    file << HiddenLayer.Weights[i][j] << ' ';
		}
	}

	// Write bias weights
	file << "\nBias Weights:\n";
	for( int i = 0; i < HiddenLayer.NumberOfChildNodes; i++ ) {
		file << HiddenLayer.BiasWeights[i] << ' ';
    }
    file << '\n';

	// Write output header into the file
	file << "// --------------------------------\n";
	file << "//           Output Layer          \n";
	file << "// --------------------------------\n";

	// Write node weights
	file << "\nWeights:\n";
	for( int i = 0; i < OutputLayer.NumberOfNodes; i++ ) {
	    for( int j = 0; j < OutputLayer.NumberOfChildNodes; j++ ) {
		    file << OutputLayer.Weights[i][j] << ' ';
		}
    }

	// Write bias weights
	file << "\nBias Weights:\n";
	for( int i = 0; i < OutputLayer.NumberOfChildNodes; i++ ) {
		file << OutputLayer.BiasWeights[i] << ' ';
    }

	// Close file
	file.close();
}

// Loads neural network settings from file
bool NeuralNetwork::LoadData( const char* name )
{
	// Open the file for reading, creating it if necessary
	std::ifstream file(name);

	// Check for failure
	if(!file.is_open()) return false;
	
	// Loop through file reading each line
	std::string line;
	int fieldnum = 0;
	while(file.good() && std::getline(file, line)) {
	    // Keep reading until we get to a line that starts with a number
	    if(!isdigit(line[0]) && line[0] != '-') {
	        continue;
	    }
	    
	    std::stringstream linestream(line, std::stringstream::in);
	    int inputnodes, hiddennodes, outputnodes;
	    double weight, bias;
	    
	    // Now do stuff based on the field number
	    switch(fieldnum) {
	        case 0: // Number of input layer nodes
	            linestream >> inputnodes; break;
	        case 1: // Number of hidden layer nodes
	            linestream >> hiddennodes; break;
	        case 2: // Number of output layer nodes
	            linestream >> outputnodes;
	            /* TODO: Cleanup(); */
	            Initialize(inputnodes, hiddennodes, outputnodes);
	            break;
	        case 3: // Input layer weights (inputnodes * hiddennodes)
	            for(int i = 0; i < inputnodes; i++) {
	                for(int j = 0; j < hiddennodes; j++) {
	                    linestream >> weight;
	                    InputLayer.Weights[i][j] = weight;
	                }
	            }
	            break;
	        case 4: // Input layer bias weights (hiddennodes)
	            for(int i = 0; i < hiddennodes; i++) {
	                linestream >> bias;
	                InputLayer.BiasWeights[i] = bias;
	            }
	            break;
	        case 5: // Hidden layer weights (hiddennodes * outputnodes)
	            for(int i = 0; i < hiddennodes; i++) {
	                for(int j = 0; j < outputnodes; j++) {
	                    linestream >> weight;
	                    HiddenLayer.Weights[i][j] = weight;
	                }
	            }
	            break;
	        case 6: // Hidden layer bias weights (outputnodes)
	            for(int i = 0; i < hiddennodes; i++) {
	                linestream >> bias;
	                HiddenLayer.BiasWeights[i] = bias;
	            }
	            break;
	        default:
	            break;
	    }
	    fieldnum++;
	}
	
	file.close();

	// Return successful
	return true;
}

// Frees memory allocated by the network
void NeuralNetwork::Cleanup( )
{
	InputLayer.Cleanup( );
	HiddenLayer.Cleanup( );
	OutputLayer.Cleanup( );
}
