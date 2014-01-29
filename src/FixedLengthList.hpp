/**
   @file
   @brief Template class ( FixedLengthList ) to implement a list with
          a limited number of elements.

   @author John Bailey

   @copyright Copyright 2013 John Bailey

   @section LICENSE

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

*/

#if !defined FIXEDLENGTHLIST_HPP
#define      FIXEDLENGTHLIST_HPP

#include <cstddef> // for size_t, NULL
#include <cstring> // For memset()
#include <algorithm> // for min()

#ifndef STATIC_ASSERT
/** Emulation of C++11's static_assert */
#define STATIC_ASSERT( condition, name ) typedef char assert_failed_ ## name [ (condition) ? 1 : -1 ]
#endif

/*
    Each item in the FixedLengthList is wrapped in a
    FixedLengthListItem which provides the actual item and
    infrastructure support for the list */
template < class L > class FixedLengthListItem
{
    public:
        /** Pointer to the next item in the list */
        FixedLengthListItem<L>* m_forward;
        /** The content/value of the item itself */
        L                       m_item;
};


/**
    Iterator support class for FixedLengthList

   Example:
   \code
          #define LIST_LEN (20U)
          FixedLengthList<int,  LIST_LEN > list;

          int main( void ) {
             // List is empty

             list.queue( 111 );
             list.queue( 222 );

             FixedLengthList<int, LIST_LEN>::iterator it = list.begin();
             *it == 111;
             it++;
             *it == 222;
             it++;
             *it == list.end();
          }
   \endcode
*/
template< class T, size_t queueMax > class FixedLengthListIter
{
    protected:
        /** The iterator hooks into the used list within the FixedLengthList */
        FixedLengthListItem<T>* m_item;
    public:
        /** Void constructor - iterator will be equal to T::end() */
        FixedLengthListIter( void );
        /** Construct an iterator which points to a list item within a
            FixedLengthList */
        FixedLengthListIter( FixedLengthListItem<T>* p_item );
        /** De-reference operator, yields the value of the list item */
        T& operator*();
        /** Inequality operator */
        bool operator!=( const FixedLengthListIter& p_comp ) const;
        /** Equality operator */
        bool operator==( const FixedLengthListIter& p_comp ) const;
        /** Move the iterator forward a specified number of list elements
 
            \param p_inc Number of items to traverse */
        FixedLengthListIter& operator+=( const unsigned p_inc );
        /** Post-increment operator */
        FixedLengthListIter operator++( int p_int );
        /** Pre-increment operator */
        FixedLengthListIter& operator++( void );
};


/**
   Template class to implement a list with a fixed maximum
   number of elements (i.e. the number of elements in the list
   is variable but cannot exceed a defined maximum).

   While stl::list may be suitable for many
   occasions, it invariably makes use of dynamic memory
   allocation which can either be undesirable or overkill
   in some situations.  Of course, the down side of this
   class is that the memory required to store all items
   in the list is allocated for the duration of the
   instantiation.

   The implementation is based around a singly linked list
   with a stack of free elements.  Adding an item to the list
   causes one of the free elements to be popped, populated
   then inserted into the linked list of used elements.  For
   convenience both a head and tail pointer of the used list
   are maintained.

   Note that the class currently is not thread safe.

   Example:
   \code
          #define LIST_LEN (20U)
          FixedLengthList<int,  LIST_LEN > list;

          int main( void ) {
             int i;
             
             // List is empty
             
             list.queue( 111 );
             // List now contains 111
             
             list.queue( 222 );
             // List now contains 111, 222
             
             list.push( 333 );
             // List now contains 333, 111, 222
          
             list.pop( &i );
             // i == 333
             // List now contains 111, 222
          
             return 0;    
          }
    \endcode
*/
template < class T, size_t queueMax > class FixedLengthList
{
    /* Pointless to have a queue with no space in it, so the various methods
       shouldn't have to deal with this situation */
    STATIC_ASSERT( queueMax > 0, Queue_must_have_a_non_zero_length );

    private:

        /** Pool of list items */
        FixedLengthListItem<T>  m_items[ queueMax ];


        /** Pointer to the start of the queue of free list slots.  Will be NULL
            in the case that there none are available */
        FixedLengthListItem<T>* m_freeHead;

        /** Pointer to the first item in the list of utilised item slots.  Will
            be NULL in the case that there are none */
        FixedLengthListItem<T>* m_usedHead;

        /** Pointer to the last item in the list of utilised item slots.  Will
            be NULL in the case that there are none */
        FixedLengthListItem<T>* m_usedTail;

        /** Keep count of the number of used items on the list.  Ranges between
            0 and queueMax */
        size_t                  m_usedCount;

        /** Remove the specified item from the list.

            \p_item Item to be removed.  Note that item must exist in the list
                    of used items
        */
        void remove_node( FixedLengthListItem<T>* p_item );

    public:
        /** Constructor for FixedLengthList */
        FixedLengthList( void );

        /** Initialising constructor for FixedLengthList.  Parameters will be
            used to initialise the list

            \param p_items An array of items used to initialise the list.  They
                           will be added in the order in which they appear in
                           p_items
            \param p_count The number of items in p_items.  Only up to queueMax
                           items will be used - any additional items will be
                           ignored */
        FixedLengthList( const T* const p_items, size_t p_count );

        /**
           push an item onto the front of the list

           \param p_item The item to be added to the list
           \returns true in the case that the item was added
                    false in the case that the item was not added (no space) */
        bool push( const T p_item );

        /**
           pop an item from the front of the list (item is removed and returned

           \param p_item Pointer to be populated with the value of the item
           \returns true in the case that an item was returned
                    false in the case that an item was not returned (list empty)
        */
        bool pop( T* const p_item );

        /**
           queue an item onto the end of the list

           \param p_item The item to be added to the list
           \returns true in the case that the item was added
                    false in the case that the item was not added (no space) */
        bool queue( const T p_item );

        /**
           dequeue an item from the end of the list (item is removed and
           returned

           \param p_item Pointer to be populated with the value of the item
           \returns true in the case that an item was returned
                    false in the case that an item was not returned (list empty)
        */
        bool dequeue( T* const p_item );

        bool remove( const T p_item );

        /** Used to find out how many items are in the list

            \returns Number of used items, ranging from 0 to queueMax */
        size_t used() const;

        /** Used to find out how many slots are still available in the list

            \returns Number of available slots, ranging from 0 to queueMax */
        size_t available() const;

        /** Determine whether or not a particular item is in the list

            \param p_val Item to be matched against
            \returns true in the case that the item is found in the list
                     false in the case that it is not found in the list
        */
        bool inList( const T p_val ) const;

        /** Remove the entire contents of the list and return it back to
            an empty state */
        void clear( void );

        typedef FixedLengthListIter<T, queueMax> iterator;
        typedef T value_type;
        typedef T * pointer;
        typedef T & reference;

        iterator begin( void );
        iterator end( void );
};


template < class T, size_t queueMax > 
FixedLengthList< T, queueMax >::FixedLengthList( void )
{
    clear();
}
 
template < class T, size_t queueMax > 
FixedLengthList< T, queueMax >::FixedLengthList( const T* const p_items, size_t p_count ) 
{

    const T* src = p_items;

    /* Can only populate up to queueMax items */
    size_t init_count = std::min( queueMax, p_count );
    FixedLengthListItem<T>* prev = NULL;

    m_usedHead = NULL;
    m_usedTail = NULL;

    /* Initialise the list from p_items, building the forward links */
    for( size_t i = 0;
         i < init_count;
         i++ )
    {
        FixedLengthListItem<T>* current = &(m_items[i]);

        current->m_forward = NULL;
        current->m_item = *(src++);

        /* If there was a previous item in the list, set up its forward pointer,
           otherwise set up the list head */
        if( prev != NULL ) {
            prev->m_forward = current;
        } else {
            m_usedHead = current;
        }

        m_usedTail = current;

        prev = current;
    }

    m_usedCount = init_count;

    m_freeHead = NULL;

    /* Any remaining items get moved into the free stack */
    
    prev = NULL;
    for( size_t i = init_count;
         i < queueMax;
         i++ )
    {
        FixedLengthListItem<T>* current = &(m_items[i]);
        current->m_forward = NULL;
        if( prev != NULL ) {
            prev->m_forward = current;
        } else {
            m_freeHead = current;
        }
        prev = current;
    }
}

template < class T, size_t queueMax > 
void FixedLengthList< T, queueMax >::clear( void )
{
    FixedLengthListItem<T>* p;
    size_t i;

    m_usedHead = NULL;
    m_usedTail = NULL;
    m_freeHead = m_items;
    
    /* Move all items into the free stack, setting up the forward links */
    for( p = m_items, i = (queueMax-1);
         i > 0 ;
         i-- )
    {
        FixedLengthListItem<T>* next = p+1;
        p->m_forward = next;
        p = next;
    }
    p->m_forward = NULL;
    m_usedCount = 0U;
}

template < class T, size_t queueMax > 
bool FixedLengthList< T, queueMax >::push( const T p_item )
{
    bool ret_val = false;
    
    /* Check that there's space in the list */
    if( m_freeHead != NULL )
    {
        FixedLengthListItem<T>* new_item = m_freeHead;

        /* Move the head pointer to the next free item in the list */
        m_freeHead = new_item->m_forward;

        new_item->m_forward = m_usedHead;

        new_item->m_item = p_item;

        m_usedHead = new_item;

        if( m_usedTail == NULL )
        {
            m_usedTail = new_item;
        }

        m_usedCount++;

        /* Indicate success */
        ret_val = true;
    }

    return ret_val;
}

template < class T, size_t queueMax > 
bool FixedLengthList< T, queueMax >::queue( const T p_item )
{
    bool ret_val = false;
    
    /* Check that there's space in the list */
    if( m_freeHead != NULL )
    {
        /* Grab a free item */
        FixedLengthListItem<T>* new_item = m_freeHead;

        /* Move the head pointer to the next free item in the list */
        m_freeHead = m_freeHead->m_forward;

        /* Item is going at end of list - no forward link */
        new_item->m_forward = NULL;

        new_item->m_item = p_item;

        /* Update the current tail item, if exists */
        if( m_usedTail != NULL )
        {
            m_usedTail->m_forward = new_item;
        }

        m_usedTail = new_item;

        if( m_usedHead == NULL )
        {
            m_usedHead = new_item;
        }

        m_usedCount++;

        /* Indicate success */
        ret_val = true;
    }

    return ret_val;
}

template < class T, size_t queueMax > 
bool FixedLengthList< T, queueMax >::pop( T* const p_item )
{
    bool ret_val = false;
    
    if( m_usedHead != NULL )
    {
        FixedLengthListItem<T>* old_item = m_usedHead;

        *p_item = old_item->m_item;

        m_usedHead = old_item->m_forward;
        
        if( m_usedTail == old_item )
        {
            m_usedTail = NULL;
        }

        old_item->m_forward = m_freeHead;

        m_freeHead = old_item;

        m_usedCount--;

        /* Indicate success */
        ret_val = true;
    }

    return ret_val;
}

template < class T, size_t queueMax >
bool FixedLengthList< T, queueMax >::dequeue( T* const p_item )
{
    bool ret_val = false;

    if( m_usedTail != NULL )
    {
        FixedLengthListItem<T>* old_item = m_usedTail;

        *p_item = old_item->m_item;

        remove_node( old_item );

        /* Indicate success */
        ret_val = true;
    }

    return ret_val;
}
        

template < class T, size_t queueMax >
void FixedLengthList< T, queueMax >::remove_node( FixedLengthListItem<T>* p_item )
{
    /* Item removed was only item in the list? */
    if( m_usedHead == p_item )
    {
        m_usedHead = NULL;
        m_usedTail = NULL;
    } else {
        FixedLengthListItem<T>* p = m_usedHead;

        /* Need to update the forward pointer of the
           item preceding the one which we've just removed.
           That item also becomes the new tail

           Iterate the list and find it */
        while( p != NULL )
        {
            if( p->m_forward == p_item )
            {
                p->m_forward = NULL;
                m_usedTail = p;
                break;
            }
            else
            {
                p = p->m_forward;
            }
        }
    }

    /* Move item to free list */
    p_item->m_forward = m_freeHead;
    m_freeHead = p_item;

    m_usedCount--;
}

template < class T, size_t queueMax >
bool FixedLengthList< T, queueMax >::remove( const T p_item )
{
    bool ret_val = false;
    FixedLengthListItem<T>* last = NULL;
    FixedLengthListItem<T>* p = m_usedHead;

    /* Run through all the items in the used list */
    while( p != NULL )
    {
        /* Does the item match the one we're looking for? */
        if( p->m_item == p_item )
        {
            /* If there was no last item then this must be the head, so update
               the head pointer, otherwise update the forward pointer on the
               prevceding item in the list */
            if( last == NULL )
            {
                m_usedHead = p->m_forward;
            }
            else
            {
                last->m_forward = p->m_forward;
            }

            /* Is this item the tail?  If so, update! */
            if( m_usedTail == p )
            {
                m_usedTail = last;
            }
 
            /* Move item to free list */
            p->m_forward = m_freeHead;
            m_freeHead = p;

            m_usedCount--;

            ret_val = true;
            break;
        }
        else
        {
            last = p;
            p = p->m_forward;
        }
    }

    return ret_val;
}

template < class T, size_t queueMax > 
size_t FixedLengthList< T, queueMax >::used() const
{
    return m_usedCount;
}

template < class T, size_t queueMax > 
size_t FixedLengthList< T, queueMax >::available() const
{
    return queueMax - m_usedCount;
}
        
template < class T, size_t queueMax > 
bool FixedLengthList< T, queueMax >::inList( const T p_val ) const
{
    bool ret_val = false;
    FixedLengthListItem<T>* p = m_usedHead;

    /* Ordered iteration of the list checking for specified item */
    while( p != NULL )
    {
        if( p->m_item == p_val ) {
            /* Item found - flag and break out */
            ret_val = true;
            break;
        } else {
            p = p->m_forward;
        }
    }

    return ret_val;
}

template < class T, size_t queueMax >
FixedLengthListIter<T, queueMax> FixedLengthList< T,queueMax >::begin( void )
{
    return iterator( m_usedHead );
}

template < class T, size_t queueMax >
FixedLengthListIter<T, queueMax> FixedLengthList< T,queueMax >::end( void )
{
    return iterator( NULL );
}

template < class T, size_t queueMax >
FixedLengthListIter< T, queueMax >::FixedLengthListIter( void ) : m_item( NULL)
{
}

template < class T, size_t queueMax >
FixedLengthListIter< T, queueMax >::FixedLengthListIter( FixedLengthListItem<T>* p_item ) : m_item( p_item )
{
} 

template < class T, size_t queueMax > 
T& FixedLengthListIter< T, queueMax >::operator*()
{
    return m_item->m_item;
} 

template < class T, size_t queueMax > 
FixedLengthListIter< T,queueMax > FixedLengthListIter< T,queueMax >::operator++( int p_int )
{
    FixedLengthListIter< T,queueMax > clone( *this );
    m_item = m_item->m_forward;
    return clone;
} 

template < class T, size_t queueMax > 
FixedLengthListIter< T,queueMax >& FixedLengthListIter< T,queueMax >::operator++( void )
{
    m_item = m_item->m_forward;
    return *this;
} 
        
template < class T, size_t queueMax > 
FixedLengthListIter< T,queueMax >& FixedLengthListIter< T,queueMax >::operator+=( const unsigned p_inc ) {
    for(unsigned i = 0;
        i < p_inc;
        i++ )
    {
        if( m_item == NULL ) {
            break;
        } else {
            m_item = m_item->m_forward;
        }
    }
    return *this;
}

template < class T, size_t queueMax > 
bool FixedLengthListIter< T,queueMax >::operator==( const FixedLengthListIter& p_comp ) const
{
    return m_item == p_comp.m_item;
}

template < class T, size_t queueMax > 
bool FixedLengthListIter< T,queueMax >::operator!=( const FixedLengthListIter& p_comp ) const
{
    return m_item != p_comp.m_item;
}


#endif

