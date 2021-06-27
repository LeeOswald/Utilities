#pragma once

#include <cassert>
#include <iterator>
#include <utility>

namespace Util
{

template <typename _Ty>
class IntrusiveList
{
public:
    using value_type = _Ty;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using pointer = _Ty *;
    using const_pointer = _Ty const *;
    using reference = _Ty &;
    using const_reference = _Ty const &;

    struct Node
    {
        virtual ~Node() = default;
        Node() = default;

        explicit Node(Node* prev, Node* next) noexcept
            : prev(prev)
            , next(next)
        {}

        Node(const Node&) = delete;
        Node& operator=(const Node&) = delete;

        void swap(Node& o) noexcept
        {
            using std::swap;
            swap(prev, o.prev);
            swap(next, o.next);
        }

        Node(Node&& o) noexcept
            : Node()
        {
            o.swap(*this);
        }

        Node& operator=(Node&& o) noexcept
        {
            if (this != &o)
            {
                Node(std::move(o)).swap(*this);
            }

            return *this;
        }

        void unlink() noexcept
        {
            if (next)
                next->prev = prev;

            if (prev)
                prev->next = next;

            prev = nullptr;
            next = nullptr;
        }

        _Ty* prev = nullptr;
        _Ty* next = nullptr;
    };

    struct const_iterator
    {
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = value_type;
        using difference_type = difference_type;
        using pointer = const_pointer;
        using reference = const_reference;

        explicit const_iterator(_Ty* m) noexcept
            : m(m)
        {}

        const_reference operator*() const noexcept
        {
            return *m;
        }

        const_pointer operator->() const noexcept
        {
            return m;
        }

        const_iterator &operator++() noexcept
        {
            if (!m)
                return const_iterator(nullptr);
            m = m->next;
            return *this;
        }

    private:
        _Ty* m;
    };

	~IntrusiveList() noexcept
	{
		clear();
	}

	IntrusiveList() = default;

	IntrusiveList(const IntrusiveList&) = delete;
	IntrusiveList& operator=(const IntrusiveList&) = delete;

	void swap(IntrusiveList& o) noexcept
	{
		using std::swap;
		swap(first_, o.first_);
		swap(last_, o.last_);
		swap(count_, o.count_);
	}

	IntrusiveList(IntrusiveList&& o) noexcept
		: IntrusiveList()
	{
		o.swap(*this);
	}

	IntrusiveList& operator=(IntrusiveList&& o) noexcept
	{
		if (this != &o)
			IntrusiveList(std::move(o)).swap(*this);

		return *this;
	}

	bool empty() const noexcept
	{
		return (count_ == 0);
	}

	size_t size() const noexcept
	{
		return count_;
	}
		
	void push_back(_Ty* item) noexcept
	{
		assert(item);

		if (!last_) // empty list
		{
			assert(!first_);
			first_ = item;
			last_ = item;
		}
		else // non-empty list
		{
			auto prev = last_;
			last_ = item;
			item->prev = prev;
			item->next = nullptr;
			prev->next = item;
		}

		++count_;
	}

	void push_front(_Ty* item) noexcept
	{
		assert(item);

		if (!first_) // empty list
		{
			assert(!last_);
			first_ = item;
			last_ = item;
		}
		else // non-empty list
		{
			auto next = first_;
			first_ = item;
			item->prev = nullptr;
			item->next = next;
			next->prev = item;
		}

		++count_;
	}

	void erase(_Ty* item) noexcept
	{
		assert(item);

		if (empty())
			return;

		if (item->next)
		{
			item->next->prev = item->prev;
			item->next->next = item->next->next;
		}
		else
		{
			last_ = item->prev;
		}

		if (item->prev)
		{
			item->prev->next = item->next;
			item->prev->prev = item->prev->prev;
		}
		else
		{
			first_ = item->next;
		}

		item->unlink();
        delete item;

		--count_;
		if (!count_)
		{
			first_ = nullptr;
			last_ = nullptr;
		}
	}

	void clear() noexcept
	{
		auto p = first_;
		first_ = nullptr;
		last_ = nullptr;
		while (p)
		{
			auto next = p->next;
			p->unlink();
            delete p;
			--count_;
			p = next;
		}
	}
    	
protected:
	_Ty* first_ = nullptr;
	_Ty* last_ = nullptr;
	size_t count_ = 0;
};


} // namespace Util {}
