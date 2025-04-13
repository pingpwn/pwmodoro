#include <gtk/gtk.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
bool br = false; // Flag that determines if we're on a break (friends refernece lol)
guint threadID = 0;
GtkWidget *tomato;
GtkWidget *label;
GtkWidget *start_button;
time_t rawtime;
time_t firetime;
int delta = 25;

// Quit gtk when pressing close
static void on_window_closed(GtkWidget *widget, gpointer data) {
    gtk_main_quit();
}

// Function that handles the timer
gboolean update_time(gpointer data) {
    int sec, h, m, s;
    struct tm *timeinfo;
    char buffer[45]; 

    // Get timer seconds
    sec = firetime - rawtime;
    // Format time
    h = (sec/3600); 
    m = (sec -(3600*h))/60;
    s = (sec -(3600*h)-(m*60));

    sprintf(buffer, "<span font='36'>%02d:%02d</span>",m,s);
    gtk_label_set_markup(GTK_LABEL(label), buffer);
    time(&rawtime);
    
    // If we reach the end of the timer, we start the break (the user has to press "Break")
    if (rawtime > firetime) {
        br = !br;
        if (br) gtk_button_set_label(GTK_BUTTON (start_button), "Break");
        else gtk_button_set_label(GTK_BUTTON (start_button), "Start");
        gtk_widget_set_sensitive(start_button, TRUE);
        g_source_remove(threadID);
        threadID = 0;   
    }
    return TRUE; 
}
// Function when button pressed
void start_func() {
    gtk_widget_set_sensitive(start_button, FALSE);
    time(&rawtime);
    // We set the timer for delta minutes if not on break, else for 5 minutes 
    if (!br){
        firetime = rawtime + delta*60;
        threadID = g_timeout_add_seconds(1, update_time, NULL);
    } else {
        firetime = rawtime + 5*60;
        threadID = g_timeout_add_seconds(1, update_time, NULL);
    }
	return;
}
int main(int argc, char *argv[]) {
    // Take minutes from cli args, if there
    if (fork() > 0)
        exit(0);  // Parent exits
    if (argc > 1) {
        delta = atoi(argv[1]);
    }
    // Initial GTK window and vbox setup
    gtk_init(&argc, &argv);
    GtkWidget *window;
    GtkWidget *vbox;
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), 225, 200);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    g_signal_connect(window, "destroy", G_CALLBACK(on_window_closed), NULL);
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_window_set_title(GTK_WINDOW(window), "Pwmodoro");
    gtk_container_add(GTK_CONTAINER(window), vbox);
    // Start button init
    start_button = gtk_button_new_with_label("Start");
    
    tomato = gtk_image_new_from_file("tomato.png");
    
    label = gtk_label_new("");
    gtk_label_set_markup(GTK_LABEL(label), "<span font='36'>00:00</span>");
    // Add elements to vbox
    gtk_box_pack_start(GTK_BOX(vbox), tomato, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), label, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), start_button, TRUE, TRUE, 0);
    // Connect start_func to the action of clicking the button
    g_signal_connect(start_button,"clicked", G_CALLBACK(start_func), NULL);
    // Start the app
    gtk_widget_show_all(window);
    gtk_main();
    return 0;
}
