#pragma once

#include <exception>
#include <string>
#include <iostream>
#include <sstream>
#include <functional>


template<class _Ty>
constexpr _Ty copy(const _Ty& value) { return _Ty{ value }; }


template<class _Ty>
class basic_read_only_pointer;

template<class _Ty>
using ConstReadOnlyPointer = basic_read_only_pointer<const _Ty>;

template<class _Ty>
class ReadOnlyPointer;


template<class _Ty>
class basic_read_only_pointer
{
protected:
	_Ty* _ptr;

public:
	inline basic_read_only_pointer(_Ty* ptr) :
		_ptr{ ptr }
	{}
	inline basic_read_only_pointer(const basic_read_only_pointer<_Ty>& rop) :
		_ptr{ rop._ptr }
	{}

	inline basic_read_only_pointer& operator= (const basic_read_only_pointer<_Ty>& rop)
	{
		_ptr = rop._ptr;
		return *this;
	}

	inline operator bool() const { return _ptr; }
	inline bool operator! () const { return !_ptr; }

	inline const _Ty& operator* () const { return *_ptr; }

	inline const _Ty* operator& () const { return _ptr; }

	//_Ty* operator-> () { return _ptr; }
	inline const _Ty* operator-> () const { return _ptr; }

	template<class _Ty>
	friend bool operator== (const ConstReadOnlyPointer<_Ty>& ptr1, const ConstReadOnlyPointer<_Ty>& ptr2);

	template<class _Ty>
	friend bool operator!= (const ConstReadOnlyPointer<_Ty>& ptr1, const ConstReadOnlyPointer<_Ty>& ptr2);

	template<class _Ty>
	friend bool operator> (const ConstReadOnlyPointer<_Ty>& ptr1, const ConstReadOnlyPointer<_Ty>& ptr2);

	template<class _Ty>
	friend bool operator< (const ConstReadOnlyPointer<_Ty>& ptr1, const ConstReadOnlyPointer<_Ty>& ptr2);

	template<class _Ty>
	friend bool operator>= (const ConstReadOnlyPointer<_Ty>& ptr1, const ConstReadOnlyPointer<_Ty>& ptr2);

	template<class _Ty>
	friend bool operator<= (const ConstReadOnlyPointer<_Ty>& ptr1, const ConstReadOnlyPointer<_Ty>& ptr2);
};

template<class _Ty>
bool operator== (const ConstReadOnlyPointer<_Ty>& ptr1, const ConstReadOnlyPointer<_Ty>& ptr2)
{
	return ptr1._ptr == ptr2._ptr;
}

template<class _Ty>
bool operator!= (const ConstReadOnlyPointer<_Ty>& ptr1, const ConstReadOnlyPointer<_Ty>& ptr2)
{
	return ptr1._ptr != ptr2._ptr;
}

template<class _Ty>
bool operator> (const ConstReadOnlyPointer<_Ty>& ptr1, const ConstReadOnlyPointer<_Ty>& ptr2)
{
	return ptr1._ptr > ptr2._ptr;
}

template<class _Ty>
bool operator< (const ConstReadOnlyPointer<_Ty>& ptr1, const ConstReadOnlyPointer<_Ty>& ptr2)
{
	return ptr1._ptr < ptr2._ptr;
}

template<class _Ty>
bool operator>= (const ConstReadOnlyPointer<_Ty>& ptr1, const ConstReadOnlyPointer<_Ty>& ptr2)
{
	return ptr1._ptr >= ptr2._ptr;
}

template<class _Ty>
bool operator<= (const ConstReadOnlyPointer<_Ty>& ptr1, const ConstReadOnlyPointer<_Ty>& ptr2)
{
	return ptr1._ptr <= ptr2._ptr;
}




template<class _Ty>
class ReadOnlyPointer : public basic_read_only_pointer<_Ty>
{
public:
	inline ReadOnlyPointer(_Ty* ptr) :
		basic_read_only_pointer<_Ty>{ ptr }
	{}
	inline ReadOnlyPointer(const ReadOnlyPointer<_Ty>& rop) :
		basic_read_only_pointer<_Ty>{ rop._rop }
	{}

	inline _Ty* operator-> () { return basic_read_only_pointer<_Ty>::_ptr; }

	inline operator ConstReadOnlyPointer<_Ty>() const { return { const_cast<const _Ty*>(basic_read_only_pointer<_Ty>::_ptr) }; }
};






/* SimpleLinkedList */
template<class _Ty>
class SimpleLinkedList
{
private:
	struct Node
	{
		_Ty data;
		Node* next;
		Node* prev;

		inline Node(_Ty data, Node* next = nullptr, Node* prev = nullptr) :
			data{ data },
			next{ next },
			prev{ prev }
		{}
	};
	static inline void free(Node* const node)
	{
		node->next = nullptr;
		node->prev = nullptr;
		delete node;
	}

public:
	class NodePointer
	{
	private:
		SimpleLinkedList<_Ty>* _owner;
		Node* _node;

		NodePointer(SimpleLinkedList<_Ty>* owner, Node* node) :
			_owner{ owner },
			_node{ node }
		{}

	public:
		inline SimpleLinkedList* owner() const { return _owner; }
		inline const _Ty& data() const { return _node->data; }
		inline NodePointer next() const { return { _owner, !_node ? nullptr : _node->next }; }
		inline NodePointer prev() const { return { _owner, !_node ? nullptr : _node->prev }; }

		inline void data(const _Ty& value) { _node->data = value; }

		inline operator bool() const { return _node; }
		inline bool operator! () const { return !_node; }

		inline _Ty& operator* () { return _node->data; }
		inline const _Ty& operator* () const { return _node->data; }

		inline _Ty* operator-> () { return &_node->data; }
		inline const _Ty* operator-> () const { return &_node->data; }

		inline NodePointer operator++ () { *this = next(); return *this; }
		inline NodePointer operator++ (int) { NodePointer p{ *this }; *this = next(); return p; }

		inline NodePointer operator-- () { *this = prev(); return *this; }
		inline NodePointer operator-- (int) { NodePointer p{ *this }; *this = prev(); return p; }
	};


private:
	Node* _head;
	Node* _tail;
	size_t _size;

public:
	SimpleLinkedList() :
		_head{ nullptr },
		_tail{ nullptr },
		_size{ 0 }
	{}
	~SimpleLinkedList() { clear(); }

	void append(const _Ty& value)
	{
		if (!_head)
		{
			_head = new Node{ value };
			_tail = _head;
		}
		else
		{
			_tail->next = new Node{ value, nullptr, _tail };
			_tail = _tail->next;
		}
		++_size;
	}

	void insert(const size_t index, const _Ty& value)
	{
		_check_index(index, true);
		if (index == size)
			append(value);
		else
		{
			Node* current = _find_node(index);
			Node* node = new Node{ value, current, current->prev };
			if (current == _head)
				_head = node;
			else node->prev->next = node;
			current->prev = node;
			++_size;
		}
	}

	void set(const size_t index, const _Ty& value)
	{
		_check_index(index, true);
		_find_node(index)->data = value;
	}

	void eraseNode(NodePointer& ptr)
	{
		if (!_same_owner(ptr))
			return;
		_erase_node(ptr._node);
	}

	bool erase(const _Ty& value)
	{
		if (!_head)
			return false;

		if (_head == _tail)
		{
			if (_head->data == value)
			{
				free(_head);
				_head = _tail = nullptr;
				_size = 0;
				return true;
			}
			return false;
		}

		Node* node;
		for (node = _head; node; node = node->next)
			if (node->data == value)
				break;
		if (!node)
			return false;

		_erase_node(node);
		return true;
	}

	void pop(size_t index)
	{
		_check_index(index, false);
		if (_head == _tail)
			_erase_node(_head);
		else _erase_node(_find_node(index));
	}
	inline void pop() { return pop(_size - 1); }

	NodePointer getNode(const size_t index)
	{
		_check_index(index, true);
		return NodePointer{ this, _find_node(index) };
	}
	inline NodePointer getNode() { return getNode(_size - 1); }
	const NodePointer getNode(const size_t index) const
	{
		_check_index(index, true);
		return NodePointer{ this, _find_node(index) };
	}
	inline const NodePointer getNode() const { return getNode(_size - 1); }

	inline const _Ty& get(const size_t index) const { return *getNode(index); }
	inline const _Ty& first() const { return _head->data; }
	inline const _Ty& last() const { return _tail->data; }

	intmax_t index(const _Ty& value) const
	{
		intmax_t idx = 0;
		for (Node* node = _head; node; node = node->next, ++idx)
			if (node->data == value)
				return idx;
		return -1;
	}

	intmax_t lastIndex(const _Ty& value) const
	{
		intmax_t idx = 0, sel = -1;
		for (Node* node = _head; node; node = node->next, ++idx)
			if (node->data == value)
				sel = idx;
		return sel;
	}

	void forEachNode(std::function<void(NodePointer)>& action)
	{
		for (Node* node = _head; node; node = node->next)
			action(NodePointer{ this, node });
	}
	void forEachNode(std::function<void(const NodePointer)>& action) const
	{
		for (Node* node = _head; node; node = node->next)
			action(NodePointer{ this, node });
	}

	void forEach(std::function<void(_Ty&)>& action)
	{
		for (Node* node = _head; node; node = node->next)
			action(node->data);
	}
	void forEach(std::function<void(const _Ty&)>& action) const
	{
		for (Node* node = _head; node; node = node->next)
			action(node->data);
	}

	bool empty() const { return !size; }

	size_t size() const { return size; }

	void clear()
	{
		Node* next;
		Node* node = _head;
		while (node)
		{
			next = node->next;
			free(node);
			node = next;
		}
		_size = 0;
	}


	inline SimpleLinkedList& operator<< (const _Ty& value) { append(value); return *this; }
	SimpleLinkedList& operator>> (_Ty& data)
	{
		_check_index(_size - 1, false);
		data = _tail->data;
		_erase_node(_tail);
		return *this;
	}

	inline _Ty& operator[] (const size_t index) { return *getNode(index); }
	inline const _Ty& operator[] (const size_t index) const { return *getNode(index); }

	operator bool() const { return _head; }
	bool operator! () const { return !_head; }

public:
	class bad_index : public std::exception
	{
	private:
		bad_index(const SimpleLinkedList<_Ty>* list, const size_t index) :
			exception(std::string{ "Bad index in range [0, " } + (list->size - 1) + "]. " + index + " found.")
		{}
	};

	class no_such_element : public std::exception {};

private:
	bool _same_owner(const NodePointer& ptr) const
	{
		return this == ptr._owner;
	}

	bool _is_valid_index(const size_t index, bool accept_next) const
	{
		return index < size || (accept_next && index == size);
	}

	void _check_index(const size_t index, bool accept_next) const
	{
		if (index >= size && (!accept_next || index > size))
			throw SimpleLinkedList::bad_index{ index };
	}

	Node* _find_node(size_t index) const
	{
		if (index == 0)
			return _head;
		else if (index == size - 1)
			return _tail;
		Node* node = _head;
		for (; index > 0; --index)
			node = node->next;
		return node;
	}

	void _erase_node(Node* node)
	{
		if (_head == _tail)
		{
			if (_head == node)
			{
				free(_head);
				_head = _tail = nullptr;
				_size = 0;
			}
		}
		else
		{
			if (node == _head)
			{
				_head = node->next;
				_head->prev = nullptr;
			}
			else if (node == _tail)
			{
				_tail = node->prev;
				_tail->next = nullptr;
			}
			else
			{
				node->next->prev = node->prev;
				node->prev->next = node->next;
			}
			free(node);
			--_size;
		}
	}

/* Iterator part */
public:

	template<class _Ty, class _RetTy>
	class basic_iterator
	{
	private:
		const SimpleLinkedList<_Ty>* _owner;
		Node* _node;
		bool _end;

		basic_iterator(const SimpleLinkedList<_Ty>* owner, Node* node, bool end) :
			_owner{ owner },
			_node{ node },
			_end{ end }
		{}

	public:
		bool operator== (const basic_iterator& it) const
		{
			if (_owner != it._owner)
				return false;
			if (_end)
				return it._end;
			return !it._end && _node == it._node;
		}
		inline bool operator!= (const basic_iterator& it) const { return !operator==(it); }

		operator bool() const { return _end; }
		bool operator! () const { !_end; }

		basic_iterator operator++ ()
		{
			if (_end)
				throw no_such_element{};

			if (!_node->next)
				_end = true;
			else _node = _node->next;

			return *this;
		}
		basic_iterator operator++ (int)
		{
			basic_iterator it{ *this };
			operator++();
			return it;
		}

		basic_iterator operator-- ()
		{
			if (_node->prev)
			{
				_node = _node->prev;
				_end = false;
			}
			else throw no_such_element();

			return *this;
		}
		basic_iterator operator-- (int)
		{
			basic_iterator it{ *this };
			operator--();
			return it;
		}

		_RetTy& operator* () { return _node->data; }
		const _RetTy& operator* () const { return std::addressof(_node->data); }

		_RetTy* operator-> () { return &_node->data; }
		const _RetTy* operator-> () const { return std::addressof(_node->data); }

		template<class _Ty>
		friend class SimpleLinkedList;
	};

	using iterator = basic_iterator<_Ty, _Ty>;
	using const_iterator = basic_iterator<_Ty, const _Ty>;

	iterator begin() { return _head ? iterator{ this, _head, false } : iterator{ this, nullptr, true }; }
	const_iterator begin() const { return _head ? const_iterator{ this, _head, false } : const_iterator{ this, nullptr, true }; }
	const_iterator cbegin() const { return _head ? const_iterator{ this, _head, false } : const_iterator{ this, nullptr, true }; }

	iterator end() { return iterator{ this, _tail, true }; }
	const_iterator end() const { return const_iterator{ this, _tail, true }; }
	const_iterator cend() const { return const_iterator{ this, _tail, true }; }


public:
	template<class _Ty>
	friend std::string to_string(const SimpleLinkedList<_Ty>&);

	template<class _Ty>
	friend std::ostream& operator<< (std::ostream&, const SimpleLinkedList<_Ty>&);
};

template<class _Ty>
std::string to_string(const SimpleLinkedList<_Ty>& list)
{
	if (!list._head)
		return "[]";
	std::stringstream ss;
	ss << "[";
	auto node = list._head;
	decltype(node) next;
	for (node = list._head; node; node = next)
	{
		next = node->next;
		ss << node->data;
		if (next)
			ss << ", ";
	}
	ss << "]";
	return ss.str();
}

template<class _Ty>
std::ostream& operator<< (std::ostream& os, const SimpleLinkedList<_Ty>& list)
{
	return os << to_string(list);
}
