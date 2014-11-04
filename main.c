/*
 * main.c
 *
 * Copyright 2014 Benoit Franquet <benoitfranquet@gmail.com>
 *
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
 *
 */


#include <stdio.h>
#include "g_neural_network.h"
int main(int argc, char **argv)
{
	gdouble in[4] = {1.0f, -0.5f, 0.5f, 0.0f};
	gdouble out[2] = {1.0f, 0.0f};

    GList *layers_list = NULL;
    layers_list = g_list_append(layers_list, GINT_TO_POINTER(2));
    layers_list = g_list_append(layers_list, GINT_TO_POINTER(3));
    layers_list = g_list_append(layers_list, GINT_TO_POINTER(2));

    GNeuralNetworkProperties ppt = {
        {.learning_rate  = 0.9f,
         .inertial_rate  = 0.9f,
         .error_rate     = 0.05f}
    };

	GNeuralNetwork *neural_network = g_neural_network_new(layers_list, ppt, 4);

	g_neural_network_set_input(neural_network, in);
	g_neural_network_set_desired_output(neural_network, out);

	g_neural_network_back_propagation_learning(neural_network);

    g_neural_network_save(neural_network, "glibnn_test.txt");

    g_object_unref(G_OBJECT(neural_network));

	neural_network = g_neural_network_new_from_file("glibnn_test.txt");

    g_neural_network_save(neural_network, "glibnn_test_1.txt");

    return 0;
}
