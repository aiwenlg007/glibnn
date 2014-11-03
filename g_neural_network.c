/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 */

#include "g_neural_network.h"
#include "g_layer.h"
#include "g_neuron.h"
#include <string.h>
#include <math.h>

enum
{
	PROP_0,
};

G_DEFINE_TYPE (GNeuralNetwork, g_neural_network, G_TYPE_OBJECT);

struct _GNeuralNetworkPrivate
{
	GList   *layers;//double linked list of neurons
	gdouble learning_rate;
	gdouble inertie;
	gdouble error_rate;
    gint    num_layers;
};

static void g_neural_network_dispose(GObject *object)
{
	G_OBJECT_CLASS (g_neural_network_parent_class)->dispose (object);
}

static void g_neural_network_finalize(GObject *object)
{
	GNeuralNetwork *neural_network= G_NEURAL_NETWORK(object);
	GList *iter = NULL;

	neural_network->priv = G_TYPE_INSTANCE_GET_PRIVATE (neural_network,
		TYPE_G_NEURAL_NETWORK, GNeuralNetworkPrivate);
	GNeuralNetworkPrivate *priv = neural_network->priv;

	for(iter = priv->layers; iter ; iter = iter->next)
	{
		g_object_unref(G_LAYER(iter->data));
	}

	g_list_free(priv->layers);

	G_OBJECT_CLASS (g_neural_network_parent_class)->finalize (object);
}

static void g_neural_network_set_property (GObject      	*gobject,
									 guint         	prop_id,
									 const GValue 	*value,
									 GParamSpec   	*pspec)
{
    switch (prop_id)
	{
        default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
			break;
	}
}

static void g_neural_network_get_property (GObject    	*gobject,
									 guint       prop_id,
									 GValue     *value,
									 GParamSpec *pspec)
{
    switch (prop_id)
	{
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
			break;
	}
}

static void g_neural_network_class_init (GNeuralNetworkClass *klass)
{
	g_type_class_add_private (klass, sizeof (GNeuralNetworkPrivate));

	GObjectClass *gklass = G_OBJECT_CLASS(klass);
	gklass->get_property = g_neural_network_get_property;
	gklass->set_property = g_neural_network_set_property;
	gklass->dispose      = g_neural_network_dispose;
	gklass->finalize     = g_neural_network_finalize;
}

static void g_neural_network_init (GNeuralNetwork *neural_network){
	g_return_if_fail(NULL != neural_network);

	neural_network->priv = G_TYPE_INSTANCE_GET_PRIVATE (neural_network,
			TYPE_G_NEURAL_NETWORK, GNeuralNetworkPrivate);
	GNeuralNetworkPrivate *priv = neural_network->priv;

	priv->layers      = NULL;
    priv->num_layers  = 0;
}

//build a neural_network
GNeuralNetwork* g_neural_network_new(const gint num_layers,
									 const gint* neurons_per_layers,
									 const gdouble learning_rate,
									 const gdouble inertie,
									 const gdouble error_rate,
									 gint ninputs)
{
	gint i;
	gint npt = ninputs;

	g_return_val_if_fail(0 		!= ninputs, 								NULL);
	g_return_val_if_fail(0 		!= num_layers, 								NULL);
	g_return_val_if_fail(NULL 	!= neurons_per_layers, 						NULL);
	g_return_val_if_fail(0.0f <= learning_rate && learning_rate <= 1.0f, 	NULL);
	g_return_val_if_fail(0.0f <= inertie && inertie <= 1.0f, 				NULL);
	g_return_val_if_fail(0.0f <= error_rate && error_rate < 1.0f, 			NULL);

	GNeuralNetwork *neural_network 	= (GNeuralNetwork *)g_object_new(TYPE_G_NEURAL_NETWORK, NULL);

	neural_network->priv = G_TYPE_INSTANCE_GET_PRIVATE (neural_network,
			TYPE_G_NEURAL_NETWORK, GNeuralNetworkPrivate);
	GNeuralNetworkPrivate *priv = neural_network->priv;

	priv->learning_rate = learning_rate;
	priv->inertie 		= inertie;
	priv->error_rate	= error_rate;
    priv->num_layers    = num_layers;

	for(i = 0; i < num_layers; ++i)
	{
		//create a new layer
		GLayer* layer = g_layer_new(i, npt, neurons_per_layers[i]);

		//add a the new layer to the list of layers
		priv->layers = g_list_append(priv->layers, layer);

		//updating the num of entries
		npt = neurons_per_layers[i];
	}

	return neural_network;
}

void g_neural_network_set_desired_output(GNeuralNetwork *neural_network,
										gdouble *desired)
{
	g_return_if_fail(NULL != neural_network);
	g_return_if_fail(NULL != desired);

	neural_network->priv = G_TYPE_INSTANCE_GET_PRIVATE (neural_network,
			TYPE_G_NEURAL_NETWORK, GNeuralNetworkPrivate);
	GNeuralNetworkPrivate *priv = neural_network->priv;

	GLayer* last_layer = (GLayer *)g_list_nth_data(priv->layers, priv->num_layers - 1);

	g_layer_set_desired_output(last_layer, desired);
}

void g_neural_network_set_input(GNeuralNetwork *neural_network,
								gdouble *input)
{
	g_return_if_fail(NULL != input);
	g_return_if_fail(NULL != neural_network);

	neural_network->priv = G_TYPE_INSTANCE_GET_PRIVATE (neural_network,
			TYPE_G_NEURAL_NETWORK, GNeuralNetworkPrivate);
	GNeuralNetworkPrivate *priv = neural_network->priv;

	GLayer* first_layer = (GLayer *)g_list_nth_data(priv->layers, 0);
	g_layer_set_inputs(first_layer, input);
}

void g_neural_network_back_propagation_learning(GNeuralNetwork *neural_network)
{
	GList 	*iter	    = NULL;
	gdouble *out 	    = NULL;
    gint next_nneurons  = -1;

	//at the beginning the error is maximale
	gdouble error 	= 1.0f;

	g_return_if_fail(NULL != neural_network);

	neural_network->priv = G_TYPE_INSTANCE_GET_PRIVATE (neural_network,
			TYPE_G_NEURAL_NETWORK, GNeuralNetworkPrivate);
	GNeuralNetworkPrivate *priv = neural_network->priv;

	//propagate the input, get output
	//back propagate the error
	//update weight
	do
	{
		//propagation
		for(iter = priv->layers; iter; iter = iter->next)
		{
			g_layer_compute_outputs(G_LAYER(iter->data));

			out = g_layer_get_outputs(G_LAYER(iter->data));

			if((iter->next) != NULL)
			{
				g_layer_set_inputs(G_LAYER((iter->next)->data), out);
			}
		}

		//get the error rate
		error = g_layer_get_error_rate(G_LAYER(g_list_nth_data(priv->layers,priv->num_layers -1)));

		//if error rate not good back propagate the error
		g_warning("error: %.10f", error);

        g_list_free(iter);

        //get the error vector for the output layers
        for(iter = g_list_nth(priv->layers, priv->num_layers - 1); iter; iter = iter->prev)
        {
            //we have no child here we are at the output layer
            if(iter->next != NULL)
            {
                next_nneurons = g_layer_get_nneurons(G_LAYER(iter->data));
            }

            g_layer_update_layer_weights(G_LAYER(iter->data),
                priv->learning_rate,
                priv->inertie,
                next_nneurons);
        }

	}while(error > priv->error_rate);
}
