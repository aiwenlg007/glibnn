#ifndef __G_NEURAL_NETWORK_H__
#define __G_NEURAL_NETWORK_H__

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define TYPE_G_NEURAL_NETWORK                  (g_neural_network_get_type ())
#define G_NEURAL_NETWORK(obj)                   (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_G_NEURAL_NETWORK, GNeuralNetwork))
#define IS_G_NEURAL_NETWORK(obj)                (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_G_NEURAL_NETWORK))
#define G_NEURAL_NETWORK_CLASS(klass)           (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_G_NEURAL_NETWORK, GNeuralNetworkClass))
#define IS_G_NEURAL_NETWORK_CLASS(klass)        (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_G_NEURAL_NETWORK))
#define G_NEURAL_NETWORK_GET_CLASS(obj)         (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_G_NEURAL_NETWORK, GNeuralNetworkClass))

typedef struct _GNeuralNetwork                 GNeuralNetwork;
typedef struct _GNeuralNetworkPrivate          GNeuralNetworkPrivate;
typedef struct _GNeuralNetworkClass            GNeuralNetworkClass;

struct _GNeuralNetwork
{
  /*< private >*/
  GInitiallyUnowned parent_instance;

  GNeuralNetworkPrivate *priv;
};

struct _GNeuralNetworkClass
{
  /*< private >*/
  GInitiallyUnownedClass parent_class;

  /*public funcs and data here*/
};


GType g_neural_network_get_type (void) G_GNUC_CONST;

GNeuralNetwork* g_neural_network_new(const gint num_layers, 
									 const gint* neurons_per_layers,
									 const gdouble learning_rate,
									 const gdouble inertie,
									 const gdouble error_rate,
									 const gint ninputs);

void g_neural_network_back_propagation_learning(GNeuralNetwork *neural_network);

void g_neural_network_set_input(GNeuralNetwork *neural_network,
								gdouble *input);
									
void g_neural_network_set_desired_output(GNeuralNetwork *neural_network,
										gdouble *desired);
#endif
