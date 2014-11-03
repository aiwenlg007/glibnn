#ifndef __G_NEURON_H__
#define __G_NEURON_H__

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define TYPE_G_NEURON                  (g_neuron_get_type ())
#define G_NEURON(obj)                   (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_G_NEURON, GNeuron))
#define IS_G_NEURON(obj)                (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_G_NEURON))
#define G_NEURON_CLASS(klass)           (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_G_NEURON, GNeuronClass))
#define IS_G_NEURON_CLASS(klass)        (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_G_NEURON))
#define G_NEURON_GET_CLASS(obj)         (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_G_NEURON, GNeuronClass))

typedef struct _GNeuron                 GNeuron;
typedef struct _GNeuronPrivate          GNeuronPrivate;
typedef struct _GNeuronClass            GNeuronClass;

struct _GNeuron
{
  /*< private >*/
  GInitiallyUnowned parent_instance;

  GNeuronPrivate *priv;
};

struct _GNeuronClass
{
  /*< private >*/
  GInitiallyUnownedClass parent_class;

  /*public funcs and data here*/
};


GType g_neuron_get_type (void) G_GNUC_CONST;

GNeuron* g_neuron_new(gint id, gint ninput);
void g_neuron_set_inputs(GNeuron* neuron, gdouble *in);
gdouble g_neuron_compute_accumulation(GNeuron *neuron);
void g_neuron_compute_activation(GNeuron *neuron);
gdouble g_neuron_get_activation(GNeuron *neuron);
void g_neuron_display(GNeuron *neuron);
void g_neuron_update_weight(GNeuron *neuron, gdouble di,gdouble learning_rate, gdouble inertial);
gdouble g_neuron_get_error_weighted(GNeuron* neuron, gint i);
G_END_DECLS
#endif
