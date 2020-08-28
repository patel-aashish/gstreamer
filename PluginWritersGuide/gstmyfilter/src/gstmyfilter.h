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

#ifndef _GST_MYFILTER_H_
#define _GST_MYFILTER_H_

#include <gst/gst.h>

G_BEGIN_DECLS

#define GST_TYPE_MYFILTER (gst_my_filter_get_type())
G_DECLARE_FINAL_TYPE (GstMyFilter, gst_my_filter, GST, MYFILTER, GstElement)

struct _GstMyFilter
{
  GstElement element;
  GstPad *sinkpad;
  GstPad *srcpad;
  gboolean silent;
};

G_END_DECLS
#endif // !_GST_MYFILTER_H_
