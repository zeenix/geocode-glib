/*
   Copyright (C) 2011 Bastien Nocera

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

#include <string.h>
#include <errno.h>
#include <locale.h>
#include <gio/gio.h>
#include <json-glib/json-glib.h>
#include <libsoup/soup.h>
#include <geocode-glib.h>
#include <geocode-error.h>
#include <geocode-glib-private.h>

/**
 * SECTION:geocode-glib
 * @short_description: Geocode glib main functions
 * @include: geocode-glib/geocode-glib.h
 *
 * Contains functions for geocoding and reverse geocoding using the
 * <ulink url="http://developer.yahoo.com/geo/placefinder/guide/requests.html">Yahoo! Place Finder APIs</ulink>
 * for reverse geocoding, and the
 * <ulink url="http://developer.yahoo.com/geo/geoplanet/">Yahoo! GeoPlanet APIs</ulink>
 * for forward geocoding.
 **/

char *
_geocode_glib_cache_path_for_query (GFile *query)
{
	const char *filename;
	char *path;
	char *uri;
	GChecksum *sum;

	/* Create cache directory */
	path = g_build_filename (g_get_user_cache_dir (),
				 "geocode-glib",
				 NULL);
	if (g_mkdir_with_parents (path, 0700) < 0) {
		g_warning ("Failed to mkdir path '%s': %s", path, g_strerror (errno));
		g_free (path);
		return NULL;
	}
	g_free (path);

	/* Create path for query */
	uri = g_file_get_uri (query);

	sum = g_checksum_new (G_CHECKSUM_SHA256);
	g_checksum_update (sum, (const guchar *) uri, strlen (uri));

	filename = g_checksum_get_string (sum);

	path = g_build_filename (g_get_user_cache_dir (),
				 "geocode-glib",
				 filename,
				 NULL);

	g_checksum_free (sum);
	g_free (uri);

	return path;
}

gboolean
_geocode_glib_cache_save (GFile      *query,
			  const char *contents)
{
	char *path;
	gboolean ret;

	path = _geocode_glib_cache_path_for_query (query);
	g_debug ("Saving cache file '%s'", path);
	ret = g_file_set_contents (path, contents, -1, NULL);

	g_free (path);
	return ret;
}

gboolean
_geocode_glib_cache_load (GFile  *query,
			  char  **contents)
{
	char *path;
	gboolean ret;

	path = _geocode_glib_cache_path_for_query (query);
	g_debug ("Loading cache file '%s'", path);
	ret = g_file_get_contents (path, contents, NULL, NULL);

	g_free (path);
	return ret;
}

static gboolean
parse_lang (const char *locale,
	    char      **language_codep,
	    char      **territory_codep)
{
	GRegex     *re;
	GMatchInfo *match_info;
	gboolean    res;
	GError     *error;
	gboolean    retval;

	match_info = NULL;
	retval = FALSE;

	error = NULL;
	re = g_regex_new ("^(?P<language>[^_.@[:space:]]+)"
			  "(_(?P<territory>[[:upper:]]+))?"
			  "(\\.(?P<codeset>[-_0-9a-zA-Z]+))?"
			  "(@(?P<modifier>[[:ascii:]]+))?$",
			  0, 0, &error);
	if (re == NULL) {
		g_warning ("%s", error->message);
		goto out;
	}

	if (!g_regex_match (re, locale, 0, &match_info) ||
	    g_match_info_is_partial_match (match_info)) {
		g_warning ("locale '%s' isn't valid\n", locale);
		goto out;
	}

	res = g_match_info_matches (match_info);
	if (! res) {
		g_warning ("Unable to parse locale: %s", locale);
		goto out;
	}

	retval = TRUE;

	*language_codep = g_match_info_fetch_named (match_info, "language");

	*territory_codep = g_match_info_fetch_named (match_info, "territory");

	if (*territory_codep != NULL &&
	    *territory_codep[0] == '\0') {
		g_free (*territory_codep);
		*territory_codep = NULL;
	}

out:
	g_match_info_free (match_info);
	g_regex_unref (re);

	return retval;
}

static char *
geocode_object_get_lang_for_locale (const char *locale)
{
	char *lang;
	char *territory;
	char *ret;

	if (parse_lang (locale, &lang, &territory) == FALSE)
		return NULL;

	ret =  g_strdup_printf ("%s%s%s",
				lang,
				territory ? "-" : "",
				territory ? territory : "");

	g_free (lang);
	g_free (territory);

	return ret;
}

char *
_geocode_object_get_lang (void)
{
	return geocode_object_get_lang_for_locale (setlocale (LC_MESSAGES, NULL));
}
