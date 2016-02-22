#include "utility_lib.h"

gboolean
check_separator (   gchar *string,
                    gchar* separator,
                    gint i)
{
    gboolean same = TRUE;
    gint sep_len = strlen(separator);
    for (int j = 0; j < sep_len; j++) {
        if (string[i+j] != separator[j]) {
            same = FALSE;
            break;
        }
    }
    return same;
}

void
clear_buffer (gchar *a, gint dim)
{
	for (int i = 0; i < dim; i++)
		a[i] = '\0';
}

void
normalize_string (gchar* string) {
    gint dim = strlen(string);
    for (gint i = 0; i < dim; i++) {
        if (string[i] == '\n')
            string[i] = '\0';
        string[i] = tolower(string[i]);
    }
    string[dim] = '\0';
}

gchar **
strtokenizer (gchar *string, gchar *separator, gint *dim)
{
    gint str_length = strlen(string);
    gint separator_length = strlen(separator);
    gint character_counter = 0;
    gint i = 0;

    GList *tokens = NULL;

    //g_message ("strtokenizer: Input string: %s, str_length: %d, separator_length: %d\n", string, str_length, separator_length);

    for (i = 0; i < str_length; i++) {
        if (string[i] == separator[0]) {
            //g_message("strtokenizer: i-th character: %c", string[i]);
            //g_message("strtokenizer: sending-> %s, %s, %d, %d", string, separator, separator_length, i);
            if (check_separator (string, separator, i)) {
                if (character_counter != 0)
                {
                    gchar * word = g_malloc(sizeof(gchar) * (character_counter + 1));
                    int k = 0;
                    while (character_counter != 0)
                    {
                        word[k] = string[i - character_counter];
                        character_counter--;
                        k++;
                    }
                    word[k] = '\0';
                    //g_message("strtokenizer: current word: %s", word);
                    tokens = g_list_append(tokens, word);
                }
                if (i + separator_length < str_length)
                    i = i + separator_length - 1;
                else
                    i = str_length;
            } else {
                character_counter++;
                //g_message("Read: %c. Incrementing...", string[i]);
            }
        } else {
            character_counter++;
            //g_message("Read: %c. Incrementing...", string[i]);
        }
    }

    //g_message("strtokenizer: last character_counter: %d", character_counter);
    if (character_counter > 0) {
        gchar * word = (gchar *) g_malloc (sizeof(gchar) * (character_counter + 1));
        int k = 0;
        while (character_counter != 0) {
            word[k] = string[i - character_counter];
            character_counter--;
            k++;
        }
        word[k] = '\0';
        //g_message("strtokenizer: current word: %s", word);
        tokens = g_list_append(tokens, word);
    }

    GList *l = tokens;
    gchar ** tokens_array = (gchar **) g_malloc (g_list_length(tokens) * sizeof(gchar *));
    //g_message("Bloccato?");
    i = 0;
    while (l != NULL) {
        GList *next = l->next;
        tokens_array[i] = (gchar *)(l->data);
        //g_message("%s", (gchar*) l->data);
        tokens = g_list_delete_link (tokens, l);
        i++;
        l = next;
    }

    (*dim) = i; // Return array dimension

    return tokens_array;
}
