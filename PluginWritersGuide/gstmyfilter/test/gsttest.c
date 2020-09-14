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

static boolean
bus_call (GstBus *bus,
          GstMessage *msg,
          gpointer data)
{
  GMainLoop *loop = data;

  switch (GST_MESSAGE_TYPE (msg)) {
    case GST_MESSAGE_EOS:
      g_print ("End-of-stream\n");
      g_main_loop_quit (loop);
      break;
    case GST_MESSAGE_ERROR: {
      gchar *debug = NULL;
      GError *err = NULL;

      gst_message_parse_error (msg, &err, &debug);

      g_print ("Error: %s \n", err->message);
      g_error_free (err);

      if (debug) {
        g_print ("Debug details: %s\n", debug);
        g_free (debug);
      }

      g_main_loop_quit (loop);
      break;
    }
    default:
      break;
  }

  return TRUE;
}

gint
main (gint argc,
      gchar *argv[])
{
  GMainLoop *loop;
  GstElement *pipeline, *filesrc, *decoder, *filter, *sink;
  GstElement *convert1, *convert2, *resample;
  GstBus *bus;
  guint watch_id;
  GstStateChangeReturn ret;

  /* Initialization */
  gst_init (&argc, &argv);
  loop = g_main_loop_new (NULL, FALSE);
  if (argc != 2) {
    g_print ("Usage: %s <mp3 filename>\n", argv[0]);
    return -1;
  }

  /* Create elements */
  pipeline = gst_pipeline_new ("my_pipeline");

  /* Watch for messages on the pipeline bus. This only works when 
   * the GLib main loop is running */
  bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
  watch_id = gst_bus_add_watch (bus, bus_call, loop);
  gst_object_unref (bus);

  filesrc = gst_element_factory_make ("filesrc", "my_filesource");
  decoder = gst_element_factory_make ("mad", "my_decoder");

  /* audio convertor to convert samples into format that myfilter can handle
   * currently it supports everything */
  convert1 = gst_element_factory_make ("audioconvert", "audioconvert1");

  filter = gst_element_factory_make ("my_filter", "my_filter");

  /* there should always be audio resample and audio convert elements before *
   * audiosink as the capability of sinks always varies based on the environment */
  convert2 = gst_elemessnt_factory_make ("audioconvert", "audioconvert2");
  resample = gst_element_factory_make ("audioresample", "audioresample");
  sink = gst_element_factory_make ("pulsesink", "audiosink");

  if (!sink || !decoder) {
    g_print ("Decoder or output could not be found, check your installation\n");
    return -1;
  } else if (!convert1 || !convert2 || !resample) {
    g_print ("Could not create audioconvert or audioresample element, "
             "check your installation\n");
    return -1;
  } else if (!filter) {
    g_print ("Your filter could not be used. Make sure it "
             "is installed correctly in $(libdir)/gstreamer-1.0/ or"
             "~/gstreamer-1.0/plugins/ and that gst-inspect-1.0 lists it."
             "If it does not check with 'GST_DEBUG=*:2 gst-inspect-1.0' for"
             " the reason why it is not loaded");
    return -1;
  }

  g_object_set (G_OBJECT(filesrc), "location", argv[1], NULL);

  gst_bin_add_many (GST_BIN (pipeline), filesrc, decoder, convert1, filter,
                    convert2, resample, sink, NULL);
  
  /* Link everything together */
  if (!gst_element_link_many (filesrc, decoder, convert1, filter, convert2,
                              resample, sink, NULL) {
    g_print ("Failed to link one or more elements \n");
    return -1;
  }

  /* run */
  ret = gst_element_set_state (pipeline, GST_STATE_PLAYING);
  if (ret == GST_STATE_CHANGE_FAILURE) {
    GstMessage *msg;

    g_print ("Failed to start up pipeline\n");

    /* Check if there is a error messages with details on the bus */
    msg = gst_bus_poll (bus, GST_MESSAGE_ERROR, 0);
    if (msg) {
      GError *err = NULL;
      gst_message_parse_error (msg, &err, NULL);
      g_print ("ERROR: %s \n", err->message);
      g_error_free (err);
      gst_message_unref (msg);
    }
    return -1;
  }

  g_main_loop_run (loop);

  /* clean up */
  gst_element_set_state (pipeline, GST_STATE_NULL);
  gst_object_unref (pipeline);
  g_source_remove (watch_id);
  g_main_loop_unref (loop);

  return 0;
}