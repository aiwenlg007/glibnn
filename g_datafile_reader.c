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

#include "g_datafile_reader.h"
#include <string.h>
#include <math.h>
#include <gio/gio.h>
#include <stdio.h>

enum
{
	PROP_0,
};

G_DEFINE_TYPE (GDatafileReader, g_datafile_reader, G_TYPE_OBJECT);

struct _GDatafileReaderPrivate
{
	GList *datas;
	//size of all inputs
	gint  ninput;
};

static void g_datafile_reader_dispose(GObject *object)
{
	G_OBJECT_CLASS (g_datafile_reader_parent_class)->dispose (object);
}

static void g_datafile_reader_finalize(GObject *object)
{
	GDatafileReader *datafile_reader= G_DATAFILE_READER(object);
	gint i;

	datafile_reader->priv = G_TYPE_INSTANCE_GET_PRIVATE (datafile_reader,
		TYPE_G_DATAFILE_READER, GDatafileReaderPrivate);
	GDatafileReaderPrivate *priv = datafile_reader->priv;

	g_list_free(priv->datas);

	G_OBJECT_CLASS (g_datafile_reader_parent_class)->finalize (object);
}

static void g_datafile_reader_set_property (GObject      	*gobject,
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

static void g_datafile_reader_get_property (GObject    	*gobject,
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

static void g_datafile_reader_class_init (GDatafileReaderClass *klass)
{
	g_type_class_add_private (klass, sizeof (GDatafileReaderPrivate));

	GObjectClass *gklass = G_OBJECT_CLASS(klass);
	gklass->get_property = g_datafile_reader_get_property;
	gklass->set_property = g_datafile_reader_set_property;
	gklass->dispose      = g_datafile_reader_dispose;
	gklass->finalize     = g_datafile_reader_finalize;
}

static void g_datafile_reader_init (GDatafileReader *datafile_reader){
	g_return_if_fail(NULL != datafile_reader);

	datafile_reader->priv = G_TYPE_INSTANCE_GET_PRIVATE (datafile_reader,
			TYPE_G_DATAFILE_READER, GDatafileReaderPrivate);
	GDatafileReaderPrivate *priv = datafile_reader->priv;

	priv->ninput = 0;
}

GDatafileReader* g_datafile_reader_new(gchar *path)
{
	gchar *line 				= NULL;
	GFile *file 				= NULL;
	GCancellable *cancellable 	= NULL;
	GFileInputStream *stream 	= NULL;
	GDataInputStream *dataStream= NULL;
	gsize size;
	gint idx = 0, count = 0;
	double res;

	g_return_val_if_fail(NULL != path, 	NULL);
	g_return_val_if_fail(g_file_test(path, G_FILE_TEST_EXISTS), NULL);

	GDatafileReader *datafile_reader 	= (GDatafileReader *)g_object_new(TYPE_G_DATAFILE_READER, NULL);

	datafile_reader->priv = G_TYPE_INSTANCE_GET_PRIVATE (datafile_reader,
			TYPE_G_DATAFILE_READER, GDatafileReaderPrivate);
	GDatafileReaderPrivate *priv = datafile_reader->priv;

	//first get a File
	file = g_file_new_for_path(path);

	//then a FileStream
	cancellable = g_cancellable_new();
	stream 		= g_file_read(file, cancellable, NULL );
	dataStream 	= g_data_input_stream_new(G_INPUT_STREAM(stream));

	while( (line = g_data_input_stream_read_line(dataStream, &size, cancellable, NULL)) != NULL)
	{
		if( !g_strcmp0(line, "") == FALSE && !g_strcmp0(line, "\n") == FALSE)
		{
			gchar** list = g_strsplit_set(line, "=; \n", -1);
			idx  = 0;

			//take into account the special case ""
			while( list != NULL && *list != NULL)
			{
				if(sscanf(*list, "%lf", &res) > 0) idx++;
				count ++;
				list ++;
			}
			//idx++;

			if(priv->ninput == 0)
			{
				priv->ninput = idx;
			}
			else
			{
				g_return_val_if_fail(idx == priv->ninput, NULL);
			}

			//construct a gdouble vector
			gdouble* vector = (gdouble *)g_malloc0(idx*sizeof(gdouble));

			//rembobine
			while( count != 0 )
			{
				list--;
				count--;
			}
			idx = 0;

			while( list != NULL && *list != NULL)
			{
				if(sscanf(*list, "%lf",&res) > 0)
				{
					vector[idx] = res;
					idx++;
				}

				list++;
			}

			priv->datas = g_list_append(priv->datas, vector);
		}
	}

	return datafile_reader;
}

gint g_datafile_reader_get_ninput(GDatafileReader *datafile_reader)
{
	g_return_val_if_fail(NULL != datafile_reader, -1);

	datafile_reader->priv = G_TYPE_INSTANCE_GET_PRIVATE (datafile_reader,
			TYPE_G_DATAFILE_READER, GDatafileReaderPrivate);
	GDatafileReaderPrivate *priv = datafile_reader->priv;

	return priv->ninput;
}

GList* g_datafile_reader_get_data(GDatafileReader *datafile_reader)
{
	g_return_val_if_fail(NULL != datafile_reader, NULL);

	datafile_reader->priv = G_TYPE_INSTANCE_GET_PRIVATE (datafile_reader,
			TYPE_G_DATAFILE_READER, GDatafileReaderPrivate);
	GDatafileReaderPrivate *priv = datafile_reader->priv;

	return priv->datas;
}
