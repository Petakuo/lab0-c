#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head =
        (struct list_head *) malloc(sizeof(struct list_head));
    if (head != NULL) {
        INIT_LIST_HEAD(head);
        return head;
    }
    return NULL;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (head == NULL)
        return;
    element_t *entry, *safe;
    list_for_each_entry_safe (entry, safe, head, list) {
        q_release_element(entry);
    }
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (head == NULL || s == NULL)
        return false;
    element_t *node = (element_t *) malloc(sizeof(element_t));
    if (node == NULL)
        return false;
    node->value = strdup(s);
    if (node->value == NULL) {
        free(node);
        return false;
    }
    list_add(&node->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (head == NULL || s == NULL)
        return false;
    element_t *node = (element_t *) malloc(sizeof(element_t));
    if (node == NULL)
        return false;
    node->value = strdup(s);
    if (node->value == NULL) {
        free(node);
        return false;
    }
    list_add_tail(&node->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (head == NULL || head->next == head)
        return NULL;
    element_t *node = list_first_entry(head, element_t, list);
    if (node == NULL)
        return NULL;
    list_del(&node->list);
    if (sp) {
        strncpy(sp, node->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return node;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (head == NULL || head->next == head)
        return NULL;
    element_t *node = list_last_entry(head, element_t, list);
    if (node == NULL)
        return NULL;
    list_del(&node->list);
    if (sp) {
        strncpy(sp, node->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return node;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    int count = 0;
    if (head == NULL)
        return 0;
    element_t *entry;
    list_for_each_entry (entry, head, list) {
        count++;
    }
    return count;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (head == NULL || head->next == head)
        return false;
    int half = q_size(head) / 2;
    struct list_head *ptr = head->next;
    for (int i = 0; i < half; i++) {
        ptr = ptr->next;
    }
    element_t *dnode = list_entry(ptr, element_t, list);
    list_del(ptr);
    q_release_element(dnode);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (head == NULL || head->next == head)
        return false;
    bool flag = 0;
    element_t *entry, *safe;
    list_for_each_entry_safe (entry, safe, head, list) {
        if (entry->list.next != head &&
            strcmp(entry->value, safe->value) == 0) {
            list_del(&entry->list);
            q_release_element(entry);
            flag = 1;
        } else if (flag) {
            list_del(&entry->list);
            q_release_element(entry);
            flag = 0;
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (head == NULL || head->next == head)
        return;
    struct list_head *node = head->next;
    while (node != head && node->next != head) {
        struct list_head *next = node->next;
        list_move(next, node->prev);
        node = node->next;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (head == NULL || head->next == head)
        return;
    struct list_head *node = head->next;
    struct list_head *next = node->next;
    while (next != head) {
        next = node->next;
        list_move(node, head);
        node = next;
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (head == NULL || head->next == head)
        return;
    struct list_head *new_head = head;
    struct list_head *node = head->next;
    struct list_head *next = node->next;
    int c = 1;
    while (next != head) {
        while (c <= k && next != head) {
            next = node->next;
            list_move(node, new_head);
            node = next;
            c++;
        }
        new_head = next->prev;
        c = 1;
    }
}

/* Sort elements of queue in ascending/descending order */
/* void q_sort(struct list_head *head, bool descend)
{
    struct list_head list_less, list_greater;
    struct listitem *pivot;
    struct listitem *item = NULL, *is = NULL;

    if (list_empty(head) || list_is_singular(head))
        return;

    INIT_LIST_HEAD(&list_less);
    INIT_LIST_HEAD(&list_greater);

    pivot = list_first_entry(head, struct listitem, list);
    list_del(&pivot->list);

    list_for_each_entry_safe (item, is, head, list) {
        if (cmpint(&item->i, &pivot->i) < 0)
            list_move_tail(&item->list, &list_less);
        else
            list_move_tail(&item->list, &list_greater);
    }

    q_sort(&list_less, descend);
    q_sort(&list_greater, descend);

    list_splice_tail_init(&list_less, head);
    list_add_tail(&pivot->list, head);
    list_splice_tail(&list_greater, head);
} */

void q_sort(struct list_head *head, bool descend)
{
    if (head == NULL || head->next == head || head->next->next == head)
        return;
    LIST_HEAD(a_head);
    LIST_HEAD(b_head);

    // Find midpooint
    struct list_head *midPoint(struct list_head * head)
    {
        struct list_head *slow = head->next;
        struct list_head *fast = head->next->next;

        while (fast != head && fast->next != head) {
            slow = slow->next;
            fast = fast->next->next;
        }
        return slow;
    }

    // Split head into a_head and b_head
    struct list_head *mid = midPoint(head);
    list_cut_position(&a_head, head, mid);
    list_splice_init(head, &b_head);

    // Merge two linked list without head
    void merge(struct list_head * a_head, struct list_head * b_head,
               struct list_head * c_head, bool descend)
    {
        // Recursive
        // Base case
        if (a_head == NULL || a_head->next == a_head)
            return;
        if (b_head == NULL || b_head->next == b_head)
            return;
        // Recursive case
        while (a_head->next != a_head && b_head->next != b_head) {
            element_t *a_e = list_entry(a_head->next, element_t, list);
            element_t *b_e = list_entry(b_head->next, element_t, list);
            if (descend) {
                if (strcmp(a_e->value, b_e->value) >= 0)
                    list_move_tail(a_head->next, c_head);
                else
                    list_move_tail(b_head->next, c_head);
            } else {
                if (strcmp(a_e->value, b_e->value) <= 0)
                    list_move_tail(a_head->next, c_head);
                else
                    list_move_tail(b_head->next, c_head);
            }
        }
        if (a_head->next != a_head)
            list_splice_tail(a_head, c_head);
        if (b_head->next != b_head)
            list_splice_tail(b_head, c_head);
    }

    // Recursive
    q_sort(&a_head, descend);
    q_sort(&b_head, descend);
    merge(&a_head, &b_head, head, descend);
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (head == NULL || head->next == head)
        return 0;
    struct list_head *current = head->next;
    struct list_head *next = current->next;
    struct list_head *temp = current->prev;
    bool flag = 0;
    int len = q_size(head);
    while (next != head) {
        while (next != head) {
            element_t *current_e = list_entry(current, element_t, list);
            element_t *next_e = list_entry(next, element_t, list);
            temp = current->prev;
            if (strcmp(current_e->value, next_e->value) > 0) {
                list_del(current);
                q_release_element(current_e);
                len--;
                flag = 1;
                break;
            }
            next = next->next;
        }
        if (flag) {
            current = temp->next;
            next = current->next;
        } else {
            current = current->next;
            next = current->next;
        }
        flag = 0;
    }
    return len;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (head == NULL || head->next == head)
        return 0;
    struct list_head *current = head->next;
    struct list_head *next = current->next;
    struct list_head *temp = current->prev;
    bool flag = 0;
    int len = q_size(head);
    while (next != head) {
        while (next != head) {
            element_t *current_e = list_entry(current, element_t, list);
            element_t *next_e = list_entry(next, element_t, list);
            temp = current->prev;
            if (strcmp(current_e->value, next_e->value) < 0) {
                list_del(current);
                q_release_element(current_e);
                len--;
                flag = 1;
                break;
            }
            next = next->next;
        }
        if (flag) {
            current = temp->next;
            next = current->next;
        } else {
            current = current->next;
            next = current->next;
        }
        flag = 0;
    }
    return len;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    if (head == NULL || head->next == head)
        return 0;
    queue_contex_t *q_contex = list_entry(head->next, queue_contex_t, chain);
    struct list_head *q_current = head->next->next;
    while (q_current != head) {
        queue_contex_t *current_contex =
            list_entry(q_current, queue_contex_t, chain);
        list_splice_init(current_contex->q, q_contex->q);
        q_current = q_current->next;
    }
    q_sort(q_contex->q, descend);
    return q_contex->size;
}
