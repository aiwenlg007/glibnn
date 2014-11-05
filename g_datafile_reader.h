#ifndef __G_DATAFILE_READER_H__
#define __G_DATAFILE_READER_H__

#include <glib.h>
#include <glib-object.h>


G_BEGIN_DECLS

#define TYPE_G_DATAFILE_READER                  (g_datafile_reader_get_type ())
#define G_DATAFILE_READER(obj)                   (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_G_DATAFILE_READER, GDatafileReader))
#define IS_G_DATAFILE_READER(obj)                (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_G_DATAFILE_READER))
#define G_DATAFILE_READER_CLASS(klass)           (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_G_DATAFILE_READER, GDatafileReaderClass))
#define IS_G_DATAFILE_READER_CLASS(klass)        (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_G_DATAFILE_READER))
#define G_DATAFILE_READER_GET_CLASS(obj)         (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_G_DATAFILE_READER, GDatafileReaderClass))

typedef struct _GDatafileReader                 GDatafileReader;
typedef struct _GDatafileReaderPrivate          GDatafileReaderPrivate;
typedef struct _GDatafileReaderClass            GDatafileReaderClass;

struct _GDatafileReader
{
  /*< private >*/
  GInitiallyUnowned parent_instance;

  GDatafileReaderPrivate *priv;
};

struct _GDatafileReaderClass
{
  /*< private >*/
  GInitiallyUnownedClass parent_class;

  /*public funcs and data here*/
};


GType g_datafile_reader_get_type (void) G_GNUC_CONST;

GDatafileReader* g_datafile_reader_new(gchar *path);
gint g_datafile_reader_get_ninput(GDatafileReader *datafile_reader);
GList* g_datafile_reader_get_data(GDatafileReader *datafile_reader);
G_END_DECLS
#endif
