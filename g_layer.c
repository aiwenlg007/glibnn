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

#include "g_layer.h"
#include <string.h>
#include <math.h>

enum
{
	PROP_0,
};

G_DEFINE_TYPE (GLayer, g_layer, G_TYPE_OBJECT);

struct _GLayerPrivate
{
	//each layer contains nneurons neurons
	GNeuron** 	neurons;
	gint		nneurons;
	gdouble*	outputs;
	gdouble*	desired;
	gint 		ninputs;
    gdouble*    error;
	gint 		id;
};

static void g_layer_dispose(GObject *object)
{
	G_OBJECT_CLASS (g_layer_parent_class)->dispose (object);
}

static void g_layer_finalize(GObject *object)
{
	GLayer *layer= G_LAYER(object);
	gint i;

	layer->priv = G_TYPE_INSTANCE_GET_PRIVATE (layer,
		TYPE_G_LAYER, GLayerPrivate);
	GLayerPrivate *priv = layer->priv;

	for(i = 0; i < priv->nneurons; ++i)
	{
		g_object_unref(priv->neurons[i]);
	}

	g_free(priv->neurons);
	g_free(priv->outputs);
	g_free(priv->desired);
	g_free(priv->error);

	G_OBJECT_CLASS (g_layer_parent_class)->finalize (object);
}

static void g_layer_set_property (GObject      	*gobject,
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

static void g_layer_get_property (GObject    	*gobject,
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

static void g_layer_class_init (GLayerClass *klass)
{
	g_type_class_add_private (klass, sizeof (GLayerPrivate));

	GObjectClass *gklass = G_OBJECT_CLASS(klass);
	gklass->get_property = g_layer_get_property;
	gklass->set_property = g_layer_set_property;
	gklass->dispose      = g_layer_dispose;
	gklass->finalize     = g_layer_finalize;
}

static void g_layer_init (GLayer *layer){
	g_return_if_fail(NULL != layer);

	layer->priv = G_TYPE_INSTANCE_GET_PRIVATE (layer,
			TYPE_G_LAYER, GLayerPrivate);
	GLayerPrivate *priv = layer->priv;

	priv->nneurons 	    = 0;
	priv->neurons 	    = NULL;
	priv->outputs	    = NULL;
	priv->desired	    = NULL;
    priv->error         = NULL;
}

gint g_layer_get_nneurons(GLayer *layer)
{
  	g_return_val_if_fail(NULL != layer, -1);

	layer->priv = G_TYPE_INSTANCE_GET_PRIVATE (layer,
			TYPE_G_LAYER, GLayerPrivate);
	GLayerPrivate *priv = layer->priv;

    return priv->nneurons;
}

void g_layer_compute_outputs(GLayer *layer)
{
	gint i;
    gdouble max=-1.0f, min=1.0f, mean = 0.0f;

	g_return_if_fail(NULL != layer);

	layer->priv = G_TYPE_INSTANCE_GET_PRIVATE (layer,
			TYPE_G_LAYER, GLayerPrivate);
	GLayerPrivate *priv = layer->priv;


	for(i = 0; i < priv->nneurons; ++i)
	{
		if(priv->id != 0 ) g_neuron_compute_activation(priv->neurons[i]);

        priv->outputs[i] = g_neuron_get_activation(priv->neurons[i]);
        mean += priv->outputs[i]/(gdouble)priv->nneurons;
        max = max <= priv->outputs[i] ? priv->outputs[i] : max;
        min = min  > priv->outputs[i] ? priv->outputs[i] : min;
    }

    for(i = 0; i < priv->nneurons; ++i)
    {
        if(priv->nneurons > 1) priv->outputs[i] -= mean;
	    if(max != min) priv->outputs[i] /= (max - min);
    }
}

gdouble* g_layer_get_outputs(GLayer *layer)
{
	g_return_val_if_fail(NULL != layer, NULL);

	layer->priv = G_TYPE_INSTANCE_GET_PRIVATE (layer,
			TYPE_G_LAYER, GLayerPrivate);
	GLayerPrivate *priv = layer->priv;

	return priv->outputs;
}

void g_layer_display(GLayer *layer)
{
	gint i;

	g_return_if_fail(NULL != layer);

	layer->priv = G_TYPE_INSTANCE_GET_PRIVATE (layer,
			TYPE_G_LAYER, GLayerPrivate);
	GLayerPrivate *priv = layer->priv;

	for(i = 0; i < priv->nneurons; ++i )
	{
		g_warning("Neuron %d", i);

		g_neuron_display(priv->neurons[i]);

		g_print("\n");

		g_print("out: %.5f\n", priv->outputs[i]);
		g_print("\n\n");
	}
}

//build a layer with nneurons
GLayer* g_layer_new(gint id, gint ninput, gint nneurons)
{
	g_return_val_if_fail(0 != ninput, 	NULL);
	g_return_val_if_fail(0 != nneurons, NULL);

	gint	i 		= 0;
	GLayer *layer 	= (GLayer *)g_object_new(TYPE_G_LAYER, NULL);

	layer->priv = G_TYPE_INSTANCE_GET_PRIVATE (layer,
			TYPE_G_LAYER, GLayerPrivate);
	GLayerPrivate *priv = layer->priv;

	priv->nneurons 	= nneurons;
	priv->id = id;
	priv->ninputs = ninput;

	//create an array of GNeuron
	priv->neurons 	= (GNeuron **)g_malloc0(nneurons*sizeof(GNeuron *));

	//create an array of outputs
	priv->outputs	= (gdouble *)g_malloc0(nneurons*sizeof(gdouble));
	priv->desired	= (gdouble *)g_malloc0(nneurons*sizeof(gdouble));
    priv->error     = (gdouble *)g_malloc0(nneurons*sizeof(gdouble));

	for(i = 0; i < priv->nneurons; ++i)
	{
		if(priv->id != 0)
		{
			priv->neurons[i] = g_neuron_new(id, ninput);
		}
		else
		{
			//if it is an input layer then create inputs neurons
			priv->neurons[i] = g_neuron_create_input_neuron();
		}
	}

	return layer;
}

gint g_layer_get_id(GLayer *layer)
{
    g_return_if_fail(NULL != layer);

    layer->priv = G_TYPE_INSTANCE_GET_PRIVATE (layer,
			TYPE_G_LAYER, GLayerPrivate);
	GLayerPrivate *priv = layer->priv;

    return priv->id;
}

void g_layer_set_inputs(GLayer *layer, gdouble *in)
{
	gint i;

	g_return_if_fail(NULL != layer);
	g_return_if_fail(NULL != in);


	layer->priv = G_TYPE_INSTANCE_GET_PRIVATE (layer,
			TYPE_G_LAYER, GLayerPrivate);
	GLayerPrivate *priv = layer->priv;

	//if we have an hidden or output layer then each neuron receive the same inputs vector
	//else we have an input layer which receive on input per neuron
	if(priv->id != 0)
	{
		for(i = 0; i < priv->nneurons; ++i)
		{
			g_neuron_set_inputs(priv->neurons[i], in);
		}
	}
	else
	{
		for(i = 0; i < priv->nneurons; ++i)
		{
			g_neuron_set_input_layer_inputs(priv->neurons[i], *(in+i));
		}
	}
}

void g_layer_set_desired_output(GLayer *layer, gdouble *desired){
	g_return_if_fail(NULL != layer);
	g_return_if_fail(NULL != desired);

	layer->priv = G_TYPE_INSTANCE_GET_PRIVATE (layer,
			TYPE_G_LAYER, GLayerPrivate);
	GLayerPrivate *priv = layer->priv;

	memmove(priv->desired, desired, priv->nneurons*sizeof(gdouble));
}

gint g_layer_get_ninputs(GLayer *layer)
{
	g_return_val_if_fail(NULL != layer, -1);

	layer->priv = G_TYPE_INSTANCE_GET_PRIVATE (layer,
			TYPE_G_LAYER, GLayerPrivate);
	GLayerPrivate *priv = layer->priv;

	return priv->ninputs;
}

gdouble g_layer_get_error_rate(GLayer *layer)
{
	gint i;
	gdouble error_rate = 0.0f ;
	gdouble *error_vector = NULL;

	//error rate fixed between 0 <= r < 1
	g_return_val_if_fail(NULL != layer, -1);

	layer->priv = G_TYPE_INSTANCE_GET_PRIVATE (layer,
			TYPE_G_LAYER, GLayerPrivate);
	GLayerPrivate *priv = layer->priv;

	error_vector = (gdouble *)g_malloc(priv->nneurons*sizeof(gdouble));

	for(i = 0; i < priv->nneurons; ++i)
	{
        priv->error[i]  = priv->desired[i] - priv->outputs[i];

		error_rate += 0.5*pow(priv->error[i], 2);
	}

	return error_rate;
}

gdouble* g_layer_get_error_vector(GLayer *layer)
{
    g_return_val_if_fail(NULL != layer, NULL);

    layer->priv = G_TYPE_INSTANCE_GET_PRIVATE (layer,
			TYPE_G_LAYER, GLayerPrivate);
	GLayerPrivate *priv = layer->priv;

    return priv->error;
}

void g_layer_update_layer_weights(GLayer *layer,
                                gdouble learning_rate,
                                gdouble inertial,
                                gint next_nneurons)
{
    gint i,j;
    gdouble di = 0.0f, som = 0.0f;

    g_return_if_fail(NULL != layer);

    layer->priv = G_TYPE_INSTANCE_GET_PRIVATE (layer,
			TYPE_G_LAYER, GLayerPrivate);
	GLayerPrivate *priv = layer->priv;

	//we will modify weights of all hidden layers and of the output layer
	//we should not modify the input layer
	if(priv->id != 0)
	{
		if( next_nneurons <= 0)
		{
			//for each neuron of the hidden layer
			for(i = 0; i < priv->nneurons; ++i)
			{
				di = priv->outputs[i] * (1.0f - priv->outputs[i])*priv->error[i];

				g_neuron_update_weights(priv->neurons[i], di, learning_rate, inertial);
			}
		}
		else
		{
			//for each neuron of the hidden layer get the correction using the impact
			//of this neuron on all child neurons
			for(i = 0; i < priv->nneurons; ++i)
			{
				//get the impart of the neuron on the error in the next layer
				som = 0.0f;

				for(j = 0; j< next_nneurons; ++j)
				{
					som += g_neuron_get_error_weighted(priv->neurons[i], i);
				}

				di = priv->outputs[i] * (1.0f - priv->outputs[i])*som;

				g_neuron_update_weights(priv->neurons[i], di, learning_rate, inertial);
			}
		}
	}
}

GNeuron* g_layer_get_nth_neuron(GLayer* layer, gint i)
{
    g_return_val_if_fail(NULL != layer, NULL);

    layer->priv = G_TYPE_INSTANCE_GET_PRIVATE (layer,
			TYPE_G_LAYER, GLayerPrivate);
	GLayerPrivate *priv = layer->priv;

    g_return_val_if_fail(i >= 0 && i < priv->nneurons, NULL);

    return priv->neurons[i];
}

void g_layer_set_neuron_weights(GLayer *layer, GList *weights)
{
	gint i;

	g_return_if_fail(NULL != layer);
	g_return_if_fail(NULL != weights);

    layer->priv = G_TYPE_INSTANCE_GET_PRIVATE (layer,
			TYPE_G_LAYER, GLayerPrivate);
	GLayerPrivate *priv = layer->priv;

	g_return_if_fail(g_list_length(weights) == priv->nneurons);

	for (i = 0;i < priv->nneurons; ++i)
	{
		gdouble *weight = (gdouble *)g_list_nth_data(weights, i);

		g_neuron_set_weights(priv->neurons[i], weight);
	}
}

void g_layer_reset_weights(GLayer *layer)
{
    gint i;
    g_return_if_fail(NULL != layer);

    layer->priv = G_TYPE_INSTANCE_GET_PRIVATE (layer,
			TYPE_G_LAYER, GLayerPrivate);
	GLayerPrivate *priv = layer->priv;

    if(priv->id != 0)
    {
        for(i=0; i<priv->nneurons; ++i)
        {
            g_neuron_set_random_weight(priv->neurons[i]);
        }
    }
}
