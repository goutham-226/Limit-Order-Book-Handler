/*
 * Create a gloabal array with 8 integers -> this is our pool.
 *
 * Keep track of free slots using a stack
 * so the initial stack has all the index values of the pool.
 *
 * when a new order is added to the pool, pop the stack -> get index -> (if stack_size is 0 then pool isnt empty)
 * and assign order value to the pool at the popped index. -> set in_use tracker to true.
 * 
 * 
 * when an order is deleted check if in_use is false, push the index to our stack
 * 
 * stack = 7 6 5 4 3 2 1 0 -> 0 is popped and a new order is added there initially every slot
 * is empty so initialise stack with index values of our pool.
 *
 * Add a method to add new_orders in the pool and return the index(used for deletion)
 * if stack is empty no slots are available.
 **/

#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>

typedef struct POOL{
    int* pool;
    int* stack;
    int stack_size;
    bool* in_use;
}FixedSizedPool;

// init mem pool, stack and active flag arr.
FixedSizedPool* pool_alloc()
{
    FixedSizedPool* new_pool = (FixedSizedPool*)malloc(sizeof(FixedSizedPool));
    new_pool->pool = (int*)malloc(8*sizeof(int));
    new_pool->stack = (int*)malloc(8*sizeof(int));
    new_pool->stack_size = 8; // full stack, empty pool.
    new_pool->in_use = (bool*)malloc(8*sizeof(bool));
    int j = 7;
    for(int i = 0; i<8; i++)
    {
        new_pool->stack[i] = j--;
        new_pool->in_use[i] = false;
    }
    return new_pool;
}

int add_order(FixedSizedPool* pool_obj, int order)
{
    if(pool_obj->stack_size == 0)
    {
        //"No slots available!\n"
        return -1;
    }
    //get the empty slot
    int index = pool_obj->stack[--pool_obj->stack_size]; // stack[7] = 0
    pool_obj->pool[index] = order;
    pool_obj->in_use[index] = true;
    return index;
}
    
bool delete_order(FixedSizedPool* p, int index)
{
    if(index < 0 || index > 7 || !p->in_use[index])
    {
        return false;
    }

    p->stack[p->stack_size++] = index; // push index onto stack to track empty slots.
    p->in_use[index] = false;
    return true;
}
 
void print_orders(FixedSizedPool* p)
{
    for(int i=0; i<8; i++)
    {
        if(p->in_use[i])
        {
            printf("%d ,",p->pool[i]);
        }
    }
    printf("\n");
    printf("stack: ");
    for(int i=0; i < p->stack_size; i++)
    {
        printf("%d ,",p->stack[i]);
    }
    printf("\n");
    printf("stack_size = %d\n",p->stack_size);
    for(int i=0; i<8; i++)
    {
        printf(" Index %d is in use %d\n",i,p->in_use[i]);
    }
}

void pool_destroy(FixedSizedPool* p)
{
    p->stack_size = 0;
    free(p->pool);
    free(p->stack);
    free(p->in_use);
    free(p);
}

int main()
{
    FixedSizedPool* p = pool_alloc();
    int orderA = add_order(p,1);
    int orderB = add_order(p,2);
    int orderC = add_order(p,3);
    print_orders(p);
    delete_order(p,orderB);
    print_orders(p);
    int orderD = add_order(p,4);
    int orderE = add_order(p,5);
    int orderF = add_order(p,6);
    int orderG = add_order(p,7);
    int orderH = add_order(p,8);
    int orderI = add_order(p,9);
    print_orders(p);
    if(add_order(p,10) == -1)
    {
        printf("Pool is Full!\n"); 
        
    }
    
    pool_destroy(p);
    return 0;
}








