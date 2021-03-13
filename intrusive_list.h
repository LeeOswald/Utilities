#pragma once

#ifndef ___MOROSE_INTRUSIVE_LIST_H_INCLUDED_H__
#define ___MOROSE_INTRUSIVE_LIST_H_INCLUDED_H__

#include <cassert>
#include <utility>

namespace perf
{

enum class search
{
	stop = 0,
	go = 1
};

#pragma region node_base

struct node_base
{
public:
	virtual ~node_base() = default;
	node_base() = default;

	explicit node_base(node_base* prev, node_base* next) noexcept
		: prev_(prev)
		, next_(next)
	{}

	node_base(const node_base&) = delete;
	node_base& operator=(const node_base&) = delete;

	void swap(node_base& o) noexcept
	{
		using std::swap;
		swap(prev_, o.prev_);
		swap(next_, o.next_);
	}

	node_base(node_base&& o) noexcept
		: node_base()
	{
		o.swap(*this);
	}

	node_base& operator=(node_base&& o) noexcept
	{
		if (this != &o)
		{
			node_base(std::move(o)).swap(*this);
		}

		return *this;
	}

	node_base* next() const noexcept
	{
		return next_;
	}

	node_base* prev() const noexcept
	{
		return prev_;
	}

	bool is_first() const noexcept
	{
		return !prev_;
	}

	bool is_lastt() const noexcept
	{
		return !next_;
	}

	node_base* set_next(node_base* o) noexcept
	{
		auto n = next_;
		next_ = o;
		return n;
	}

	node_base* set_prev(node_base* o) noexcept
	{
		auto p = prev_;
		prev_ = o;
		return p;
	}

	void unlink() noexcept
	{
		if (next_)
			next_->set_prev(prev_);

		if (prev_)
			prev_->set_next(next_);

		prev_ = nullptr;
		next_ = nullptr;
	}

private:
	node_base* prev_ = nullptr;
	node_base* next_ = nullptr;
};

inline void swap(node_base& a, node_base& b) noexcept
{
	a.swap(b);
}

#pragma endregion // node_base

#pragma region intrusive_list

struct intrusive_list
{
public:
	~intrusive_list() noexcept
	{
		clear();
	}

	intrusive_list() = default;

	intrusive_list(const intrusive_list&) = delete;
	intrusive_list& operator=(const intrusive_list&) = delete;

	void swap(intrusive_list& o) noexcept
	{
		using std::swap;
		swap(first_, o.first_);
		swap(last_, o.last_);
		swap(count_, o.count_);
	}

	intrusive_list(intrusive_list&& o) noexcept
		: intrusive_list()
	{
		o.swap(*this);
	}

	intrusive_list& operator=(intrusive_list&& o) noexcept
	{
		if (this != &o)
			intrusive_list(std::move(o)).swap(*this);

		return *this;
	}

	node_base* first() const noexcept
	{
		return first_;
	}

	node_base* last() const noexcept
	{
		return last_;
	}

	node_base* next(node_base* item) const noexcept
	{
		if (item && item->next())
			return item->next();

		return nullptr;
	}

	node_base* prev(node_base* item) const noexcept
	{
		if (item && item->prev())
			return item->prev();

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
			if (handler(p) == search::stop)
				break;

			p = p0;
		}
	}

	template <typename THandler>
	node_base* find(THandler handler) const noexcept
	{
		auto p = first_;
		while (p)
		{
			if (handler(static_cast<typename THandler::T*>(p)) == search::stop)
				return p;

			p = p->next();
		}

		return nullptr;
	}

	node_base* push_back(node_base* item) noexcept
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
			item->set_prev(prev);
			item->set_next(nullptr);
			prev->set_next(item);
		}

		++count_;
		return last_;
	}

	node_base* push_front(node_base* item) noexcept
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
			item->set_prev(nullptr);
			item->set_next(next);
			next->set_prev(item);
		}

		++count_;
		return first_;
	}

	void remove(node_base* item) noexcept
	{
		assert(item);

		if (empty())
			return;

		if (item->next())
		{
			item->next()->set_prev(item->prev());
			item->next()->set_next(item->next()->next());
		}
		else
		{
			last_ = item->prev();
		}

		if (item->prev())
		{
			item->prev()->set_next(item->next());
			item->prev()->set_prev(item->prev()->prev());
		}
		else
		{
			first_ = item->next();
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
			auto next = p->next();
			p->unlink();
			--count_;
			p = next;
		}
	}

	
protected:
	node_base* first_ = nullptr;
	node_base* last_ = nullptr;
	size_t count_ = 0;
};


inline void swap(intrusive_list& a, intrusive_list& b)
{
	a.swap(b);
}

#pragma endregion // intrusive_list

} // namespace perf {}

#endif // ___MOROSE_INTRUSIVE_LIST_H_INCLUDED_H__