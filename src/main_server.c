#include "main_server.h"

gint
_compare_games (    gconstpointer a,
                    gconstpointer b)
{
    gchar* list_data = (gchar *) a;
    gchar* user_data = (gchar *) b;

    return strcmp(list_data, user_data);
}

gboolean
_lobby_create_game (loop_data_t *loop_data, gchar *game_data, player_t *player)
{
    gint size;
    gchar** tokens = strtokenizer (game_data, ",", &size);
    gchar *username = tokens[0];
    gchar *game_name = tokens[1];

    if (g_list_find_custom(loop_data->games, game_name, (GCompareFunc) _compare_games) == NULL) {
        gchar* name = g_malloc(sizeof(gchar) * strlen(game_name));
        strcpy(name, game_name);
        loop_data->games = g_list_append(loop_data->games, name);
        g_print("Game '%s' created\n", name);
        player->room = name;
        player->username = username;
        g_print("Player %s added to game: %s\n", player->username, player->room);
        return TRUE;
    } else {
        return FALSE;
    }
}

gboolean
_lobby_join_game (loop_data_t *loop_data, gchar *game_data, player_t *player)
{
    gint size;
    gchar** tokens = strtokenizer (game_data, ",", &size);
    gchar *username = tokens[0];
    gchar *game_name = tokens[1];

    GList *found = g_list_find_custom(loop_data->games, game_name, (GCompareFunc) _compare_games);
    if (found != NULL) {
        gchar *name = (gchar *) found->data;
        player->room = name;
        player->username = username;
        g_print("Player %s added to game: %s\n", player->username, player->room);
        return TRUE;
    } else {
        g_print("No exists\n");
        return FALSE;
    }
}

void
_lobby_start_game (loop_data_t *loop_data, player_t *player)
{
    g_message("In _lobby_start_game");
    GList *l = loop_data->players;
    game_instance_t *game_instance = g_new(game_instance_t, 1);
    game_instance->players = NULL;

    gchar *game_name = g_malloc(strlen(player->room) * sizeof (gchar));
    strcpy(game_name, player->room);
    g_message("Copied game_name");

    while (l != NULL) {
        g_message("While iteration");
        GList *next = l->next;
        g_message("Got next item reference");
        player_t *lp = (player_t *) l->data;
        if (lp->room != NULL) { // If a player hasn't joined any room there's no need to check
            g_message("Before strcmp");
            if (strcmp(player->room, lp->room) == 0) { // Same game room of game master
                g_message("Compared strings. s1: %s s2: %s", player->room, lp->room);
                gchar* buffer = create_message ("k", START_GAME_CODE);
                g_socket_send ( lp->socket_connection->socket,
                                buffer,
                                strlen(buffer),
                                NULL,
                                NULL);
                g_free(buffer);
                g_message("Message sent. Cleared buffer");
                _move_player(loop_data, game_instance, l);
            }
        }
        l = next;
    }

    GList* found = g_list_find_custom(loop_data->games, game_name, (GCompareFunc) _compare_games);
    if (found != NULL)
        loop_data->games = g_list_delete_link(loop_data->games, found);
    g_free(game_name);

    // Lobby thread is clean. Now we can start game instance thread:
    GThread *new_game_thread = g_thread_new(NULL, (GThreadFunc)game_loop , game_instance);
    loop_data->active_games = g_list_append (loop_data->active_games, new_game_thread);
    g_message("End _lobby_start_game. Thread run");
}

void
_move_player (  loop_data_t *loop_data,
                game_instance_t *game_instance,
                GList *link)
{
    g_message("In _move_player");
    player_t *p = (player_t *) link->data;
    g_source_destroy(p->socket_connection->socket_source);
    loop_data->players = g_list_remove_link(loop_data->players, link);
    game_instance->players = g_list_append(game_instance->players, p);
    g_message("End _move_player");
}

void
lobby_new_game_answer ( player_t *player,
                        msg_types_t msg_type,
                        gboolean result)
{
    if (msg_type == CREATE_GAME) {
        gchar *buffer = create_message(GAME_CREATED, CREATE_GAME_CODE);
        g_socket_send ( player->socket_connection->socket,
                        buffer,
                        strlen (buffer),
                        NULL,
                        NULL);
        g_free (buffer);
    } else if (msg_type == JOIN_GAME && result) {
        gchar *buffer = create_message(GAME_JOINED, JOIN_GAME_CODE);
        g_socket_send ( player->socket_connection->socket,
                        buffer,
                        strlen (buffer),
                        NULL,
                        NULL);
        g_free (buffer);
    } else if (msg_type == JOIN_GAME && !result) {
        gchar *buffer = create_message(GAME_NO_EXIST, JOIN_GAME_CODE);
        g_socket_send ( player->socket_connection->socket,
                        buffer,
                        strlen (buffer),
                        NULL,
                        NULL);
        g_free (buffer);
    }
}

void
lobby_actions ( loop_data_t *loop_data,
                socket_message_t *socket_message,
                player_t *player)
{
    if (evaluate_message (socket_message)) { // Evaluation was fine
        switch (socket_message->msg_type) {
            case CREATE_GAME:
                if (!_lobby_create_game(loop_data, socket_message->msg, player)) {
                    if (!_lobby_join_game(loop_data, socket_message->msg, player)) {
                        g_print("Game doesn't exist. Create a new one\n");
                        lobby_new_game_answer(player, JOIN_GAME, FALSE);
                    } else {
                        lobby_new_game_answer(player, JOIN_GAME, TRUE);
                    }
                } else {
                    lobby_new_game_answer(player, CREATE_GAME, TRUE);
                }
                g_free(socket_message);
                break;
            case JOIN_GAME:
                if (!_lobby_join_game(loop_data, socket_message->msg, player)) {
                    g_print("Game doesn't exist. Create a new one\n");
                    lobby_new_game_answer(player, JOIN_GAME, FALSE);
                } else {
                    lobby_new_game_answer(player, JOIN_GAME, TRUE);
                }
                g_free(socket_message);
                break;
            case START_GAME:
                _lobby_start_game(loop_data, player);
                g_free(socket_message);
                break;
            default:
                break;
        }
    }
}

gboolean
handle_incoming (   GSocketService *service,
                    GSocketConnection *connection,
                    GObject *source_object,
                    gpointer user_data)
{

	/* Casting user_data */
    loop_data_t *loop_data = (loop_data_t *) user_data;
    player_t *player = g_new (player_t, 1);
	player->socket_connection = g_new (socket_connection_t, 1);
    player->room = NULL;

	g_object_ref (connection); // Reference GSocketConnection as it's automatically dereferenced after the callback returns
	g_print ("Connection incoming from a client\n"); // Debug

	GSocket *socket = g_socket_connection_get_socket (connection); // Get Socket from GSocketConnection
	g_print ("Got socket\n"); // Debug

	GSource *socket_source = g_socket_create_source (socket, G_IO_IN, NULL); // Create a GSource watching for data ready to be read
	g_print ("source created for G_IO_IN\n"); // Debug

	g_source_set_callback (socket_source,
		(GSourceFunc)read_socket,
		loop_data,
		NULL); // Set a Callback to the previous GSource

	g_source_attach (socket_source, g_main_loop_get_context (loop_data->loop)); // Attach the GSource to the context of the main loop

	player->socket_connection->connection = connection;
    player->socket_connection->socket = socket;
    player->socket_connection->socket_source = socket_source;

	loop_data->players = g_list_append (loop_data->players, player);

	return FALSE;
}

gboolean
read_socket (	GSocket *socket,
                GIOCondition condition,
               	gpointer user_data)
{
    loop_data_t *loop_data = (loop_data_t *) user_data;
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

    GList *me = g_list_find_custom(loop_data->players, socket, (GCompareFunc)_compare_players);
    player_t *me_player = (player_t *) me->data;
    /* Check if client closed connection */
    if (bytes_read == 0) {
		g_print ("Connection closed by the client!\n");
		g_socket_close (socket, NULL);
        g_object_unref (G_OBJECT(me_player->socket_connection->connection));
        g_source_destroy(me_player->socket_connection->socket_source);
		loop_data->players = g_list_delete_link (loop_data->players, me);
		return FALSE;
	} else {
        normalize_string(buffer);
        g_print("%s\n", buffer); // Print it
        socket_msg->msg = buffer;
        lobby_actions(loop_data, socket_msg, me_player);
        return TRUE; // Everything went fine
    }
}

int
main (int argc, char **argv)
{
    GError *error = NULL;

    loop_data_t *loop_data = g_new(loop_data_t, 1);
    GSocketService *socket_service = g_socket_service_new ();

    /* Listen to a port for incoming connections */
	g_socket_listener_add_inet_port (G_SOCKET_LISTENER(socket_service),
		SERVER_PORT,
		NULL,
		&error);

	/* Check for errors on listening */
	if (error != NULL) {
		g_error("%s\n", error->message);
		g_clear_error(&error);
		return 1;
	}

    /* Register callback for handling incoming event */
	g_signal_connect ( socket_service,
                       "incoming",
                       G_CALLBACK(handle_incoming),
                       loop_data);

	g_socket_service_start(socket_service); // Start socket service

	GMainLoop *main_loop = g_main_loop_new (NULL, FALSE); // Create glib main loop
	loop_data->loop = main_loop; // save it to our data struct
    loop_data->active_games = NULL;
    loop_data->games = NULL;
    loop_data->players = NULL;
	g_main_loop_run (main_loop); // run main loop

	g_socket_service_stop (socket_service);
	return 0;
}
