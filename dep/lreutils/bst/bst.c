#ifndef bst_c
#define bst_c

#include "bst.h"

struct bst* bst_create(bst_T value) {
    struct bst* new = (struct bst*)malloc(sizeof(struct bst));
    new->key = value;
    new->left = NULL;
    new->right = NULL;
    return new; 
}

struct bst* bst_insert(struct bst* head, bst_T value) {
    struct bst* start = head;
    struct bst* newNode = bst_create(value);
    while (true) {
        if (head->key > value) {
            if (head->left == NULL) {
                head->left = newNode;
                break;
            } else head = head->left;
        } else {
            if (head->right == NULL) {
                head->right = newNode;
                break;
            } else head = head->right;
        }
    }
    return start;
}

struct bst* bst_search(struct bst* head, bst_T value) {
    while (true) {
        if (head->key == value) return head;
        else if (head->key > value) {
            if (head->left == NULL) return (struct bst*)NULL;
            else head = head->left;
        } else {
            if (head->right == NULL) return (struct bst*)NULL;
            else head = head->right;
        }
    }
    return head;
}

struct bst* bst_remove(struct bst* node,bst_T key) {
    if (node == NULL) {
        return node;
    }

    if (node->key > key) {
        node->left = bst_remove(node->left,key);
        return node;
    }
    else if (node->key < key) {
        node->right = bst_remove(node->right,key);
        return node;
    }
    
    if (node->left == NULL) {
        struct bst* temp = node->right;
        free(node);
        return temp;
    } 
    else if (node->right == NULL) {
        struct bst* temp = node->left;
        free(node);
        return temp;
    }
    else {
        struct bst* succParent = node;
        struct bst* succ = node->right;

        while (succ->left != NULL) {
            succParent = succ;
            succ = succ->left;
        }

        if (succParent != node)
            succParent->left = succ->right;
        else
            succParent->right = succ->right;


        succ->left = node->left;
        succ->right = node->right;
        // node->key = succ->key;
        // free(succ);
        // return node;
        free(node);
        return succ;
    }

}

void bst_cleanUp(struct bst* node) {
    free(node);
}

void bst_cleanUpAll(struct bst* head) { //recursive call
    if (head->left != NULL) {bst_cleanUpAll(head->left);}
    if (head->right != NULL) {bst_cleanUpAll(head->right);}
    bst_cleanUp(head);
}

void bst_displayTree(struct bst* head, int depth, char edge) {
    if (head == NULL) {
        return;
    }
    bst_displayTree(head->right, depth + 1, '/');
    for (int i = 0; i < depth; i++) {
        printf("    ");
    }
    printf("%c%llu\n", edge, head->key);
    bst_displayTree(head->left, depth + 1, '\\');
}


#endif