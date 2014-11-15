#ifndef __G_LAYER_H__
#define __G_LAYER_H__

#include <glib.h>
#include <glib-object.h>
#include "g_neuron.h"


G_BEGIN_DECLS

#define TYPE_G_LAYER                  (g_layer_get_type ())
#define G_LAYER(obj)                   (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_G_LAYER, GLayer))
#define IS_G_LAYER(obj)                (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_G_LAYER))
#define G_LAYER_CLASS(klass)           (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_G_LAYER, GLayerClass))
#define IS_G_LAYER_CLASS(klass)        (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_G_LAYER))
#define G_LAYER_GET_CLASS(obj)         (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_G_LAYER, GLayerClass))

typedef struct _GLayer                 GLayer;
typedef struct _GLayerPrivate          GLayerPrivate;
typedef struct _GLayerClass            GLayerClass;

struct _GLayer
{
  /*< private >*/
  GInitiallyUnowned parent_instance;

  GLayerPrivate *priv;
};

struct _GLayerClass
{
  /*< private >*/
  GInitiallyUnownedClass parent_class;

  /*public funcs and data here*/
};


GType g_layer_get_type (void) G_GNUC_CONST;

GLayer* g_layer_new(gint id, gint ninput, gint nneurons);
void g_layer_set_inputs(GLayer *layer, gdouble *in);
gdouble* g_layer_get_outputs(GLayer *layer);
void g_layer_compute_outputs(GLayer *layer);
void g_layer_display(GLayer *layer);
void g_layer_set_desired_output(GLayer *layer, gdouble *desired);
gint g_layer_get_ninputs(GLayer *layer);
gdouble g_layer_get_error_rate(GLayer *layer);
gdouble* g_layer_get_error_vector(GLayer *layer);
void g_layer_update_layer_weights(GLayer *layer, gdouble learning_rate, gdouble inertial, gint next_nneurons);
gint g_layer_get_nneurons(GLayer *layer);
gint g_layer_get_id(GLayer *layer);
GNeuron* g_layer_get_nth_neuron(GLayer* layer, gint i);
void g_layer_set_neuron_weights(GLayer *layer, GList *weights);
void g_layer_reset_weights(GLayer *layer);
G_END_DECLS
#endif
