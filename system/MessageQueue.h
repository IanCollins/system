/******************************* C++ Header File *******************************
 *
 *  Copyright (c) Masuma Ltd 2003-2016
 *
 *  MODULE:      utils
 *
 *  DESCRIPTION: between thread queue
 *
 ******************************************************************************/

#ifndef _utils_MessageQueue_h_
#define _utils_MessageQueue_h_

#include <deque>
#include <mutex>
#include <condition_variable>

namespace masuma
{
  namespace utils
  {
    template <typename Message, typename Allocator=std::allocator<Message>>
    class MessageQueue
    {
      std::mutex              mutex;
      std::condition_variable condition;

      using Queue = std::deque<Message,Allocator>;

      Queue queue;

      struct CheckEmpty
      {
        Queue& queue;

        CheckEmpty( Queue& queue ) : queue {queue} {}

        bool operator()() { return !queue.empty(); }
      };

    public:

      MessageQueue() = default;

      MessageQueue( const MessageQueue& ) = delete;
      MessageQueue( MessageQueue&& ) = delete;
      MessageQueue& operator=( const MessageQueue& ) = delete;
      MessageQueue& operator=( MessageQueue&& ) = delete;

      bool empty() const { return queue.empty(); }

      void post( const Message& t )
      {
        std::lock_guard<std::mutex> lock {mutex};

        queue.push_back( t );

        condition.notify_one();
      }

      void post( Message&& t )
      {
        std::lock_guard<std::mutex> lock {mutex};

        queue.push_back( t );

        condition.notify_one();
      }

      void pend( Message& t )
      {
        std::unique_lock<std::mutex> lock {mutex};

//        condition.wait( lock,[this]{ return !queue.empty();} );
        condition.wait( lock, CheckEmpty {queue} );

        t = queue.front();
        queue.pop_front();
      }

//      T pend( uint32_t ms )
//      {
//      }
    };

    template <typename Message>
    class MessageAllocator
    {
      union node
      {
        node* next;
        typename std::aligned_storage<sizeof(Message), alignof(Message)>::type storage;
      };

      node* list = nullptr;

      void clear() noexcept
      {
        auto p = list;
        while (p)
        {
          auto tmp = p;
          p = p->next;
          delete tmp;
        }

        list = nullptr;
      }

    public:

      using value_type = Message;
      using pointer    = Message*;
      using size_type  = std::size_t;

      using propagate_on_container_move_assignment = std::true_type;


      MessageAllocator() = default;
      ~MessageAllocator() = default;
      MessageAllocator( const MessageAllocator& ) noexcept {};

      template <typename M>
      MessageAllocator( const MessageAllocator<M>& ) noexcept {}

      MessageAllocator( MessageAllocator&& other) noexcept
        : list {other.list}
      {
        other.list = nullptr;
      }

      MessageAllocator& operator=( MessageAllocator&& other ) noexcept
      {
        clear();
        list = other.list;
        other.list = nullptr;
        return *this;
      }

      pointer allocate( size_type n )
      {
        if( n == 32 )
        {
          auto ptr = list;

          if( ptr )
          {
            list = list->next;
          }
          else
          {
            ptr = new node[32];
          }

          return reinterpret_cast<Message*>(ptr);
        }

        return static_cast<Message*>(::operator new(n * sizeof(Message)));
      }

      void deallocate( pointer ptr, size_type n )
      {
        if( n == 32 )
        {
          auto nodeP = reinterpret_cast<node*>(ptr);

          nodeP->next = list;

          list = nodeP;
        }
        else
        {

          ::operator delete(ptr);
        }
      }
    };

    template <typename T, typename U>
    inline bool operator == (const MessageAllocator<T>&, const MessageAllocator<U>&)
    {
        return true;
    }

    template <typename T, typename U>
    inline bool operator != (const MessageAllocator<T>&, const MessageAllocator<U>&)
    {
        return false;
    }

    template <typename Message>
    class StaticMessageQueue : public MessageQueue<Message, MessageAllocator<Message>>
    {

    };
  }
}

#endif
