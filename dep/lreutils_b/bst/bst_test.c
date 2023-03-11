static void bst_tests() {
    struct bst* tree = bst_create(5);
    
    bst_insert(tree,7);
    bst_insert(tree,2);
    bst_insert(tree,1);
    bst_insert(tree,4);
    bst_insert(tree,10);
    bst_insert(tree,23);
    bst_insert(tree,9);
    
    bst_displayTree(tree,0,0); printf("\n\n\n");
    
    bst_remove(tree,10);

    bst_displayTree(tree,0,0); printf("\n\n\n");

    struct bst* node = bst_search(tree,9);

    bst_displayTree(node,0,0);

    bst_cleanUpAll(tree);

    bst_displayTree(tree,0,0);
}