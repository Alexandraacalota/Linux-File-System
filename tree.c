#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"
#define TREE_CMD_INDENT_SIZE 4
#define NO_ARG ""
#define PARENT_DIR ".."

// Function that creates the root directory of the filetree and allocates
// memory for its content and the list of children that it contains
FileTree createFileTree(char *rootFolderName)
{
	FileTree filetree;
	filetree.root = (TreeNode *)malloc(sizeof(TreeNode));
	filetree.root->parent = NULL;
	filetree.root->name = (char *)malloc((strlen(rootFolderName) + 1) *
										 sizeof(char));
	strcpy(filetree.root->name, rootFolderName);
	filetree.root->type = FOLDER_NODE;
	filetree.root->content = NULL;
	filetree.root->content = (FolderContent *)malloc(sizeof(FolderContent));
	FolderContent *foldercontent = filetree.root->content;
	foldercontent->children = (List *)malloc(sizeof(List));
	foldercontent->children->head = NULL;
	return filetree;
}

// Recursive function that deletes each node of the filetree, whether it is a
// directory or a file
void freeListNode(TreeNode *currentNode)
{
	// In case the currentNode is an empty directory, free the memory
	// allocated for its name
	if (currentNode->content == NULL && currentNode->type == FOLDER_NODE)
	{
		currentNode->parent = NULL;
		free(currentNode->name);
		return;
	}
	// In case the currentNode is a file, delete its name and then, if
	// existent, its content
	if (currentNode->type == FILE_NODE)
	{
		currentNode->parent = NULL;
		free(currentNode->name);
		if (currentNode->content != NULL)
		{
			free(currentNode->content);
			currentNode->content = NULL;
		}
		return;
	}
	// In case the currentNode is a folder with a content, recursively delete
	// each child with its content
	FolderContent *foldercontent = currentNode->content;
	ListNode *current = foldercontent->children->head;
	while (current != NULL)
	{
		ListNode *currentNext = current->next;
		freeListNode(current->info);
		current->info->parent = NULL;
		current = currentNext;
	}
	free(currentNode->name);
	free(foldercontent->children);
	free(currentNode->content);
	free(currentNode);
}

// Function that frees the memory allocated for the entire filetree at
// the end of the program execution
void freeTree(FileTree fileTree)
{
	freeListNode(fileTree.root);
	fileTree.root = NULL;
}

// Function that prints the files and directories contained in currentNode
void ls(TreeNode *currentNode, char *arg)
{
	int ok = 0;
	// In case there is no argument, check if there is any content inside
	// the directory and, if so, print it
	if (*arg == 0)
	{
		if (currentNode->content == NULL)
			return;
		FolderContent *foldercontent = currentNode->content;
		ListNode *current = foldercontent->children->head;
		while (current != NULL)
		{
			printf("%s\n", current->info->name);
			current = current->next;
		}
		// If there is a given argument, search for the file/ directory in the
		// currentNode
	}
	else
	{
		FolderContent *foldercontent = currentNode->content;
		ListNode *current = foldercontent->children->head;
		TreeNode *searchedNode = current->info;
		while (current != NULL)
		{
			if (strcmp(current->info->name, arg) == 0)
			{
				searchedNode = current->info;
				ok = 1;
			}
			current = current->next;
		}
		if (ok == 0)
		{
			printf("ls: cannot access '%s': No such file or directory\n", arg);
		}
		else
		{
			// If the file or directory has been found, print its name and
			// content
			if (searchedNode->type == FOLDER_NODE)
			{
				if (!searchedNode->content)
					return;
				foldercontent = searchedNode->content;
				current = foldercontent->children->head;
				while (current != NULL)
				{
					printf("%s\n", current->info->name);
					current = current->next;
				}
			}
			else
			{
				printf("%s: %s\n", searchedNode->name,
					   (char *)searchedNode->content);
			}
		}
	}
}

// Function that writes all of the parents inside a matrix of letters, each
// parent and the treeNode itself being written on a column of the matrix
void pwd(TreeNode *treeNode)
{
	char directories[50][50];
	int n = 0;
	TreeNode *current = treeNode;
	while (current != NULL)
	{
		strcpy(directories[n], current->name);
		n++;
		current = current->parent;
	}
	// Print the parents starting with the last column of the matrix (root)
	while (n)
	{
		printf("%s", directories[n - 1]);
		if (n != 1)
			printf("/");
		n--;
	}
}

TreeNode *cd(TreeNode *currentNode, char *path, int from_outside)
{
	if (!path)
	{
		return currentNode;
	}
	char *token = strtok(path, "/");  // We split the path in sections
	if (strcmp(token, "..") == 0)  // Case for the ".." argument
	{
		char *rest_of_path = strtok(NULL, "");
		if (currentNode->parent)  // If the current node has a parent
			currentNode = currentNode->parent;
		// Recursive call to the rest of the path
		return cd(currentNode, rest_of_path, from_outside);
	}
	List *subdirectories = ((FolderContent *)(currentNode->content))->children;
	ListNode *copy_head = subdirectories->head;
	while (subdirectories->head)
	{
		if (strcmp((subdirectories->head->info)->name, token) == 0)
		{
			if (subdirectories->head->info->type == FILE_NODE)
			{
				// If the name corresponds but it's a file not a directory
				return NULL;
			}
			char *rest_of_path = strtok(NULL, "");
			ListNode *required_node = subdirectories->head;
			subdirectories->head = copy_head;
			return cd(required_node->info, rest_of_path, from_outside);
		}
		// Continue to iterate through the file system
		subdirectories->head = subdirectories->head->next;
	}
	subdirectories->head = copy_head;
	if (from_outside)
	{
		printf("cd: no such file or directory: %s\n", token);
		return currentNode;
	}
	else
	{
		return NULL;
	}
}

// Helper function to print spaces for the tree function
void print_spacing(int level)
{
	for (int i = 0; i < (level * 4); i++)
	{
		printf(" ");
	}
}

// Helper function for tree; works recursively
void my_tree(TreeNode *currentNode, int level, int *nr_dirs, int *nr_files)
{
	if (!(currentNode->content))
	{
		return;
	}
	List *list = ((FolderContent *)(currentNode->content))->children;
	ListNode *copy_head = list->head;
	while (list->head)
	{
		if (list->head->info->type == FOLDER_NODE)
		{
			// Print adequate spacing
			print_spacing(level);
			// Print name of element
			printf("%s\n", list->head->info->name);
			(*nr_dirs)++;
			my_tree(list->head->info, level + 1, nr_dirs, nr_files);
		}
		else
		{
			print_spacing(level);
			printf("%s\n", list->head->info->name);
			(*nr_files)++;
		}
		list->head = list->head->next;
	}
	list->head = copy_head;
}

void tree(TreeNode *currentNode, char *arg)
{
	int level = 0;
	// Store number of files and directories found
	int nr_dirs = 0;
	int nr_files = 0;
	if (strcmp(arg, "") != 0)
	{
		while (currentNode->parent)
		{
			currentNode = cd(currentNode, "..", 0);
		}
		currentNode = cd(currentNode, arg, 0);

		if (!currentNode)
		{
			printf("%s [error opening dir]\n\n", arg);
		}
		else
		{
			my_tree(currentNode, level, &nr_dirs, &nr_files);
		}
	}
	else
	{
		my_tree(currentNode, level, &nr_dirs, &nr_files);
	}
	printf("%d directories, %d files\n", nr_dirs, nr_files);
}

// Function that allocates memory for a new directory, sets its name, type
// and parent
void mkdir(TreeNode *currentNode, char *folderName)
{
	if (currentNode->content != NULL)
	{
		FolderContent *foldercontent = currentNode->content;
		ListNode *current = foldercontent->children->head;
		while (current != NULL)
		{
			if (strcmp(current->info->name, folderName) == 0)
			{
				printf("mkdir: cannot create directory '%s': File exists",
					   folderName);
				return;
			}
			current = current->next;
		}
	}
	TreeNode *newNode = (TreeNode *)malloc(sizeof(TreeNode));
	newNode->parent = currentNode;
	newNode->name = (char *)malloc((strlen(folderName) + 1) * sizeof(char));
	strcpy(newNode->name, folderName);
	newNode->type = FOLDER_NODE;
	newNode->content = NULL;
	// In case the currentNode directory is empty, allocate memory for its
	// content and the list of children
	if (!currentNode->content)
	{
		currentNode->content = (FolderContent *)malloc(sizeof(FolderContent));
		FolderContent *foldercontent = currentNode->content;
		foldercontent->children = (List *)malloc(sizeof(List));
		foldercontent->children->head = NULL;
	}
	FolderContent *foldercontent = currentNode->content;
	// Keep a pointer to the previous head inside a variable in order to
	// make the link between the initial and the new head
	ListNode *head = foldercontent->children->head;
	// Allocate memory for the new head of the list of children
	ListNode *newList = (ListNode *)malloc(sizeof(ListNode));
	newList->info = newNode;
	foldercontent->children->head = newList;
	foldercontent->children->head->next = head;
}

// Function to recursively remove a directory that isn't empty
void rmrec(TreeNode *currentNode, char *resourceName)
{
	int ok = 0;
	if (currentNode->content == NULL)
	{
		printf("rmrec: failed to remove '%s': No such file or directory",
			   resourceName);
		return;
	}
	FolderContent *foldercontent = currentNode->content;
	ListNode *current = foldercontent->children->head;
	ListNode *current_prev = NULL;
	while (current != NULL)
	{
		if (current->info)
		{
			if (strcmp(current->info->name, resourceName) == 0)
			{
				// In case we have a file, we delete it with rm
				if (current->info->type == FILE_NODE)
				{
					rm(currentNode, resourceName);
					return;
				}
				if (current->info->content != NULL)
				{
					// We delete all the content recursively from the folder
					while (((FolderContent *)(current->info->content))->
											  children->head)
					{
						rmrec(current->info, (((FolderContent *)(current->
							  info->content))->children->head)->info->name);
					}
				}
				ok = 1;
				if (current_prev)
					current_prev->next = current->next;
				else
					foldercontent->children->head = current_prev;
				current->info->parent = NULL;
				free(current->info->name);
				if (current->info->content != NULL)
				{
					free(current->info->content);
					current->info->content = NULL;
				}
				free(current->info);
			}
		}
		current_prev = current;
		current = current->next;
	}
	if (ok == 0)
	{
		printf("rmrec: failed to remove '%s': No such file or directory",
			   resourceName);
	}
}

// Function that removes a file from the currentNode directory
void rm(TreeNode *currentNode, char *fileName)
{
	int ok = 0;
	if (currentNode->content == NULL)
	{
		printf("rm: failed to remove '%s': No such file or directory",
			   fileName);
		return;
	}
	FolderContent *foldercontent = currentNode->content;
	ListNode *current = foldercontent->children->head;
	ListNode *current_prev = NULL;
	while (current != NULL)
	{
		if (current->info)
		{
			if (strcmp(current->info->name, fileName) == 0)
			{
				if (current->info->type == FOLDER_NODE)
				{
					printf("rm: cannot remove '%s': Is a directory", fileName);
					return;
				}
				ok = 1;
				if (current_prev)
					current_prev->next = current->next;
				else
					foldercontent->children->head = current_prev;
				current->info->parent = NULL;
				// Free the memory allocated for the name of the file
				free(current->info->name);
				// If existent, free the memory allocated for the content of
				// the file
				if (current->info->content != NULL)
				{
					free(current->info->content);
					current->info->content = NULL;
				}
				// Free the memory allocated for the info of the node in the
				// list representing the file that has to be removed
				free(current->info);
			}
		}
		current_prev = current;
		current = current->next;
	}
	// If the file has not been found in the currentNode directory, print
	// error message
	if (ok == 0)
	{
		printf("rm: failed to remove '%s': No such file or directory",
			   fileName);
	}
}

// Function that removes a file from the currentNode directory only if
// it is empty
void rmdir(TreeNode *currentNode, char *folderName)
{
	int ok = 0;
	if (currentNode->content == NULL)
	{
		printf("rmdir: failed to remove '%s': No such file or directory",
			   folderName);
		return;
	}
	FolderContent *foldercontent = currentNode->content;
	ListNode *current = foldercontent->children->head;
	ListNode *current_prev = NULL;
	while (current != NULL)
	{
		if (current->info)
		{
			if (strcmp(current->info->name, folderName) == 0)
			{
				if (current->info->type == FILE_NODE)
				{
					printf("rmdir: failed to remove '%s': Not a directory",
						   folderName);
					return;
				}
				if (current->info->content != NULL)
				{
					printf("rmdir: failed to remove '%s': Directory not empty",
						   folderName);
					return;
				}
				// The directory has been found and it is empty
				ok = 1;
				// Remake the links between the nodes of the list of children
				// belonging to the currentNode directory
				if (current_prev)
					current_prev->next = current->next;
				else
					foldercontent->children->head = current_prev;
				current->info->parent = NULL;
				// Free the memory allocated for storing the name of the
				// directory
				free(current->info->name);
				free(current->info);
			}
		}
		current_prev = current;
		current = current->next;
	}
	if (ok == 0)
	{
		printf("rmdir: failed to remove '%s': No such file or directory",
			   folderName);
	}
}

// Function that allocates memory for the new file and sets its name,
// type and parent
void touch(TreeNode *currentNode, char *fileName, char *fileContent)
{
	if (currentNode->content != NULL)
	{
		FolderContent *foldercontent = currentNode->content;
		ListNode *current = foldercontent->children->head;
		while (current != NULL)
		{
			if (strcmp(current->info->name, fileName) == 0)
			{
				return;
			}
			current = current->next;
		}
	}
	// Allocate memory for new the file node about to be stored in the
	// filetree
	TreeNode *newNode = (TreeNode *)malloc(sizeof(TreeNode));
	newNode->parent = currentNode;
	newNode->name = (char *)malloc((strlen(fileName) + 1) * sizeof(char));
	strcpy(newNode->name, fileName);
	newNode->type = FILE_NODE;
	newNode->content = (char *)malloc((strlen(fileContent) + 1) *
									  sizeof(char));
	char *Content = newNode->content;
	strcpy(Content, fileContent);
	// In case the currentNode directory is empty, allocate memory for its
	// content and the list of children
	if (!currentNode->content)
	{
		currentNode->content = (FolderContent *)malloc(sizeof(FolderContent));
		FolderContent *foldercontent = currentNode->content;
		foldercontent->children = (List *)malloc(sizeof(List));
		foldercontent->children->head = NULL;
	}
	FolderContent *foldercontent = currentNode->content;
	// Keep a pointer to the previous head of the children list inside the
	// variable head in order to make the link between the initial and the
	// new head
	ListNode *head = foldercontent->children->head;
	ListNode *newList = (ListNode *)malloc(sizeof(ListNode));
	newList->info = newNode;
	foldercontent->children->head = newList;
	foldercontent->children->head->next = head;
}
