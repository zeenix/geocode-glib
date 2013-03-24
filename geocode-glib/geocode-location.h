/*
   Copyright (C) 2012 Bastien Nocera

   The Gnome Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The Gnome Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the Gnome Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301  USA.

   Authors: Bastien Nocera <hadess@hadess.net>

 */

#ifndef GEOCODE_LOCATION_H
#define GEOCODE_LOCATION_H

#include <glib-object.h>

G_BEGIN_DECLS

GType geocode_location_get_type (void) G_GNUC_CONST;

#define GEOCODE_TYPE_LOCATION                  (geocode_location_get_type ())
#define GEOCODE_LOCATION(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), GEOCODE_TYPE_LOCATION, GeocodeLocation))
#define GEOCODE_IS_LOCATION(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GEOCODE_TYPE_LOCATION))
#define GEOCODE_LOCATION_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), GEOCODE_TYPE_LOCATION, GeocodeLocationClass))
#define GEOCODE_IS_LOCATION_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), GEOCODE_TYPE_LOCATION))
#define GEOCODE_LOCATION_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), GEOCODE_TYPE_LOCATION, GeocodeLocationClass))

/**
 * GeocodeLocation:
 *
 * All the fields in the #GeocodeLocation structure are private and should never be accessed directly.
**/
typedef struct _GeocodeLocation        GeocodeLocation;
typedef struct _GeocodeLocationClass   GeocodeLocationClass;
typedef struct _GeocodeLocationPrivate GeocodeLocationPrivate;

struct _GeocodeLocation {
        /* <private> */
        GObject parent_instance;
        GeocodeLocationPrivate *priv;
};

/**
 * GeocodeLocationClass:
 *
 * All the fields in the #GeocodeLocationClass structure are private and should never be accessed directly.
**/
struct _GeocodeLocationClass {
        /* <private> */
        GObjectClass parent_class;
};

GeocodeLocation *geocode_location_new (gdouble latitude,
                                       gdouble longitude);

GeocodeLocation *geocode_location_new_with_description (gdouble     latitude,
                                                        gdouble     longitude,
                                                        const char *description);

double geocode_location_get_distance_from (GeocodeLocation *loca,
                                           GeocodeLocation *locb);

void geocode_location_set_description (GeocodeLocation *loc,
                                       const char      *description);

const char *geocode_location_get_description (GeocodeLocation *loc);

gdouble geocode_location_get_latitude (GeocodeLocation *loc);
void geocode_location_set_latitude (GeocodeLocation *loc,
                                    gdouble          latitude);

gdouble geocode_location_get_longitude (GeocodeLocation *loc);
void geocode_location_set_longitude (GeocodeLocation *loc,
                                     gdouble          longitude);

void geocode_location_set_timestamp (GeocodeLocation *loc,
                                     gint64           timestamp);
gint64 geocode_location_get_timestamp (GeocodeLocation *loc);

G_END_DECLS

#endif /* GEOCODE_LOCATION_H */
