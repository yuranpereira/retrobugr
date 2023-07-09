/*
 * Copyright (c) 2023 Yuran Pereira
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the “Software”), 
 * to deal in the Software without restriction, including without limitation 
 * the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom the Software 
 * is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all 
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS 
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN 
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION 
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


/* 
 * This is a simple header with generic functions for creating and
 * manipulating "Singly Linked Lists".
 * 
 * It must be noted that the list structures herein are "intrusive"
 * linked lists,  meaning that  one must  embbed  the `*_list_node` 
 * structure  within the  actual  "target" node  data structure. 
 * This implementation is similar to the  one used  in the Linux 
 * kernel, as a matter of fact it is inspired by that implementation.
 *
 * Memory Allocation: Note that this library does not perform any type
 * type of memory management, and that  allocating and deallocating
 * memory is responsibility of the user. 
 *
 */

#include <stddef.h>

/* Generic Singly Linked List structure */
struct sl_list_node {
    struct sl_list_node *next;
};

/**
 * Initialize a newly created signly linked list.
 */
static void sl_list_init(struct sl_list_node *head) 
{
    head->next = NULL;
}

/**
 * This "for loop" macro traverses a list.
 *
 * @param head      - The `head` of the list to be traversed
 * @param current   - A `sl_list_node *` to store the current node
 */
#define sl_list_traverse(head, current) \
    for (current = head->next; current != NULL; current = current->next)

/**
 * Get the structure in which the sl_list_node is embedded
 */
#define sl_list_node_container(node, type, member) \
    (type *)((void *)node - offsetof(type, member))

static int sl_list_is_empty(struct sl_list_node *head)
{
    return (head->next == NULL);
}

/**
 * Add `n2` to list at position following `n1`
 */
static void __sl_list_add_after(struct sl_list_node *n1, 
        struct sl_list_node *n2) 
{
    n2->next = n1->next;
    n1->next = n2;
}

/**
 * Adds a node to the list
 * This defaults to adding the node to the head of
 * the list.
 *
 * @param head - pointer to the head of the list
 * @param node - pointer to the node to be added
 */
static void sl_list_add(struct sl_list_node *head, 
        struct sl_list_node *node) 
{
    __sl_list_add_after(head, node);
}

/**
 * Adds a list node to the end of a list referenced by
 * `head`.
 *
 * @param head - pointer to the head of the list
 * @param node - pointer to the node to be added
 */
static void sl_list_add_end(struct sl_list_node *head, 
        struct sl_list_node *node)
{
    struct sl_list_node *current;

    if (sl_list_is_empty(head)) {
        __sl_list_add_after(head, node);
        return;
    }

    sl_list_traverse(head, current) {
        if (current->next == NULL) {
            __sl_list_add_after(current, node);
            
            /* 
             * NOTE - The following break is extremely
             * important to avoid an infinite loop.
             */
            break;
        }
    }
}

/**
 * Adds a node to a position `pos` in the list.
 * Returns `pos` on success, or -1 on error.
 
 * @param head - pointer to the head of the list
 * @param node - pointer to the node to be added
 * @param pos  - position to insert the node at.
 */
static int sl_list_add_pos(struct sl_list_node *head, 
        struct sl_list_node *node, unsigned int pos)
{
    struct sl_list_node *current;
    int i = 1;

    if (sl_list_is_empty(head)) {
        return -1;
    }

    sl_list_traverse(head, current) {
        if (current != NULL && i == pos) {
            __sl_list_add_after(current, node);
            return i;
        }
        i++;
    }

    return -1;
}

/*
 * Deletes the node after the node pointed to by `n`
 */
static void __sl_list_delete_after(struct sl_list_node *n)
{
    n->next = n->next->next;
}

/*
 * Deletes a node from the list. This defaults to the
 * first node of the list.
 *
 * @param head - pointer to the head of the list
 */
static void sl_list_delete(struct sl_list_node *head)
{
    if (sl_list_is_empty(head))
        return;
    
    __sl_list_delete_after(head);
}

/** 
 * Deletes last node from the list pointed by `head`
 *
 * @param head - pointer to the head of the list
 */
static void sl_list_delete_end(struct sl_list_node *head)
{
    struct sl_list_node *prev, *current;

    if (sl_list_is_empty(head))
        return;
    
    prev = head;
    sl_list_traverse(head, current) {
        if (current->next == NULL) {
            __sl_list_delete_after(prev);
            break;
        }
        prev = current;
    }
}

/**
 * Deletes a node from position `pos` from a list pointed to by
 * `head`
 *
 * @param head - pointer to the head of the list
 * @param pos  - specifies the position of the node to be deleted
 */
static void sl_list_delete_pos(struct sl_list_node *head, unsigned int pos)
{
    struct sl_list_node *prev, *current;
    int i = 1;

    if (sl_list_is_empty(head))
        return;

    prev = head;
    sl_list_traverse(head, current) {
        if (i == pos) {
            __sl_list_delete_after(prev);
            break;
        }
        prev = current;
        i++;
    }

}

/**
 * Deletes a node `node` from the list pointed to by `head`
 *
 * @param head - pointer to the head of the list
 * @param node - pointer to the node to be added
 */
static void sl_list_delete_node(struct sl_list_node *head,
        struct sl_list_node *node)
{
    struct sl_list_node *prev, *current;

    if (sl_list_is_empty(head))
        return;

    prev = head;
    sl_list_traverse(head, current) {
        if (prev->next == node) {
            __sl_list_delete_after(prev);
            break;
        }
        prev = current;
    }
}

/* 
 * Returns the length of a list pointed to by head
 *
 * @param head - pointer to the head of the list
 */
static unsigned int sl_list_length(struct sl_list_node *head)
{
    struct sl_list_node *current;
    int len = 0;

    if (sl_list_is_empty(head))
        return len;

    sl_list_traverse(head, current)
        len++;

    return len;
}

/*
 * Clears the list pointed to by head
 */
static void sl_list_clear(struct sl_list_node *head)
{
    sl_list_init(head);
}

/*
 * Obliterates list. Unlike sl_list_clear, this function doesn't
 * just reset the head->next back to NULL.  Instead it  deletes
 * every single entry of the  list prior to setting  head->next
 * back to NULL.
 *
 * This is considerably slower than "sl_list_clear" but it ensures
 * that no node  contains the  pointer of another  node after  the
 * list is cleared.
 *
 * Only use it if absolutely necessary. As the process of looping
 * and deleting each and every node is slow.
 *
 * @param - pointer to the head of the list
 */
static void sl_list_oblit(struct sl_list_node *head)
{
    struct sl_list_node *prev, *current;

    if (sl_list_is_empty(head))
        return;

    prev = head;
    sl_list_traverse(head, current) {
        prev->next = NULL;
        prev = current;
    }
}

/**
 * This comment acts as a delimeter for the functions that
 * have not yet been implemented.
 *
 * In the future I should consider implementing the functions
 * below this point. If any of the following functions are
 * ever implemented, they MUST be moved upward.
 */
static struct sl_list_node *sl_list_search(void)
{
}

static int sl_list_contains(void) 
{
}

static void sl_list_reverse(void) 
{
}


static void sl_list_insert_before(void) 
{
}

static void sl_list_insert_after(void) 
{
}

static void sl_list_copy(void) 
{
}

static void sl_list_merge(void) 
{
}

