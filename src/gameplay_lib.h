/**
 * @file gameplay_lib.h
 *
 * Functions and data structures for the game logic. Operative codes, match abstraction
 * and game actions' emulation are located here.
 *
 * @author Leonardo Arcari
 * @date Feb. 2016
 */

#ifndef _GAMEPLAY_LIB_H
#define _GAMEPLAY_LIB_H
#include "constants.h"
#include "utility_lib.h"
#include <glib.h>
#include <gio/gio.h>
#include <strings.h>

/**
 * #msg_types_t is an enum of types of message sent in the socket. These codes
 * identify the goal of the message, so it's possible to interpret the message's
 * content. <br>
 * Communication between server-client is made possible by sending a string in
 * the following format: <br>
 * `OP_CODE` #MESSAGE_SEPARATOR `MESSAGE_CONTENT` <br>
 * As string representation of OP_CODE, natural binary coding has been chosen.
 * Here's a short description of them: <br>
 * Enum Name    | String representation | Description
 * ------------ | --------------------- | --------------
 * PLAIN        | 0000                  | Chat message
 * CREATE_GAME  | 0001                  | Create a new game room. Message content must have username and game name separated by a comma
 * JOIN_GAME    | 0010                  | Join a game room. Message content must have username and game name separated by a comma
 * START_GAME   | 0011                  | Start a game. Will be sent to all players in the same room
 * END_GAME     | 0100                  | Quit a game. Usually sent after a tombola has been granted
 * CLIENT_READY | 0101                  | Placeholder. Not used.
 * AMBO         | 0110                  | Ambo: 2 numbers on the same row
 * TERNA        | 0111                  | Terna: 3 numbers on the same row
 * QUATERNA     | 1000                  | Quaterna: 4 numbers on the same row
 * CINQUINA     | 1001                  | Cinquina: 5 numbers on the same row
 * TOMBOLA      | 1010                  | Tombola: all numbers in a table
 * EXTRACTED    | 1011                  | New number extracted
 */
typedef enum {  PLAIN,          //0000
                CREATE_GAME,    //0001
                JOIN_GAME,      //0010
                START_GAME,     //0011
                END_GAME,       //0100
                CLIENT_READY,   //0101
                AMBO,           //0110
                TERNA,          //0111
                QUATERNA,       //1000
                CINQUINA,       //1001
                TOMBOLA,        //1010
                EXTRACTED       //1011
                }
msg_types_t;

/**
 * Data structure to store informations about an extraction session. It's supposed
 * to be accessed by following functions:
 * - #init_numbers
 * - #extract
 * - #print_numbers
 * - #sort_extracted_nums
 */
struct _numbers {
    gint num[MAX_NUM]; /**< Array of integers [1, 90] yet to be extracted */
    gint left; /**< How many numbers haven't been extracted yet */
    gint extracted[MAX_NUM]; /**< Extracted numbers */
    GRand *_rand_; /**< Reference to object for random numbers generation */
};

/**
 * Define type for _numbers struct
 */
typedef struct _numbers numbers_t;

/**
 * Description of a message going through the socket.
 */
struct _socket_message {
	msg_types_t msg_type; /**< Message type. @see msg_types_t*/
	gchar *msg; /**< Message content. */
};

/**
 * Define type for _socket_message struct
 */
typedef struct _socket_message socket_message_t;

/**
 * Abstract representation of a game (match). When a game master starts a game
 * a new _game_instance is created and a new thread to handle it is run
 */
struct _game_instance {
    guint extraction_routine_id; /**< ID of extraction routine function */
	GMainLoop *loop; /**< Game loop */
    numbers_t *numbers; /**< Extracted numbers */
	GList *players; /**< GList of _player playing this _game_instance */

};

/**
 * Define type for _game_instance struct
 */
typedef struct _game_instance game_instance_t;

/**
 * Representation of a connection through a socket. This struct is used to collect
 * references to objects that manage client-server networking
 */
struct _socket_connection {
	GSocketConnection *connection; /**< Pointer to a GSocketConnection */
    GSocket *socket; /**< Pointer to a GSocket */
	GSource *socket_source; /**< Pointer to a socket GSource attachable to an event loop */
};

/**
 * Define type for _socket_connection struct
 */
typedef struct _socket_connection socket_connection_t;

/**
 * Representation of a player. This struct collects informations to a client connecting
 * to the server in order to access to its facilities fast.
 */
struct _player{
    socket_connection_t *socket_connection; /**< Pointer to a _socket_connetion struct */
    gchar *room; /**< Player's game room */
    gchar *username; /**< Player's username */
};

/**
 * Define type for _player struct
 */
typedef struct _player player_t;

/**
 * Utility function to check if numbers the client sent for evaluation have already
 * been extracted.
 *
 * @param gi Pointer to _game_instance struct
 * @param client_nums An array of string representation of numbers sent by the client
 * @param size number of elements in @p client_nums
 * @return TRUE if they all have been extracted, FALSE otherwise.
 */
gboolean
_check_client_nums (game_instance_t *gi,
                    gchar **client_nums,
                    gint size);

/**
 * Comparison function to check whether two #player_t are the same one. This will
 * check if @p a 's _socket_connection#socket is the same of @p b
 *
 * @param a Pointer to _player struct
 * @param b Pointer to a GSocket object
 * @return 0 if they match. A meaning-less integer otherwise.
 */
gint
_compare_players (  gconstpointer a,
                    gconstpointer b);

/**
 * Callback function to handle a `END_GAME` message.<p>
 * A `END_GAME` message is sent to each player waiting for him to
 * answer back. Then his data structures are freed and connection is closed. <p>
 * After all players have been drop, the Main event loop is quit and game thread
 * exits.
 *
 * @param gi Pointer to _game_instance struct
 * @param player Pointer to winning #player_t
 */
void
_game_loop_end_game (   game_instance_t *gi,
                        player_t *player);

/**
 * Callback function to handle the following messages:
 * -  `AMBO`
 * -  `TERNA`
 * -  `QUATERNA`
 * -  `CINQUINA`
 * -  `TOMBOLA`
 * <p>The set on numbers sent by the client is checked and he's told whether numbers
 * have been extracted yet or not, granting or denying his win.
 *
 * @param gi Pointer to game_instance struct
 * @param socket_message Pointer to _socket_message struct
 * @param player Pointer to _player struct of the user who sent the set of numbers.
 * @return Number of bytes sent to the client
 */
gint
_game_loop_ev_win ( game_instance_t *gi,
                    socket_message_t *socket_message,
                    player_t *player);

/**
 * Timeout function to be added to G Event loop. It's responsable for extracting
 * a number from a _numbers struct and sending it to every player in the game. <p>
 * It's called every #EXT_INTERVAL seconds until all 90 numbers have been extracted.
 *
 * @param user_data Pointer to a _game_instance struct
 * @return TRUE if there are numbers left to be extracted. FALSE otherwise.
 */
gboolean
_game_loop_extraction (gpointer user_data);

/**
 * Callback function to handle an incoming chat message. Message is sent to every
 * player but the one who wrote it.
 *
 * @param game_instance Pointer to a _game_instance struct
 * @param message String representing the chat message's text
 * @param player Pointer to the _player struct who sent the message in first place.
 * @return Number of client who couldn't receive the message (hence disconnected).
 */
gint
_game_loop_plain (  game_instance_t *game_instance,
                    gchar *message,
                    player_t *player);

/**
 * Comparison function between integers. It's used in the quicksort algorithm to keep
 * _numbers#num array ordered and to move extracted numbers at the end of it
 *
 * @return 1 if @p _a > @p _b, 0 if they're equal, -1 otherwise
 */
int _numbers_comp ( const void * _a,
                    const void * _b);

/**
 * Utility function to register every players' socket as source to check on G_IO_IN in the
 * G Main loop. Should be run once at the beginning of the game.
 *
 * @param game_instance Pointer to a _game_instance struct
 */
void
_setup_socket_sources ( game_instance_t* game_instance);

/**
 * Utility function. It allocates a new string following _socket_message structures.
 *
 * @param message String of socket message's content
 * @param op_code String representation of a #msg_types_t
 * @return A new-ly allocated string
 */
gchar * create_message (gchar *message,
                        gchar *op_code);

/**
 * Initializes _numbers struct
 *
 * @param numbers Pointer to a _numbers struct
 */
void
init_numbers(numbers_t *numbers);

/**
 * Utility function to print the current state of a _numbers struct
 *
 * @param numbers Pointer to a _numbers struct
 */
void
print_numbers(numbers_t *numbers);

/**
 * Sort _numbers#extracted numbers with a quicksort algorithm
 *
 * @param numbers Pointer to a _numbers struct
 */
void
sort_extracted_nums(numbers_t *numbers);

/**
 * Extract a number from _numbers. The extracted one is moved at the end of
 * _numbers#num array
 *
 * @param numbers Pointer to a _numbers struct
 */
gint
extract(numbers_t *numbers);

/**
 * Evaluates a message string read from the socket. String gets tokenized in
 * substrings and interpreted according to _socket_message format. First token
 * is the `OPERATIVE CODE` and it's compared with those described in #msg_types_t.
 * <p> If recognized, @p socket_msg struct is filled with proper values.
 *
 * @param socket_msg A pointer to a _socket_message struct where
 * _socket_message#msg field contains the string to be evaluated
 * @return `TRUE` if `OP_CODE` is recognized. `FALSE` otherwise.
 */
gboolean
evaluate_message (socket_message_t *socket_msg);

/**
 * Thread function to actually start a new game. This
 * thread function is usually called by main_server.h as a result of a game master who
 * asked to start a new game.
 * #_setup_socket_sources is invoked and extraction routine is started.
 *
 * @param data Pointer to a _game_instance struct
 */
gpointer
game_loop (gpointer data);

/**
 * Function to be invoked from #thread_read_socket to answer a request from client.
 * After evaluating message string, a proper choice is taken:
 * - If a chat message, it's sent to other players (through #_game_loop_plain)
 * - If a end-game message #_game_loop_end_game is invoked
 * - If a win message, the sent numbers set is checked through #_game_loop_ev_win
 *
 * @param game_instance Pointer to a _game_instance struct
 * @param socket_message Pointer to a _socket_message struct where
 * _socket_message#msg field contains the string to be evaluated
 * @param player Pointer to _player struct of the one who sent the message in first place.
 */
void
game_loop_actions ( game_instance_t *game_instance,
                    socket_message_t *socket_message,
                    player_t *player);

/**
 * Callback function to be invoked when G_IO_IN condition is satisfied on a socket.
 * Message is read from socket, normalized and sent to #game_loop_actions for evaluation.
 * In case reading funtion from socket returns 0 (which means the client disconnected)
 * housekeeping is done on that player and in case there are no more players, game thread
 * exits.
 *
 * @return TRUE if reading went fine. FALSE otherwise.
 */
gboolean
thread_read_socket (    GSocket *socket,
                        GIOCondition condition,
                        gpointer user_data);

#endif
