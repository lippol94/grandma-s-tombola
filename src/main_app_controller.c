/**
 * Implementation of functions in main_app_controller.h
 * @see main_app_controller.h for their documentation (and notes)
 * on their implementation
 *
 * @author Leonardo Arcari
 * @date Feb 2016
 */

#include <gtk/gtk.h>
#include "main_app_controller.h"
#include "gameplay_lib.h"

void
_add_game_tables (interface_references_t *interface_references)
{

    for (int i = 0; i < TABLES_N; i++) { // Tables
        interface_references->gameplay_references->game_tables[i] = gtk_grid_new();
        gtk_grid_set_row_homogeneous(GTK_GRID(interface_references->gameplay_references->game_tables[i]), TRUE);
        gtk_grid_set_column_homogeneous(GTK_GRID(interface_references->gameplay_references->game_tables[i]), TRUE);
        numbers_t *numbers = g_new (numbers_t, 1);
        init_numbers(numbers);

        for (int s = 0; s < TABLE_ROWS * TABLE_COLUMNS; s++) {
            extract(numbers);
        } sort_extracted_nums(numbers);

        gint counter = 0;
        for (int j = 0; j < TABLE_COLUMNS; j++) { // Columns
            for (int k = 0; k < TABLE_ROWS; k++) { // Rows
                gint extracted = numbers->extracted[counter++];
                gchar *buffer = g_malloc(sizeof(gchar) * 3);
                g_ascii_dtostr(buffer, 3, extracted);
                GtkWidget *label = gtk_label_new(buffer);
                gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_CENTER);
                gtk_widget_set_margin_top (label, 5);
                gtk_widget_set_margin_start (label, 5);
                gtk_widget_set_margin_bottom (label, 5);
                gtk_widget_set_margin_end (label, 5);
                gtk_widget_set_name (label, TABLE_LABEL_UNPICKED);
                GtkWidget* event_box = gtk_event_box_new();
                gtk_widget_set_hexpand(event_box, TRUE);
                gtk_widget_set_vexpand(event_box, TRUE);
                gtk_event_box_set_visible_window (GTK_EVENT_BOX (event_box), TRUE);
                gtk_event_box_set_above_child (GTK_EVENT_BOX (event_box), TRUE);
                gtk_container_add(GTK_CONTAINER(event_box), label);
                g_signal_connect(event_box, "button-press-event", G_CALLBACK(number_clicked_handler), interface_references);
                gtk_grid_attach(GTK_GRID(interface_references->gameplay_references->game_tables[i]),
                                event_box,
                                j,
                                k,
                                1,
                                1);
            }
        }

        gchar* string = g_malloc(sizeof(gchar) * 10);
        sprintf(string, "Cartella %d", i+1);
        GtkWidget *tab_label = gtk_label_new(string);
        g_free(string);
        gtk_notebook_append_page(   GTK_NOTEBOOK(interface_references->gameplay_references->game_tables_nb),
                                    interface_references->gameplay_references->game_tables[i],
                                    tab_label);
    }
    gtk_widget_show_all(interface_references->gameplay_references->game_tables_nb);
}

void
_add_numbers_table (interface_references_t *ir)
{
    for (int i = 0; i < 90; i++) {
        gint num = i+1;
        gint row = i/10;
        gint column = i%10;
        gchar *buffer = g_malloc(sizeof(gchar) * 3);
        g_ascii_dtostr(buffer, 3, num);
        GtkWidget *label = gtk_label_new (buffer);
        gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_CENTER);
        gtk_widget_set_margin_top (label, 5);
        gtk_widget_set_margin_start (label, 5);
        gtk_widget_set_margin_bottom (label, 5);
        gtk_widget_set_margin_end (label, 5);
        gtk_grid_attach (   GTK_GRID (ir->gameplay_references->numbers_table),
                            label,
                            column,
                            row,
                            1,
                            1);
        g_free (buffer);
    }
    gtk_widget_show_all (ir->gameplay_references->numbers_table);
}

void
_append_text_view ( GtkTextView *tv,
                    gchar *message)
{
    GtkTextIter end_iter;
    GtkTextMark *end_mark = NULL;
    GtkTextBuffer *text_buffer = gtk_text_view_get_buffer (tv);
    gchar *tv_message = g_malloc ((strlen (message) + 2) * sizeof(gchar));
    strcpy (tv_message, message);
    strcat (tv_message, "\n");
    end_mark = gtk_text_buffer_get_insert(text_buffer);
    gtk_text_buffer_get_end_iter (text_buffer, &end_iter);
    gtk_text_buffer_insert (text_buffer,
                            &end_iter,
                            tv_message,
                            -1);
    gtk_text_buffer_get_end_iter (text_buffer, &end_iter);
    gtk_text_buffer_move_mark (text_buffer, end_mark, &end_iter);
    gtk_text_view_scroll_mark_onscreen (tv,
                                        end_mark);
    g_free (tv_message);

}

gchar *
_build_possible_win_string (interface_references_t *ir,
                            GList *list,
                            gint row)
{
    GList *l;
    picked_number_t *pn;
    gint table = ((picked_number_t *) list->data)->table;
    gchar *string = NULL;
    gint win_row_index = row + 3 * table;
    if (ir->win_rows[win_row_index][0] != 0) {
        string = g_malloc ((strlen (WIN_MESSAGE_MODEL) + strlen (PICKED_NUMS_MODEL)) * sizeof (gchar));
        sprintf (string, "%d-th row of table %d: {", row, table);
        for (l = list; l != NULL; l = l->next) {
            pn = (picked_number_t *) l->data;
            if (pn->row == row) {
                gchar *num = g_malloc (3 * sizeof(gchar));
                strcat (string, g_ascii_dtostr (num, 3, pn->number));
                g_print ("catted: %s\n", g_ascii_dtostr (num, 3, pn->number));
                strcat (string, ", ");
                g_free (num);
            }
        }
        gchar *comma = strrchr (string, ',');
        g_print ("Subbing: %c\n", comma[0]);
        comma[0] = '\0';
        strcat (string, "}");
        g_print ("Built: %s\n", string);
    }
    return string;
}

gchar *
_build_tombola_string (interface_references_t *ir)
{
    GList *l;
    gchar *message = NULL;
    gint current_page_index = gtk_notebook_get_current_page(GTK_NOTEBOOK(ir->gameplay_references->game_tables_nb));
    if (g_list_length (ir->picked_numbers[current_page_index]) == 15) {
        message = g_malloc (BUFFER_SIZE * sizeof(gchar));
        strcpy (message, TOMBOLA_CODE);
        strcat (message, MESSAGE_SEPARATOR);
        for (l = ir->picked_numbers[current_page_index]; l != NULL; l = l->next) {
            picked_number_t *pn = (picked_number_t *) l->data;
            gchar* num = g_malloc (3 * sizeof (gchar));
            strcpy (num, g_ascii_dtostr (num, 3, pn->number));
            strcat (message, num);
            strcat (message, ",");
            g_free (num);
        }
        gchar *comma = strrchr (message, ',');
        g_print ("Subbing: %c\n", comma[0]);
        comma[0] = '\0';
        g_print ("Tombola message: %s\n", message);
    } return message;
}

void
_change_current_gif (   interface_references_t *ir,
                        animated_pixbuf_index_t api)
{
    gtk_image_set_from_animation (  GTK_IMAGE (ir->gameplay_references->grandma_gif),
                                    ir->gameplay_references->gifs[api]);
    gdk_threads_add_timeout (   CHANGE_GIF_TIMEOUT,
                                (GSourceFunc) _reset_waiting_pix,
                                ir);
}

gboolean
_client_chat_message (gpointer user_data)
{
    void** data = (void **) user_data;
    interface_references_t *ir = (interface_references_t *) data[0];
    socket_message_t *socket_message = (socket_message_t *) data[1];

    _append_text_view ( GTK_TEXT_VIEW (ir->chat_references->chat_tv),
                        socket_message->msg);
    //g_free (socket_message);

    return G_SOURCE_REMOVE;
}

gboolean
_client_new_game (gpointer user_data)
{
    void **data = (void **) user_data;
    interface_references_t *ir = (interface_references_t *) data[0];
    socket_message_t *socket_message = (socket_message_t *) data[1];

    if (socket_message->msg_type == CREATE_GAME) {
        if (strcasecmp(socket_message->msg, GAME_CREATED) == 0) {
            gtk_label_set_text(GTK_LABEL(ir->launcher_references->wait_players_label), WAIT_GAME_LABEL_MASTER);
            gtk_spinner_start(GTK_SPINNER(ir->launcher_references->waiting_spinner));
            gtk_widget_destroy (ir->launcher_references->back_button);
            ir->launcher_references->back_button = NULL;
            gtk_stack_set_visible_child(GTK_STACK(ir->main_stack), ir->launcher_references->waiting_box);
        }
    } else {
        if (strcasecmp(socket_message->msg, GAME_JOINED) == 0) {
            gtk_label_set_text(GTK_LABEL(ir->launcher_references->wait_players_label), WAIT_GAME_LABEL_MASTER);
            gtk_widget_set_visible(ir->launcher_references->start_game_bt, FALSE);
            gtk_spinner_start(GTK_SPINNER(ir->launcher_references->waiting_spinner));
            gtk_widget_destroy (ir->launcher_references->back_button);
            ir->launcher_references->back_button = NULL;
            gtk_stack_set_visible_child(GTK_STACK(ir->main_stack), ir->launcher_references->waiting_box);
        } else {
            gtk_label_set_text( GTK_LABEL(ir->launcher_references->new_game_answer), NEW_GAME_ANSWER_NEGATIVE);
            gtk_entry_set_text( GTK_ENTRY(ir->launcher_references->game_entry), "");
            gtk_widget_set_sensitive (ir->launcher_references->game_entry, TRUE);
            gtk_widget_set_sensitive (ir->launcher_references->username_entry, TRUE);
        }
    }

    return G_SOURCE_REMOVE;

}

gboolean
_client_end_game (gpointer user_data)
{
    void** data = (void **) user_data;
    interface_references_t *ir = (interface_references_t *) data[0];
    socket_message_t *socket_message = (socket_message_t *) data[1];

    g_socket_send ( ir->socket,
                    READ_MSG,
                    strlen (READ_MSG),
                    NULL,
                    NULL);

    if (strcasecmp (socket_message->msg, ir->username) == 0) {
        gtk_label_set_text (GTK_LABEL (ir->launcher_references->end_game_msg),
                            END_GAME_YOU_WON);
        gtk_widget_hide (ir->launcher_references->end_game_winner);
        gtk_image_set_from_animation (  GTK_IMAGE (ir->launcher_references->win_grandma_gif),
                                        ir->gameplay_references->gifs[TOMBOLA_PIX]);
    } else {
        gtk_label_set_text (GTK_LABEL (ir->launcher_references->end_game_msg),
                            END_GAME_YOU_LOST);
        gtk_label_set_text (GTK_LABEL (ir->launcher_references->end_game_winner),
                            socket_message->msg);
        gtk_image_set_from_animation (  GTK_IMAGE (ir->launcher_references->win_grandma_gif),
                                        ir->gameplay_references->gifs[LOSE_PIX]);
    }

    gtk_container_remove (  GTK_CONTAINER (ir->header_bar),
                            ir->gameplay_stckswc);
    gtk_image_clear (GTK_IMAGE (ir->gameplay_references->grandma_gif));
    gtk_window_resize(GTK_WINDOW(ir->main_window), 667, 381);
    gtk_grid_set_column_homogeneous (GTK_GRID (ir->gameplay_references->main_grid), FALSE);
    gtk_grid_set_column_homogeneous (GTK_GRID (ir->gameplay_references->gameplay_grid), FALSE);
    gtk_stack_set_visible_child(GTK_STACK(ir->main_stack), ir->launcher_references->end_game_box);

    return G_SOURCE_REMOVE;
}

gboolean
_client_extracted (gpointer user_data)
{
    void **data = (void *) user_data;
    interface_references_t *ir = (interface_references_t *) data[0];
    gchar *ext_str = (gchar *) data[1];

    gchar *message = g_malloc ((strlen (EXT_MODEL) + 4) * sizeof (gchar));
    strcpy (message, EXT_MODEL);
    strcat (message, ext_str);
    _append_text_view ( GTK_TEXT_VIEW (ir->gameplay_references->server_tv),
                        message);

    gint ext_num = g_ascii_strtod (ext_str, NULL) - 1;
    gint row = ext_num / 10;
    gint column = ext_num % 10;
    GtkWidget *label = gtk_grid_get_child_at (  GTK_GRID (ir->gameplay_references->numbers_table),
                                                column,
                                                row);

    gtk_widget_set_name (label, TABLE_LABEL_PICKED);

    g_free (message);
    return G_SOURCE_REMOVE;
}

gpointer
_client_send_message (gpointer data)
{
    void** user_data = (void **) data;
    interface_references_t *ir = (interface_references_t *) user_data[0];
    gchar *message = (gchar *) user_data[1];

    g_socket_send ( ir->socket,
                    message,
                    strlen (message),
                    NULL,
                    NULL);

    g_free (message);
    g_free (user_data);
    g_thread_exit (NULL);
    return NULL;
}

gpointer
_client_send_win (gpointer data)
{
    interface_references_t *ir = (interface_references_t *) data;
    gchar *message = g_malloc (BUFFER_SIZE * sizeof (gchar));
    gint bytes_sent = -2;
    GList *l;
    switch (g_list_length(ir->gameplay_references->selected_n_set)) {
        case 2:
            strcpy (message, AMBO_CODE);
            break;
        case 3:
            strcpy (message, TERNA_CODE);
            break;
        case 4:
            strcpy (message, QUATERNA_CODE);
            break;
        case 5:
            strcpy (message, CINQUINA_CODE);
            break;
        default:
            g_print ("Error in g_list_length");
            break;
    } strcat (message, MESSAGE_SEPARATOR);
    g_print ("Message separator added\n");

    for (l = ir->gameplay_references->selected_n_set; l != NULL; l = l->next) {
        gchar *num_s = g_malloc (3 * sizeof (gchar));
        gint *num = (gint *) l->data;
        strcat (message, g_ascii_dtostr (num_s, 3, *num));
        if (l->next != NULL) {
            strcat (message, ",");
        }
        g_free (num_s);
    }

    bytes_sent = g_socket_send (ir->socket,
                                message,
                                strlen(message),
                                NULL,
                                NULL);
    g_print ("Win sent: %s \n", message);
    g_free (message);
    g_thread_exit (NULL);
    return NULL;
}

gboolean
_client_start_game (gpointer user_data)
{
    interface_references_t *ir = (interface_references_t *) user_data;

    gtk_spinner_stop(GTK_SPINNER(ir->launcher_references->waiting_spinner));
    ir->gameplay_stckswc = gtk_stack_switcher_new ();
    gtk_stack_switcher_set_stack (  GTK_STACK_SWITCHER (ir->gameplay_stckswc),
                                    GTK_STACK (ir->gameplay_references->gameplay_stack));
    gtk_container_add ( GTK_CONTAINER (ir->header_bar),
                        ir->gameplay_stckswc);
    gtk_widget_show_all (ir->header_bar);
    gtk_window_resize(GTK_WINDOW(ir->main_window), 1280, 720);
    gtk_image_set_from_animation (  GTK_IMAGE (ir->gameplay_references->grandma_gif),
                                    ir->gameplay_references->gifs[0]);
    gtk_grid_set_column_homogeneous (GTK_GRID (ir->gameplay_references->main_grid), TRUE);
    gtk_grid_set_column_homogeneous (GTK_GRID (ir->gameplay_references->gameplay_grid), TRUE);
    gtk_stack_set_visible_child(GTK_STACK(ir->main_stack), ir->gameplay_references->main_grid);

    return G_SOURCE_REMOVE;
}

gboolean
_client_tombola_result (gpointer user_data)
{
    g_print ("Client_tombola_result\n");
    void **data = (void *) user_data;
    interface_references_t *ir = (interface_references_t *) data[0];
    socket_message_t *socket_message = (socket_message_t *) data[1];

    if (strcmp (socket_message->msg, "0") == 0) {
        g_print ("%s\n", WIN_TOMBOLA);
        //gtk_widget_destroy (ir->main_window);

        /* Send end game signal */
        gchar *end_signal = g_malloc ((strlen (END_GAME_CODE) + strlen (MESSAGE_SEPARATOR) + 2) * sizeof(gchar));
        strcpy (end_signal, END_GAME_CODE);
        strcat (end_signal, MESSAGE_SEPARATOR);
        strcat (end_signal, "0");
        g_socket_send ( ir->socket,
                        end_signal,
                        strlen(end_signal),
                        NULL,
                        NULL);
        g_free (end_signal);
    } else {
        for (int i = 0; i < 5; i++) {
            gtk_widget_set_sensitive(ir->gameplay_references->action_buttons[i], TRUE);
        }
        _append_text_view ( GTK_TEXT_VIEW (ir->gameplay_references->server_tv),
                            NO_WIN);
    }

    return G_SOURCE_REMOVE;
}

gboolean
_client_win_result (gpointer user_data)
{
    void **data = (void *) user_data;
    interface_references_t *ir = (interface_references_t *) data[0];
    socket_message_t *socket_message = (socket_message_t *) data[1];
    gchar *win_string = NULL;

    if (strcmp(socket_message->msg, "0") == 0) {
        switch (socket_message->msg_type) {
            case AMBO:
                _append_text_view ( GTK_TEXT_VIEW (ir->gameplay_references->server_tv),
                                    WIN_AMBO);
                _change_current_gif (ir, AMBO_PIX);
                break;
            case TERNA:
                _append_text_view ( GTK_TEXT_VIEW (ir->gameplay_references->server_tv),
                                    WIN_TERNA);
                _change_current_gif(ir, TERNA_PIX);
                break;
            case QUATERNA:
                _append_text_view ( GTK_TEXT_VIEW (ir->gameplay_references->server_tv),
                                    WIN_QUATERNA);
                _change_current_gif(ir, QUATERNA_PIX);

                break;
            case CINQUINA:
                _append_text_view ( GTK_TEXT_VIEW (ir->gameplay_references->server_tv),
                                    WIN_CINQUINA);
                _change_current_gif(ir, CINQUINA_PIX);

                break;
            default:
                g_print ("Un-handled scenario. How did you get here?\n");
                break;
        }
        ir->win_rows[ir->temp_win_row[0]][0] = 0;
        ir->win_rows[ir->temp_win_row[0]][0] = 0;
    } else {
        _append_text_view ( GTK_TEXT_VIEW(ir->gameplay_references->server_tv),
                            NO_WIN);
    }
    gtk_widget_destroy (ir->gameplay_references->action_dialog);
    return G_SOURCE_REMOVE;
}

gint
_compare_picked_number    ( gconstpointer a,
                            gconstpointer b)
{
    picked_number_t *list_data = (picked_number_t *) a;
    picked_number_t *user_data = (picked_number_t *) b;

    if ((list_data->number == user_data->number) && (list_data->row == user_data->row))
        return 0;
    else
        return -1; // meaning-less number
}

void
_connect_signals (interface_references_t *interface_references)
{
    /* Chat References Signals */
    g_signal_connect(   interface_references->chat_references->chat_entry,
                        "activate",
                        G_CALLBACK(chat_entry_handler),
                        interface_references);
    /* Launcher References Signals */
    g_signal_connect(   interface_references->launcher_references->join_game_bt,
                        "clicked",
                        G_CALLBACK(join_game_bt_handler),
                        interface_references);
    g_signal_connect(   interface_references->launcher_references->new_game_bt,
                        "clicked",
                        G_CALLBACK(new_game_bt_handler),
                        interface_references);
    g_signal_connect(   interface_references->launcher_references->send_game_name,
                        "clicked",
                        G_CALLBACK(send_game_name_bt_handler),
                        interface_references);
    g_signal_connect(   interface_references->launcher_references->start_game_bt,
                        "clicked",
                        G_CALLBACK(start_game_bt_handler),
                        interface_references);
    g_signal_connect(   gtk_entry_get_buffer (GTK_ENTRY(interface_references->launcher_references->username_entry)),
                        "inserted-text",
                        G_CALLBACK (launcher_entries_insert_handler),
                        interface_references);
    g_signal_connect(   gtk_entry_get_buffer (GTK_ENTRY(interface_references->launcher_references->game_entry)),
                        "inserted-text",
                        G_CALLBACK (launcher_entries_insert_handler),
                        interface_references);
    g_signal_connect(   gtk_entry_get_buffer (GTK_ENTRY(interface_references->launcher_references->username_entry)),
                        "deleted-text",
                        G_CALLBACK (launcher_entries_delete_handler),
                        interface_references);
    g_signal_connect(   gtk_entry_get_buffer (GTK_ENTRY(interface_references->launcher_references->game_entry)),
                        "deleted-text",
                        G_CALLBACK (launcher_entries_delete_handler),
                        interface_references);
    /* Gameplay References Signals */
    g_signal_connect(   interface_references->gameplay_references->game_tables_nb,
                        "switch-page",
                        G_CALLBACK (switch_page_handler),
                        interface_references);
    for (int i = 0; i < 5; i++) {
        g_signal_connect (  interface_references->gameplay_references->action_buttons[i],
                            "clicked",
                            G_CALLBACK (client_send_win),
                            interface_references);
    }
}

GtkWidget *
_draw_action_dialog (   interface_references_t * ir,
                        GList *possible_wins)
{
    GList *l;
    GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;
    GtkWidget *content_area;
    ir->gameplay_references->action_dialog = gtk_dialog_new_with_buttons (  "Claim a win",
                                                                            GTK_WINDOW(ir->main_window),
                                                                            flags,
                                                                            "Claim",
                                                                            GTK_RESPONSE_OK,
                                                                            "Cancel",
                                                                            GTK_RESPONSE_CANCEL,
                                                                            NULL);
    content_area = gtk_dialog_get_content_area (GTK_DIALOG(ir->gameplay_references->action_dialog));
    gtk_box_set_homogeneous (GTK_BOX (content_area), TRUE);
    gtk_widget_set_margin_top (content_area, 5);
    gtk_widget_set_margin_start (content_area, 5);
    gtk_widget_set_margin_bottom (content_area, 5);
    gtk_widget_set_margin_end (content_area, 5);
    GtkWidget *dialog_title = gtk_label_new ("Pick a set of numbers to clam for a win!");
    gtk_widget_set_name (dialog_title, "dialog_title");
    gtk_box_pack_start (GTK_BOX (content_area),
                        dialog_title,
                        TRUE,
                        FALSE,
                        10);

    GtkWidget *radio_button, *last_button;
    last_button = gtk_radio_button_new_with_label (NULL, (gchar *) possible_wins->data);
    g_signal_connect (  last_button,
                        "toggled",
                        G_CALLBACK(radio_bt_handler),
                        ir);
    gtk_box_pack_start (GTK_BOX (content_area),
                        last_button,
                        TRUE,
                        TRUE,
                        0);
    for (l = possible_wins->next; l != NULL; l = l->next) {
        radio_button = gtk_radio_button_new_with_label (NULL, (gchar *) l->data);
        gtk_radio_button_join_group (GTK_RADIO_BUTTON (radio_button), GTK_RADIO_BUTTON (last_button));
        gtk_box_pack_start (GTK_BOX (content_area),
                            radio_button,
                            TRUE,
                            TRUE,
                            0);
        g_signal_connect (  radio_button,
                            "toggled",
                            G_CALLBACK(radio_bt_handler),
                            ir);
        last_button = radio_button;
    }


    g_signal_connect (  ir->gameplay_references->action_dialog,
                        "response",
                        G_CALLBACK (action_dialog_response_handler),
                        ir);
    return ir->gameplay_references->action_dialog;
}

gint
_how_many_on_row (  GList *list,
                    gint row)
{
    gint n = 0;
    picked_number_t *pn;
    GList *l;
    for (l = list; l != NULL; l = l->next) {
        pn = (picked_number_t *) l->data;
        if (pn->row == row) {
            ++n;
        }
    } return n;
}

void
_init_picked_numbers (GList **picked_numbers)
{
    for (int i = 0; i < 3; i++) {
        picked_numbers[i] = NULL;
    }
}

void
_init_win_rows (gint a[][2])
{
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 2; j++) {
            a[i][j] = -1;
        }
    }
}

void
_load_animated_pixbufs (interface_references_t *ir)
{
    ir->gameplay_references->gifs[WAITING_PIX] = gdk_pixbuf_animation_new_from_file (   WAITING_GIF,
                                                                                        NULL);
    ir->gameplay_references->gifs[AMBO_PIX] = gdk_pixbuf_animation_new_from_file (      AMBO_GIF,
                                                                                        NULL);
    ir->gameplay_references->gifs[TERNA_PIX] = gdk_pixbuf_animation_new_from_file (     TERNA_GIF,
                                                                                        NULL);
    ir->gameplay_references->gifs[QUATERNA_PIX] = gdk_pixbuf_animation_new_from_file (  QUATERNA_GIF,
                                                                                        NULL);
    ir->gameplay_references->gifs[CINQUINA_PIX] = gdk_pixbuf_animation_new_from_file (  CINQUINA_GIF,
                                                                                        NULL);
    ir->gameplay_references->gifs[TOMBOLA_PIX] = gdk_pixbuf_animation_new_from_file (   TOMBOLA_GIF,
                                                                                        NULL);
    ir->gameplay_references->gifs[LOSE_PIX] = gdk_pixbuf_animation_new_from_file (      LOSE_GIF,
                                                                                        NULL);
}

gboolean
_reset_waiting_pix (gpointer user_data)
{
    interface_references_t *ir = (interface_references_t *) user_data;
    gtk_image_set_from_animation (  GTK_IMAGE (ir->gameplay_references->grandma_gif),
                                    ir->gameplay_references->gifs[WAITING_PIX]);
    return G_SOURCE_REMOVE;
}

gint
_set_selected_n (   interface_references_t * ir,
                    gchar *radio_label)
{
    /* Set selected_n_set */
    gchar *first_number = strchr (radio_label, '{');
    gchar *last_number = strchr (radio_label, '}');
    ++first_number;
    --last_number;
    size_t size = (last_number - first_number + 1)/sizeof(gchar);
    gchar *num_str = g_malloc (size * sizeof(gchar));
    strncpy (num_str, first_number, size);
    gchar **numbers = strtokenizer (num_str, ", ", (gint *)&size);

    g_list_free (ir->gameplay_references->selected_n_set);
    ir->gameplay_references->selected_n_set = NULL;
    gint *nums = g_malloc (size * sizeof (gint));
    for (int i = 0; i < size; i++) {
        nums[i] = g_ascii_strtod(numbers[i], NULL);
        ir->gameplay_references->selected_n_set = g_list_append (   ir->gameplay_references->selected_n_set,
                                                                    &nums[i]);
    }
    g_free (num_str);
    g_free (numbers);

    /* Set temp_win_row */
    gchar *row_number_s = g_malloc (3 * sizeof(gchar));
    gchar *table_number_s = g_malloc (3 * sizeof(gchar));
    gchar *table_number = strchr (radio_label, ':');
    --table_number;
    strncpy (row_number_s, radio_label, 1 * sizeof(gchar));
    strncpy (table_number_s, table_number, 1 * sizeof(gchar));

    ir->temp_win_row[1] = g_ascii_strtod (&table_number[0], NULL); // Table number
    g_print ("Temp table: %d", ir->temp_win_row[1]);

    ir->temp_win_row[0] = g_ascii_strtod (&radio_label[0], NULL) + (3 * ir->temp_win_row[1]); // Row number
    g_print ("Temp row: %d", ir->temp_win_row[0]);


    g_free (row_number_s);
    g_free (table_number_s);
    return 0;
}

void
action_dialog_response_handler (GtkDialog *dialog,
                                gint       response_id,
                                gpointer   user_data)
{
    interface_references_t *ir = (interface_references_t *) user_data;
    if (response_id == GTK_RESPONSE_OK) {
        g_print ("Starting thread\n");
        gtk_widget_set_sensitive (GTK_WIDGET (dialog), FALSE);
        GThread *send_win_thread = g_thread_new (NULL, (GThreadFunc) _client_send_win, ir);
    } else {
        gtk_widget_destroy ( GTK_WIDGET (dialog));
    }
}

void
activate_handler (  GApplication    *application,
                    gpointer        user_data)
{
    GtkBuilder *builder;

    /* Allocate memory for references struct */
    interface_references_t *interface_references = (interface_references_t *)user_data;
    interface_references->chat_references = g_new(chat_references_t, 1);
    interface_references->launcher_references = g_new(launcher_references_t, 1);
    interface_references->gameplay_references = g_new(gameplay_references_t, 1);

    /* Save references from GtkBuilder */
    builder = gtk_builder_new_from_file (UI_FILE);
    interface_references->main_window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
    interface_references->main_stack = GTK_WIDGET(gtk_builder_get_object(builder, "main_stack"));
    interface_references->header_bar = GTK_WIDGET(gtk_builder_get_object(builder, "header_bar"));
    interface_references->chat_references->chat_label = GTK_WIDGET(gtk_builder_get_object(builder, "chat_label"));
    interface_references->chat_references->chat_tv = GTK_WIDGET(gtk_builder_get_object(builder, "chat_tv"));
    interface_references->chat_references->chat_entry = GTK_WIDGET(gtk_builder_get_object(builder, "chat_entry"));
    interface_references->launcher_references->launcher_box = GTK_WIDGET(gtk_builder_get_object(builder, "launcher_box"));
    interface_references->launcher_references->new_game_bt = GTK_WIDGET(gtk_builder_get_object(builder, "new_game_bt"));
    interface_references->launcher_references->join_game_bt = GTK_WIDGET(gtk_builder_get_object(builder, "join_game_bt"));
    interface_references->launcher_references->new_game_box = GTK_WIDGET(gtk_builder_get_object(builder, "new_game_box"));
    interface_references->launcher_references->new_game_title = GTK_WIDGET(gtk_builder_get_object(builder, "new_game_title"));
    interface_references->launcher_references->username_entry = GTK_WIDGET(gtk_builder_get_object(builder, "username_entry"));
    interface_references->launcher_references->game_entry = GTK_WIDGET(gtk_builder_get_object(builder, "game_entry"));
    interface_references->launcher_references->new_game_answer = GTK_WIDGET(gtk_builder_get_object(builder, "new_game_answer"));
    interface_references->launcher_references->send_game_name = GTK_WIDGET(gtk_builder_get_object(builder, "send_game_name"));
    interface_references->launcher_references->waiting_box = GTK_WIDGET(gtk_builder_get_object(builder, "waiting_box"));
    interface_references->launcher_references->wait_players_label = GTK_WIDGET(gtk_builder_get_object(builder, "wait_players_label"));
    interface_references->launcher_references->waiting_spinner = GTK_WIDGET(gtk_builder_get_object(builder, "waiting_spinner"));
    interface_references->launcher_references->start_game_bt = GTK_WIDGET(gtk_builder_get_object(builder, "start_game_bt"));
    interface_references->launcher_references->win_grandma_gif = GTK_WIDGET(gtk_builder_get_object(builder, "win_grandma_gif"));
    interface_references->launcher_references->end_game_box = GTK_WIDGET(gtk_builder_get_object(builder, "end_game_box"));
    interface_references->launcher_references->end_game_msg = GTK_WIDGET(gtk_builder_get_object(builder, "end_game_msg"));
    interface_references->launcher_references->end_game_winner = GTK_WIDGET(gtk_builder_get_object(builder, "end_game_winner"));
    interface_references->gameplay_references->action_buttons[0] = GTK_WIDGET(gtk_builder_get_object(builder, "ambo_bt"));
    interface_references->gameplay_references->action_buttons[1] = GTK_WIDGET(gtk_builder_get_object(builder, "terna_bt"));
    interface_references->gameplay_references->action_buttons[2] = GTK_WIDGET(gtk_builder_get_object(builder, "quaterna_bt"));
    interface_references->gameplay_references->action_buttons[3] = GTK_WIDGET(gtk_builder_get_object(builder, "cinquina_bt"));
    interface_references->gameplay_references->action_buttons[4] = GTK_WIDGET(gtk_builder_get_object(builder, "tombola_bt"));
    interface_references->gameplay_references->main_grid = GTK_WIDGET(gtk_builder_get_object(builder, "main_grid"));
    interface_references->gameplay_references->gameplay_grid = GTK_WIDGET(gtk_builder_get_object(builder, "gameplay_grid"));
    interface_references->gameplay_references->server_tv = GTK_WIDGET(gtk_builder_get_object(builder, "server_tv"));
    interface_references->gameplay_references->game_tables_nb = GTK_WIDGET(gtk_builder_get_object(builder, "game_tables_nb"));
    interface_references->gameplay_references->gameplay_stack = GTK_WIDGET(gtk_builder_get_object(builder, "gameplay_stack"));
    interface_references->gameplay_references->numbers_table = GTK_WIDGET(gtk_builder_get_object(builder, "numbers_table"));
    interface_references->gameplay_references->grandma_gif = GTK_WIDGET(gtk_builder_get_object(builder, "grandma_gif"));
    interface_references->temp_win_row[0] = -1;
    interface_references->temp_win_row[1] = -1;
    interface_references->username = NULL;
    interface_references->launcher_references->back_button = NULL;
    interface_references->gameplay_stckswc = NULL;
    interface_references->gameplay_references->action_dialog = NULL;
    interface_references->gameplay_references->selected_n_set = NULL;

    gtk_application_add_window(GTK_APPLICATION(application), GTK_WINDOW(interface_references->main_window));
    _init_picked_numbers(interface_references->picked_numbers);
    _init_win_rows (interface_references->win_rows);
    _add_game_tables(interface_references);
    _add_numbers_table(interface_references);
    _load_animated_pixbufs(interface_references);
    _connect_signals(interface_references);

    /* CSS Initilization */
    GtkCssProvider *provider;
    GdkDisplay *display;
    GdkScreen *screen;

    provider = gtk_css_provider_new ();
    display = gdk_display_get_default ();
    screen = gdk_display_get_default_screen (display);
    gtk_style_context_add_provider_for_screen ( screen,
                                                GTK_STYLE_PROVIDER(provider),
                                                GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    gtk_css_provider_load_from_path (provider, CSS_FILE, NULL);

    g_object_unref (provider);
    g_object_unref(builder);
}

void
back_bt_handler (   GtkButton *button,
                    gpointer   user_data)
{
    interface_references_t *ir = (interface_references_t *) user_data;
    gtk_widget_hide (GTK_WIDGET(button));
    gtk_widget_set_name (ir->main_window, "homescreen");
    gtk_stack_set_visible_child(GTK_STACK(ir->main_stack), ir->launcher_references->launcher_box);

}

void
chat_entry_handler (GtkEntry *entry,
                    gpointer  user_data)
{
    interface_references_t *ir = (interface_references_t *) user_data;
    if (gtk_entry_get_text_length (entry) > 0) {
        gchar *entry_txt = (gchar *) gtk_entry_get_text (entry);
        gchar *entry_txt_s = g_malloc ((strlen (entry_txt) + 4) * sizeof(gchar));
        strcpy (entry_txt_s, entry_txt);
        void **data = g_malloc (2 * sizeof (gpointer));
        gchar *you_chat = g_malloc ((strlen (entry_txt_s) + strlen (CHAT_YOU))  * sizeof (gchar));
        strcpy (you_chat, CHAT_YOU);
        strcat (you_chat, entry_txt_s);
        _append_text_view ( GTK_TEXT_VIEW (ir->chat_references->chat_tv),
                            you_chat);

        gchar *buffer = create_message (entry_txt_s, PLAIN_CODE);
        data[0] = ir;
        data[1] = buffer;
        GThread *send_thread = g_thread_new (NULL, (GThreadFunc) _client_send_message, data);

        //Clear
        gtk_entry_set_text (entry, "");
        g_free (you_chat);
        g_free (entry_txt_s);

    }
}

void
client_actions (interface_references_t *interface_references,
                socket_message_t* socket_message)
{
    if (evaluate_message (socket_message)) { // Evaluation was fine
        //g_message("evaluation_fine");
        void ** data = g_malloc (2 * sizeof (void *));
        switch (socket_message->msg_type) {
            case PLAIN:
                data[0] = interface_references;
                data[1] = socket_message;
                gdk_threads_add_idle ((GSourceFunc) _client_chat_message, data);
                break;
            case CREATE_GAME:
                data[0] = interface_references;
                data[1] = socket_message;
                gdk_threads_add_idle((GSourceFunc) _client_new_game, data);
                break;
            case JOIN_GAME:
                data[0] = interface_references;
                data[1] = socket_message;
                gdk_threads_add_idle((GSourceFunc) _client_new_game, data);
                break;
            case START_GAME:
                gdk_threads_add_idle ((GSourceFunc) _client_start_game, interface_references);
                g_free (socket_message);
                break;
            case END_GAME:
                data[0] = interface_references;
                data[1] = socket_message;
                gdk_threads_add_idle ((GSourceFunc) _client_end_game, data);
                break;
            case AMBO:
                data[0] = interface_references;
                data[1] = socket_message;
                gdk_threads_add_idle ((GSourceFunc) _client_win_result, data);
                break;
            case TERNA:
                data[0] = interface_references;
                data[1] = socket_message;
                gdk_threads_add_idle ((GSourceFunc) _client_win_result, data);
                break;
            case QUATERNA:
                data[0] = interface_references;
                data[1] = socket_message;
                gdk_threads_add_idle ((GSourceFunc) _client_win_result, data);
                break;
            case CINQUINA:
                data[0] = interface_references;
                data[1] = socket_message;
                gdk_threads_add_idle ((GSourceFunc) _client_win_result, data);
                break;
            case TOMBOLA:
                data[0] = interface_references;
                data[1] = socket_message;
                gdk_threads_add_idle ((GSourceFunc) _client_tombola_result, data);
                break;
            case EXTRACTED:
                data[0] = interface_references;
                data[1] = socket_message->msg;
                gdk_threads_add_idle ((GSourceFunc) _client_extracted, data);
                break;
            default:
                g_message("Un-handled message. How did you get here?");
                break;
        } //g_free (data);
    }
}

void
client_send_win (   GtkButton *button,
                    gpointer   user_data)
{
    interface_references_t *ir = (interface_references_t *) user_data;
    gint type = 0; // How many numbers are we looking for?
    GList *possible_wins = NULL;
    GList *radio_references = NULL;

    // Detect the win type
    for (int i = 0; i < 5; i++) {
        if (GTK_WIDGET (button) == ir->gameplay_references->action_buttons[i]) {
            type = i+2;
            break;
        }
    }

    // Collect data from all tables
    if (type < 6) { // Not looking for tombola
        GList *l;

        for (int k = 0; k < 3; k++) { // Number of lists/tables
            for (int s = 0; s < 3; s++) { // Number of rows
                if (ir->picked_numbers[k] != NULL) { // Check integrity
                    if (_how_many_on_row(ir->picked_numbers[k], s) == type) {
                        gchar *possible_win_string = _build_possible_win_string (ir, ir->picked_numbers[k], s);
                        if (possible_win_string != NULL) {
                            possible_wins = g_list_append (possible_wins, possible_win_string);
                        }
                    }
                }
            }
        }

        if (possible_wins != NULL) { // We have some numbers picked
            // Create widgets
            GtkWidget *dialog = _draw_action_dialog (ir, possible_wins);
            GtkWidget *content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
            GList *children = gtk_container_get_children (GTK_CONTAINER (content_area));
            GtkWidget *first_radio_button = (GtkWidget *) children->next->data;
            _set_selected_n (ir, (gchar *) gtk_button_get_label (GTK_BUTTON (first_radio_button)));
            gtk_widget_show_all (dialog);
        }
    } else if (type == 6) { // Tombola
        gchar *tombola_string = _build_tombola_string (ir);
        void **data = g_malloc (2 * sizeof (gpointer));
        if (tombola_string != NULL) {
            for (int i = 0; i < 5; i++) {
                gtk_widget_set_sensitive(ir->gameplay_references->action_buttons[i], FALSE);
            }
            data[0] = ir;
            data[1] = tombola_string;
            g_thread_new (NULL, (GThreadFunc) _client_send_message, data);
        }
    }
}

gboolean
connection_handler (    GSocket *socket,
                        GIOCondition condition,
                        gpointer user_data)
{
    interface_references_t *ir = (interface_references_t *) user_data;

    socket_message_t *socket_msg = g_new (socket_message_t, 1);
    char *buffer = (char *) g_malloc (BUFFER_SIZE * sizeof(gchar));
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

    if (bytes_read != 0) {
        normalize_string(buffer);
        socket_msg->msg = buffer;
        client_actions(ir, socket_msg);
        return TRUE;
    } else {
        return FALSE;
    }
}

void
create_back_bt (interface_references_t *ir)
{
    if (ir->launcher_references->back_button == NULL) {
        ir->launcher_references->back_button = gtk_button_new_with_label ("Back");
        g_signal_connect (  ir->launcher_references->back_button,
                            "clicked",
                            G_CALLBACK(back_bt_handler),
                            ir);
        gtk_container_add ( GTK_CONTAINER (ir->header_bar),
                            ir->launcher_references->back_button);
    }
    gtk_widget_show_all (ir->header_bar);
}

void
join_game_bt_handler (  GtkButton *button,
                        gpointer   user_data)
{
    interface_references_t *ir = (interface_references_t *) user_data;

    create_back_bt(ir);

    gtk_widget_set_name (ir->main_window, "entries");
    gtk_button_set_label (GTK_BUTTON(ir->launcher_references->send_game_name), JOIN_BT);
    gtk_label_set_text(GTK_LABEL(ir->launcher_references->new_game_title), JOIN_GAME_TITLE);
    gtk_stack_set_visible_child(GTK_STACK(ir->main_stack), ir->launcher_references->new_game_box);
}

void
launcher_entries_delete_handler (   GtkEntryBuffer *buffer,
                                    guint           position,
                                    guint           n_chars,
                                    gpointer        user_data)
{
    guint chars_username;
    guint chars_game;
    interface_references_t *ir = (interface_references_t *) user_data;


    chars_username = gtk_entry_buffer_get_length (gtk_entry_get_buffer(GTK_ENTRY(ir->launcher_references->username_entry)));
    chars_game = gtk_entry_buffer_get_length (gtk_entry_get_buffer(GTK_ENTRY(ir->launcher_references->game_entry)));

    if (chars_username > 0 && chars_game > 0)
        gtk_widget_set_sensitive (ir->launcher_references->send_game_name, TRUE);
    else
        gtk_widget_set_sensitive (ir->launcher_references->send_game_name, FALSE);
}


void
launcher_entries_insert_handler (   GtkEntryBuffer *buffer,
                                    guint           position,
                                    gchar          *chars,
                                    guint           n_chars,
                                    gpointer        user_data)
{
    guint chars_username;
    guint chars_game;
    interface_references_t *ir = (interface_references_t *) user_data;


    chars_username = gtk_entry_buffer_get_length (gtk_entry_get_buffer(GTK_ENTRY(ir->launcher_references->username_entry)));
    chars_game = gtk_entry_buffer_get_length (gtk_entry_get_buffer(GTK_ENTRY(ir->launcher_references->game_entry)));

    if (chars_username > 0 && chars_game > 0)
        gtk_widget_set_sensitive (ir->launcher_references->send_game_name, TRUE);
    else
        gtk_widget_set_sensitive (ir->launcher_references->send_game_name, FALSE);

}


gpointer
network_loop (gpointer user_data)
{
	GError *error = NULL;
    interface_references_t *ir = (interface_references_t *) user_data;
	GSocketClient *client = g_socket_client_new();

	GSocketConnection *connection = g_socket_client_connect_to_host(client, (gchar*) SERVER_ADDRESS, SERVER_PORT, NULL, &error);

	GMainContext* context;
	context = g_main_context_new();

	GMainLoop *main_loop = g_main_loop_new (context, FALSE);

	GSocket *socket = g_socket_connection_get_socket (connection);
	ir->socket = socket;
	GSource *socket_source = g_socket_create_source (socket, G_IO_IN, NULL);
	g_source_set_callback (socket_source, (GSourceFunc) connection_handler, ir, NULL);
	g_source_attach (socket_source, context);
	g_main_loop_run (main_loop);
	return NULL;
}

gboolean
number_clicked_handler (GtkWidget   *widget,
                        GdkEvent    *event,
                        gpointer    user_data)
{
    interface_references_t *interface_references = (interface_references_t *) user_data;
    picked_number_t *picked_number = g_new(picked_number_t, 1);
    GValue value = G_VALUE_INIT;

    g_value_init(&value, G_TYPE_INT);
    GtkWidget *child = gtk_bin_get_child(GTK_BIN(widget));
    gint active_table = gtk_notebook_get_current_page(GTK_NOTEBOOK(interface_references->gameplay_references->game_tables_nb));
    gtk_container_child_get_property(   GTK_CONTAINER(interface_references->gameplay_references->game_tables[active_table]),
                                        widget,
                                        "top-attach",
                                        &value);
    picked_number->row = g_value_get_int(&value);
    picked_number->number = (gint) g_ascii_strtod(gtk_label_get_text(GTK_LABEL(child)), NULL);
    picked_number->table = active_table;
    g_value_unset(&value);
    GList *found = g_list_find_custom(interface_references->picked_numbers[active_table], picked_number, _compare_picked_number);

    if (found == NULL) {
        interface_references->picked_numbers[active_table] = g_list_append(interface_references->picked_numbers[active_table], picked_number);
        g_print("Picked %d on row %d of %d-th table\n", picked_number->number, picked_number->row, active_table);

        // GdkRGBA color;
        // gdk_rgba_parse (&color, "yellow");
        // gtk_widget_override_background_color (child, GTK_STATE_FLAG_NORMAL, &color);

        gtk_widget_set_name (child, TABLE_LABEL_PICKED);

    } else {
        picked_number_t *data = (picked_number_t *) found->data;
        g_print("Un-picked %d on row %d of %d-th table\n", data->number, data->row, active_table);

        // GdkRGBA color;
        // gdk_rgba_parse (&color, "rgba(0,0,0,0)");
        // gtk_widget_override_background_color (child, GTK_STATE_FLAG_NORMAL, &color);

        gtk_widget_set_name (child, TABLE_LABEL_UNPICKED);

        interface_references->picked_numbers[active_table] = g_list_delete_link(interface_references->picked_numbers[active_table], found);
    }

    if (g_list_length (interface_references->picked_numbers[active_table]) == 15) {
        gtk_widget_set_sensitive (interface_references->gameplay_references->action_buttons[4], TRUE);
    } else {
        gtk_widget_set_sensitive (interface_references->gameplay_references->action_buttons[4], FALSE);
    }

    return TRUE;
}

void
new_game_bt_handler (   GtkButton *button,
                        gpointer   user_data)
{
    interface_references_t *ir = (interface_references_t *) user_data;

    create_back_bt(ir);

    gtk_widget_set_name (ir->main_window, "entries");
    gtk_button_set_label (GTK_BUTTON(ir->launcher_references->send_game_name), CREATE_BT);
    gtk_label_set_text(GTK_LABEL(ir->launcher_references->new_game_title), NEW_GAME_TITLE);
    gtk_stack_set_visible_child(GTK_STACK(ir->main_stack), ir->launcher_references->new_game_box);
}

void
radio_bt_handler (  GtkToggleButton *togglebutton,
                    gpointer         user_data)
{
    interface_references_t *ir = (interface_references_t *) user_data;
    if (gtk_toggle_button_get_active (togglebutton)) {
        _set_selected_n (ir, (gchar *) gtk_button_get_label (GTK_BUTTON (togglebutton)));
    }
}

void startup_handler (  GApplication    *application,
                        gpointer        user_data)
{
    interface_references_t *ir = (interface_references_t *) user_data;
    GThread *socket_connection_thread;
	socket_connection_thread = g_thread_new("socket_connection_thread", network_loop, ir);
}

void
send_game_name_bt_handler (     GtkButton *button,
                                gpointer   user_data)
{
    interface_references_t *ir = (interface_references_t *) user_data;
    gchar* game_entry_txt = (gchar *) gtk_entry_get_text(GTK_ENTRY(ir->launcher_references->game_entry));
    gchar* username_entry_txt = (gchar *) gtk_entry_get_text(GTK_ENTRY(ir->launcher_references->username_entry));
    gchar* bt_txt = (gchar *) gtk_button_get_label(GTK_BUTTON(ir->launcher_references->send_game_name));
    gchar* message = g_malloc ((strlen (game_entry_txt) + strlen (username_entry_txt) + 2) * sizeof (gchar));

    sprintf (message, "%s,%s", username_entry_txt, game_entry_txt);

    gchar* buffer;
    if (strcmp(bt_txt, CREATE_BT) == 0) {
        buffer = create_message (message, CREATE_GAME_CODE);
    } else {
        buffer = create_message (message, JOIN_GAME_CODE);
    }
    g_socket_send ( ir->socket,
                    buffer,
                    strlen(buffer),
                    NULL,
                    NULL);
    g_print("Data sent: %s\n", buffer);
    g_free(buffer);
    g_free (message);

    /* Set username */
    gchar* username = g_malloc ((strlen (username_entry_txt) + 1) * sizeof(gchar));
    strcpy (username, username_entry_txt);
    ir->username = username;

    // Wait server response
    gtk_widget_set_sensitive (ir->launcher_references->game_entry, FALSE);
    gtk_widget_set_sensitive (ir->launcher_references->username_entry, FALSE);
    gtk_widget_set_sensitive (ir->launcher_references->send_game_name, FALSE);
}

void
start_game_bt_handler ( GtkButton *button,
                        gpointer   user_data)
{
    interface_references_t *ir = (interface_references_t *) user_data;
    gchar* buffer = create_message ("k", START_GAME_CODE);
    g_socket_send ( ir->socket,
                    buffer,
                    strlen(buffer),
                    NULL,
                    NULL);
    g_free(buffer);
}

void
switch_page_handler (   GtkNotebook *notebook,
                        GtkWidget   *page,
                        guint        page_num,
                        gpointer     user_data)
{
    interface_references_t *interface_references = (interface_references_t *) user_data;

    if (interface_references->picked_numbers[page_num] != NULL) {
        if (g_list_length (interface_references->picked_numbers[page_num]) == 15) {
            gtk_widget_set_sensitive (interface_references->gameplay_references->action_buttons[4], TRUE);
        } else {
            gtk_widget_set_sensitive (interface_references->gameplay_references->action_buttons[4], FALSE);
        }
    } else {
        gtk_widget_set_sensitive (interface_references->gameplay_references->action_buttons[4], FALSE);
    }
}
