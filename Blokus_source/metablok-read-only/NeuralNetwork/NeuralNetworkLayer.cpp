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

// Standard library 
#include <stdlib.h>
#include <math.h>
#include <time.h>

// Include header
#include "NeuralNetworkLayer.h"

// Constructor
NeuralNetworkLayer::NeuralNetworkLayer( )
{
	ParentLayer = NULL;
	ChildLayer = NULL;
	LinearOutput = false;
	UseMomentum = false;
	MomentumFactor = 0.9;
}

// Allocates memory for the neural network
void NeuralNetworkLayer::Initialize( int NumNodes, 
						   NeuralNetworkLayer* parent, 
						   NeuralNetworkLayer* child )
{
	NeuronValues = (double*)malloc(sizeof(double)*
									NumberOfNodes);
	DesiredValues = (double*)malloc(sizeof(double)*
									NumberOfNodes);
	Errors = (double*)malloc(sizeof(double)*NumberOfNodes);

	ParentLayer = parent;
	
	if( child != NULL )
	{ 
		ChildLayer = child;

		Weights =(double**)malloc(sizeof(double)*
								   NumberOfNodes);
		WeightChanges =(double**)malloc(sizeof(double)*
										 NumberOfNodes);
		for( int i = 0; i < NumberOfNodes; i++ )
		{
			Weights[i] = (double*)malloc(sizeof(double)*
										  NumberOfChildNodes);
			WeightChanges[i] = (double*)malloc(sizeof(double)*
										    NumberOfChildNodes);
		}

		BiasValues = (double*)malloc(sizeof(double)*
									  NumberOfChildNodes);
		BiasWeights = (double*)malloc(sizeof(double)*
									   NumberOfChildNodes);
	} else {
		Weights = NULL;
		BiasValues = NULL;
		BiasWeights = NULL;
		WeightChanges = NULL;
	}

	for( int i = 0; i < NumberOfNodes; i++ )
	{
		NeuronValues[i] = 0;
		DesiredValues[i] = 0;
		Errors[i] = 0;

		if( ChildLayer != NULL )
			for( int j = 0; j < NumberOfChildNodes; j++ )
			{
				Weights[i][j] = 0;
				WeightChanges[i][j] = 0;
			}
	}

	if( ChildLayer != NULL )
		for( int i = 0; i < NumberOfChildNodes; i++ )
		{
			BiasValues[i] = -1;
			BiasWeights[i] = 0;
		}
}

// Randomizes neural network weights
void NeuralNetworkLayer::RandomizeWeights( )
{
	srand( (unsigned)time( NULL ) );

	for( int i = 0; i < NumberOfNodes; i++ )
		for( int j = 0; j < NumberOfChildNodes; j++ )
			Weights[i][j] = rand() / ((double)RAND_MAX/2.0) - 1.0;

	for( int i = 0; i < NumberOfChildNodes; i++ )
		BiasWeights[i] = rand() / ((double)RAND_MAX/2.0) - 1.0;
}

// Calculates the neuron values from input
void NeuralNetworkLayer::CalculateNeuronValues( )
{
	if( ParentLayer != NULL )
		for( int j = 0; j < NumberOfNodes; j++ )
		{
			double x = 0;
			for( int i = 0; i < NumberOfParentNodes; i++ )
				x += ParentLayer->NeuronValues[i]*
					 ParentLayer->Weights[i][j];
			x += ParentLayer->BiasValues[j]*
				 ParentLayer->BiasWeights[j];

			if( ChildLayer == NULL && LinearOutput ) NeuronValues[j] = x;
			else NeuronValues[j] = 1.0 / (1+exp(-x));
		}
}

// Calulates the error assosciated with each neuron
void NeuralNetworkLayer::CalculateErrors( )
{
	if( ChildLayer == NULL )
		for( int i = 0; i < NumberOfNodes; i++ )
			Errors[i] = (DesiredValues[i] - NeuronValues[i])*
						 NeuronValues[i]*
						(1.0 - NeuronValues[i]);
	else if( ParentLayer == NULL )
		for( int i = 0; i < NumberOfNodes; i++ )
			Errors[i] = 0.0;
	else
		for( int i = 0; i < NumberOfNodes; i++ )
		{
			double sum = 0.0;
			for( int j = 0; j < NumberOfChildNodes; j++ )
				sum += ChildLayer->Errors[j]*Weights[i][j];
			Errors[i] = sum*NeuronValues[i]*
						(1.0-NeuronValues[i]);
		}
}

// Adjusts weights using error values
void NeuralNetworkLayer::AdjustWeights( )
{
	if( ChildLayer != NULL )
	{
		for( int i = 0; i < NumberOfNodes; i++ )
			for( int j = 0; j < NumberOfChildNodes; j++ )
			{
				double dw = LearningRate*ChildLayer->Errors[j]*
						    NeuronValues[i];
				if( UseMomentum )
				{
					Weights[i][j] += dw + MomentumFactor*
								      WeightChanges[i][j];
					WeightChanges[i][j] = dw;
				} else 
					Weights[i][j] += dw;
			}

		for( int j = 0; j < NumberOfChildNodes; j++ )
			BiasWeights[j] += LearningRate*
							  ChildLayer->Errors[j]*
							  BiasValues[j];
	}
}

// Frees memory allocated by the neural network layer
void NeuralNetworkLayer::Cleanup( )
{
	free(NeuronValues);
	free(DesiredValues);
	free(Errors);

	if( Weights != NULL ) 
	{
		for( int i = 0; i < NumberOfNodes; i++ )
		{
			free(Weights[i]);
			free(WeightChanges[i]);
		}

		free(Weights);
		free(WeightChanges);
	}

	if( BiasValues ) free(BiasValues);
	if( BiasWeights ) free(BiasWeights);
}