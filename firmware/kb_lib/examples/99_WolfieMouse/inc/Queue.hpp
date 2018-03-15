#ifndef Queue_h
#define Queue_h

#include <config_maze.hpp>
#include <stdlib.h>

#ifndef mazeQUEUE_MAX_BUFFER
#define mazeQUEUE_MAX_BUFFER 1000
#warning "mazeQUEUE_MAX_BUFFER is not defined to we use default size 1000"
#endif

#define safeIncrease(index) if(index != (mazeQUEUE_MAX_BUFFER - 1)) \
        index++; \
      else \
        index = 0
#define safeDecrease(index) if(index != 0) \
        index--; \
      else \
        index = mazeQUEUE_MAX_BUFFER - 1

/**
 * @brief      Double-sided queue without using dynamic memory allocation
 *
 * @tparam     T     type of class
 */
template<class T>
	class Queue
	{
	private:
		T buffer[mazeQUEUE_MAX_BUFFER]; /* buffer inside the queue */
		int front; /* index of front */
		int back; /* index of back */
		int size; /* number of objects currently stored in the queue */

	public:

		/**
		 * @brief      constructor of the class
		 */
		Queue ()
		{
			init();
		}

		/**
		 * @brief      initialize the Queue
		 */
		void
		init ()
		{
			front = 0;
			back = 0;
			size = 0;
		}

		/**
		 * @brief      push an object to the Queue from back
		 *
		 * @param[in]  obj   object to push
		 *
		 * @return     On Success: the size of Queue
		 *             On failure: -1
		 */
		int
		pushToBack (T obj)
		{
			if (!isFull())
			{
				/* Increase the back */
				safeIncrease(back);
				size++; /* Increase the size */
				buffer[back] = obj; /* fill the object later */
				return size;
			}
			return -1;
		}

		/**
		 * @brief      push an object to the Queue from front
		 *
		 * @param[in]  obj   object to push
		 *
		 * @return     On Success: the size of Queue
		 *             On failure: -1
		 */
		int
		pushToFront (T obj)
		{
			if (!isFull())
			{
				/* fill the object first */
				buffer[front] = obj;
				/* decrease the front */
				safeDecrease(front);
				size++; /* Increase the size */
				return size;
			}
			return -1;
		}

		/**
		 * @brief      pop the Queue from back
		 *
		 * @return     On Success: an object from the back
		 *             On failure: T()
		 */
		T
		popFromBack ()
		{
			T temp;
			if (!isEmpty())
			{
				temp = buffer[back];
				/* decrease back */
				safeDecrease(back);
				size--; /* decrease the size */
				return temp;
			}
			return T();
		}

		/**
		 * @brief      pop the Queue from front
		 *
		 * @return     On Success: an object from the front
		 *             On failure: T()
		 */
		T
		popFromFront ()
		{
			T temp;
			if (!isEmpty())
			{
				temp = buffer[front + 1];
				/* Increase the front */
				safeIncrease(front);
				size--; /* decrease the size */
				return temp;
			}
			return T();
		}

		/**
		 * @brief      pop the Queue from back
		 *
		 * @return     On Success: an object from the back
		 *             On failure: T()
		 */
		T
		peekFromBack ()
		{
			if (!isEmpty())
			{
				return buffer[back];
			}
			return T();
		}

		/**
		 * @brief      pop the Queue from front
		 *
		 * @return     On Success: an object from the front
		 *             On failure: T()
		 */
		T
		peekFromFront ()
		{
			if (!isEmpty())
			{
				return buffer[front + 1];
			}
			return T();
		}

		/**
		 * @brief      check if the Queue is empty
		 *
		 * @return     if empty: True
		 *             if not: false
		 */
		bool
		isEmpty ()
		{
			if (size != 0)
			{
				return false;
			}
			return true;
		}

		/**
		 * @brief      check if the Queue is full
		 *
		 * @return     if full: True
		 *             if not: false
		 */
		bool
		isFull ()
		{
			if (size != mazeQUEUE_MAX_BUFFER)
			{
				return false;
			}
			return true;
		}

		void
		print (void (T::*pvPrint)(T))
		{
			T tmp;
			int i;
			int j;
			for(i = 0, j = front; i<size; i++)
			{
				(tmp.*pvPrint)(buffer[j]);
				safeIncrease(j);
			}
		}
	};

#endif
