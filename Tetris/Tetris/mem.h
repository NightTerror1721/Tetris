#pragma once

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
