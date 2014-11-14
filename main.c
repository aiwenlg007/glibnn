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
#include "g_datafile_reader.h"
#include <math.h>

int main(int argc, char **argv)
{
	/*
	gint i,j;
	gdouble in[2][4] = {{1.0f, 0.0f, 0.0f, 0.0f}, 
						{0.0f, 0.0f, 1.0f, 0.0f}};
	gdouble out[2][2] = {{1.0f, 0.0f}, {0.0f, 1.0f}};
    GList *layers_list = NULL;
    layers_list = g_list_append(layers_list, GINT_TO_POINTER(4));
    layers_list = g_list_append(layers_list, GINT_TO_POINTER(3));
    layers_list = g_list_append(layers_list, GINT_TO_POINTER(2));
    GNeuralNetworkProperties ppt = {
        {.learning_rate  = 0.5f,
         .inertial_rate  = 0.5f,
         .error_rate     = 0.01f}
    };
	GNeuralNetwork *neural_network = g_neural_network_new(layers_list, ppt);
	
	g_neural_network_set_input(neural_network, in[0]);
	g_neural_network_set_desired_output(neural_network, out[0]);
	g_neural_network_back_propagation_learning(neural_network);
	
	g_neural_network_set_input(neural_network, in[1]);
	g_neural_network_set_desired_output(neural_network, out[1]);
	g_neural_network_back_propagation_learning(neural_network);
	
    g_neural_network_save(neural_network, "glibnn_test.txt");
    g_object_unref(G_OBJECT(neural_network));
	neural_network = g_neural_network_new_from_file("glibnn_test.txt");
    g_neural_network_save(neural_network, "glibnn_test_1.txt");

		gdouble *o = g_neural_network_get_outputs(neural_network,in[0]);
		for( i = 0; i <2; ++i) g_print("%f ", o[i]);
		g_print("\n");
	
	*/
	//reading inputs and outputs datas
	GDatafileReader *inputs_file = g_datafile_reader_new("input.txt");
    GDatafileReader *outputs_file = g_datafile_reader_new("output.txt");

    gint n_inputs       = g_datafile_reader_get_ninput(inputs_file);
    gint n_outputs      = g_datafile_reader_get_ninput(outputs_file);

    GList *inputs_datas = g_datafile_reader_get_data(inputs_file);
    GList *outputs_datas= g_datafile_reader_get_data(outputs_file);

    gint i, j;
	gdouble *in, *out, norme;

    g_return_val_if_fail(g_list_length(inputs_datas) == g_list_length(outputs_datas), -1);

    g_print("%s\n", "Inputs datas");
    for(i = 0; i < g_list_length(inputs_datas); ++i)
    {
        in = (gdouble *)g_list_nth_data(inputs_datas, i);

		norme = 0.0f;
        for(j = 0; j < n_inputs; ++j ) norme += in[j]*in[j];
       
        norme = pow(norme, 0.5f);
        for(j = 0; j < n_inputs; ++j)
        {
			if(norme != 0.0f) in[j] /= norme;
        	g_print("%f ", in[j]);
        }
        g_print("\n");
    }

    g_print("%s\n", "Outputs datas");
    for(i = 0; i < g_list_length(outputs_datas); ++i)
    {
        out = (gdouble *)g_list_nth_data(outputs_datas, i);

		norme = 0.0f;
        for(j = 0; j < n_outputs; ++j ) norme += out[j]*out[j];
		
		norme = pow(norme, 0.5f);
		
		for(j=0; j< n_outputs; ++j)
		{
			if(norme != 0.0f) out[j] /= norme;
			g_print("%f ", out[j]);
		}
        g_print("\n");
    }

    //create a new neural network
    GList *layers_list = NULL;
    layers_list = g_list_append(layers_list, GINT_TO_POINTER(n_inputs));
    layers_list = g_list_append(layers_list, GINT_TO_POINTER(3));
    layers_list = g_list_append(layers_list, GINT_TO_POINTER(n_outputs));

    //an union to store parameters
    GNeuralNetworkProperties ppt = {
        {.learning_rate  = 0.5f,
         .inertial_rate  = 0.3f,
         .error_rate     = 0.05f}
    };

	GNeuralNetwork *neural_network = g_neural_network_new(layers_list, ppt);

    //now for each couple of vector input[i]/output[i] train the eural network
    g_neural_network_train(neural_network, inputs_datas, outputs_datas);

    //save and destroy the old neural network
    g_neural_network_save(neural_network, "alerte.txt");
    g_object_unref(G_OBJECT(neural_network));

    //rebuild a neural network using a file
	neural_network = g_neural_network_new_from_file("alerte.txt");

	for(j=0;j<g_list_length(inputs_datas);j++)
	{
		gdouble *o = g_neural_network_get_outputs(neural_network, (gdouble *)g_list_nth_data(inputs_datas, j));
		for( i = 0; i <n_outputs; ++i) g_print("%f ", o[i]);
		g_print("\n");
	}
   return 0;
}
