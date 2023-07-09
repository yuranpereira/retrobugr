#include <stdio.h>
#include <stdlib.h>
#include "list.h"

struct list {
    int data;
    struct sl_list_node node;
};

struct moal {
    struct sl_list_node n;
}mn;

int 
main(int argc, char **argv)
{
    struct list *local_list = malloc(sizeof(struct list));

    struct sl_list_node *current;
    struct sl_list_node *head = malloc(sizeof(struct sl_list_node)); // = &local_list->node;

    struct list *n1 = malloc(sizeof(struct list));
    struct list *n2 = malloc(sizeof(struct list));
    struct list *n3 = malloc(sizeof(struct list));
    struct list *n4 = malloc(sizeof(struct list));
    
    n1->data = 11;
    n2->data = 22;
    n3->data = 3333;
    n4->data = 44;

    sl_list_init(head);
    sl_list_add(head, &n1->node);
    sl_list_add_end(head, &n2->node);
    sl_list_add_end(head, &n3->node);

    /*int x;
    if ((x = sl_list_add_pos(head, &n3->node, 2)) < 0)
        printf("Couldn't add in position %d\n", x);
    printf("ret %d\n", x);
    // sl_list_add_end(head, &n4->node);*/

    int i = 1;
    sl_list_traverse (head, current) {
        struct list *l1 = sl_list_node_container(current, struct list, node);
        printf("%p\n", l1);
        printf("%d\n", l1->data);
    }
    
    printf("------------------\n");
    sl_list_oblit(head);
    //sl_list_delete_end(head);
    //sl_list_delete_pos(head, 2);
    //sl_list_delete_pos(head, 2);
    //sl_list_delete_pos(head, 2);
    //sl_list_delete_node(head, &n2->node);
    //sl_list_delete(head);
    //sl_list_delete(head);


    printf("%d\n", sl_list_length(head));

    sl_list_traverse (head, current) {
        struct list *l1 = sl_list_node_container(current, struct list, node);
        printf("%p\n", l1);
        printf("%d\n", l1->data);
    }
    printf("%p\n", head->next);
    printf("n1->next %p\n", n1->node.next);
    printf("n2->next %p\n", n2->node.next);
    printf("n3->next %p\n", n3->node.next);



    return 0;
}
