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

// Begin definition
#ifndef NEURAL_NETWORK_H
#define NEURAL_NETWORK_H

#include "NeuralNetworkLayer.h"

// Neural network layer
class NeuralNetwork
{
public:
	NeuralNetworkLayer InputLayer;
	NeuralNetworkLayer HiddenLayer;
	NeuralNetworkLayer OutputLayer;

	void Initialize( int nNodesInput, int nNodesHidden, 
				     int nNodesOutput );
	void Cleanup( );
	void SetInput( int i, double value );
	double GetOutput( int i );
	void SetDesiredOutput( int i, double value );
	void FeedForward( );
	void BackPropogate( );
	int GetMaxOutputID( );
	double CalculateError( );
	void SetLearningRate( double rate );
	void SetLinearOutput( bool useLinear );
	void SetMomentum( bool useMomentum, double factor );
	void DumpData( const char* name );
	bool LoadData( const char* name );
};

// End definition
#endif
