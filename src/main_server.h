/**
 * @file main_server.h
 *
 * This contains the prototypes and data structures for main server application.
 * Main server thread waits for new clients to connect, arrange them in rooms
 * according to their requestes and starts new games' threads running logic
 * described in gameplay_lib.h
 *
 * @author Leonardo Arcari
 * @date Feb 2016
 */

#ifndef _MAIN_SERVER_H
#define _MAIN_SERVER_H
#include "constants.h"
#include "gameplay_lib.h"
#include <gio/gio.h>
#include <glib.h>
#include <string.h>
#include "utility_lib.h"

/**
 * Data structure to store data of the main server. It's supposed to collect lists
 * of not-yet-started games and waiting-to-play players until they're all arranged
 * in game rooms and <i>game masters</i> start them.
 */
struct _loop_data {
	GMainLoop *loop;		/**< Reference to the server main event loop */
	GList *active_games;	/**< List of active games threads. Not really used
							 *	 actually. */
    GList *games;			/**< List of game rooms' names. Elements are strings. */
	GList *players;			/**< List of players waiting to start playing. Elements
							 *   are _player structs. */
};

/**
 * Define type for _loop_data struct
 */
typedef struct _loop_data loop_data_t;

/**
 * Comparison function to check whether two _loop_data#games are the same or not.
 * This shouldn't be called except from `g_list_find_custom` to find out if a game
 * a user is trying to join exists or not.
 *
 * @return The return value of a `strcmp` between 2 game names
 */
gint
_compare_games (    gconstpointer a,
                    gconstpointer b);

/**
 * Utility function. It creates a new game room and add @p player to it. If
 * game already exists no action is taken
 *
 * @param loop_data Pointer to a _loop_data struct
 * @param game_data String representing game's name asked to create
 * @param player Pointer to _player struct of the one who requested to create a
 * new room.
 * @return TRUE if @p game_data game is created. FALSE otherwise.
 */
gboolean
_lobby_create_game (loop_data_t *loop_data,
                    gchar *game_data,
                    player_t *player);

/**
 * Utility function. It adds @p player to an existing game. If game doesn't exist
 * no action is taken.
 *
 * @param loop_data Pointer to a _loop_data struct
 * @param game_data String representing game's name asked to join
 * @param player Pointer to _player struct of the one who requested to join a
 * room.
 * @return TRUE if @p player has been added to @p game_data game. FALSE otherwise.
 */
gboolean
_lobby_join_game (	loop_data_t *loop_data,
					gchar *game_data,
					player_t *player);

/**
 * Utility function. It starts a game, running a thread with a new _game_instance.
 * Every player in _loop_data#players in the beginning game room is moved to
 * a _game_instance struct (through #_move_player ) and removed from _loop_data. Then housekeeping is done
 * and game is started.
 *
 * @param loop_data Pointer to a _loop_data struct
 * @param player Pointer to _player struct of the one who requested to start the game
 */
void
_lobby_start_game (	loop_data_t *loop_data,
					player_t *player);

/**
 * Moves a _player from a _loop_data#players to a _game_instance#players.
 *
 * @param loop_data Pointer to a _loop_data struct to move a @p link of _loop_data#players from.
 * @param game_instance Pointer to a _game_instance struct to move a @p link of _loop_data#players to.
 * @param link A _loop_data#players list link to move.
 */
void
_move_player (  loop_data_t *loop_data,
                game_instance_t *game_instance,
                GList *link);

/**
 * Sends to the client an answer after his request to create or join a game:
 * - Game created, @p player added to it
 * - @p player joined to requested game
 * - Requested game doesn't exists, @p player not added
 *
 * @param player Pointer to a _player struct of the one who requested to create
 * / join a game
 * @param msg_type Type of request. CREATE_GAME and JON_GAME are allowed
 * @param result @p player was granted or not to create/join the game
 */
void
lobby_new_game_answer ( player_t *player,
                        msg_types_t msg_type,
                        gboolean result);

/**
 * Function to be invoked by #read_socket to answer a request from client. After
 * evaluating message string a proper choice is taken:
 * - If a create-game message, a new game room is created and @p player is added
 * to it if requested game's name doesn't exists. If already exists a call to
 * #_lobby_join_game is done to add @p player to it
 * - If a join-game message, @p player is added to requested game room. If the room
 * doesn't exists a negative answer is sent through #lobby_new_game_answer
 * - If a start-game message a call to #_lobby_start_game is done.
 *
 * @param loop_data Pointer to a _loop_data struct
 * @param socket_message Pointer to a _socket_message struct where
 * _socket_message#msg field contains the string to be evaluated
 * @param player Pointer to _player struct of the one who sent the message in first place.
 */
void
lobby_actions ( loop_data_t *loop_data,
                socket_message_t *socket_message,
                player_t *player);

/**
 * Callback function to handle GSocketService's `incoming` signal. The signal is
 * sent when a new client connects to the server. A new _player struct is allocated
 * and fields are filled. Related GSocket object is registered a source to check
 * on G_IO_IN condition in server's main event loop.
 */
gboolean
handle_incoming (   GSocketService *service,
                    GSocketConnection *connection,
                    GObject *source_object,
                    gpointer user_data);

/**
 * Callback function to be invoked when G_IO_IN condition is satisfied on a socket.
 * Message is read from socket, normalized and sent to #lobby_actions for evaluation.
 * In case reading funtion from socket returns 0 (which means the client disconnected)
 * housekeeping is done on that player and in case there are no more players, game thread
 * exits.
 *
 * @return TRUE if reading went fine. FALSE otherwise.
 */
gboolean
read_socket (	GSocket *socket,
                GIOCondition condition,
               	gpointer user_data);

#endif
