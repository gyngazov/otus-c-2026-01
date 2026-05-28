#include <gtk/gtk.h>


static GtkStringList *get_list(const char *parent)
{
    GtkStringList *child_list = gtk_string_list_new(NULL);
    GError *error = NULL;
    GFile *child_file, *dir = g_file_new_for_path(parent);
    GFileInfo *file_info;
    GFileType file_type;
    char *file_name, *path;

	GFileEnumerator *enumerator = g_file_enumerate_children(dir, "standard", 
            G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS, NULL, &error);
    while ((file_info = g_file_enumerator_next_file(enumerator, NULL, &error)) != NULL) {
		file_type = g_file_info_get_file_type(file_info);
		file_name = g_file_info_get_name(file_info);
		child_file = g_file_get_child(dir, file_name);

		path = g_file_get_path(child_file);
		gtk_string_list_append(child_list, path);
        g_free(path);
		g_object_unref(file_info);
		g_object_unref(child_file);
    }
    g_object_unref(enumerator);
    return child_list;
}

GListModel* forest (void* item, gpointer user_data)
{
    GtkStringObject* strobj = (GtkStringObject *)item;
    const char* root = gtk_string_object_get_string(strobj);
    GtkStringList *child_list = get_list(root);

    return G_LIST_MODEL(child_list);
}