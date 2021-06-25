#pragma once

#include <cassert>
#include <utility>

namespace Util
{

enum class Search
{
	stop = 0,
	go = 1
};

#pragma region NodeBase

struct NodeBase
{
	virtual ~NodeBase() = default;
	NodeBase() = default;

	explicit NodeBase(NodeBase* prev, NodeBase* next) noexcept
		: prev(prev)
		, next(next)
	{}

	NodeBase(const NodeBase&) = delete;
	NodeBase& operator=(const NodeBase&) = delete;

	void swap(NodeBase& o) noexcept
	{
		using std::swap;
		swap(prev, o.prev);
		swap(next, o.next);
	}

	NodeBase(NodeBase&& o) noexcept
		: NodeBase()
	{
		o.swap(*this);
	}

	NodeBase& operator=(NodeBase&& o) noexcept
	{
		if (this != &o)
		{
			NodeBase(std::move(o)).swap(*this);
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

	NodeBase* prev = nullptr;
	NodeBase* next = nullptr;
};

#pragma endregion // NodeBase

#pragma region IntrusiveList

struct IntrusiveList
{
public:
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

	NodeBase* first() const noexcept
	{
		return first_;
	}

	NodeBase* last() const noexcept
	{
		return last_;
	}

	NodeBase* next(NodeBase* item) const noexcept
	{
		if (item && item->next)
			return item->next;

		return nullptr;
	}

	NodeBase* prev(NodeBase* item) const noexcept
	{
		if (item && item->prev)
			return item->prev;

		return nullptr;
	}

	bool empty() const noexcept
	{
		return (count_ == 0);
	}

	size_t count() const noexcept
	{
		return count_;
	}
		
	template <typename THandler>
	void for_each(THandler handler) const noexcept
	{
		auto p = first_;
		while (p)
		{
			auto p0 = p->next();
			if (handler(p) == Search::stop)
				break;

			p = p0;
		}
	}

	template <typename THandler>
	NodeBase* find(THandler handler) const noexcept
	{
		auto p = first_;
		while (p)
		{
			if (handler(static_cast<typename THandler::T*>(p)) == Search::stop)
				return p;

			p = p->next();
		}

		return nullptr;
	}

	NodeBase* push_back(NodeBase* item) noexcept
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
		return last_;
	}

	NodeBase* push_front(NodeBase* item) noexcept
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
		return first_;
	}

	void remove(NodeBase* item) noexcept
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
			--count_;
			p = next;
		}
	}

	
protected:
	NodeBase* first_ = nullptr;
	NodeBase* last_ = nullptr;
	size_t count_ = 0;
};


inline void swap(IntrusiveList& a, IntrusiveList& b)
{
	a.swap(b);
}

#pragma endregion // IntrusiveList

} // namespace Util {}
