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

        explicit const_iterator(pointer m) noexcept
            : m(m)
        {}

        reference operator*() const noexcept
        {
            return *m;
        }

        pointer operator->() const noexcept
        {
            return m;
        }

        const_iterator &operator++() noexcept
        {
            if (m)
            {
                m = m->next;
            }

            return *this;
        }

        const_iterator &operator++(int) noexcept
        {
            auto Tmp = *this;
            ++*this;
            return Tmp;
        }

        bool operator==(const const_iterator &o) const noexcept
        {
            return (m == o.m);
        }

        bool operator!=(const const_iterator &o) const noexcept
        {
            return (m != o.m);
        }

        pointer m;
    };

    struct iterator
    {
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = value_type;
        using difference_type = difference_type;
        using pointer = pointer;
        using reference = reference;

        explicit iterator(pointer m) noexcept
            : m(m)
        {}

        reference operator*() const noexcept
        {
            return *m;
        }

        pointer operator->() const noexcept
        {
            return m;
        }

        iterator &operator++() noexcept
        {
            if (m)
            {
                m = m->next;
            }

            return *this;
        }

        iterator &operator++(int) noexcept
        {
            auto Tmp = *this;
            ++*this;
            return Tmp;
        }

        bool operator==(const iterator &o) const noexcept
        {
            return (m == o.m);
        }

        bool operator!=(const iterator &o) const noexcept
        {
            return (m != o.m);
        }

        bool operator==(const const_iterator &o) const noexcept
        {
            return (m == o.m);
        }

        bool operator!=(const const_iterator &o) const noexcept
        {
            return (m != o.m);
        }

        pointer m;
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

    const_iterator begin() const noexcept
    {
        return const_iterator(first_);
    }

    iterator begin() noexcept
    {
        return iterator(first_);
    }

    const_iterator end() const noexcept
    {
        return const_iterator(nullptr);
    }

    iterator end() noexcept
    {
        return iterator(nullptr);
    }

    const_reference front() const
    {
        assert(!empty());
        if (empty())
            throw std::exception("Calling front() on an empty list");
        return *first_;
    }

    reference front()
    {
        assert(!empty());
        if (empty())
            throw std::exception("Calling front() on an empty list");
        return *first_;
    }

    const_reference back() const
    {
        assert(!empty());
        if (empty())
            throw std::exception("Calling back() on an empty list");
        return *last_;
    }

    reference back()
    {
        assert(!empty());
        if (empty())
            throw std::exception("Calling back() on an empty list");
        return *last_;
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

    void erase(iterator i) noexcept
    {
        if (i.m)
            erase(i.m);
    }

    void pop_front()
    {
        assert(!empty());
        if (empty())
            throw std::exception("Calling pop_front() on an empty list");
        erase(first_);
    }

    void pop_back()
    {
        assert(!empty());
        if (empty())
            throw std::exception("Calling pop_back() on an empty list");
        erase(last_);
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


template <typename _Ty>
inline bool operator==(const typename Util::IntrusiveList<_Ty>::const_iterator& a, const typename Util::IntrusiveList<_Ty>::const_iterator& b) noexcept
{
    return a.operator==(b);
}

template <typename _Ty>
inline bool operator==(const typename Util::IntrusiveList<_Ty>::iterator& a, const typename Util::IntrusiveList<_Ty>::iterator& b) noexcept
{
    return a.operator==(b);
}

template <typename _Ty>
inline bool operator!=(const typename Util::IntrusiveList<_Ty>::const_iterator& a, const typename Util::IntrusiveList<_Ty>::const_iterator& b) noexcept
{
    return a.operator!=(b);
}

template <typename _Ty>
inline bool operator!=(const typename Util::IntrusiveList<_Ty>::iterator& a, const typename Util::IntrusiveList<_Ty>::iterator& b) noexcept
{
    return a.operator!=(b);
}
