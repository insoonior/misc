/**
 * @file linked_list.c
 * Handle linked lists.
 * The nodes are dynamically allocated by the dyn_mem module,
 */

/*********************
 *      INCLUDES
 *********************/
#include "misc_conf.h"
#if USE_LINKED_LIST != 0
#include <stdint.h>
#include <string.h>

#include "linked_list.h"
#include "dyn_mem.h"

/*********************
 *      DEFINES
 *********************/
#define LL_NODE_META_SIZE (sizeof(ll_node_t*) + sizeof(ll_node_t*))
#define LL_PREV_P_OFFSET(ll_p) (ll_p->n_size)
#define LL_NEXT_P_OFFSET(ll_p) (ll_p->n_size + sizeof(ll_node_t*))
/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void node_set_prev(ll_dsc_t * ll_p, ll_node_t* act_dp, ll_node_t* prev_dp);
static void node_set_next(ll_dsc_t * ll_p, ll_node_t* act_dp, ll_node_t* next_dp);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Initialize linked list
 * @param ll_dsc pointer to ll_dsc variable
 * @param n_size the size of 1 node in bytes 
 */
void ll_init(ll_dsc_t * ll_p, uint32_t n_size)
{
    ll_p->head_dp = NULL;
    ll_p->tail_dp = NULL;

    if(n_size & 0b11) {
        n_size &= ~0b11;
        n_size += 4;
    }

    ll_p->n_size = n_size;   
}

/**
 * Add a new head to a linked list
 * @param ll_p pointer to linked list
 * @return pointer to the new head
 */
void * ll_ins_head(ll_dsc_t * ll_p)
{
    ll_node_t* n_new;
    
    n_new = dm_alloc(ll_p->n_size + LL_NODE_META_SIZE);
    
    if(n_new != NULL) { 
        node_set_prev(ll_p, n_new, NULL);           /*No prev. before the new head*/
        node_set_next(ll_p, n_new, ll_p->head_dp);     /*After new comes the old head*/
        
        if(ll_p->head_dp != NULL) { /*If there is old head then before it goes the new*/
            node_set_prev(ll_p, ll_p->head_dp, n_new);
        }
        
        ll_p->head_dp = n_new;         /*Set the new head in the dsc.*/
        if(ll_p->tail_dp == NULL) {/*If there is no tail (1. node) set the tail too*/
            ll_p->tail_dp = n_new;
        }
    }
    
    return n_new;
}

/**
 * Add a new tail to a linked list
 * @param ll_p pointer to linked list
 * @return pointer to the new tail
 */
void * ll_ins_tail(ll_dsc_t * ll_p)
{   
    ll_node_t* n_new;
    
    n_new = dm_alloc(ll_p->n_size + LL_NODE_META_SIZE);
    
    if(n_new != NULL) {
        node_set_next(ll_p, n_new, NULL); /*No next after the new tail*/
        node_set_prev(ll_p, n_new, ll_p->tail_dp); /*The prev. before new is tho old tail*/
        if(ll_p->tail_dp != NULL) {    /*If there is old tail then the new comes after it*/
            node_set_next(ll_p, ll_p->tail_dp, n_new);
        }
        
        ll_p->tail_dp = n_new;      /*Set the new tail in the dsc.*/
        if(ll_p->head_dp == NULL) { /*If there is no head (1. node) set the head too*/
            ll_p->head_dp = n_new;
        }
    }
    
    return n_new;
}


/**
 * Remove the node 'node_p' from 'll_p' linked list. 
 * It Dose not free the the memory of node.
 * @param ll_p pointer to the linked list of 'node_p'
 * @param node_p pointer to node in 'll_p' linked list
 */
void ll_rem(ll_dsc_t  * ll_p, void * node_p)
{
    if(ll_get_head(ll_p) == node_p) {
        /*The new head will be the node after 'n_act'*/
        ll_p->head_dp = ll_get_next(ll_p, node_p);
        if(ll_p->head_dp == NULL) {
            ll_p->tail_dp = NULL;
        }
        else {
            node_set_prev(ll_p, ll_p->head_dp, NULL);
        }   
    }
    else if(ll_get_tail(ll_p) == node_p) {
        /*The new tail will be the  node before 'n_act'*/
        ll_p->tail_dp = ll_get_prev(ll_p, node_p);
        if(ll_p->tail_dp == NULL) {
            ll_p->head_dp = NULL;
        }
        else {
            node_set_next(ll_p, ll_p->tail_dp, NULL);
        }
    }
    else
    {
        ll_node_t* n_prev = ll_get_prev(ll_p, node_p);
        ll_node_t* n_next = ll_get_next(ll_p, node_p);
        
        node_set_next(ll_p, n_prev, n_next);
        node_set_prev(ll_p, n_next, n_prev);
    }
}

/**
 * Move a node to a new linked list
 * @param ll_ori_p pointer to the original (old) linked list
 * @param ll_new_p pointer to the new linked list
 * @param node_dp pointer to a node
 */
void ll_chg_list(ll_dsc_t * ll_ori_p, ll_dsc_t * ll_new_p, void * node_dp)
{
    ll_rem(ll_ori_p, node_dp);
    
    /*Set node_dp as head*/
    node_set_prev(ll_new_p, node_dp, NULL);      
    node_set_next(ll_new_p, node_dp, ll_new_p->head_dp);
    
    if(ll_new_p->head_dp != NULL) { /*If there is old head then before it goes the new*/
        node_set_prev(ll_new_p, ll_new_p->head_dp, node_dp);
    }

    ll_new_p->head_dp = node_dp;        /*Set the new head in the dsc.*/
    if(ll_new_p->tail_dp == NULL) {     /*If there is no tail (first node) set the tail too*/
        ll_new_p->tail_dp = node_dp;
    }
}

/**
 * Return with head node of the linked list
 * @param ll_p pointer to linked list
 * @return pointer to the head of 'll_p' 
 */
void * ll_get_head(ll_dsc_t * ll_p)
{   
    void * head = NULL;
    
    if(ll_p != NULL)    {
        head = ll_p->head_dp;
    }
    
    return head;
}

/**
 * Return with tail node of the linked list
 * @param ll_p pointer to linked list
 * @return pointer to the head of 'll_p'
 */
void * ll_get_tail(ll_dsc_t * ll_p)
{
    void * tail = NULL;
    
    if(ll_p != NULL)    {
        tail = ll_p->tail_dp;
    }
    
    return tail;
}

/**
 * Return with the pointer of the next node after 'n_act_dp' 
 * @param ll_p pointer to linked list
 * @param n_act pointer a node 
 * @return pointer to the next node 
 */
void * ll_get_next(ll_dsc_t * ll_p, void * n_act_dp)
{
    void * next = NULL;
 
    if(ll_p != NULL)    {
        ll_node_t* n_act_d = n_act_dp;
        memcpy(&next, n_act_d + LL_NEXT_P_OFFSET(ll_p), sizeof(void *));
    } 
    
    return next;
}

/**
 * Return with the pointer of the previous node after 'n_act_dp' 
 * @param ll_p pointer to linked list
 * @param n_act pointer a node 
 * @return pointer to the previous node 
 */
void * ll_get_prev(ll_dsc_t * ll_p, void * n_act_dp)
{
    void * prev = NULL;
 
    if(ll_p != NULL) {
        ll_node_t* n_act_d = n_act_dp;
        memcpy(&prev, n_act_d + LL_PREV_P_OFFSET(ll_p), sizeof(void *));
    }
    
    return prev;
}

void ll_swap(ll_dsc_t * ll_p, void * n1_p, void * n2_p)
{

}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Set the 'pervious node pointer' of a node 
 * @param ll_p pointer to linked list
 * @param act_dp pointer to a node which prev. node pointer should be set
 * @param prev_dp pointer to a node which should be the previous node before 'act_dp'
 */
static void node_set_prev(ll_dsc_t * ll_p, ll_node_t* act_dp, ll_node_t* prev_dp)
{
    memcpy(act_dp + LL_PREV_P_OFFSET(ll_p), &prev_dp, sizeof(ll_node_t*));
}

/**
 * Set the 'next node pointer' of a node 
 * @param ll_p pointer to linked list
 * @param act_dp pointer to a node which next node pointer should be set
 * @param next_dp pointer to a node which should be the next node before 'act_dp'
 */
static void node_set_next(ll_dsc_t * ll_p, ll_node_t* act_dp, ll_node_t* next_dp)
{
    memcpy(act_dp + LL_NEXT_P_OFFSET(ll_p), &next_dp, sizeof(ll_node_t*));
}

#endif