/**
 * @file utility_lib.h
 *
 * This contains useful utility functions to operate on strings. They're thought
 * to work along main_server.h, main_app_controller.h and gameplay_lib.h facilities.
 *
 * @author Leonardo Arcari
 * @date Feb 2016
 */

#ifndef _UTILITY_LIB_H
#define _UTILITY_LIB_H
#include "constants.h"
#include <glib.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

/**
 * Utility function for #strtokenizer. Since strings separator might be a sequence
 * of characters, the first char of it can be either the beginning of the separator or
 * simply used as a character inside a message content. <p>
 * This function checks it.
 *
 * @param string String to check
 * @param separator String separator
 * @param i index of first character of a possible separation sequence
 * @return TRUE if `string[i]` is the beginning of separator string. FALSE otherwise.
 */
gboolean
check_separator (   gchar *string,
                    gchar* separator,
                    gint i);

/**
 * Fills a string with `NULL-characters`
 *
 * @param a String to clean
 * @param dim String length
 */
void
clear_buffer (  gchar *a,
                gint dim);

/**
 * Scans @p string looking for @p separator occurences and splits it into substring
 * discarting separators.
 *
 * @param string String to check
 * @param separator String separator
 * @param dim Address of an integer variable to save returned array of strings dimension.
 * @return An array of strings (tokens) of @p dim cells.
 */
gchar **
strtokenizer (  gchar *string,
                gchar *separator,
                gint *dim);

/**
 * Makes sure that @p string is a `NULL-terminated` string (`\0`). Then all characters
 * are lowered.
 *
 * @param string String to normalize.
 */
void
normalize_string (gchar* string);

#endif
