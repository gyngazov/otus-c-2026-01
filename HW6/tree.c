#include <gtk/gtk.h>

#include "recursive.h"

#define APP                         "otus.ru"

 
static void setup_listitem (GtkListItemFactory *factory, GtkListItem *listitem)
{
    GtkWidget *expander = gtk_tree_expander_new();
    GtkWidget *label = gtk_label_new(NULL);
     
    gtk_tree_expander_set_child(GTK_TREE_EXPANDER(expander), label);
    gtk_list_item_set_child (listitem, expander);
}
 
static void bind_listitem (GtkListItemFactory *factory, GtkListItem *listitem)
{
    GtkTreeListRow *tree_row = gtk_list_item_get_item(listitem);
    GtkStringObject* strobj = gtk_tree_list_row_get_item (tree_row);
    GtkWidget *tree_expander = gtk_list_item_get_child(listitem);
    gtk_tree_expander_set_list_row(GTK_TREE_EXPANDER(tree_expander), tree_row);
 
    GtkWidget* label  = gtk_tree_expander_get_child(GTK_TREE_EXPANDER(tree_expander));
 
    gtk_label_set_text (GTK_LABEL (label), gtk_string_object_get_string(strobj));
     
}
 
static void app_activate (GApplication *app, gpointer *user_data) 
{
 
    GtkWidget *window = gtk_application_window_new (GTK_APPLICATION (app));
    gtk_window_set_title (GTK_WINDOW (window), APP);
    gtk_window_set_default_size (GTK_WINDOW (window), 250, 200);
    const char *curr[] = {".", NULL};

    GtkStringList* categories =  gtk_string_list_new(curr);
    GtkTreeListModel* list = gtk_tree_list_model_new(G_LIST_MODEL(categories), FALSE, FALSE, forest, NULL, NULL);
    GtkSingleSelection* model = gtk_single_selection_new(G_LIST_MODEL(list));
 
    GtkListItemFactory *factory = gtk_signal_list_item_factory_new();
    g_signal_connect (factory, "setup", G_CALLBACK (setup_listitem), NULL);
    g_signal_connect (factory, "bind", G_CALLBACK (bind_listitem), NULL);
 
    GtkWidget* treeView = gtk_list_view_new(GTK_SELECTION_MODEL(model), factory);
    gtk_widget_set_valign(treeView, GTK_ALIGN_START);
 
    gtk_window_set_child (GTK_WINDOW (window), treeView); 
    gtk_window_present (GTK_WINDOW (window));
}
 
int main (int argc, char **argv)
{
    GtkApplication *app = gtk_application_new (APP, 0);
    g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
    int status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref(app);
    return status;
}