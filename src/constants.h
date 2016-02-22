/**
 * @file constants.h
 * Define statements to set constants values. This file is included in every
 * other source file.
 *
 * @author Leonardo Arcari
 * @date Feb 2016
 */

/**
 * Default size for socket message
 */
#define BUFFER_SIZE 1024
/**
 * Time (in seconds) between 2 extraction
 */
#define EXT_INTERVAL 5
/**
 * Conversion factor from ms to s
 */
#define SEC_MILLIS 1000
/**
 * Max number of extractions
 */
#define MAX_NUM 90
/**
 * Separator between OP_CODE and message content in a _socket_message
 */
#define MESSAGE_SEPARATOR "::"
/**
 * VPS address
 */
#define SERVER_ADDRESS "51.254.220.123"
/**
 * Listening port on VPS
 */
#define SERVER_PORT 1723
// #define SERVER_ADDRESS "localhost"
// #define SERVER_PORT 3456

/**
 * CSS file path
 */
#define CSS_FILE "ui/rules.css"
/**
 * XML file path
 */
#define UI_FILE "ui/main_app.ui"

/* UI Constants */
/**
 * Number of rows on a game table
 */
#define TABLE_ROWS 3
/**
 * Number of columns on a game table
 */
#define TABLE_COLUMNS 5
/**
 * Number of game tables for each player
 */
#define TABLES_N 3
/**
 * Reset time to the default animated gif after switching to another one
 */
#define CHANGE_GIF_TIMEOUT 5000 //ms
/**
 * Number of gif files loaded
 */
#define GN 7
/**
 * Waiting gif file path
 */
#define WAITING_GIF "ui/waiting_something_200.gif"
/**
 * Ambo gif file path
 */
#define AMBO_GIF "ui/ambo_200.gif"
/**
 * Terna gif file path
 */
#define TERNA_GIF "ui/terna_200.gif"
/**
 * Quaterna gif file path
 */
#define QUATERNA_GIF "ui/quaterna_200_2.gif"
/**
 * Cinquina gif file path
 */
#define CINQUINA_GIF "ui/cinquina_200.gif"
/**
 * Tombola gif file path
 */
#define TOMBOLA_GIF "ui/tombola.gif"
/**
 * Losing gif file path
 */
#define LOSE_GIF "ui/lose.gif"


/* Message type string code */

/**
 * String representation of PLAIN msg type. Check #msg_types_t
 */
#define PLAIN_CODE          "0000"
/**
 * String representation of CREATE_GAME msg type. Check #msg_types_t
 */
#define CREATE_GAME_CODE    "0001"
/**
 * String representation of JOIN_GAME msg type. Check #msg_types_t
 */
#define JOIN_GAME_CODE      "0010"
/**
 * String representation of START_GAME msg type. Check #msg_types_t
 */
#define START_GAME_CODE     "0011"
/**
 * String representation of END_GAME msg type. Check #msg_types_t
 */
#define END_GAME_CODE       "0100"
/**
 * String representation of CLIENT_READY msg type. Check #msg_types_t
 */
#define CLIENT_READY_CODE   "0101"
/**
 * String representation of AMBO msg type. Check #msg_types_t
 */
#define AMBO_CODE           "0110"
/**
 * String representation of TERNA msg type. Check #msg_types_t
 */
#define TERNA_CODE          "0111"
/**
 * String representation of QUATERNA msg type. Check #msg_types_t
 */
#define QUATERNA_CODE       "1000"
/**
 * String representation of CINQUINA msg type. Check #msg_types_t
 */
#define CINQUINA_CODE       "1001"
/**
 * String representation of TOMBOLA msg type. Check #msg_types_t
 */
#define TOMBOLA_CODE        "1010"
/**
 * String representation of EXTRACTED msg type. Check #msg_types_t
 */
#define EXTRACTED_CODE      "1011"

/* Message standard answer */

/**
 * Message content for positive answer from server on CREATE_GAME request
 */
#define GAME_CREATED "gc"
/**
 * Message content for positive answer from server on JOIN_GAME request
 */
#define GAME_JOINED "gj"
/**
 * Message content for negative answer from server on JOIN_GAME request
 */
#define GAME_NO_EXIST "nil"

/* Texts */

/**
 * Text string for _launcher_references#new_game_bt
 */
#define CREATE_BT "Create"
/**
 * Text string for _launcher_references#join_game_bt
 */
#define JOIN_BT "Join"
/**
 * Text string for _launcher_references#new_game_title in case user pressed
 * _launcher_references#new_game_bt
 */
#define NEW_GAME_TITLE "Create a new game by choosing its name"
/**
 * Text string for _launcher_references#new_game_title in case user pressed
 * _launcher_references#join_game_bt
 */
#define JOIN_GAME_TITLE "Join an existing game by choosing its name"
/**
 * Text string for _launcher_references#new_game_answer
 */
#define NEW_GAME_ANSWER_NEGATIVE "Game doesn't exist"
/**
 * Text string for _launcher_references#wait_players_label in case user is the
 * game master
 */
#define WAIT_GAME_LABEL_MASTER "Wait for other players to join or start the game"
/**
 * Text string for _launcher_references#wait_players_label in case user is waiting
 * for game master to start the match
 */
#define WAIT_GAME_LABEL_JOINER "Wait for the game master to start the game"
/**
 * Text string to append to _gameplay_references#server_tv after a new extraction
 */
#define EXT_MODEL "New number extracted!    "
/**
 * Text string of radio buttons in _gameplay_references#action_dialog (part 1)
 */
#define WIN_MESSAGE_MODEL "##-th row of table ##: "
/**
 * Text string of radio buttons in _gameplay_references#action_dialog (part 2)
 */
#define PICKED_NUMS_MODEL "{DD, DD, DD, DD, DD}"
/**
 * Text string to append to _gameplay_references#server_tv if numbers sent for
 * evaluation haven't been extracted yet.
 */
#define NO_WIN "Sorry mate! Numbers you sent haven't been extracted yet!"
/**
 * Text string to append to _gameplay_references#server_tv if AMBO has been granted
 */
#define WIN_AMBO "Ci siamo! Ambo!"
/**
 * Text string to append to _gameplay_references#server_tv if TERNA has been granted
 */
#define WIN_TERNA "Vai forte! Terna!"
/**
 * Text string to append to _gameplay_references#server_tv if QUATERNA has been granted
 */
#define WIN_QUATERNA "Congratulazioni! Quaterna!"
/**
 * Text string to append to _gameplay_references#server_tv if CINQUINA has been granted
 */
#define WIN_CINQUINA "Booom!! Grande Cinquina!"
/**
 * Text string to append to _gameplay_references#server_tv if TOMBOLA has been granted
 */
#define WIN_TOMBOLA "Tombola! Partita finita!"
/**
 * Answer from client to server after a END_GAME message has been received
 */
#define READ_MSG "_gotcha_"
/**
 * Text string to append to _chat_references#chat_tv
 */
#define CHAT_YOU "You: "
/**
 * Text string of radio buttons in _launcher_references#end_game_msg in case of
 * player won
 */
#define END_GAME_YOU_WON "You won!"
/**
 * Text string of radio buttons in _launcher_references#end_game_msg in case of
 * opponent players won
 */
#define END_GAME_YOU_LOST "The winner is:"

/* Styles */

/**
 * Widget's name for CSS styling (number un-picked on game table)
 */
#define TABLE_LABEL_UNPICKED "table-label-unpicked"
/**
 * Widget's name for CSS styling (number picked on game table)
 */
#define TABLE_LABEL_PICKED "table-label-picked"
