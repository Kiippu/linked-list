//  Example of Custom doublely Linked List
#include <cstdio>
#include <memory>
#include <functional>
#include <cassert>

template <class T>
class Node
{
public:
    Node(T val){m_value = val;}
    T m_value;
    Node<T>* m_previous = nullptr;
    std::unique_ptr<Node<T>> m_next = nullptr;
};
using NodeInt = Node<int>;

template <class T>
class LinkedList
{
public:

    // if shuffleOptimize is true, link order will be changed
    const Node<T>* Find(std::function<bool(T& val)> func, bool shuffleOptimize = false)
    {
        Node<T> * nodePtr = m_root_first.get();
        while(nodePtr)
        {
            if(func(nodePtr->m_value))
            {
                if(shuffleOptimize)
                {
                    // prune node
                    auto node = std::move(nodePtr->m_previous->m_next);
                    // connect previous and next node together
                    node->m_previous->m_next = std::move(node->m_next);
                    // connect root to nodes next
                    node->m_next = std::move(m_root_first);
                    // move node to address of link class root
                    m_root_first = std::move(node);
                }
                return nodePtr;
            }
            else
                nodePtr = nodePtr->m_next.get();
        }
        return nullptr;
    }

    void Display(const bool reverse = false)
    {
        printf("-Start Display Linked List-\n");
        Node<T> * nodePtr = reverse ? m_root_last : m_root_first.get();
        while(nodePtr)
        {
            if(nodePtr)
            {
                printf("node: %d\n", nodePtr->m_value);
                nodePtr = reverse ? nodePtr->m_previous : nodePtr->m_next.get();
            }
            else
            {
                break;
            }
        }
        printf("-End Display Linked List-\n");
    }
    
    Node<T> * Push_back(const T val)
    {
        if(!m_root_first.get())
            InitRoot(val);
        else
        {
            m_root_last->m_next = std::make_unique<Node<T>>(val);
            //if(m_root_last->m_previous)
            m_root_last->m_next->m_previous = m_root_last;
            m_root_last = m_root_last->m_next.get();
        }
        return m_root_last;
    }
    
    Node<T> * Push_front(const T val)
    {
        if(!m_root_first.get())
            InitRoot(val);
        else
        {
            auto newNode = std::make_unique<Node<T>>(val);
            newNode->m_next = std::move(m_root_first);
            m_root_first = std::move(newNode);
            m_root_first->m_next->m_previous = m_root_first.get();
        }
        return m_root_first.get();
    }

    Node<T> * Insert(const size_t index, const T val)
    {
        if(index >= Size())
            return nullptr;

        if(!m_root_first.get() && index == 0)
            return InitRoot(val);

        Node<T>* nodePtr = m_root_first.get();
        size_t indexCount = 0;
        while(nodePtr)
        {
            if(indexCount == index)
            {
                auto newNode = std::make_unique<Node<T>>(val);
                // make result pointer newNodes
                if(nodePtr->m_previous)
                    newNode->m_previous = nodePtr->m_previous;
                // assign newNode next as result unique ptr
                newNode->m_next = std::move(nodePtr->m_previous->m_next);
                // assign newNode to previous node
                newNode->m_previous->next = std::move(newNode);
                return nodePtr->m_previous;
            }
            else
            {
                indexCount++;
                nodePtr = nodePtr->m_next.get();
                // last node
                if(indexCount == index && !nodePtr)
                {
                    nodePtr.m_next = std::make_unique<Node<T>>(val);
                    nodePtr.m_next.m_previous = nodePtr;
                    break;
                }
                // out of scope
                else
                    break;
            }
        }
        return nullptr;
    }

    void Remove(const size_t index)
    {
        Node<T>* nodePtr = m_root_first.get();
        size_t indexCount = 0;
        while(nodePtr)
        {
            if(indexCount >= index)
            {
                // remove only node in link
                if(!nodePtr->m_previous && !nodePtr->m_next && m_root_first)
                {
                    m_root_first.reset();
                }
                // rmove first link
                else if(!nodePtr->m_previous && nodePtr->m_next)
                {
                    m_root_first = std::move(m_root_first->m_next);
                }
                // remove last link
                else if(nodePtr->m_previous && !nodePtr->m_next)
                {
                    m_root_last = nodePtr->m_previous;
                    nodePtr->m_previous->m_next.reset();
                }
                // remoave all else
                else
                {
                    auto nextPtr = std::move(nodePtr->m_next);
                    auto previousPtr = nodePtr->m_previous;
                    nodePtr->m_previous->m_next.reset();
                    nextPtr->m_previous = previousPtr;
                    previousPtr->m_next = std::move(nextPtr);
                }
                return;
            }
            else
            {
                indexCount++;
                nodePtr = nodePtr->m_next.get();
            }
        }
    }

    void Clear()
    {
        m_root_first.release();
    }

    // get size/length of linked list
    const size_t Size()
    {
        size_t count = 0;
        Node<T>* nodePtr = m_root_first.get();
        while(nodePtr)
        {
            count++;
            nodePtr = nodePtr->m_next.get();
        }
        return std::move(count);
    }

    private:

    Node<T>* InitRoot(const T val)
    {
        m_root_first = std::make_unique<Node<T>>(val);
        m_root_last = m_root_first.get();
        return m_root_last;
    };
    
    std::unique_ptr<Node<T>> m_root_first = nullptr;
    Node<T> * m_root_last = nullptr;

};

int main()
{
    // init
    LinkedList<int> list;
    // add one value
    list.Push_back(10);
    assert(("list is empty", list.Size()));
    // remove first index
    list.Remove(0);
    assert(("list is empty", list.Size() == 0));
    // add multiple value then remove a middle value
    list.Push_back(11);
    list.Push_back(12);
    list.Push_back(13);
    list.Push_back(14);
    list.Push_back(15);
    list.Remove(2);
    assert(("list did removed 2nd element", list.Size() == 4));
    // add to front
    list.Push_front(-1);
    list.Push_front(-90);
    assert(("list is size 6", list.Size() == 6));
    // find valid value
    auto val_valid = list.Find([](int& val){ return val == 14; });
    assert(("list is size 6", val_valid->m_value == 14));
    // handle find invalid value
    auto val_invalid = list.Find([](int& val){ return val == 34; });
    assert(("list is size 6", !val_invalid));
    // remove mid node
    list.Remove(4);
    assert(("list is size 5", list.Size() == 5));
    // remove out of range
    list.Remove(6);
    assert(("list did not change", list.Size() == 5));
    // clean whole list
    list.Clear();
    assert(("list is empty", list.Size() == 0));


    
    return 0;
}