/**
   @file
   @brief Tests for the FixedLengthList class

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

#if defined __CC_ARM
#include "mbed.h"
Serial pc(USBTX, USBRX); // tx, rx   
#define PRINTF( ... ) pc.printf(__VA_ARGS__)
#else
#include <stdio.h>
#define PRINTF( ... ) printf(__VA_ARGS__)
#endif

#include "FixedLengthList.hpp"
   
#define LIST_LEN (20U)
#define LIST2_INI (17U)
#define CHECK( _x, ... ) do { PRINTF( __VA_ARGS__ ); if( _x ) { PRINTF(" OK\r\n"); } else { PRINTF(" FAILED!\r\n"); } } while( 0 )
   
/* Check that we can have multiple lists co-existing */

int init_list[ LIST2_INI ] = { 12, 23, 34, 45, 56, 67, 78, 89, 100, 111,
                               122, 133, 144, 155, 166, 177, 188, };

FixedLengthList<int,  LIST_LEN > list;
FixedLengthList<int,  LIST_LEN > list2( init_list, LIST2_INI );
#if 0
FixedLengthList<char, LIST_LEN > list3;
#endif

static void check_iterators( void );
   
int main() {
    int i = 0;
    PRINTF("FixedLengthList test\n");

    /* Test operations on an empty list */
    CHECK( list.used() == 0, "Initial used()" ); 
    CHECK( list.inList(1) == false, "inList() on empty list" ); 
    CHECK( list.available() == LIST_LEN, "Initial available()" ); 
    CHECK( list.pop(&i) == false, "pop() on empty list" ); 
    CHECK( list.dequeue(&i) == false, "dequeue() on empty list" ); 

    /* Push a little data into the list, few tests, return to empty list */
    CHECK( list.push( 1 ),   "Initial push()" );
    CHECK( list.inList(1) == true, "inList() for item which is in list" ); 
    CHECK( list.inList(2) == false, "inList() for item which is not in list" ); 
    CHECK( list.used() == 1, "used() after initial push()" ); 
    CHECK( list.pop( &i ),       "Check pop() returned OK" );
    CHECK( i == 1,       "pop() yielded correct value" );
    CHECK( list.used() == 0, "used() after fully depleting pop" ); 
    CHECK( list.pop(&i) == false, "pop() on empty list" ); 
    CHECK( list.dequeue(&i) == false, "dequeue() on empty list" ); 

    /* Push data onto an empty list and then dequeue & pop it */
    CHECK( list.push( 22 ),   "push() 22 (getting some data in)" );
    CHECK( list.push( 33 ),   "push() 33 (getting some data in)" );
    CHECK( list.push( 44 ),   "push() 33 (getting some data in)" );
    CHECK( list.dequeue(&i) == true, "dequeue() returned OK" ); 
    CHECK( i == 22,       "dequeue() yielded correct value" );
    CHECK( list.dequeue(&i) == true, "dequeue() returned OK" ); 
    CHECK( i == 33,       "dequeue() yielded correct value" );
    CHECK( list.dequeue(&i) == true, "dequeue() returned OK" ); 
    CHECK( i == 44,       "dequeue() yielded correct value" );
    CHECK( list.pop(&i) == false, "pop() on empty list" ); 
    CHECK( list.dequeue(&i) == false, "dequeue() on empty list" ); 
    CHECK( list.push( 55 ),   "push() 55 (getting some data in)" );
    CHECK( list.push( 66 ),   "push() 66 (getting some data in)" );
    CHECK( list.push( 77 ),   "push() 77 (getting some data in)" );
    CHECK( list.pop(&i) == true, "pop() returned OK" ); 
    CHECK( i == 77,       "pop() yielded correct value" );
    CHECK( list.dequeue(&i) == true, "dequeue() returned OK" ); 
    CHECK( i == 55,       "dequeue() yielded correct value" );
    CHECK( list.queue( 88 ),   "push() 88 (getting some data in)" );
    CHECK( list.pop(&i) == true, "pop() returned OK" ); 
    CHECK( i == 66,       "pop() yielded correct value" );
    CHECK( list.pop(&i) == true, "pop() returned OK" ); 
    CHECK( i == 88,       "pop() yielded correct value" );
    CHECK( list.available() == LIST_LEN, "available()" ); 
    CHECK( list.pop(&i) == false, "pop() on empty list" ); 
    CHECK( list.dequeue(&i) == false, "dequeue() on empty list" ); 

    /* Push data onto list until it's full & test operations on full list */
    CHECK( list.push( 100 ),   "push() 100 (getting some data in)" );
    CHECK( list.push( 101 ),   "push() 101 (getting some data in)" );
    CHECK( list.push( 102 ),   "push() 102 (getting some data in)" );
    CHECK( list.push( 103 ),   "push() 103 (getting some data in)" );
    CHECK( list.push( 104 ),   "push() 104 (getting some data in)" );
    CHECK( list.push( 105 ),   "push() 105 (getting some data in)" );
    CHECK( list.push( 106 ),   "push() 106 (getting some data in)" );
    CHECK( list.push( 107 ),   "push() 107 (getting some data in)" );
    CHECK( list.push( 108 ),   "push() 108 (getting some data in)" );
    CHECK( list.push( 109 ),   "push() 109 (getting some data in)" );
    CHECK( list.push( 110 ),   "push() 110 (getting some data in)" );
    CHECK( list.queue( 111 ),   "queue() 111 (getting some data in)" );
    CHECK( list.queue( 112 ),   "queue() 112 (getting some data in)" );
    CHECK( list.queue( 113 ),   "queue() 113 (getting some data in)" );
    CHECK( list.queue( 114 ),   "queue() 114 (getting some data in)" );
    CHECK( list.queue( 115 ),   "queue() 115 (getting some data in)" );
    CHECK( list.queue( 116 ),   "queue() 116 (getting some data in)" );
    CHECK( list.queue( 117 ),   "queue() 117 (getting some data in)" );
    CHECK( list.queue( 118 ),   "queue() 118 (getting some data in)" );
    CHECK( list.queue( 119 ),   "queue() 119 (getting some data in)" );
    CHECK( list.queue( 120 ) == false,   "queue() on a full list" );
    CHECK( list.push( 120 ) == false,   "push() on a full list" );
    CHECK( list.available() == 0, "available()" ); 
    CHECK( list.used() == LIST_LEN, "used() on full list" ); 

    /* Some tests on the list using the initialising constructor - focused 
       around checking that the m_usedHead, m_usedTail, etc have been set
       up correctly */
    CHECK( list2.available() == (LIST_LEN-LIST2_INI), "list 2 available()" ); 
    CHECK( list2.used() == LIST2_INI, "list 2 used()" ); 
    CHECK( list2.pop(&i) == true, "pop() returned OK" ); 
    CHECK( i == 12,       "pop() yielded correct value" );
    CHECK( list2.dequeue(&i) == true, "dequeue() returned OK" ); 
    CHECK( i == 188,       "dequeue() yielded correct value" );
    CHECK( list2.queue( 199 ),   "queue() 199 (getting some data in)" );
    CHECK( list2.queue( 210 ),   "queue() 210 (getting some data in)" );
    CHECK( list2.queue( 221 ),   "queue() 221 (getting some data in)" );
    CHECK( list2.queue( 232 ),   "queue() 232 (getting some data in)" );
    CHECK( list2.queue( 243 ),   "queue() 243 (getting some data in)" );
    CHECK( list2.queue( 254 ) == false,   "queue() on a full list" );
    
    check_iterators();

    PRINTF("FixedLengthList test - Done\n");

    return 0;
}

static void check_iterators( void )
{
    FixedLengthList<int,  LIST_LEN > ilist( init_list, LIST2_INI );
    FixedLengthList<int,  LIST_LEN >::iterator it;

    it = ilist.begin();
    CHECK( *it     == init_list[0],   "iterators: begin()" );
    CHECK( it      == ilist.begin(),  "iterators: operator==" );
    CHECK( it      != ilist.end(),    "iterators: operator!=" );
    CHECK( *(it++) == init_list[0],   "iterators: operator++ (post-increment)" );
    CHECK( *it     == init_list[1],   "iterators: operator++ (post-increment)" );
    CHECK( *(++it) == init_list[2],   "iterators: operator++ (pre-increment)" );
    CHECK( *it     == init_list[2],   "iterators: operator++ (pre-increment)" );
    it+=15;
    CHECK( it      != ilist.begin(),  "iterators: operator!=" );
    CHECK( it      == ilist.end(),    "iterators: operator==" );

}

