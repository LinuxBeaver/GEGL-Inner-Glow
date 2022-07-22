/* This file is an image processing operation for GEGL
 *
 * GEGL is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * GEGL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEGL; if not, see <https://www.gnu.org/licenses/>.
 *
 * Copyright 2006 Øyvind Kolås <pippin@gimp.org>
 * 2022 Beaver (GEGL inner glow)
 */

#include "config.h"
#include <glib/gi18n-lib.h>

#ifdef GEGL_PROPERTIES

property_double (x, _("X"), 1.0)
  description   (_("Horizontal shadow offset"))
  ui_range      (-2.0, 2.0)
  value_range   (-2.0, 2.0)
  ui_steps      (1, 2)
  ui_meta       ("unit", "pixel-distance")
  ui_meta       ("axis", "x")

property_double (y, _("Y"), 1.0)
  description   (_("Vertical shadow offset"))
  ui_range      (-2.0, 2.0)
  value_range   (-2.0, 2.0)
  ui_steps      (1, 104)
  ui_meta       ("unit", "pixel-distance")
  ui_meta       ("axis", "y")



property_double (radius, _("Blur radius"), 10.7)
  value_range   (0.0, 30.0)
  ui_range      (0.0, 30.0)
  ui_steps      (1, 5)
  ui_gamma      (1.5)
  ui_meta       ("unit", "pixel-distance")


property_double (grow_radius, _("Grow radius"), 8.0)
  value_range   (2, 30.0)
  ui_range      (2, 30.0)
  ui_digits     (0)
  ui_steps      (1, 5)
  ui_gamma      (1.5)
  ui_meta       ("unit", "pixel-distance")
  description (_("The distance to expand the shadow before blurring; a negative value will contract the shadow instead"))

property_double (opacity, _("Opacity"), 1.4)
  value_range   (0.0, 2.0)
  ui_steps      (0.01, 0.10)


property_color  (notouch, _("Don't touch. Needed for filter to work"), "#")
    ui_meta     ("role", "output-extent")


property_color (value2, _("Color"), "#fbff00")
    description (_("The color to paint over the input"))
    ui_meta     ("role", "color-primary")

property_int  (mradius, _("Radius"), 1)
  value_range (-400, 400)
  ui_range    (0, 100)
  ui_meta     ("unit", "pixel-distance")
  description (_("Neighborhood radius, a negative value will calculate with inverted percentiles"))
    ui_meta     ("role", "output-extent")


property_double  (fixoutline, _("Median to fix unwanted pixel outline"), 50)
  value_range (50, 100)
  description (_("Neighborhood alpha percentile"))





#else

#define GEGL_OP_META
#define GEGL_OP_NAME     inner_glow
#define GEGL_OP_C_SOURCE inner-glow.c

#include "gegl-op.h"

static void attach (GeglOperation *operation)
{
  GeglNode *gegl = operation->node;
  GeglNode *input, *it, *shadow, *c2a, *white, *color, *nop, *color2, *eblack, *atop, *median, *median2, *output;

  input    = gegl_node_get_input_proxy (gegl, "input");
  output   = gegl_node_get_output_proxy (gegl, "output");



  it    = gegl_node_new_child (gegl,
                                  "operation", "gegl:it",
                                  NULL);

  shadow    = gegl_node_new_child (gegl,
                                  "operation", "gegl:dropshadow",
                                  NULL);

  color    = gegl_node_new_child (gegl,
                                  "operation", "gegl:color-overlay",
                                  NULL);

  color2    = gegl_node_new_child (gegl,
                                  "operation", "gegl:color-overlay",
                                  NULL);



  atop    = gegl_node_new_child (gegl,
                                  "operation", "gegl:src-atop",
                                  NULL);

  white    = gegl_node_new_child (gegl,
                                  "operation", "gegl:zzwhite",
                                  NULL);

  nop    = gegl_node_new_child (gegl,
                                  "operation", "gegl:nop",
                                  NULL);

  eblack    = gegl_node_new_child (gegl,
                                  "operation", "gegl:zzeraseblack",
                                  NULL);

  eblack    = gegl_node_new_child (gegl,
                                  "operation", "gegl:zzeraseblack",
                                  NULL);

  median    = gegl_node_new_child (gegl,
                                  "operation", "gegl:median-blur",
                                  NULL);

  median2    = gegl_node_new_child (gegl,
                                  "operation", "gegl:median-blur",
                                  NULL);




gegl_operation_meta_redirect (operation, "grow_radius", shadow, "grow-radius");

gegl_operation_meta_redirect (operation, "radius", shadow, "radius");

  gegl_operation_meta_redirect (operation, "opacity", shadow, "opacity");


  gegl_operation_meta_redirect (operation, "value2", color2, "value");

gegl_operation_meta_redirect (operation, "x", shadow, "x");

gegl_operation_meta_redirect (operation, "y", shadow, "y");

gegl_operation_meta_redirect (operation, "notouch", color, "value");

gegl_operation_meta_redirect (operation, "mradius", median, "radius");

gegl_operation_meta_redirect (operation, "fixoutline", median, "alpha-percentile");

gegl_operation_meta_redirect (operation, "fixoutline", median2, "alpha-percentile");














  gegl_node_link_many (input, it, nop, shadow, color, atop, eblack, color2, median, median2, output, NULL);
 gegl_node_connect_from (atop, "aux", nop, "output");


}

static void
gegl_op_class_init (GeglOpClass *klass)
{
  GeglOperationClass *operation_class;

  operation_class = GEGL_OPERATION_CLASS (klass);

  operation_class->attach = attach;

  gegl_operation_class_set_keys (operation_class,
    "name",        "gegl:innerglow",
    "title",       _("Inner Glow"),
    "categories",  "Aristic",
    "reference-hash", "g3do6aaoo1100g0fjf25sb2ac",
    "description", _("GEGL does an inner glow effect. For this filter to work you need to use the 'Normal' or other blending options. Or a duplicate layer on top.  "
                     ""),
    NULL);
}

#endif
