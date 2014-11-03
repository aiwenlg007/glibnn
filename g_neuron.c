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

#include "g_neuron.h"
#include <string.h>
#include <math.h>

enum
{
	PROP_0,
};

G_DEFINE_TYPE (GNeuron, g_neuron, G_TYPE_OBJECT);

struct _GNeuronPrivate
{
	gdouble *weights;//an array on input
	gdouble *in;//an array of output
	gdouble out;//only one output
	gint 	ninput;
	gint 	layer_id;//if the neuron is in a layer
    gdouble activation;
    gdouble *old_corrections;
    gdouble di;
};

static void g_neuron_dispose(GObject *object)
{
	G_OBJECT_CLASS (g_neuron_parent_class)->dispose (object);
}

static void g_neuron_finalize(GObject *object)
{
	GNeuron *neuron= G_NEURON(object);

	neuron->priv = G_TYPE_INSTANCE_GET_PRIVATE (neuron,
		TYPE_G_NEURON, GNeuronPrivate);
	GNeuronPrivate *priv = neuron->priv;

	g_free(priv->in);
	g_free(priv->weights);
    g_free(priv->old_corrections);

	G_OBJECT_CLASS (g_neuron_parent_class)->finalize (object);
}

static void g_neuron_set_property (GObject      	*gobject,
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

static void g_neuron_get_property (GObject    	*gobject,
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

static void g_neuron_class_init (GNeuronClass *klass)
{
	g_type_class_add_private (klass, sizeof (GNeuronPrivate));

	GObjectClass *gklass = G_OBJECT_CLASS(klass);
	gklass->get_property = g_neuron_get_property;
	gklass->set_property = g_neuron_set_property;
	gklass->dispose      = g_neuron_dispose;
	gklass->finalize     = g_neuron_finalize;
}

static void g_neuron_init (GNeuron *neuron){
	g_return_if_fail(NULL != neuron);

	neuron->priv = G_TYPE_INSTANCE_GET_PRIVATE (neuron,
			TYPE_G_NEURON, GNeuronPrivate);
	GNeuronPrivate *priv = neuron->priv;

	priv->layer_id 	      = 0;//by default
	priv->in 		      = NULL;
	priv->out 		      = 0.0;
	priv->weights 	      = NULL;
	priv->ninput	      = 0;
    priv->activation      = 0.0f;
    priv->old_corrections = NULL;
    priv->di              = 0;
}

static void g_neuron_set_random_weight(GNeuron *neuron)
{
	GRand* rand;
	gint i;

	g_return_if_fail(NULL != neuron);

	neuron->priv = G_TYPE_INSTANCE_GET_PRIVATE (neuron,
			TYPE_G_NEURON, GNeuronPrivate);
	GNeuronPrivate *priv = neuron->priv;

	rand = g_rand_new();

	for(i = 0; i < priv->ninput; ++i)
	{
		priv->weights[i] = g_rand_double_range(rand, -0.5f, 0.5f);
	}
}

gdouble g_neuron_compute_accumulation(GNeuron *neuron)
{
	gint i;
	gdouble activation = 0.0f;

	neuron->priv = G_TYPE_INSTANCE_GET_PRIVATE (neuron,
			TYPE_G_NEURON, GNeuronPrivate);
	GNeuronPrivate *priv = neuron->priv;

	//make the scalar product between entries and weight

	for(i = 0; i < priv->ninput; ++i)
	{
		activation += priv->weights[i] * priv->in[i];
	}

	return activation;
}

void g_neuron_compute_activation(GNeuron *neuron)
{
	g_return_val_if_fail(NULL != neuron, -1.0f);

    neuron->priv = G_TYPE_INSTANCE_GET_PRIVATE (neuron,
			TYPE_G_NEURON, GNeuronPrivate);
	GNeuronPrivate *priv = neuron->priv;

	gdouble acc = g_neuron_compute_accumulation(neuron);

	priv->activation = 1.0f / ( 1.0f + exp(acc) );
}

gdouble g_neuron_get_activation(GNeuron *neuron)
{
  	g_return_val_if_fail(NULL != neuron, -1.0f);

    neuron->priv = G_TYPE_INSTANCE_GET_PRIVATE (neuron,
			TYPE_G_NEURON, GNeuronPrivate);
	GNeuronPrivate *priv = neuron->priv;

    return priv->activation;
}

void g_neuron_display(GNeuron *neuron)
{
	gint i;

	g_return_if_fail(NULL != neuron);

	neuron->priv = G_TYPE_INSTANCE_GET_PRIVATE (neuron,
			TYPE_G_NEURON, GNeuronPrivate);
	GNeuronPrivate *priv = neuron->priv;

	for(i = 0; i < priv->ninput; ++i)
	{
		g_print("%.5f | %.5f\n",priv->in[i], priv->weights[i]);
	}
}

GNeuron* g_neuron_new(gint id, gint ninput)
{
	g_return_val_if_fail(0 != ninput, NULL);

	GNeuron *neuron = (GNeuron *)g_object_new(TYPE_G_NEURON, NULL);

	neuron->priv = G_TYPE_INSTANCE_GET_PRIVATE (neuron,
			TYPE_G_NEURON, GNeuronPrivate);
	GNeuronPrivate *priv = neuron->priv;

	//create an array of input of size ninput+1
	priv->in 		      = (gdouble *)g_malloc0((ninput + 1)*sizeof(gdouble));
	priv->weights 	      = (gdouble *)g_malloc0((ninput + 1)*sizeof(gdouble));
    priv->old_corrections = (gdouble *)g_malloc0((ninput + 1)*sizeof(gdouble));
	priv->ninput 	      = ninput + 1;

	//generate random weights at the neuron creation
	g_neuron_set_random_weight(neuron);

	return neuron;
}

void g_neuron_set_inputs(GNeuron* neuron, gdouble *in)
{
	g_return_if_fail(NULL != neuron);
	g_return_if_fail(NULL != in);

	neuron->priv = G_TYPE_INSTANCE_GET_PRIVATE (neuron,
			TYPE_G_NEURON, GNeuronPrivate);
	GNeuronPrivate *priv = neuron->priv;

	memmove(priv->in, in, (priv->ninput-1)*sizeof(gdouble));
	priv->in[priv->ninput-1] = 1.0f;
}

void g_neuron_update_weights(GNeuron *neuron, gdouble di,gdouble learning_rate, gdouble inertial)
{
    gint j;
    gdouble correction;

    g_return_if_fail(NULL != neuron);

	neuron->priv = G_TYPE_INSTANCE_GET_PRIVATE (neuron,
			TYPE_G_NEURON, GNeuronPrivate);
	GNeuronPrivate *priv = neuron->priv;

    priv->di = di;

    //for all entries of the neuron
    for(j = 0; j < priv->ninput; ++j)
    {
        correction = -learning_rate*di*priv->in[j] + inertial * priv->old_corrections[j];

        priv->weights[j] += correction;

        priv->old_corrections[j] = correction;
    }
}

gdouble g_neuron_get_error_weighted(GNeuron* neuron, gint i)
{
    g_return_val_if_fail(NULL != neuron, 0.0f);

    neuron->priv = G_TYPE_INSTANCE_GET_PRIVATE (neuron,
			TYPE_G_NEURON, GNeuronPrivate);
	GNeuronPrivate *priv = neuron->priv;

    g_return_val_if_fail(i <= priv->ninput, 0.0f);

    //return the contribution of the ième previous layer'sneurons
    return priv->di * priv->weights[i];
}
