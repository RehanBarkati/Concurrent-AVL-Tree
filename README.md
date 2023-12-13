# Concurrent-AVL-Tree

Implemented the dynamic (insertion/deletion/contains) data structure AVL Tree in concurrent environment.

• Here the data structure is shared among multiple threads responsible for each of the operations: insertion, deletion, contains and in-order traversal

• Separate thread was created for each insertion, deletion, contains and any in-order traversals

• For in-order traversal, just print out the traversal in one line separated by space.

• At the end, also print the pre order traversal of the tree.

• Example input:

    insert 3
    
    insert 4
    
    insert 2
    
    delete 2
    
    contains 3 - Return true if 3 is there in the tree otherwise false
     
    in order - print in order traversal of the tree
  
• Example output:

    yes
    
    3 4
    
    3 4
