/**
 * @file main_app_controller.h
 *
 * This contains the prototypes and data structures for client application.
 * Structures are useful to keep a reference to every GtkWidget or data that
 * you need going through the execution flow.
 *
 * @author Leonardo Arcari
 * @date Feb 2016
 */

#ifndef _MAIN_APP_CONTROLLER_H
#define _MAIN_APP_CONTROLLER_H
#include "constants.h"
#include "gameplay_lib.h"
#include <gtk/gtk.h>
#include <stdio.h>

/**
 * Enum type for a better readability of animated images loading code. Since
 * graphical resources are loaded at the same time, a place for saving references
 * was needed to be able to access to them. An array was chosen and indexes
 * work as keys to access to gifs' references. For better readability enum type
 * was a choice instead of plain integers.
 */
typedef enum {  WAITING_PIX,
                AMBO_PIX,
                TERNA_PIX,
                QUATERNA_PIX,
                CINQUINA_PIX,
                TOMBOLA_PIX,
                LOSE_PIX
                }
animated_pixbuf_index_t;

/**
 * Keeps references to GtkWidgets related to the multiplayer chat section of UI.
 */
struct _chat_references
{
    GtkWidget *chat_label;  /**< Displays a message to user */
    GtkWidget *chat_tv;     /**< GtkTextView for showing chat messages */
    GtkWidget *chat_entry;  /**< GtkEntry for typing a chat message */
};

/**
 * Define type for _chat_references struct
 */
typedef struct _chat_references chat_references_t;

/**
 * Keeps references to:
 * - GtkWidgets related to the gameplay section of UI. That means everything
 *   the user interacts with and that involves the game logic
 * - A useful GList to store numbers the user chose to communicate to the server
 *   to be evaluated.
 */
struct _gameplay_references
{
    GtkWidget *action_buttons[5];   /**< Array of 5 GtkButton. They let the user
                                     *   to communicate different type of wins. */
    GtkWidget *game_tables_nb;      /**< GtkNotebook to let the user to switch
                                     *   between tables. */
    GtkWidget *game_tables[3];      /**< Array of 3 GtkGrid. By default the user is
                                     *   granted with 3 tables. Grid cells will be
                                     *   filled with GtkLabel with numbers. */
    GtkWidget *main_grid;           /**< Client main GtkGrid. Implemented to give
                                     *   5/6 of window width to gameplay widgets
                                     *   and 1/6 to chat section */
    GtkWidget *gameplay_grid;       /**< Gameplay section GtkGrid. Contains server_tv,
                                     *   game_tables_nb and action_buttons */
    GtkWidget *server_tv;           /**< A GtkTextView widget for showing messages
                                     *   from the server such as extracted numbers or wins. */
    GtkWidget *action_dialog;       /**< Popup dialog to let user to choose the
                                     *   combination of picked numbers to send to
                                     *   the server to be evaluated */
    GtkWidget *gameplay_stack;      /**< Gameplay GtkStack */
    GtkWidget *numbers_table;       /**< GtkGrid for showing extracted numbers */
    GtkWidget *grandma_gif;         /**< GtkImage widget */
    GdkPixbufAnimation *gifs[GN];   /**< Array of GdkPixbufAnimation references */
    GList *selected_n_set;          /**< GList containing numbers selected to be
                                     *   sent to the server to be evaluated */
};

/**
 * Define type for _gameplay_references struct
 */
typedef struct _gameplay_references gameplay_references_t;

/**
 * Keeps references to Launcher widgets. Launcher is the part of client application
 * that drives the user through the pre-game part of execution flow. So everything
 * from creating or joining a game, setting up username and game name to actually
 * start it is handled by the Launcher. Obviously, it's the first UI part showed to
 * the user when he launches the client.
 */
struct _launcher_references
{
    GtkWidget *launcher_box;        /**< GtkBox container for launcher homescreen */
    GtkWidget *new_game_bt;         /**< GtkButton to create a new game. */
    GtkWidget *join_game_bt;        /**< GtkButton to join an existing game. */
    GtkWidget *back_button;         /**< GtkButton to go back to home screen */
    GtkWidget *new_game_box;        /**< GtkBox container for new/join game view. */
    GtkWidget *new_game_title;      /**< GtkLabel to show user a new/join game view 's message. */
    GtkWidget *username_entry;      /**< GtkEntry to type your username. */
    GtkWidget *game_entry;          /**< GtkEntry to type new game name. */
    GtkWidget *new_game_answer;     /**< GtkLabel for server answer */
    GtkWidget *send_game_name;      /**< GtkButton to send username and game-name to server. */
    GtkWidget *waiting_box;         /**< GtkBox container for waiting-for-players game view. */
    GtkWidget *wait_players_label;  /**< GtkLabel to show waiting-for-players game view's message. */
    GtkWidget *waiting_spinner;     /**< GtkSpinner waiting spinner. */
    GtkWidget *start_game_bt;       /**< GtkButton to send to server a start game request. */
    GtkWidget *win_grandma_gif;     /**< GtkImage widget for winning gif */
    GtkWidget *end_game_box;        /**< GtkBox container for end-game's view */
    GtkWidget *end_game_msg;        /**< GtkLabel for winning message */
    GtkWidget *end_game_winner;     /**< GtkLabel for winner's name */
};

/**
 * Define type for _launcher_references struct
 */
typedef struct _launcher_references launcher_references_t;

/**
 * Picked number object description. Describes a number clicked by the player
 * in a table.
 */
struct _picked_number
{
    gint number;    /**< Value of picked number. */
    gint row;       /**< Row index of picked number. */
    gint table;     /**< Table index (in _gameplay_references#game_tables_nb)
                     *   of picked nuber. */
};

/**
 * Define type for _picked_number struct
 */
typedef struct _picked_number picked_number_t;

/**
 * Application headquarter. <p>
 * It contains references to everything the application needs at runtime. Using
 * a single structure to handle references is a choice made necessary by G*
 * callback functions prototypes that allow only one parameter as data. This way
 * accessing multiple application references is possible by a single callback (like
 * updating multiple ui fields in response to an event).
 */
struct _interface_references
{
    GtkWidget               *main_stack;            /**< Application window GtkStack. */
    GtkWidget               *main_window;           /**< Application window. */
    GtkWidget               *header_bar;            /**< Application GtkHeaderBar */
    GtkWidget               *gameplay_stckswc;      /**< Gameplay GtkStack Switcher */
    GList                   *picked_numbers[3];     /**< GList to store _picked_number
                                                     *   objects for each table. */
    gint                    win_rows[9][2];         /**< Matrix to keep track whether
                                                     *   a row of a table has already
                                                     *   been part of a win. */
    gint                    temp_win_row [2];       /**< Temporary win row index (#table,
                                                     *   #row). Keeping it while waiting
                                                     *   for server to evaluate it. */
    GSocket                 *socket;                /**< GSocket between client and server. */
    gchar                   *username;              /**< Player username typed in
                                                     *   _launcher_references#username_entry */
    chat_references_t       *chat_references;       /**< Instance of #chat_references_t */
    launcher_references_t   *launcher_references;   /**< Instance of #launcher_references_t */
    gameplay_references_t   *gameplay_references;   /**< Instance of #gameplay_references_t */
};

/**
 * Define type for _interface_references struct
 */
typedef struct _interface_references interface_references_t;

/**
 * Utility function to populate _gameplay_references#game_tables_nb with tables
 * of numbers. <p>
 * Numbers are generated randomly through #extract function in gameplay_lib.h
 * converted to strings and set as text to GtkLabel in _gameplay_references#game_tables
 *
 * @param interface_references Pointer to #interface_references_t struct
 */
void
_add_game_tables(interface_references_t *interface_references);

/**
 * Utility function to populate _gameplay_references#numbers_table with numbers
 * from 1 to 90. They're set as text of GtkLabel widgets.
 *
 * @param ir Pointer to #interface_references_t struct
 */
void
_add_numbers_table (interface_references_t *ir);
/**
 * Utility function to append a `gchar *` (string) to the GtkTextBuffer
 * of a GtkTextView. <br>
 * @p message memory is not freed inside this function.
 *
 * @param tv Pointer to GtkTextView to append a @p message to.
 * @param message String to append.
 */
void
_append_text_view ( GtkTextView *tv,
                    gchar *message);

/**
 * Utility function to build the string message to show to the user when he presses
 * one of the _gameplay_references#action_buttons. A popup dialog will come up letting
 * user to choose among possible win sequences to send to server to be evaluated. <p>
 * This function checks whether a row with picked numbers has already been sent and
 * confirmed or not. If so, that row will be skipped. <p>
 * The returned string is in the form of <br>
 * `%d-th row of table %d: {%d, %d, ...}` <br>
 *
 * @param ir Pointer to #interface_references_t struct
 * @param list GList of _picked_number. Possibly one of _interface_references#picked_numbers
 * @param table row to build string for
 * @return A newly allocated string following above format. `NULL` @param row was already
 * confirmed for a win.
 */
gchar *
_build_possible_win_string (interface_references_t *ir,
                            GList *list,
                            gint row);
/**
 * Utility function to build a string composed of char representation of picked numbers
 * of a table separated by commas (,). String is built only if there are 15 picked
 * numbers in the currently selected table.
 *
 * @param ir Pointer to #interface_references_t struct
 * @return A newly allocate string with picked numbers. `NULL` if not every number
 * of the currently selected table is picked.
 */
gchar *
_build_tombola_string (interface_references_t *ir);

/**
 * Utility function to change the animated image source of _gameplay_references#grandma_gif
 * GtkImage widget.
 *
 * @param ir Pointer to #interface_references_t struct.
 * @param api #animated_pixbuf_index_t enum value of the new image to show.
 */
void
_change_current_gif (   interface_references_t *ir,
                        animated_pixbuf_index_t api);

/**
 * Idle function to be added to GTK Event Loop. It appends text to the chat GtkTextView.
 * After that it will be removed from the event loop. <br>
 * @p user_data must be an array of 2 `void *`. The first must be a Pointer
 * to #interface_references_t struct and the second must be a pointer to _socket_message.
 * They will be casted to correct types inside the function.
 *
 * @param user_data Array of two `void *`. See above.
 * @return `G_SOURCE_REMOVE`. It tells the GTK event loop to run this function only once.
 */
gboolean
_client_chat_message (gpointer user_data);

/**
 * Idle function to be added to the GTK Event Loop. It notifies the server that a
 * `END_GAME` message has been received and closes the client application.
 * @p user_data must be an array of 2 `void *`. The first must be a Pointer
 * to #interface_references_t struct and the second must be a pointer to _socket_message.
 * They will be casted to correct types inside the function.
 *
 * @param user_data Array of two `void *`. See above.
 * @return `G_SOURCE_REMOVE`. It tells the GTK event loop to run this function only once.
 */
gboolean
 _client_end_game (gpointer user_data);

 /**
  * Idle function to be added to GTK Event Loop. It appends text to the
  * _gameplay_references#server_tv GtkTextView informing the user on a newly extracted
  * number. <br>
  * @p user_data must be an array of 2 `void *`. The first must be a Pointer
  * to #interface_references_t struct and the second must be a string representation
  * of the extracted number.
  * They will be casted to correct types inside the function.
  *
  * @param user_data Array of two `void *`. See above.
  * @return `G_SOURCE_REMOVE`. It tells the GTK event loop to run this function only once.
  */
gboolean
_client_extracted (gpointer user_data);

/**
 * Idle function to be added to GTK Event loop. It evaluates the answer from server
 * after requesting to create/join a game.
 * - If player created a new game, the game master view is shown
 * - If player joined a game a proper waiting view is shown
 * - If player tried to join a not-existing game he's made aware of it.
 * <br>
 * @p user_data must be an array of 2 `void *`. The first must be a Pointer
 * to #interface_references_t struct and the second must be a pointer to _socket_message.
 * They will be casted to correct types inside the function.
 *
 * @param user_data Array of two `void *`. See above.
 * @return `G_SOURCE_REMOVE`. It tells the GTK event loop to run this function only once.
 */
gboolean
_client_new_game (gpointer user_data);

/**
 * Thread function to send a message to the server through the socket.
 * @p data must be an array of 2 `void *`. The first must be a Pointer
 * to #interface_references_t struct and the second must be a string (`char *`) to send.
 * They will be casted to correct types inside the function.
 *
 * @param data Array of two `void *`. See above.
 * @return `NULL`.
 * @note Return value from thread will be ignored by the calling thread.
 */
gpointer
_client_send_message (gpointer data);

/**
 * Thread function to send a possible win string (generated by #_build_possible_win_string )
 * to the server through the socket. The function evaluates the length of
 * _gameplay_references#selected_n_set GList from a _interface_references struct.
 * Then it builds a correct string message and sends it. See constants.h documentation
 * for an explanation of how message is formatted.
 * @p data must be a pointer to a _interface_references struct. It will be casted to
 * correct types inside the function.
 *
 * @param data Pointer to _interface_references struct
 * @return 'NULL'
 * @note Return value from thread will be ignored by the calling thread.
 */
gpointer
_client_send_win (gpointer data);

/**
 * Idle function to be added to GTK Event Loop. It will be called in response to a
 * START_GAME message from server. It resizes the main window to the gameplay size
 * and shows the gameplay user-interface.
 * @p data must be a pointer to a _interface_references struct. It will be casted to
 * correct types inside the function.
 *
 * @param data Pointer to _interface_references struct
 * @return `G_SOURCE_REMOVE`. It tells the GTK event loop to run this function only once.
 */
gboolean
_client_start_game (gpointer user_data);

/**
 * Idle function to be added to GTK Event Loop. It evaluates the message sent by
 * the server in response to a possible TOMBOLA message. If client sent already extracted
 * numbers the server will return a "0" in the message, granting the TOMBOLA. On the other
 * hand a NO_WIN message will be appended to the _gameplay_references#server_tv
 * and games keeps on.
 * @p user_data must be an array of 2 `void *`. The first must be a Pointer
 * to #interface_references_t struct and the second must be a pointer to _socket_message.
 * They will be casted to correct types inside the function.
 *
 * @param user_data Array of two `void *`. See above.
 * @return `G_SOURCE_REMOVE`. It tells the GTK event loop to run this function only once.
 */
gboolean
_client_tombola_result (gpointer user_data);

/**
 * Idle function to be added to GTK Event Loop. It builds a string to notify the
 * user of a win granted by the server (AMBO, TERNA, QUATERNA, CINQUINA). The built string
 * is appended to the _gameplay_references#server_tv.
 * @p user_data must be an array of 2 `void *`. The first must be a Pointer
 * to #interface_references_t struct and the second must be a pointer to _socket_message.
 * They will be casted to correct types inside the function.
 *
 * @param user_data Array of two `void *`. See above.
 * @return `G_SOURCE_REMOVE`. It tells the GTK event loop to run this function only once.
 */
gboolean
_client_win_result (gpointer user_data);

/**
 * Comparison function for the
 * [g_list_find_custom()](https://developer.gnome.org/glib/stable/glib-Doubly-Linked-Lists.html#g-list-find-custom/)
 * . It compares two _picked_number and returns whether they have the same _picked_number#number.
 *
 * @param a Pointer to _picked_number
 * @param b Pointer to _picked_number
 * @return `0` if @p a -> number == @p b -> number. A meaning-less value if not.
 */
gint
_compare_picked_number    ( gconstpointer a,
                            gconstpointer b);

/**
 * Utility function to connect callback functions to widgets' signals. Launcher's,
 * chat's and gameplay's widgets signals that react to user interactions are registered
 * here. Check implementation for a list of those.
 *
 * @param interface_references Pointer to a _interface_references struct
 */
void
_connect_signals(interface_references_t *interface_references);

/**
 * Utility function to create the popup dialog that will let the user choose the
 * numbers to send to server to be evaluated. It shows a set of GtkRadioButtons
 * listing strings built by #_build_possible_win_string <br>
 * The new GtkDialog reference is stored in _gameplay_references#action_dialog .
 *
 * @param ir Pointer to a _interface_references struct
 * @param possible_wins Pointer to a GList of strings (usually returned by
 * #_build_possible_win_string )
 * @return Pointer to the newly created GtkDialog (casted to `GtkWidget *`).
 */
GtkWidget *
_draw_action_dialog (   interface_references_t * ir,
                        GList *possible_wins);
/**
 * Utility function to count how many _picked_number in @p list are there
 * on a @p row .
 *
 * @param list Pointer to GList of _picked_number
 * @param row Row index to check [0,2]
 * @return Number of _picked_number items on @p row
 */
gint
_how_many_on_row (  GList *list,
                    gint row);

/**
 * Utility function to initialize an array of GList of _picked_number. Each
 * GList is set to `NULL` .
 *
 * @param picked_numbers Array of GList
 */
void
_init_picked_numbers (GList **picked_numbers);

/**
 * Utility function to initialize _interface_references#win_rows . As a initial
 * value, `-1` has been chosen. When a row of a table is granted for a win, its
 * index is set to `0` in _interface_references#win_rows
 *
 * @param a A _interface_references#win_rows array
 */
void
_init_win_rows (gint a[][2]);

/**
 * Utility function to load animated images from their files and store references
 * to them in _gameplay_references#gifs array.
 *
 * @param ir Pointer to a _interface_references struct
 */
void
_load_animated_pixbufs (interface_references_t *ir);

/**
 * Timeout function to be added to GTK Event loop. It resets the current animated
 * image to default idling one after 5 seconds.
 *
 * @param user_data Pointer to a _interface_references struct
 * @return `G_SOURCE_REMOVE`. It tells the GTK event loop to run this function only once.
 */
gboolean
_reset_waiting_pix (gpointer user_data);

/**
 * Utility function to extract selected numbers from a string built by
 * #_build_possible_win_string . Those numbers are stored in
 * _gameplay_references#selected_n_set GList. This is done to save a temporary
 * list to be sent to the server if the user request it. Additionally, table and row
 * indexes of those number is temporary stored in case the win will be granted by
 * the server. This way we can set corresponding indexes of _interface_references#win_rows
 * to `0`.
 *
 * @param ir Pointer to a _interface_references struct
 * @param radio_label String built by #_build_possible_win_string to extract numbers
 * from
 * @return Always `0`
 */
gint
_set_selected_n (   interface_references_t * ir,
                    gchar *radio_label);

/**
 * Callback function. Handles _gameplay_references#action_dialog `response` signal.
 * This signal is emitted when user clicks one of dialog action buttons. This function
 * checks which one was clicked. If `OK` starts a new thread executing #_client_send_win.
 * If `CANCEL` dialog widget is destroyed.
 *
 * @param dialog Pointer to GtkDialog widget
 * @param response_id ID of dialog clicked button
 * @param user_data Pointer to a _interface_references struct
 */
void
action_dialog_response_handler (GtkDialog *dialog,
                                gint       response_id,
                                gpointer   user_data);

/**
 * Callback function. Handles GApplication `activate` signal.<br>
 * This signal is emitted when the client is launched. This is the time the whole
 * user-interface is drawn (reading widgets' properties from a XML file)
 * and references to GtKWidgets are set to correct _interface_references fields.
 * Then all initializing functions are called and callback functions for widgets
 * are set. <br>
 * CALLED FUNCTIONS:
 * - #_init_picked_numbers
 * - #_init_win_rows
 * - #_add_game_tables
 * - #_connect_signals
 *
 * After that, styles for widgets are read from CSS file.
 *
 * @param application Pointer to GApplication
 * @param user_data Pointer to a _interface_references struct
 */
void
activate_handler (  GApplication    *application,
                    gpointer        user_data);

/**
 * Callback function. Handles _launcher_references#back_button `clicked` signal.<br>
 * Shows _launcher_references#launcher_box bringing user back to homescreen, then
 * hides itself.
 *
 * @param button Pointer to GtkButton widget
 * @param user_data Pointer to a _interface_references struct
 */
void
back_bt_handler (   GtkButton *button,
                    gpointer   user_data);

/**
 * Callback function. Handles GtkEntry `activate` signal.<br>
 * This signal is emitted when the user presses ENTER after focusing the GtkEntry.
 * If there's text inside the entry, a new PLAIN message string is built. This text
 * is a chat message so:
 * - Text is appended to client _chat_references#chat_tv through #_append_text_view
 * - A new thread executing #client_send_message is run
 * - GtkEntry's text is cleared
 *
 * @param entry Pointer to GtkEntry
 * @param user_data Pointer to a _interface_references struct
 */
void
chat_entry_handler (GtkEntry *entry,
                    gpointer  user_data);

/**
 * React to a new message from the server. After evaluating the message through
 * #evaluate_message, depending on the _socket_message#msg_type, an idle function
 * is added to the user-interface thread event-loop:
 * MESSAGE TYPE | IDLE FUNCTION
 * ------------ | -------------------
 * PLAIN        | #_client_chat_message
 * START_GAME   | #_client_start_game
 * END_GAME     | #_client_end_game
 * AMBO         | #_client_win_result
 * TERNA        | #_client_win_result
 * QUATERNA     | #_client_win_result
 * CINQUINA     | #_client_win_result
 * TOMBOLA      | #_client_tombola_result
 * EXTRACTED    | #_client_extracted
 *
 * @param interface_references Pointer to a _interface_references struct
 * @param socket_message Pointer to a _socket_message struct
 */
void
client_actions (interface_references_t *interface_references,
                socket_message_t* socket_message);

/**
 * Callback function. Handles _gameplay_references#action_buttons `clicked`
 * signal. This function is the beginning point of a macro-action: looking through
 * tables for possible win sequences for the GtkButton pressed related action. E.g.:
 * if user clicks TERNA action button this function searches for rows with 4 numbers
 * picked. Here's the execution flow:
 * 1. Detect the win type based on what GtkButton has been pressed.
 * 2. If it's not a Tombola action, look for `k` _picked_number on each row (through
 * #_how_many_on_row) and build string for GtkDialog's radio buttons (through
 * #_build_possible_win_string).
 * 3. If there are possible wins a _gameplay_references#action_dialog is created
 * (through #_draw_action_dialog) and showed
 * 4. If Tombola action button was pressed a TOMBOLA message string is built
 * (through #_build_tombola_string) and a thread executing #_client_send_message
 * is run.
 *
 * @param button Pointer to a GtkButton
 * @param user_data Pointer to a _interface_references struct
 */
void
client_send_win (   GtkButton *button,
                    gpointer   user_data);

/**
 * Callback function attached to the socket's GSource. When data is ready to be read
 * a signal is risen and this function handles it. A new string is allocated and socket's
 * data is copied into it. Then it's sent to #client_actions to be evaluated. <br>
 * According to GIO documentation, a 0 (zero) returned from a socket reading means that
 * the counter part has closed the connection. In that case socket is closed.
 *
 * @param socket Pointer to GSocket
 * @param condition GIOCondition id (this function handles a G_IO_IN)
 * @param user_data Pointer to a _interface_references struct
 */
gboolean
connection_handler (    GSocket *socket,
                        GIOCondition condition,
                        gpointer user_data);

/**
 * Utility function to create a _launcher_references#back_button widget if it's
 * not been created yet. In any case it will be shown.
 *
 * @param ir Pointer to a _interface_references struct
 */
void
create_back_bt (interface_references_t *ir);

/**
 * Callback function. It handles _launcher_references#join_game_bt `clicked` signal.
 * This shows a view responsable for prompting the user for username and game name.
 *
 * @param button Pointer to GtkButton
 * @param user_data Pointer to a _interface_references struct
 */
void
join_game_bt_handler (  GtkButton *button,
                        gpointer   user_data);

/**
 * Callback function. It handles entries' `delete` signal. Using this to disable
 * a send button if there's no text in username and game-name entries, or enabling
 * it if there's any.
 *
 * @param buffer Pointer to GtkEntryBuffer
 * @param position Position of the caret in the entry
 * @param n_chars Number of chars deleted
 * @param user_data Pointer to a _interface_references struct
 */
void
launcher_entries_delete_handler (   GtkEntryBuffer *buffer,
                                    guint           position,
                                    guint           n_chars,
                                    gpointer        user_data);

/**
 * Callback function. It handles entries' `insert` signal. Using this to disable
 * a send button if there's no text in username and game-name entries, or enabling
 * it if there's any.
 *
 * @param buffer Pointer to GtkEntryBuffer
 * @param position Position of the caret in the entry
 * @param chars String typed
 * @param n_chars Number of chars deleted
 * @param user_data Pointer to a _interface_references struct
 */
void
launcher_entries_insert_handler (   GtkEntryBuffer *buffer,
                                    guint           position,
                                    gchar          *chars,
                                    guint           n_chars,
                                    gpointer        user_data);

/**
 * Thread function to initialize a new event-loop for networking. Since we don't
 * want the UI to freeze when a new message is processed, a new event loop is run
 * besides the user-interface one. A new connection to the server is enstablished
 * and a `Data is ready in the socket` condition is added to the loop.
 * Then #connection_handler is set as callback function to handle above event.
 * Finally we run the event loop.
 *
 * @param user_data Pointer to a _interface_references struct
 * @return `NULL`
 * @note Return value from thread will be ignored by the calling thread.
 */
gpointer
network_loop (gpointer user_data);

/**
 * Callback function. It handles the `clicked` signal emitted by the GTkEventBox
 * widget that contains tables' numbers.
 * In first place, the function collects number informations (int number, row and table
 * index) and packs them in a new _picked_number.
 * There are two cases here:
 * - The number already exists in its table's _interface_references#picked_numbers
 * list. It's removed and its label's background set to transparent.
 * - The number doesn't exist so it's added to the list and its background is set to
 * yellow.
 *
 * @param widget Pointer to GtkEventBox widget
 * @param event GdkEvent emitted
 * @param user_data Pointer to a _interface_references struct
 * @return Always `TRUE`
 * @bug Two fast clicks on the same @p widget doesn't change number's background
 * two times, so an un-picked numbers has still yellow background. Apparently
 * `gtk_widget_override_background_color` is added as idle function to GTK event loop
 * so it will be executed when no other "more important" signals have to be handled.
 */
gboolean
number_clicked_handler (GtkWidget   *widget,
                        GdkEvent    *event,
                        gpointer    user_data);

/**
 * Callback function. It handles _launcher_references#new_game_bt `clicked` signal.
 * This shows a view responsable for prompting the user for username and game name.
 *
 * @param button Pointer to GtkButton
 * @param user_data Pointer to a _interface_references struct
 */
void
new_game_bt_handler (   GtkButton *button,
                        gpointer   user_data);

/**
 * Callback function. It handles gameplay_references#action_dialog 's GtkRadioButtons
 * `toggled` signal. It invokes #_set_selected_n with toggled radiobutton label's text.
 *
 * @param togglebutton Pointer to GtkRadioButton
 * @param user_data Pointer to a _interface_references struct
 */
void
radio_bt_handler (  GtkToggleButton *togglebutton,
                    gpointer         user_data);

/**
 * Callback function. It handles client's `startup` signal. That signal is emitted
 * at application launch and setting operations can be done here. So a new thread
 * executing #network_loop is run.
 *
 * @param application Pointer to GApplication
 * @param user_data Pointer to a _interface_references struct
 */
void
startup_handler (   GApplication    *application,
                    gpointer        user_data);

/**
 * Callback function. It handles the _launcher_references#new_game_bt and
 * launcher_references#join_game_bt `clicked` signal. A new string is allocated
 * to ask the server to create or join a game. Message has the format: <br>
 * `OP_CODE::username,gamename` <br>
 * Then the string is sent to the server and a waiting-for-users view is shown.
 *
 * @param button Pointer to GtkButton
 * @param user_data Pointer to a _interface_references struct
 */
void
send_game_name_bt_handler (     GtkButton *button,
                                gpointer   user_data);

/**
 * Callback function. It handles the _launcher_references#start_game_bt `clicked`
 * signal. A START_GAME message is sent to the server.
 *
 * @param button Pointer to GtkButton
 * @param user_data Pointer to a _interface_references struct
 */
void
start_game_bt_handler ( GtkButton *button,
                        gpointer   user_data);

/**
 * Callback function. It handles the _gameplay_references#game_tables_nb
 * `switch-page` signal. It's emitted when user changes table to show. In response,
 * this function enables or disables Tombola action button depending on how many
 * _picked_number are there (In fact you can't call Tombola if you don't have 15
 * picked numbers).
 *
 * @param notebook Pointer to GtkNoteboox
 * @param page Pointer to GtkWidget shown
 * @param page_num New page number
 * @param user_data Pointer to a _interface_references struct
 */
void
switch_page_handler (   GtkNotebook *notebook,
                        GtkWidget   *page,
                        guint        page_num,
                        gpointer     user_data);
#endif
