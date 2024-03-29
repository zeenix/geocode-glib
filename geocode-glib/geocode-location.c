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

#include <config.h>

#include <geocode-location.h>
#include <math.h>
#include <glib/gi18n-lib.h>

#define EARTH_RADIUS_KM 6372.795

/**
 * SECTION:geocode-location
 * @short_description: Geocode location object
 * @include: geocode-glib/geocode-glib.h
 *
 * The #GeocodeLocation instance represents a location on earth, with an
 * optional description.
 **/

struct _GeocodeLocationPrivate {
        gdouble longitude;
        gdouble latitude;
        gint64  timestamp;
        char   *description;
};

enum {
        PROP_0,

        PROP_LATITUDE,
        PROP_LONGITUDE,
        PROP_DESCRIPTION,
        PROP_TIMESTAMP
};

G_DEFINE_TYPE (GeocodeLocation, geocode_location, G_TYPE_OBJECT)

static void
geocode_location_get_property (GObject    *object,
                               guint       property_id,
                               GValue     *value,
                               GParamSpec *pspec)
{
        GeocodeLocation *location = GEOCODE_LOCATION (object);

        switch (property_id) {
        case PROP_DESCRIPTION:
                g_value_set_string (value,
                                    geocode_location_get_description (location));
                break;

        case PROP_LATITUDE:
                g_value_set_double (value,
                                    geocode_location_get_latitude (location));
                break;

        case PROP_LONGITUDE:
                g_value_set_double (value,
                                    geocode_location_get_longitude (location));
                break;

        case PROP_TIMESTAMP:
                g_value_set_int64 (value,
                                   geocode_location_get_timestamp (location));
                break;

        default:
                /* We don't have any other property... */
                G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
                break;
        }
}

static void
geocode_location_set_property(GObject      *object,
                              guint         property_id,
                              const GValue *value,
                              GParamSpec   *pspec)
{
        GeocodeLocation *location = GEOCODE_LOCATION (object);

        switch (property_id) {
        case PROP_DESCRIPTION:
                geocode_location_set_description (location,
                                                  g_value_get_string (value));
                break;

        case PROP_LATITUDE:
                geocode_location_set_latitude (location,
                                               g_value_get_double (value));
                break;

        case PROP_LONGITUDE:
                geocode_location_set_longitude (location,
                                                g_value_get_double (value));
                break;

        case PROP_TIMESTAMP:
                geocode_location_set_timestamp (location,
                                                g_value_get_int64 (value));
                break;

        default:
                /* We don't have any other property... */
                G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
                break;
        }
}

static void
geocode_location_finalize (GObject *glocation)
{
        GeocodeLocation *location = (GeocodeLocation *) glocation;

        g_clear_pointer (&location->priv->description, g_free);

        G_OBJECT_CLASS (geocode_location_parent_class)->finalize (glocation);
}

static void
geocode_location_class_init (GeocodeLocationClass *klass)
{
        GObjectClass *glocation_class = G_OBJECT_CLASS (klass);
        GParamSpec *pspec;

        glocation_class->finalize = geocode_location_finalize;
        glocation_class->get_property = geocode_location_get_property;
        glocation_class->set_property = geocode_location_set_property;

        g_type_class_add_private (klass, sizeof (GeocodeLocationPrivate));

        /**
         * GeocodeLocation:description:
         *
         * The description of this location.
         */
        pspec = g_param_spec_string ("description",
                                     "Description",
                                     _("Description of this location"),
                                     NULL /* default value */,
                                     G_PARAM_READWRITE |
                                     G_PARAM_STATIC_STRINGS);
        g_object_class_install_property (glocation_class, PROP_DESCRIPTION, pspec);

        /**
         * GeocodeLocation:latitude:
         *
         * The latitude of this location in degrees.
         */
        pspec = g_param_spec_double ("latitude",
                                     "Latitude",
                                     _("The latitude of this location in degrees"),
                                     -90.0,
                                     90.0,
                                     0.0 /* default value */,
                                     G_PARAM_READWRITE |
                                     G_PARAM_STATIC_STRINGS);
        g_object_class_install_property (glocation_class, PROP_LATITUDE, pspec);

        /**
         * GeocodeLocation:longitude:
         *
         * The longitude of this location in degrees.
         */
        pspec = g_param_spec_double ("longitude",
                                     "Longitude",
                                     _("The longitude of this location in degrees"),
                                     -180.0,
                                     180.0,
                                     0.0 /* default value */,
                                     G_PARAM_READWRITE |
                                     G_PARAM_STATIC_STRINGS);
        g_object_class_install_property (glocation_class, PROP_LONGITUDE, pspec);

        /**
         * GeocodeLocation:timestamp:
         *
         * A timestamp in seconds since
         * <ulink url="http://en.wikipedia.org/wiki/Unix_epoch">Epoch</ulink>.
         */
        pspec = g_param_spec_int64 ("timestamp",
                                    "Timestamp",
                                    _("The timestamp of this location "
                                      "in seconds since Epoch"),
                                    0,
                                    G_MAXINT64,
                                    0.0 /* default value */,
                                    G_PARAM_READWRITE |
                                    G_PARAM_STATIC_STRINGS);
        g_object_class_install_property (glocation_class, PROP_TIMESTAMP, pspec);
}

static void
geocode_location_init (GeocodeLocation *location)
{
        GTimeVal tv;

        location->priv = G_TYPE_INSTANCE_GET_PRIVATE ((location),
                                                      GEOCODE_TYPE_LOCATION,
                                                      GeocodeLocationPrivate);

        g_get_current_time (&tv);
        location->priv->timestamp = tv.tv_sec;
}

/**
 * geocode_location_new:
 * @latitude: a valid latitude
 * @longitude: a valid longitude
 *
 * Creates a new #GeocodeLocation object.
 *
 * Returns: a new #GeocodeLocation object. Use g_object_unref() when done.
 **/
GeocodeLocation *
geocode_location_new (gdouble latitude,
                      gdouble longitude)
{
        return g_object_new (GEOCODE_TYPE_LOCATION,
                             "latitude", latitude,
                             "longitude", longitude,
                             NULL);
}

/**
 * geocode_location_new_with_description:
 * @latitude: a valid latitude
 * @longitude: a valid longitude
 * @description: a description for the location
 *
 * Creates a new #GeocodeLocation object.
 *
 * Returns: a new #GeocodeLocation object. Use g_object_unref() when done.
 **/
GeocodeLocation *
geocode_location_new_with_description (gdouble     latitude,
                                       gdouble     longitude,
                                       const char *description)
{
        return g_object_new (GEOCODE_TYPE_LOCATION,
                             "latitude", latitude,
                             "longitude", longitude,
                             "description", description,
                             NULL);
}

/**
 * geocode_location_set_description:
 * @description: a description for the location
 *
 * Sets the description of @loc to @description.
 **/
void
geocode_location_set_description (GeocodeLocation *loc,
                                  const char      *description)
{
        g_return_if_fail (GEOCODE_IS_LOCATION (loc));
        g_return_if_fail (description != NULL);

        g_free (loc->priv->description);
        loc->priv->description = g_strdup (description);
}

const char *
geocode_location_get_description (GeocodeLocation *loc)
{
        g_return_val_if_fail (GEOCODE_IS_LOCATION (loc), NULL);

        return loc->priv->description;
}

/**
 * geocode_location_set_latitude:
 * @latitude: a latitude, in degrees
 *
 * Sets the latitude of @loc to @latitude.
 **/
void
geocode_location_set_latitude (GeocodeLocation *loc,
                               gdouble          latitude)
{
        g_return_if_fail (GEOCODE_IS_LOCATION (loc));
        g_return_if_fail (latitude >= -90.0 && latitude <= 90.0);

        loc->priv->latitude = latitude;
}

gdouble
geocode_location_get_latitude (GeocodeLocation *loc)
{
        g_return_val_if_fail (GEOCODE_IS_LOCATION (loc), 0.0);

        return loc->priv->latitude;
}

/**
 * geocode_location_set_longitude:
 * @longitude: a longitude, in degrees
 *
 * Sets the longitude of @loc to @longitude.
 **/
void
geocode_location_set_longitude (GeocodeLocation *loc,
                                gdouble          longitude)
{
        g_return_if_fail (GEOCODE_IS_LOCATION (loc));
        g_return_if_fail (longitude >= -180.0 && longitude <= 180.0);

        loc->priv->longitude = longitude;
}

gdouble
geocode_location_get_longitude (GeocodeLocation *loc)
{
        g_return_val_if_fail (GEOCODE_IS_LOCATION (loc), 0.0);

        return loc->priv->longitude;
}

/**
 * geocode_location_set_timestamp:
 * @timestamp: a timestamp in seconds since
 * <ulink url="http://en.wikipedia.org/wiki/Unix_epoch">Epoch</ulink>.
 *
 * Sets the timestamp of @loc to @timestamp.
 **/
void
geocode_location_set_timestamp (GeocodeLocation *loc,
                                gint64           timestamp)
{
        g_return_if_fail (GEOCODE_IS_LOCATION (loc));
        g_return_if_fail (timestamp > 0);

        loc->priv->timestamp = timestamp;
}

gint64
geocode_location_get_timestamp (GeocodeLocation *loc)
{
        g_return_val_if_fail (GEOCODE_IS_LOCATION (loc), -1);

        return loc->priv->timestamp;
}

/**
 * geocode_location_get_distance_from:
 * @loca: a #GeocodeLocation
 * @locb: a #GeocodeLocation
 *
 * Calculates the distance in km, along the curvature of the Earth,
 * between 2 locations. Note that altitude changes are not
 * taken into account.
 *
 * Returns: a distance in km.
 **/
double
geocode_location_get_distance_from (GeocodeLocation *loca,
                                    GeocodeLocation *locb)
{
        gdouble dlat, dlon, lat1, lat2;
        gdouble a, c;

        g_return_val_if_fail (GEOCODE_IS_LOCATION (loca), 0.0);
        g_return_val_if_fail (GEOCODE_IS_LOCATION (locb), 0.0);

        /* Algorithm from:
         * http://www.movable-type.co.uk/scripts/latlong.html */

        dlat = (locb->priv->latitude - loca->priv->latitude) * M_PI / 180.0;
        dlon = (locb->priv->longitude - loca->priv->longitude) * M_PI / 180.0;
        lat1 = loca->priv->latitude * M_PI / 180.0;
        lat2 = locb->priv->latitude * M_PI / 180.0;

        a = sin (dlat / 2) * sin (dlat / 2) +
            sin (dlon / 2) * sin (dlon / 2) * cos (lat1) * cos (lat2);
        c = 2 * atan2 (sqrt (a), sqrt (1-a));
        return EARTH_RADIUS_KM * c;
}
