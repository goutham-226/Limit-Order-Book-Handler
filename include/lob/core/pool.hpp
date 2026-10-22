#pragma once // ensures file is read only once per compilation.

#include<iostream>
#include<array>
#include<cstdint>

// namespace wrapper

namespace lob {

class Pool
{
    private:
         static constexpr std::uint32_t capacity = 8;
         std::array<std::uint32_t, capacity> memory;
         std::array<std::uint32_t,capacity> stack;
         std::uint32_t stack_size;
         std::array<bool,capacity> is_active;
    
    public:
         static constexpr std::uint32_t max_val = UINT32_MAX;
         
         Pool()
         {
             stack_size = capacity;
             std::uint32_t j = capacity-1;
             for(std::uint32_t i=0; i<capacity; i++)
             {
                 stack[i] = j--;
                 is_active[i] = false;
             }
                 
         }
        
         std::uint32_t add_order(std::uint32_t order)
         {
             if(stack_size == 0)
             {
                 return max_val;
             }
      
             //pop an empty_slot from the stack and assign order to the pool. 
             std::uint32_t index = stack[--stack_size];
             memory[index] = order;
             is_active[index] = true;
              
             return index;
         }

	 bool delete_order(std::uint32_t index)
         {
             if(index >= capacity || !is_active[index])
             {
                 return false;
             }
   
             //push the index onto the stack
             stack[stack_size++] = index;
             is_active[index] = false;
          
             return true;
             // now a new order is set to the index in the stack
         }

         void print_orders()
         {
              for(std::uint32_t i=0; i<capacity; i++)
              {
                   if(is_active[i])
                   {
                        std::cout << memory[i] << ", ";
                   }
              }
              std::cout << std::endl;
              std::cout << "stack: ";
              for(std::uint32_t i=0; i < stack_size; i++)
              {
                  std::cout << stack[i] << ", ";
              }
              std::cout << std::endl;
             
              std::cout << " stack_size = " << stack_size << std::endl;
     
              for(std::uint32_t i=0; i<capacity ; i++)
              {
                   std::cout << " Index i = " << i << "is in use: " << is_active[i] << std::endl;
              }
 	 }

         std::uint32_t slots_in_use()
         {
             return capacity - stack_size;
         }
             
  
         
};

}





