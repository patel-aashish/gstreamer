/**
 * Copyright (C) 2020 Aashish Patel (aashish.a.pate@gmail.com)
 * 
 * MyFilter is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * MyFilter is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with MyFilter. If not, see <http://www.gnu.org/licenses/>.
 */

#include <gst/gst.h>

#include "gstmyfilter.h"

GST_DEBUG_CATEGORY_STATIC (gst_my_filter_debug)
#define GST_CAT_DEFAULT gst_my_filter_debug

/* Filter signals and arguments */
enum
{
  LAST_SIGNAL
};

enum
{
  PROP_0,
  PROP_SILENT,
  PROP_LAST
};

/* the pad caps */
static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("ANY")
    );

static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("ANY")
    );

#define gst_my_filter_parent_class parent_class
G_DEFINE_TYPE (GstMyFilter, gst_my_filter, GST_TYPE_ELEMENT);

static void
gst_my_filter_set_property (GObject *object, guint prop_id,
    const GValue *value, GParamSpec *pspec);
static void
gst_my_filter_get_property (GObject *object, guint prop_id,
    const GValue *value, GParamSpec *pspec);
/* GObject vmethod implementations */

/* initialize the myfilter's class */
static void
gst_my_filter_class_init (GstMyFilterClass *klass)
{
  GObjectClass *gobject_class;
  GstElementClass *gstelement_class;

  gobject_class = (GstObjectClass *) klass;
  gstelement_class = (GstElementClass *) klass;

  gobject_class->set_property = gst_my_filter_set_property;
  gobject_class->get_property = gst_my_filter_get_property;

  g_object_class_install_property (gobject_class, PROP_SILENT,
      g_param_spec_boolean ("silent", "Silent","Produce verbose output",
          FALSE, G_PARAM_READWRITE));

  gst_element_class_set_details_simple (gstelement_class,
      "MyFilter",
      "Generic",
      "This filter prints the buffer information",
      "Aashish Patel <aashish.a.patel@gmail.com>")
  
  gst_element_class_add_pad_template (gstelement_class,
      gst_static_pad_template_get (&src_factory));
  gst_element_class_add_pad_template (gstelement_class,
      gst_static_pad_template_get (&sink_factory));
}

/* Initialize the new element
 * instantiate pads and add them to element
 * set pad callbacks functions
 * initialize instance structure
 */
static void
gst_my_filter_init (GstMyFilter * filter)
{
  filter->sinkpad = gst_pad_new_from_static_template (&sink_factory, "sink");
  gst_pad_set_event_function (filter->sinkpad,
                              GST_DEBUG_FUNCPTR(gst_my_filter_sink_event));
  gst_pad_set_chain_function (filter->sinkpad,
                              GST_DEBUG_FUNCPTR(gst_my_filter_chain));
  gst_pad_set_query_function (filter->sinkpad,
                              GST_DEBUG_FUNCPTR(gst_my_filter_sink_query));
  gst_element_add_pad (GST_ELEMENT (filter), filter->sinkpad);sss

  filter->srcpad = gst_pad_new_from_static_template (&src_factory, "src");
  gst_element_add_pad (GST_ELEMENT (filter), filter->srcpad);

  filter->silent = FALSE;
}

static void
gst_my_filter_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec *pspec)
{
  GstMyFilter *filter = GST_MYFILTER (object);

  switch (prop_id) {
    case PROP_SILENT:
      filter->silent = g_value_get_boolean (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_my_filter_get_property (GObject * object, guint prop_id,
    GValue *value, GParamSpec *pspec)
{
  GstMyFilter *filter = GST_MYFILTER (object);

  switch (prop_id) {
    case PROP_SILENT:
      g_value_set_boolean (value, filter->silent);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

/* GstElement vmethod implementation */
/* This function handles sink events */
static gboolean
gst_my_filter_sink_event (GstPad *pad, GstObject *parent, GstEvent *event)
{
  GstMyFilter *filter;
  gboolean ret;

  filter = GST_MYFILTER(parent);

  GST_LOG_OBJECT (filter, "Received %s event: %" GST_PTR_FORMAT,
      GST_EVENT_TYPE_NAME (event), event);

  switch (GST_EVENT_TYPE(event)) {
    case GST_EVENT_CAPS:
    {
      GstCaps *caps;

      gst_event_parse_caps (event, &caps);

      /* we can process something based on caps here */
      ret = gst_pad_event_default (pad, parent, event);
      break;
    }
    default:
      ret = gst_pad_event_default (pad, parent, event);
      break;
  }
  return ret;
}

/* Sink query functions
 * Here we handle different types of query that concern us
 */
static gboolean
gst_my_filter_sink_query (GstPad *pad, GstObject *parent, GstQuery *query)
{
  gboolean ret;
  GstMyFilter *filter;

  filter = GST_MYFILTER (parent);
  
  GST_DEBUG_OBJECT(filter, "query");

  switch (GST_QUERY_TYPE(query)) {
    case GST_QUERY_POSITION:
      /* We should report the current position */
      ret = gst_pad_query_default (pad, parent, query);
      break;
    case GST_QUERY_DURATION:
      /* We should report the duration */
      ret = gst_pad_query_default (pad, parent, query);
      break;
    case GST_QUERY_CAPS:
      /* we should report the supported caps here */
      ret = gst_pad_query_default (pad, parent, query);
      break;
    default:
      ret = gst_pad_query_default(pad, parent, query);
      break;
  }
  return ret;
}

/* chain function
 * this function does the actual processing for this element
 */
void GstFlowReturn
gst_my_filter_chain (GstPad *pad, GstObject *parent, GstBuffer *buf)
{
  GstMyFilter *filter;

  filter = GST_MYFILTER (parent);
  
  GST_DEBUG_OBJECT(filter, "chain");

  if (!filter->silent) {
      GST_INFO_OBJECT(filter, " PTS: %" G_GUINT64_FORMAT " DTS: %" 
          G_GUINT64_FORMAT " size % " G_SIZE_FORMAT " bytes !\n ",
              buf->pts, buf->dts, gst_buffer_get_size(buf));
  }

  /* Just push out the incoming buffer without touching it */
  return gst_pad_push (filter->srcpad, buf);
}


/* entry point to the plug-in, called when the plugin is loaded
 * initializes the plugin itself, register the element factories
 * other features
 */
static boolean
myfilter_init GstPlugin *myfilter)
{
  /* Debug catergory for filtering log messages */
  GST_DEBUG_CATEGORY_INIT (gst_my_filter_debug, "myfilter", 0,
      "This filter prints the buffer information");

  return gst_element_register (myfilter, "myfilter", GST_RANK_NONE,
      GST_TYPE_MYFILTER)
}

/* This structure is used by the gstreamer to register the plugin */
GST_PLUGIN_DEFINE (
  GST_VERSION_MAJOR,
  GST_VERSION_MINOR,
  myfilter,
  "This filter prints the buffer information",
  myfilter_init,
  PACKAGE_VERSION,
  GST_LICENSE,
  GST_PACKAGE_NAME,
  GST_PACKAGE_ORIGINsss
)