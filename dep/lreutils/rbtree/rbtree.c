#ifndef rbtree_c
#define rbtree_c

#include "rbtree.h"

struct rbtree* rbtree_create(rbtree_T value) {
    struct rbtree* node = (struct rbtree*)malloc(sizeof(struct rbtree));
    node->key = value;
    node->left = NULL;
    node->right = NULL;
    node->parent = NULL;
    node->c = rbc_black;
    return node;
}

static inline struct rbtree* _rbtree_insert_create(rbtree_T value,enum rbtree_color c) {
    struct rbtree* node = (struct rbtree*)malloc(sizeof(struct rbtree));
    node->key = value;
    node->left = NULL;
    node->right = NULL;
    node->parent = NULL;
    node->c = c;
    return node;
}

struct rbtree* rbtree_search(struct rbtree* head, rbtree_T value) {
    while (true) {
        if (head->key == value) return head;
        else if (head->key > value) {
            if (head->left == NULL) return (struct rbtree*)NULL;
            else head = head->left;
        } else {
            if (head->right == NULL) return (struct rbtree*)NULL;
            else head = head->right;
        }
    }
    return head;
}

void rbtree_displayTree(struct rbtree* head, int depth, char edge) {
    if (head == NULL) {
        return;
    }
    rbtree_displayTree(head->right, depth + 1, '/');
    for (int i = 0; i < depth; i++) {
        printf("    ");
    }
    char color;
    if (head->c == rbc_black) {
        color = 'b';
    }
    else {
        color = 'r';
    }

    printf("(%c%c)%llu\n", edge, color, head->key);
    rbtree_displayTree(head->left, depth + 1, '\\');
}

static inline update_parent(struct rbtree* node1, struct rbtree* node2) {
    if (node1 != NULL) {
        node1->parent = node2;
    }
}

//takes in the grandparent
static inline void _rbtree_rotate_left(struct rbtree* head) {
    struct rbtree* ogParent = head->parent; 
    struct rbtree* newHead = head->left;
    struct rbtree* right = newHead->right;

    newHead->right = head;
    update_parent(head, newHead); // fix parent
    head->left = right;
    update_parent(right, head); // fix parent

    if (ogParent != NULL) {
        if (ogParent->left == head) {
            ogParent->left = newHead;
        }
        else {
            ogParent->right = newHead;
        }
    }
    update_parent(newHead,ogParent);

}

//takes in the grandparent
static inline void _rbtree_rotate_right(struct rbtree* head) {
    struct rbtree* ogParent = head->parent;
    struct rbtree* newHead = head->right;
    struct rbtree* left = newHead->left;

    newHead->left = head;
    update_parent(head, newHead); // fix parent
    head->right = left;
    update_parent(left, head); // fix parent
    
    if (ogParent != NULL) {
        if (ogParent->left == head) {
            ogParent->left = newHead;
        }
        else {
            ogParent->right = newHead;
        }
    }
    update_parent(newHead,ogParent);

}

static bool l1 = false;
static bool l2 = false;
static bool r1 = false;
static bool r2 = false;
static inline void resetFlags() {
    l1 = false;
    l2 = false;
    r1 = false;
    r2 = false;
} 

static inline void _rbtree_color_swap(struct rbtree* node1,struct rbtree* node2) {
    if (node2 == NULL) {
        node1->c = rbc_black;
    }
    else {
        enum rbtree_color temp = node2->c;
        node2->c = node1->c;
        node1->c = temp;
    }
    
    
}

static inline void _rbtree_color_swap_c(struct rbtree* node1,enum rbtree_color c) {
    node1->c = c;
}

static inline enum rbtree_color _rbtree_get_color(struct rbtree* node) {
    if (node == NULL) {
        return rbc_black;
    } else {
        return node->c;
    }
}

static inline struct rbtree* _rbtree_rotate_generic(struct rbtree* node) {
    
    struct rbtree* base = node->parent->parent->parent;
    
    char dir = 0;
    if (base != NULL) {
        if (node->parent->parent == base->left) {
            dir = 'l';
        } else {
            dir = 'r';
        }
    }

    if (l1 && l2) {
        //rotate left
        _rbtree_rotate_left(node->parent->parent);

        //swap grandparent and parent
        _rbtree_color_swap(node->parent,node->parent->right);

        if (dir == 0) {
            // node->parent->c = rbc_black;
            return node->parent;
        } else {
            if (dir == 'l')
                base->left = node->parent;
            else base->right = node->parent;
        }
        return NULL;
    }
    else if (l1 && r2) {
        //swap node and parent (parent is now nodes left)
        _rbtree_rotate_left(node->parent); //node is now base
        //rotate left
        _rbtree_rotate_right(node->parent); //node is now in parents location

        //swap grandparent and inserted node
        _rbtree_color_swap(node,node->left);
        
        if (dir == 0) {
            // node->c = rbc_black;
            return node;
        } else {
            if (dir == 'l')
                base->left = node;
            else base->right = node;
        }
        return NULL;
        
    }
    else if (r1 && r2) {
        //rotate right
        _rbtree_rotate_right(node->parent->parent);
        //swap parent and parent->left
        _rbtree_color_swap(node->parent,node->parent->left);

        if (dir == 0) {
            // node->parent->c = rbc_black;
            return node->parent;
        } else {
            if (dir == 'l')
                base->left = node->parent;
            else base->right = node->parent;
        }
        return NULL;
    }
    else if (r1 && l2) {
        //swap node and parent (parent is now nodes right)
        _rbtree_rotate_right(node->parent); //node is now base
        //rotate right
        _rbtree_rotate_left(node->parent); //node is now in parents location
        //swap inserted node and grandparent
        _rbtree_color_swap(node,node->right); 
        
        if (dir == 0) {
            // node->c = rbc_black;
            return node;
        } else {
            if (dir == 'l')
                base->left = node;
            else base->right = node;
        }
        return NULL;
        
    }
    return NULL;
}

//ONLY CALL IF NODE IS RED
static inline struct rbtree* _rbtree_insert_fixup(struct rbtree* node) {
    resetFlags();
    struct rbtree* parent = node->parent;
    if (parent->right == node) {
        r1 = true; //right
    } else {
        l1 = true; //left
    }
    if (node->parent == NULL) {
        parent->c = rbc_black;
    } else {
        if (parent->c == rbc_red) { //not currently needed
            struct rbtree* grandparent = parent->parent;
            struct rbtree* uncle;
            if (grandparent->left == parent) {
                uncle = grandparent->right; //left rotation
                l2 = true;
            } else {
                uncle = grandparent->left; //right rotation
                r2 = true;
            }
            
            enum rbtree_color uncle_c = _rbtree_get_color(uncle);

            if (uncle_c == rbc_red) {
                parent->c = rbc_black;
                uncle->c = rbc_black;
                if (grandparent->parent != NULL) {
                    grandparent->c = rbc_red;
                    return _rbtree_insert_fixup(grandparent);
                }
            } else {
                
                return _rbtree_rotate_generic(node);

            }
        }
    }
    
    return NULL;
}

struct rbtree* rbtree_insert(struct rbtree* head,rbtree_T value) {
    struct rbtree* root = head;
    struct rbtree* newNode = _rbtree_insert_create(value,rbc_red);

    while (true) {
        if (head->key > value) {
            if (head->left == NULL) {
                head->left = newNode;
                newNode->parent = head;
                break;
            } else {
                head = head->left;
            }
        } else {
            if (head->right == NULL) {
                head->right = newNode;
                newNode->parent = head;
                break;
            } else {
                head = head->right;
            }
        }
    }

    if (head->c == rbc_red) {
        struct rbtree* newHead = _rbtree_insert_fixup(newNode);
        if (newHead != NULL) {
            root = newHead;
        }
        
    }

    return root;
}

static enum rbtree_color flag_deleted_color;
static struct rbtree* flag_replacement_node;


static struct rbtree* _generic_remove(struct rbtree* node,rbtree_T key) {
    flag_replacement_node = NULL;

    if (node == NULL) {
        return node;
    }

    if (node->key > key) {
        node->left = _generic_remove(node->left,key);
        update_parent(node->left,node);
        return node;
    }
    else if (node->key < key) {
        node->right = _generic_remove(node->right,key);
        update_parent(node->right,node);
        return node;
    }
    
    if (node->left == NULL) {
        struct rbtree* temp = node->right;
        update_parent(temp,node);
        flag_deleted_color = node->c;
        flag_replacement_node = temp;
        free(node);
        return temp;
    } 
    else if (node->right == NULL) {
        struct rbtree* temp = node->left;
        update_parent(temp,node);
        flag_deleted_color = node->c;
        flag_replacement_node = temp;
        free(node);
        return temp;
    }
    else {
        struct rbtree* succParent = node;
        struct rbtree* succ = node->right;

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
        succ->parent = node->parent;
        
        flag_deleted_color = node->c;
        flag_replacement_node = succ;
        free(node);
        return succ;
    }

}

static struct rbtree* _rbtree_remove_fixup(struct rbtree* node) {
    resetFlags();
    struct rbtree* parent = node->parent;
    struct rbtree* newSibling;

    if (node->parent == NULL) {
        return NULL;
    }

    if (parent->right == node) {
        l1 = true; //left case
        newSibling = parent->left;
    } else {
        r1 = true; //right case
        newSibling = parent->right;
    }

    if (newSibling != NULL) {
        //CASE A (sibling S is black and one of children is red)
        enum rbtree_color siblingColor = _rbtree_get_color(newSibling);
        enum rbtree_color siblingColorL = _rbtree_get_color(newSibling->left);
        enum rbtree_color siblingColorR = _rbtree_get_color(newSibling->right);

        if (siblingColor == rbc_black) {
            if (siblingColorR == rbc_red) {
                r2 = true;
                struct rbtree* newHead = _rbtree_rotate_generic(newSibling);
                return newHead;
            } else if (siblingColorL == rbc_red) {
                l2 = true;
                struct rbtree* newHead = _rbtree_rotate_generic(newSibling);
                return newHead;
            } else {
                //Apply recoloring 
                //if the parent of the sibling is red just make it black
                //and do not recurs
                if (newSibling->parent->c == rbc_red) {
                    newSibling->c = rbc_red;
                    newSibling->parent->c = rbc_black;
                } else {
                    //blackbox recoloring logic
                    newSibling->c = rbc_red;
                    return _rbtree_remove_fixup(newSibling->parent);
                }
            }
        }  else {
            //rotation to move old sibling up (rotate parent of old sibling)
            if (!r1) {
                //RIGHT CASE
                l1 = false;
                l2 = false;
                r1 = true;
                r2 = true;
                struct rbtree* newHead = _rbtree_rotate_generic(node);
                return newHead;
            } else {
                //LEFT CASE
                r1 = false;
                r2 = false;
                l1 = true; 
                l2 = true;
                struct rbtree* newHead = _rbtree_rotate_generic(node);
                return newHead;
            }
        }
    }
    return NULL;
}

struct rbtree* rbtree_remove(struct rbtree* head, rbtree_T value) {
    head = _generic_remove(head,value);

    // printf("sibling node color %d\nprinting replacement node:\n", (int)flag_deleted_color);
    // rbtree_displayTree(head,0,0); printf("\n");
    if (flag_replacement_node != NULL) {
        if ((flag_deleted_color == rbc_red || flag_replacement_node->c == rbc_red)) {
            flag_replacement_node->c = rbc_black;
        }
        else {
            if (flag_replacement_node->parent != NULL) { //NOT ROOT
                struct rbtree* newHead = _rbtree_remove_fixup(flag_replacement_node);
                if (newHead != NULL) return newHead;
            }
        }
    }
    

    return head;
}

void rbtree_cleanUp(struct rbtree* node) {
    free(node);
}

void rbtree_cleanUpAll(struct rbtree* head) { //recursive call
    if (head->left != NULL) {rbtree_cleanUpAll(head->left);}
    if (head->right != NULL) {rbtree_cleanUpAll(head->right);}
    rbtree_cleanUp(head);
}




#endif