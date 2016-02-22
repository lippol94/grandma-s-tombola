#include <stdlib.h>
#include "gameplay_lib.h"

// supporting Functions

gboolean
_check_client_nums (game_instance_t *gi,
                    gchar **client_nums,
                    gint size)
{
    gboolean result = FALSE;
    gint counter = 0;
    for (int i = 0; i < size; i++) {
        for (int k = 0; k < MAX_NUM - gi->numbers->left; k++) {
            if (g_ascii_strtod (client_nums[i], NULL) == gi->numbers->extracted[k]) {
                ++counter;
                break;
            }
        }
    } if (counter == size) {
        result = TRUE;
    }
    return result;
}

gint
_compare_players (  gconstpointer a,
                    gconstpointer b)
{
    player_t *list_data = (player_t *) a;
    GSocket *user_data = (GSocket *) b;

    if (list_data->socket_connection->socket == user_data)
        return 0;
    else
        return -1; // meaning-less number
}

void
_game_loop_end_game (   game_instance_t *gi,
                        player_t *player)
{
    GList *l;
    gint res;
    gchar *end_signal = g_malloc ((strlen (END_GAME_CODE) + strlen (MESSAGE_SEPARATOR) + strlen (player->username)) * sizeof(gchar));
    gchar *buffer = g_malloc (BUFFER_SIZE * sizeof (gchar));

    strcpy (end_signal, END_GAME_CODE);
    strcat (end_signal, MESSAGE_SEPARATOR);
    strcat (end_signal, player->username);

    l = gi->players;

    while (l != NULL) { // Remove every single player
        player_t *p = (player_t *) l->data;
        GList *next = l->next;
        clear_buffer (buffer, BUFFER_SIZE);
        res = -2;

        g_socket_send ( p->socket_connection->socket,
                        end_signal,
                        strlen(end_signal),
                        NULL,
                        NULL);
        res = g_socket_receive (p->socket_connection->socket,
                                buffer,
                                BUFFER_SIZE,
                                NULL,
                                NULL);
        if (res > 0) {
            g_socket_close (p->socket_connection->socket, NULL);
            g_object_unref (G_OBJECT(p->socket_connection->connection));
            g_source_destroy(p->socket_connection->socket_source);
            g_free (p);
    		gi->players = g_list_delete_link (gi->players, l);
        } else {
            g_print ("Error closing socket\n");
        }

        l = next; // Next item
    }

    g_free (end_signal);
    g_free (buffer);

    if (gi->extraction_routine_id != -2) // Stop extraction
        g_source_remove (gi->extraction_routine_id);

    g_print ("Exiting. Thank you for playing!\n");
    g_main_loop_quit (gi->loop);
    g_thread_exit (NULL); // See ya babes
}

gint
_game_loop_ev_win ( game_instance_t *gi,
                    socket_message_t *socket_message,
                    player_t *player)
{
    gint size = -1;
    gint bytes_sent = -2;
    gchar *buffer = NULL;
    gchar **client_nums = strtokenizer (socket_message->msg, ",", &size);
    gboolean result = FALSE;
    switch (socket_message->msg_type) {
        case AMBO:
            if (size == 2) {
                result = _check_client_nums (gi, client_nums, size);
                buffer = g_malloc ((strlen (AMBO_CODE) + strlen (MESSAGE_SEPARATOR) + strlen ("0")) * sizeof (gchar));
                strcpy (buffer, AMBO_CODE);
            }
            break;
        case TERNA:
            if (size == 3) {
                result = _check_client_nums (gi, client_nums, size);
                buffer = g_malloc ((strlen (TERNA_CODE) + strlen (MESSAGE_SEPARATOR) + strlen ("0")) * sizeof (gchar));
                strcpy (buffer, TERNA_CODE);
            }
            break;
        case QUATERNA:
            if (size == 4) {
                result = _check_client_nums (gi, client_nums, size);
                buffer = g_malloc ((strlen (QUATERNA_CODE) + strlen (MESSAGE_SEPARATOR) + strlen ("0")) * sizeof (gchar));
                strcpy (buffer, QUATERNA_CODE);
            }
            break;
        case CINQUINA:
            if (size == 5) {
                result = _check_client_nums (gi, client_nums, size);
                buffer = g_malloc ((strlen (CINQUINA_CODE) + strlen (MESSAGE_SEPARATOR) + strlen ("0")) * sizeof (gchar));
                strcpy (buffer, CINQUINA_CODE);
            }
            break;
        case TOMBOLA:
            if (size == 15) {
                result = _check_client_nums (gi, client_nums, size);
                buffer = g_malloc ((strlen (TOMBOLA_CODE) + strlen (MESSAGE_SEPARATOR) + strlen ("0")) * sizeof (gchar));
                strcpy (buffer, TOMBOLA_CODE);
            }
            break;
        default:
            g_print ("Un-handled scenario. How did you get here?\n");
            break;
    }
    strcat (buffer, MESSAGE_SEPARATOR);
    if (result) {
        strcat (buffer, "0");
    } else {
        strcat (buffer, "1");
    }

    bytes_sent = g_socket_send (player->socket_connection->socket,
                                buffer,
                                strlen(buffer),
                                NULL,
                                NULL);
    return bytes_sent;
}

gboolean
_game_loop_extraction (gpointer user_data)
{
    game_instance_t *gi = (game_instance_t *) user_data;
    if (gi->numbers->left > 0) {
        gint extracted = extract (gi->numbers);
        gchar *ext_str = g_malloc (3 * sizeof(gchar));
        g_ascii_dtostr (ext_str, 3, extracted);
        gchar *buffer = create_message (ext_str, EXTRACTED_CODE);
        GList *l;
        for (l = gi->players; l != NULL; l = l->next) {
            player_t *lp = (player_t *) l->data;
            g_socket_send ( lp->socket_connection->socket,
                            buffer,
                            strlen(buffer),
                            NULL,
                            NULL);
        } g_free (buffer);
        g_free (ext_str);
        return TRUE;
    } else {
        g_source_remove (gi->extraction_routine_id);
        gi->extraction_routine_id = -2;
        return FALSE;
    }

}


gint
_game_loop_plain (  game_instance_t *game_instance,
                    gchar *message,
                    player_t *player)
{
    GList *l;
    gint bytes_sent = -2;
    gint result = 0;
    //g_message ("chat_message_allocating...");
    gchar *chat_message = g_malloc ((strlen (message) + strlen (player->username) + 4) * sizeof (gchar));
    //g_message ("chat_message_allocated");
    strcpy (chat_message, player->username);
    strcat (chat_message, ": ");
    strcat (chat_message, message);
    //g_message ("Message created: %s\n", chat_message);
    gchar *buffer = create_message (chat_message, PLAIN_CODE);
    //g_message ("Buffer created: %s\n", buffer);

    for (l = game_instance->players; l != NULL; l = l->next) {
        player_t *lp = (player_t *) l->data;
        if (lp->socket_connection->socket != player->socket_connection->socket) {
            //g_message ("sending message\n");
            bytes_sent = g_socket_send (    lp->socket_connection->socket,
                                            buffer,
                                            strlen(buffer),
                                            NULL,
                                            NULL);
            if (bytes_sent < 1)
                ++result;
        }
    }
    g_free (buffer);
    g_free (chat_message);
    return result;
}

int _numbers_comp (const void * _a, const void * _b)
{
    int *a = (int *) _a;
    int *b = (int *) _b;

    if (*a == -1 && *b == -1)
        return 0;
    else if (*a == -1 && *b != -1)
        return 1;
    else if (*a != -1 && *b == -1)
        return -1;
    else if (*a > *b)
        return 1;
    else if (*a < *b)
        return -1;
    else
        return 0;
}

void
_setup_socket_sources ( game_instance_t* game_instance)
{
    GList* l;
    for (l = game_instance->players; l != NULL; l = l->next) {
        player_t* p = (player_t *) l->data;
        p->socket_connection->socket_source = g_socket_create_source(   p->socket_connection->socket,
                                                                        G_IO_IN,
                                                                        NULL);

        g_source_set_callback ( p->socket_connection->socket_source,
                                (GSourceFunc) thread_read_socket,
                                game_instance,
                                NULL);
        g_source_attach (   p->socket_connection->socket_source,
                            g_main_loop_get_context(game_instance->loop));
    }
}

gchar *
create_message (gchar *message,
                gchar *op_code)
{
    gchar *buffer = g_malloc ((strlen(op_code) + 2 * strlen(MESSAGE_SEPARATOR) + strlen(message) + 2) * sizeof(gchar));
    strcpy (buffer, op_code);
    strcat (buffer, MESSAGE_SEPARATOR);
    strcat (buffer, message);
    strcat (buffer, MESSAGE_SEPARATOR);
    return buffer;
}

gboolean
evaluate_message (socket_message_t *socket_msg)
{
    gboolean return_value = FALSE;
	gint dim;
	g_print("Msg: %s\n", socket_msg->msg);
	gchar **tokens = strtokenizer(socket_msg->msg, MESSAGE_SEPARATOR, &dim);
    //g_message ("Str tokenized");

	if (strcmp(tokens[0], "") != 0) { // token[i] isn't an empty string
		if (strcmp(tokens[0], PLAIN_CODE) == 0) {
			socket_msg->msg_type = PLAIN;
			socket_msg->msg = tokens[1];
			return_value = TRUE;
        } else if (strcmp(tokens[0], CREATE_GAME_CODE) == 0){
            socket_msg->msg_type = CREATE_GAME;
            socket_msg->msg = tokens[1];
            return_value = TRUE;
		} else if (strcmp(tokens[0], JOIN_GAME_CODE) == 0) {
			socket_msg->msg_type = JOIN_GAME;
			socket_msg->msg = tokens[1];
			return_value = TRUE;
		} else if (strcmp(tokens[0], START_GAME_CODE) == 0) {
            socket_msg->msg_type = START_GAME;
            return_value = TRUE;
        } else if (strcmp(tokens[0], END_GAME_CODE) == 0) {
            socket_msg->msg_type = END_GAME;
            socket_msg->msg = tokens[1];
            return_value = TRUE;
        } else if (strcmp(tokens[0], CLIENT_READY_CODE) == 0) {
            socket_msg->msg_type = CLIENT_READY;
            return_value = TRUE;
        } else if (strcmp(tokens[0], AMBO_CODE) == 0) {
            socket_msg->msg_type = AMBO;
            socket_msg->msg = tokens[1];
            return_value = TRUE;
        } else if (strcmp(tokens[0], TERNA_CODE) == 0) {
            socket_msg->msg_type = TERNA;
            socket_msg->msg = tokens[1];
            return_value = TRUE;
        } else if (strcmp(tokens[0], QUATERNA_CODE) == 0) {
            socket_msg->msg_type = QUATERNA;
            socket_msg->msg = tokens[1];
            return_value = TRUE;
        } else if (strcmp(tokens[0], CINQUINA_CODE) == 0) {
            socket_msg->msg_type = CINQUINA;
            socket_msg->msg = tokens[1];
            return_value = TRUE;
        } else if (strcmp(tokens[0], TOMBOLA_CODE) == 0) {
            socket_msg->msg_type = TOMBOLA;
            socket_msg->msg = tokens[1];
            return_value = TRUE;
        } else if (strcmp(tokens[0], EXTRACTED_CODE) == 0) {
            socket_msg->msg_type = EXTRACTED;
            socket_msg->msg = tokens[1];
            return_value = TRUE;
        }
	}
	return return_value;
}

gint extract(numbers_t *numbers)
{
    gint ex_pos = g_rand_int_range (numbers->_rand_, 0, numbers->left);
    gint extracted = numbers->num[ex_pos];
    numbers->num[ex_pos] = -1;
    qsort (numbers->num, MAX_NUM, sizeof(gint), _numbers_comp);
    numbers->extracted[MAX_NUM - numbers->left] = extracted;
    (numbers->left)--;
    return extracted;
}


void init_numbers (numbers_t *numbers)
{
    for (int i = 0; i < MAX_NUM; i++) {
        numbers->num[i] = i+1;
        numbers->extracted[i] = -1;
    } numbers->left = MAX_NUM;
    numbers->_rand_ = g_rand_new();
}


void print_numbers (numbers_t *numbers)
{
    g_print("Tabellone:\n");
    for (int i = 0; i < MAX_NUM; i++) {
        if (numbers->num[i] != -1)
            g_print("%d ", numbers->num[i]);
        else
            g_print("NULL ");
    }
    g_print("\nNumbers left: %d\n", numbers->left);
    g_print("Estratti:\n");
    for (int i = 0; i < MAX_NUM - numbers->left; i++) {
        if (numbers->extracted[i] != -1)
            g_print("%d ", numbers->extracted[i]);
        else
            g_print("NULL ");
    } g_print("\n");
}

void
sort_extracted_nums (numbers_t *numbers)
{
    qsort(numbers->extracted, MAX_NUM - numbers->left, sizeof(gint), _numbers_comp);
}

gpointer
game_loop (gpointer data)
{
    game_instance_t *game_instance = (game_instance_t *) data;
    game_instance->numbers = g_new (numbers_t, 1);

    GMainContext *context;

    context = g_main_context_new();
    game_instance->loop = g_main_loop_new(context, FALSE);

    _setup_socket_sources(game_instance);
    init_numbers (game_instance->numbers);
    game_instance->extraction_routine_id = g_timeout_add (  EXT_INTERVAL * SEC_MILLIS,
                                                            (GSourceFunc) _game_loop_extraction,
                                                            game_instance);

    g_main_loop_run (game_instance->loop);
    return NULL;
}

void
game_loop_actions ( game_instance_t *game_instance,
                    socket_message_t *socket_message,
                    player_t *player)
{
    if (evaluate_message (socket_message)) { // Evaluation was fine
        gint result;
        switch (socket_message->msg_type) {
            case PLAIN:
                //g_message ("Message evaluated: entering _game_loop_plain");
                result = _game_loop_plain (game_instance, socket_message->msg, player);
                if (result != 0) {
                    g_message("Error occured in _game_loop_plain");
                }
                break;
            case END_GAME:
                _game_loop_end_game (game_instance, player);
                break;
            case CLIENT_READY:
                break;
            case AMBO:
                result = _game_loop_ev_win (game_instance, socket_message, player);
                break;
            case TERNA:
                result = _game_loop_ev_win (game_instance, socket_message, player);
                break;
            case QUATERNA:
                result = _game_loop_ev_win (game_instance, socket_message, player);
                break;
            case CINQUINA:
                result = _game_loop_ev_win (game_instance, socket_message, player);
                break;
            case TOMBOLA:
                result = _game_loop_ev_win (game_instance, socket_message, player);
                break;
            default:
                break;
        }
    }
}

gboolean
thread_read_socket (    GSocket *socket,
                        GIOCondition condition,
                        gpointer user_data)
{
    game_instance_t *gi = (game_instance_t *) user_data;
    socket_message_t *socket_msg = g_new (socket_message_t, 1);
    char *buffer = (char *) g_malloc (BUFFER_SIZE * sizeof(char));
    gint bytes_read = -2;
    GError *error = NULL;
    clear_buffer (buffer, BUFFER_SIZE);

    bytes_read = g_socket_receive (socket, buffer, BUFFER_SIZE, NULL, &error); // Recieve data from the socket
    /* Check for errors */
    if (error != NULL) {
        g_print("Unable to read from socket: %s\n", error->message);
        g_error_free(error);
        g_socket_close(socket, &error); // Does 'error' still exist?
        return FALSE; //Destroy source?
    }

    GList *me = g_list_find_custom(gi->players, socket, (GCompareFunc)_compare_players);
    player_t *me_player = (player_t *) me->data;
    /* Check if client closed connection */
    if (bytes_read == 0) {
		g_print ("Connection closed by the client!\n");
		g_socket_close (socket, NULL);
        g_object_unref (G_OBJECT(me_player->socket_connection->connection));
        g_source_destroy(me_player->socket_connection->socket_source);
		gi->players = g_list_delete_link (gi->players, me);
        if (g_list_length (gi->players) == 0) {
            if (gi->extraction_routine_id != -2)
                g_source_remove (gi->extraction_routine_id);
            g_main_loop_quit (gi->loop);
            g_free (gi->numbers);
            g_free (gi);
            g_thread_exit (NULL); // DIE!
        }
		return FALSE;
	} else {
        normalize_string(buffer);
        g_print("%s\n", buffer); // Print it
        socket_msg->msg = buffer;
        game_loop_actions(gi, socket_msg, me_player);
        return TRUE; // Everything went fine
    }
}
