/* ===========================================================================

	Project: Neural Network 

	Description:
	 Implements a neural network layer.

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
#ifndef NEURAL_NETWORK_LAYER_H
#define NEURAL_NETWORK_LAYER_H

// Neural network layer
class NeuralNetworkLayer
{
public:
	int			NumberOfNodes;
	int			NumberOfChildNodes;
	int			NumberOfParentNodes;
	double**	Weights;
	double**	WeightChanges;
	double*		NeuronValues;
	double*		DesiredValues;
	double*		Errors;
	double*		BiasWeights;
	double*		BiasValues;
	double		LearningRate;

	bool		LinearOutput;
	bool		UseMomentum;
	double		MomentumFactor;

	NeuralNetworkLayer* ParentLayer;
	NeuralNetworkLayer* ChildLayer;

	NeuralNetworkLayer( );

	void Initialize( int NumNodes, 
				  NeuralNetworkLayer* parent, 
				  NeuralNetworkLayer* child );
	void Cleanup( );
	void RandomizeWeights( );
	void CalculateErrors( );
	void AdjustWeights( );
	void CalculateNeuronValues( );
};

// End definition
#endif