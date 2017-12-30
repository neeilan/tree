#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>

#include "ftree.h"

void add_node_props(struct TreeNode *node, const char* fname, int pms);
void mark_as_regfile(struct TreeNode* node);

char* join_path(const char *parent_path, const char *child_name);
struct TreeNode *_generate_ftree(const char *fname, const char *short_name);
int is_dir(struct TreeNode* node);


struct TreeNode *generate_ftree(const char *root_path)
{
    return _generate_ftree(root_path, root_path);
}

struct TreeNode *_generate_ftree(const char *fname, const char *short_name)
{
    struct stat sb;
    DIR* p_dir;
    struct dirent* entry;
    struct TreeNode* root = malloc(sizeof(struct TreeNode));
    
    root->reg_file = 0;
    root->contents = NULL;
    root->contents = NULL;
    root->next = NULL;
    root->fname = NULL;

    if (lstat(fname, &sb) == -1) {
        printf("lstat: the name '%s' is not a file or directory\n", fname);
        return NULL;
    }
    
    add_node_props(root, short_name, sb.st_mode & 0777);
    
    switch (sb.st_mode & S_IFMT) {
        case S_IFLNK:
        case S_IFREG: {  
            mark_as_regfile(root);
            break;
        }
        case S_IFDIR: {
            root->reg_file = 0;
            p_dir = opendir(fname);
        
            if (p_dir == NULL) {
                printf("Error opening directory");
                return NULL;
            }

            while ((entry = readdir(p_dir)) != NULL) {
                if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                    continue;
                
                if (root->contents == NULL) {
                    root->contents = _generate_ftree(join_path(fname, entry->d_name), entry->d_name);
                } else {
                    struct TreeNode* old_head = root->contents;
                    root->contents = _generate_ftree(join_path(fname, entry->d_name), entry->d_name);
                    (root->contents)->next = old_head;
                }
            }
            root->contents = root->contents;
            closedir(p_dir);
            break;
        }

    default:
        printf("Unknown file");
    }

    return root;
}

void add_node_props(struct TreeNode* node, const char* fname, int pms)
{
    node->fname = malloc(strlen(fname) * (sizeof(char) + 1));
    strcpy(node->fname, fname);
    node->permissions = pms;
}

void mark_as_regfile(struct TreeNode* node)
{
    node->reg_file = 1;
}

char* join_path(const char *parent_path, const char *child_name)
{
    // +2 for the zero-terminator and /
    int path_size = sizeof(char) * (strlen(parent_path) + strlen(child_name) + 2);
    char *result = malloc(path_size); 

    strcpy(result, parent_path);
    strcat(result, "/");
    strcat(result, child_name);

    return result;
}

/*
 * Prints the TreeNodes encountered on a preorder traversal of a FTree.
 */
void print_ftree(struct TreeNode* root)
{
    if (!root) return;

    static int depth = 0;

    if (is_dir(root))
        printf("%*s===== %s (%03o) =====\n", depth * 2, "", root->fname, root->permissions);
    else
        printf("%*s%s (%03o)\n", depth * 2, "", root->fname, root->permissions);


    free(root->fname);
    
    depth++;

    struct TreeNode* curr_node = root->contents;
    while (curr_node != NULL){
        if (curr_node->reg_file == 0)
            print_ftree(curr_node);
        else
            printf("%*s%s (%03o)\n", depth * 2, "", curr_node->fname, curr_node->permissions);
        
      curr_node = curr_node->next;
    } 
    
    depth--;
}

int is_dir(struct TreeNode* node)
{
    return node->reg_file == 0; 
}
