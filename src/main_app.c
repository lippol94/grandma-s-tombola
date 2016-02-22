#include <gtk/gtk.h>
#include "main_app_controller.h"

int
main (  int argc,
        char ** argv)
{
    GtkApplication *app;
    gint status;
    interface_references_t *interface_references = g_new(interface_references_t, 1);

    app = gtk_application_new ("com.leonardoarcari.mobatombola", G_APPLICATION_NON_UNIQUE);
    g_signal_connect(app, "startup", G_CALLBACK(startup_handler), interface_references);
    g_signal_connect(app, "activate", G_CALLBACK(activate_handler), interface_references);
    status = g_application_run (G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}
