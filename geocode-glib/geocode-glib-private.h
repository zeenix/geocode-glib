/*
   Copyright (C) 2010 Bastien Nocera

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

#ifndef GEOCODE_GLIB_PRIVATE_H
#define GEOCODE_GLIB_PRIVATE_H

#include <glib.h>

G_BEGIN_DECLS

#define YAHOO_APPID "zznSbDjV34HRU5CXQc4D3qE1DzCsJTaKvWTLhNJxbvI_JTp1hIncJ4xTSJFRgjE-"
#define DEFAULT_ANSWER_COUNT 10

typedef enum {
	GEOCODE_GLIB_RESOLVE_FORWARD,
	GEOCODE_GLIB_RESOLVE_REVERSE
} GeocodeLookupType;

GHashTable *_geocode_parse_resolve_json (const char *contents,
					 GError    **error);
GList      *_geocode_parse_search_json  (const char *contents,
					 GError    **error);
GFile      *_get_resolve_query_for_params (GHashTable  *orig_ht,
					   gboolean     reverse);

char       *_geocode_object_get_lang (void);

char *_geocode_glib_cache_path_for_query (GFile *query);
gboolean _geocode_glib_cache_save (GFile      *query,
				  const char *contents);
gboolean _geocode_glib_cache_load (GFile  *query,
				  char  **contents);


G_END_DECLS

#endif /* GEOCODE_GLIB_PRIVATE_H */
