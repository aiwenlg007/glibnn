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
GNeuralNetwork* g_neural_network_new(GList* layers_list,
									 GNeuralNetworkProperties ppt,
                                     gint ninputs)
{
	gint i;
	gint npt = ninputs;

    g_return_val_if_fail(NULL   != layers_list,                                     NULL);
	g_return_val_if_fail(0 		!= ninputs, 								        NULL);
	g_return_val_if_fail(0 		!= g_list_length(layers_list),				        NULL);
	g_return_val_if_fail(0.0f <= ppt.property.learning_rate && ppt.property.learning_rate <= 1.0f, 	NULL);
	g_return_val_if_fail(0.0f <= ppt.property.inertial_rate && ppt.property.inertial_rate <= 1.0f, 	NULL);
	g_return_val_if_fail(0.0f <= ppt.property.error_rate && ppt.property.error_rate < 1.0f, 			NULL);

	GNeuralNetwork *neural_network 	= (GNeuralNetwork *)g_object_new(TYPE_G_NEURAL_NETWORK, NULL);

	neural_network->priv = G_TYPE_INSTANCE_GET_PRIVATE (neural_network,
			TYPE_G_NEURAL_NETWORK, GNeuralNetworkPrivate);
	GNeuralNetworkPrivate *priv = neural_network->priv;

	priv->learning_rate = ppt.property.learning_rate;
	priv->inertie 		= ppt.property.inertial_rate;
	priv->error_rate	= ppt.property.error_rate;
    priv->num_layers    = g_list_length(layers_list);

	for(i = 0; i < g_list_length(layers_list); ++i)
	{
        gint nneurons = GPOINTER_TO_INT(g_list_nth_data(layers_list, i));

        g_return_val_if_fail(1 <= nneurons, NULL);

		//create a new layer
		GLayer* layer = g_layer_new(i, npt, nneurons);

		//add a the new layer to the list of layers
		priv->layers = g_list_append(priv->layers, layer);

		//updating the num of entries
		npt = nneurons;
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

void g_neural_network_save(GNeuralNetwork *neural_network, const gchar* path)
{
    g_return_if_fail(NULL != neural_network);
    g_return_if_fail(NULL != path);

	neural_network->priv = G_TYPE_INSTANCE_GET_PRIVATE (neural_network,
			TYPE_G_NEURAL_NETWORK, GNeuralNetworkPrivate);
	GNeuralNetworkPrivate *priv = neural_network->priv;

    GKeyFile  *keyfile;
    GError    *error          = NULL;
    gsize     size;
    GList     *iter           = NULL;
    gint      id;
    gint      nneurons;
    gchar     *layer_name     = NULL;
    gchar     *neuron_name    = NULL;
    gint      i;
    GNeuron*  neuron;
    gdouble   *neuron_weights = NULL;
    gint      neuron_inputs;
    gchar     *data = NULL;

    //create a new keyfile
    keyfile = g_key_file_new();

    //record intenal parameters
    g_key_file_set_double(keyfile, "properties", "error_rate", priv->error_rate);
    g_key_file_set_double(keyfile, "properties", "learning_rate", priv->learning_rate);
    g_key_file_set_double(keyfile, "properties", "inertial_rate", priv->inertie);

    //record each layer
    for(iter = priv->layers; iter; iter = iter->next)
    {
        id = g_layer_get_id(G_LAYER(iter->data));
        nneurons  = g_layer_get_nneurons(G_LAYER(iter->data));
        layer_name = g_strdup_printf("Layer%d", id);

        //record each neuron
        for(i = 0; i < nneurons; ++i)
        {
            neuron          = g_layer_get_nth_neuron(G_LAYER(iter->data), i);
            neuron_name     = g_strdup_printf("neuron%d",i);
            neuron_inputs   = g_neuron_get_ninputs(neuron);
            neuron_weights  = g_neuron_get_weights(neuron);

            g_key_file_set_double_list(keyfile, layer_name, neuron_name, neuron_weights, neuron_inputs);

            g_free(neuron_name);
        }

        g_free(layer_name);
    }

    data = g_key_file_to_data(keyfile, &size, &error);
    g_file_set_contents(path, data, size, &error);

    g_list_free(iter);
    g_free(data);
    g_key_file_free(keyfile);
}

GNeuralNetwork* g_neural_network_new_from_file(gchar *path)
{
    g_return_val_if_fail(NULL != path, NULL);
	g_return_val_if_fail(TRUE == g_file_test(path, G_FILE_TEST_EXISTS), NULL);

	GNeuralNetwork *neural_network 	= (GNeuralNetwork *)g_object_new(TYPE_G_NEURAL_NETWORK, NULL);

	neural_network->priv = G_TYPE_INSTANCE_GET_PRIVATE (neural_network,
			TYPE_G_NEURAL_NETWORK, GNeuralNetworkPrivate);
	GNeuralNetworkPrivate *priv = neural_network->priv;

	//read the file under path
    GKeyFile* keyfile;
	GKeyFileFlags flags;
	GError *error 	= NULL;
	gchar **groups 	= NULL;
	gchar **keys	= NULL;
	GList *neuron_weights	= NULL;
	gsize size;
	gdouble *weights = NULL;
	GList *iter 	= NULL;

	priv->num_layers = 0;

	keyfile = g_key_file_new();
	flags = G_KEY_FILE_KEEP_COMMENTS;

	
	if(g_key_file_load_from_file (keyfile, path, flags, &error ) )
	{	
		//first read properties
		priv->inertie 		= g_key_file_get_double(keyfile, "properties", "inertial_rate", NULL);
		priv->learning_rate = g_key_file_get_double(keyfile, "properties", "learning_rate", NULL);
		priv->error_rate	= g_key_file_get_double(keyfile, "properties", "error_rate", 	NULL);
		
		//now read all layers and neurons
		groups = g_key_file_get_groups(keyfile, NULL);
		
		while(*groups != NULL)
		{
			//properties have already be read
			if( !g_strcmp0(*groups, "properties") == FALSE )
			{
				keys = g_key_file_get_keys(keyfile, *groups, NULL, NULL);
			
				while(*keys != NULL)
				{
					weights = g_key_file_get_double_list(keyfile, *groups, *keys, &size, NULL);
					neuron_weights = g_list_append(neuron_weights, weights);
					
					keys ++;
				}
				
				//should give size - 1 because there is a 1 entry for the bias
				GLayer *layer = g_layer_new(priv->num_layers, size - 1 ,g_list_length(neuron_weights));
				
				//set the weight for all neurons of this layer
				g_layer_set_neuron_weights(layer, neuron_weights);
				
				//add the layer to the list
				priv->layers = g_list_append(priv->layers, layer);
				priv->num_layers ++;
				
				//cleaning
				for(iter = neuron_weights; iter; iter=iter->next)
				{
					g_free((gdouble *)(iter->data));
				}				
				g_list_free(neuron_weights);
				neuron_weights = NULL;
			
				g_list_free(iter);
			}
			
			groups ++;
		}
	}
	else
	{
		return NULL;
	}

	g_key_file_free(keyfile);
	
	return neural_network;
}
