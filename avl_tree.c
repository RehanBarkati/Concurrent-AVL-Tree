#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdbool.h>
#include<semaphore.h>
#include<pthread.h>
#include<unistd.h>


sem_t readLock, writeLock,mutex;
int readCount=0;


typedef struct tree_node{
	int key;
	struct tree_node* left;
	struct tree_node* right;
	struct tree_node* parent;
} tree_node;


typedef struct help_print{
	int index;
	int *arr;
} help_print;


typedef struct ThreadArgs{
    char *operation;
    int value;
    tree_node **root;
} ThreadArgs;


int max(int a, int b){
	if(a>b)
		return a;
	else
		return b;
}

int get_height(tree_node* node){
	
	if(node == NULL)
		return 0;
	return (max(get_height(node->left), get_height(node->right)) + 1);
}

int get_balance_factor(tree_node* node){
	return (get_height(node->left) - get_height(node->right));
}

tree_node* single_left_rotation(tree_node* node){

	
	tree_node* current = node->right;
	
	node->right = current->left;
	if(node->right != NULL)
		node->right->parent = node;
	
	current->left = node;
	current->parent = node->parent;
	
	if(node->parent != NULL){
		if(node->parent->left == node)
			node->parent->left = current;
		else
			node->parent->right = current;
	}
	
	node->parent = current;
	
	return current;
}

tree_node* single_right_rotation(tree_node* node){
	
	tree_node* current = node->left;
	
	node->left = current->right;
	if(node->left != NULL)
		node->left->parent = node;
	
	current->right = node;
	current->parent = node->parent;
	
	if(node->parent != NULL){
		if(node->parent->left == node)
			node->parent->left = current;
		else
			node->parent->right = current;
	}
	
	node->parent = current;
	
	return current;
}

tree_node* double_left_right_rotation(tree_node* node){
	
	
	node->left = single_left_rotation(node->left);
	node = single_right_rotation(node);
	
	return node;
}

tree_node* double_right_left_rotation(tree_node *node){
	
	
	node->right = single_right_rotation(node->right);
	node = single_left_rotation(node);

	return node;
}

tree_node* insert_AVL(tree_node* root, int val){
	
	
	sem_wait(&writeLock);
	
	if(root == NULL){
		
		
		tree_node *new_node = (tree_node *)(malloc(sizeof(tree_node)));
		
		new_node->parent = NULL;
		new_node->left = NULL;
		new_node->right = NULL;
		new_node->key = val;
		
		root = new_node;
		
		
	}
	
	else{
		
		
		tree_node* current = root;
		tree_node* temp_parent = NULL;
		while(current != NULL){
			
			temp_parent = current;
			if(val <= current->key)
				current = current->left;
			else
				current = current->right;
		}
		tree_node* new_node = (tree_node*)(malloc(sizeof(tree_node)));
		
		new_node->key = val;
		new_node->parent = temp_parent;
		new_node->left = NULL;
		new_node->right = NULL;
		
		
		if(val <= temp_parent->key)
			temp_parent->left = new_node;
		else
			temp_parent->right = new_node;
		
		
		tree_node* curr = temp_parent;
		
		while(curr != root){
			
			if(abs(get_balance_factor(curr)) > 1){
				
				
				if(get_balance_factor(curr) > 1){
					if(get_balance_factor(curr->left) > 0)
						curr = single_right_rotation(curr);
					else
						curr = double_left_right_rotation(curr);
					break;
				}
				else{
					if(get_balance_factor(curr->right) < 0)
						curr = single_left_rotation(curr);
					else
						curr = double_right_left_rotation(curr);
					break;
				}
			}
			curr = curr->parent;
		}
		
		curr = root;
		
		
		if(abs(get_balance_factor(curr)) > 1){
			
			
			if(get_balance_factor(curr) > 1){
				if(get_balance_factor(curr->left) > 0)
					root = single_right_rotation(curr);
				else
					root = double_left_right_rotation(curr);
			}
			else{
				if(get_balance_factor(curr->right) < 0)
					root = single_left_rotation(curr);
				else
					root = double_right_left_rotation(curr);
			}
		}
		
	}
	
	sem_post(&writeLock);
	
	return root;
	
}	
	
tree_node* leftMost(tree_node* A) {
	
	tree_node* curr = A;
    while (curr->left != NULL) {
        curr = curr->left;
    }
    return curr;
}

void adjust_nodes(tree_node** root, tree_node* A, tree_node* B){
	
	if(A->parent == NULL)
		*root = B;
	else if(A == A->parent->left)
		A->parent->left = B;
	else
		A->parent->right = B;
	if(B != NULL)
		B->parent = A->parent;
	
}

tree_node* delete_AVL(tree_node* root, int val){
	
	
	sem_wait(&writeLock);
	
	tree_node* current = root;

	tree_node* D = NULL;
	while(current != NULL){
		
		if(current->key == val){
			D = current;
			break;
		}
		else if(current->key < val){
			current = current->right;
		}
		else{
			current = current->left;
		}
	}
	
	if(D == NULL){
		sem_post(&writeLock);
		return root;
	}
	
	
	if(D->left == NULL)
		adjust_nodes(&root, D, D->right);
	else if(D->right == NULL){
		adjust_nodes(&root, D, D->left);
	}
	else{
		tree_node* D_suc = leftMost(D->right);
		if(D_suc->parent != D){
			adjust_nodes(&root, D_suc, D_suc->right);
			D_suc->right = D->right;
			D_suc->right->parent = D_suc;
		}

		adjust_nodes(&root, D, D_suc);
		D_suc->left = D->left;
		D_suc->left->parent = D_suc;

	}
	
	tree_node* curr = D;

	while(curr != NULL && curr != root){
		if(abs(get_balance_factor(curr)) > 1){
			if(get_balance_factor(curr) > 1){
				if(get_balance_factor(curr->left) > 0){
					curr = single_right_rotation(curr);
				}
				else{
					curr = double_left_right_rotation(curr);
				}
			}
			else{
				if(get_balance_factor(root->right) < 0){
					curr = single_left_rotation(root);
				}
				else
					curr = double_right_left_rotation(root);
			}
		
		}
		curr = curr->parent;
	}
	curr = root;
	if(abs(get_balance_factor(curr)) > 1){
		if(get_balance_factor(curr) > 1){
			if(get_balance_factor(curr->left) > 0){
				root = single_right_rotation(curr);
			}
			else{
				root = double_left_right_rotation(curr);
			}
		}
		else{
			if(get_balance_factor(root->right) < 0){
				root = single_left_rotation(curr);
			}
			else
				root = double_right_left_rotation(curr);
		}
		
	}

	free(D);
	
	sem_post(&writeLock);
	
	return root;
	
}

bool contains_AVL(tree_node* root, int val){
     
	sem_wait(&mutex);
	readCount++;
	if(readCount==1){
		sem_wait(&writeLock);
	}
	sem_post(&mutex);
	tree_node* current = root;
	while(current != NULL){
		
		if(val == current->key)
			break;
		else if(val < current->key)
			current = current->left;
		else
			current = current->right;
	}

	sem_wait(&mutex);
	readCount--;
	if(readCount==0){
		sem_post(&writeLock);
	}
	sem_post(&mutex);

	if(current != NULL){
		
		sem_wait(&readLock);
		printf("yes\n");
		sem_post(&readLock);
	}
	else{
		
		sem_wait(&readLock);
		printf("no\n");
		sem_post(&readLock);
	}	
}

void in_order_AVL(tree_node* root, help_print **help_object){
	
	if(root == NULL)
		return;
	
	in_order_AVL(root->left, help_object);
	(*help_object)->arr[(*help_object)->index++] = root->key;
	in_order_AVL(root->right, help_object);
}

void pre_order_AVL(tree_node* root){
	
	if(root == NULL)
		return;
	
	printf("%d ", root->key);
	pre_order_AVL(root->left);
	pre_order_AVL(root->right);
}


void* perform_operation(void *args){
	
    ThreadArgs *targs = (ThreadArgs *)args;
    
    if (strcmp(targs->operation, "insert") == 0){
        *(targs->root) = insert_AVL(*(targs->root), targs->value);
    }
	else if (strcmp(targs->operation, "delete") == 0){
        *(targs->root) = delete_AVL(*(targs->root), targs->value);
    }
	else if (strcmp(targs->operation, "contains") == 0){
        contains_AVL(*(targs->root), targs->value);
    }
	else if (strcmp(targs->operation, "in") == 0){
		
		help_print* help_object = (help_print*)(malloc(sizeof(help_print))); 
		help_object->index = 0;
		help_object->arr = (int *)(malloc(sizeof(int)*200));
        
		sem_wait(&mutex);
		readCount++;
		if(readCount==1){
			sem_wait(&writeLock);
		}
		sem_post(&mutex);
        in_order_AVL(*(targs->root), &help_object);

		sem_wait(&mutex);
		readCount--;
		if(readCount==0){
			sem_post(&writeLock);
		}
		sem_post(&mutex);

		sem_wait(&readLock);
		
		for(int k=0; k<help_object->index; k++)
			printf("%d ", help_object->arr[k]);
		
		printf("\n");
		
		sem_post(&readLock);
		
    }
	else{
		printf("Invalid input\n");
	}
    
    free(args);
    return NULL;
}

	
int main(int argc, char *argv[]) {
	
	//printf("PID = %d\n", getpid());
	
	if(sem_init(&writeLock, 0, 1) != 0){
		perror("Failed to initialize write Lock semaphore\n");
		exit(EXIT_FAILURE);
	}
	if(sem_init(&readLock, 0, 1) != 0){
		perror("Failed to initialize read Lock semaphore\n");
		exit(EXIT_FAILURE);
	}
	if(sem_init(&mutex, 0, 1) != 0){
		perror("Failed to initialize mutex Lock semaphore\n");
		exit(EXIT_FAILURE);
	}
	
	tree_node* root = NULL;
	
    int threadCount = 0;
	pthread_t *threads = (pthread_t *) malloc(sizeof(pthread_t) * 200);
    char* input = (char *)(malloc(200));  

    while (1) {
		
        // Read input from the user.
		fgets(input, 200, stdin);
		size_t input_length = strcspn(input, "\n\r");
		input[input_length] = '\0';
		
		// printf("%s\n", input);

        if (strcmp(input, "exit") == 0) {
            break;
        }
		
        // Tokenize the input.
        char *operation = strtok(input, " ");
        char *valueStr = strtok(NULL, " ");
		int value;
		if (valueStr) { // Checking if the token exists
			value = atoi(valueStr);
		} else {
			value = 0;
		}
		
        ThreadArgs *args = malloc(sizeof(ThreadArgs));
        args->operation = strdup(operation);
        args->root = &root;
        args->value = value;
        pthread_t thread;
        pthread_create(&thread, NULL, perform_operation, args);
		threads[threadCount++] = thread;

    }

	for (int i = 0; i < threadCount; i++){
		  pthread_join(threads[i], NULL);
	}
	
	free(threads);
	free(input);
	
	pre_order_AVL(root);

	sem_destroy(&readLock);
	sem_destroy(&writeLock);
	
	
    return 0;
}
















/*
tree_node* delete_AVL(tree_node* root, int val){
	
	
	// A recrusive function to delete the node with key "val"
	// in a subtree rooted at "root_node" and return the root of the modified subtree. 
	
	
	if(root == NULL)
		return root;
	
	if(val < root->key)
		root->left = delete_AVL(root->left, val);
	
	else if(val > root->key)
		root->right = delete_AVL(root->right, val);
	
	else{
		// At this point: root->key = val.
		// Delete the root_node
		
		if(root->left == NULL && root->right == NULL){
			root = NULL;
		}
		else if(root->left == NULL){
			
			tree_node* child = root->right;
			
			
			
			
			
			tree_node* child = root->right;
			root->key = child->key;
			root->left = child->left;
			root->right = child->right;
			
			free(child);
		}
		else if(root->right == NULL){
			tree_node* child = root->left;
			root->key = child->key;
			root->left = child->left;
			root->right = child->right;
			
			free(child);
		}
		else{
			tree_node* suc = leftMost(root->right);
			root->key = suc->key;
			root->right = delete_AVL(root->right, suc->key);
		}
	}
	if(root == NULL)
		return root;
	
	if(abs(get_balance_factor(root)) > 1){
		if(get_balance_factor(root) > 1){
			if(get_balance_factor(root->left) > 0){
				root = single_right_rotation(root);
			}
			else{
				root = double_left_right_rotation(root);
			}
		}
		else{
			if(get_balance_factor(root->right) < 0){
				root = single_left_rotation(root);
			}
			else
				root = double_right_left_rotation(root);
		}
	}
	
	return root;
}

*/