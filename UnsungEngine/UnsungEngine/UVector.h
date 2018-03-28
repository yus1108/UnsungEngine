#pragma once
#include "stdafx.h"
template<typename T>
class UVector
{
private:
	T *arr;
	unsigned int Size, Capacity;
public:
	UVector();
	UVector(const UVector<T>& that);
	~UVector();

	UVector<T>& operator=(const UVector<T>& that);
	T& operator[](const unsigned int index);
	const T& operator[](const unsigned int index) const;

	unsigned int size() const;
	unsigned int capacity() const;
	void clear();
	void push_back(T& item);
	void push_back(const T& item);
	void resize(const unsigned int & newCap = 0);
	void reserve(const unsigned int & newCap = 0);

	void insert(const T val, const unsigned int index);
	void insert(const T * val, const unsigned int n, const unsigned int index);
	void erase(const unsigned int index);
	void erase(const unsigned int index, const unsigned int n);
};

/////////////////////////////////////////////////////////////////////////////
// Function :	Constructor
// Notes : Constructs an empty array (Size 0 Capacity 0)
/////////////////////////////////////////////////////////////////////////////
template<typename T>
UVector<T>::UVector()
{
	arr = nullptr;
	Size = Capacity = 0;
}

/////////////////////////////////////////////////////////////////////////////
// Function :	Destructor
// Notes : cleans up any dynamic memory
/////////////////////////////////////////////////////////////////////////////
template<typename T>
UVector<T>::~UVector()
{
	delete[] arr;
}

/////////////////////////////////////////////////////////////////////////////
// Function :	Copy Constructor
/////////////////////////////////////////////////////////////////////////////
template<typename T>
UVector<T>::UVector(const UVector<T>& that)
{
	Size = that.size();
	Capacity = that.capacity();

	if (Capacity == 0)
		arr = nullptr;
	else
		arr = new T[Capacity];

	for (decltype(Size) i = 0; i < Size; i++)
		arr[i] = that[i];
}

/////////////////////////////////////////////////////////////////////////////
// Function :	Assignment Operator
/////////////////////////////////////////////////////////////////////////////
template<typename T>
UVector<T>& UVector<T>::operator=(const UVector<T>& that)
{
	if (this != &that) {
		delete[] arr;

		Size = that.size();
		Capacity = that.capacity();

		if (Capacity == 0)
			arr = nullptr;
		else
			arr = new T[Capacity];

		for (decltype(Size) i = 0; i < Size; i++)
		{
			arr[i] = that[i];
		}
	}

	return *this;
}

/////////////////////////////////////////////////////////////////////////////
// Function : operator[]
// Parameters : index - the index to access
// Return : Type & - returns the array element at [index]
// Notes : performs no error checking. user should ensure index is 
//		valid with the size() method
/////////////////////////////////////////////////////////////////////////////
template<typename T>
T & UVector<T>::operator[](const unsigned int index)
{
	return arr[index];
}

/////////////////////////////////////////////////////////////////////////////
// Function : operator[]
// Parameters : index - the index to access
// Return : const Type & - returns the array element at [index]
// Notes : performs no error checking. user should ensure index is 
//		valid with the size() method
/////////////////////////////////////////////////////////////////////////////
template<typename T>
const T & UVector<T>::operator[](const unsigned int index) const
{
	return arr[index];
}

/////////////////////////////////////////////////////////////////////////////
// Function :	size
// Returns : int - returns the number of items being stored
// Notes : this function returns the number of items being stored, 
/////////////////////////////////////////////////////////////////////////////
template<typename T>
unsigned int UVector<T>::size() const
{
	return Size;
}

/////////////////////////////////////////////////////////////////////////////
// Function : capacity
// Returns : int - returns the number of items the array can store before 
//		the next resize
// Notes : this function returns the number of items the array can store, 
//		not the number of bytes
/////////////////////////////////////////////////////////////////////////////
template<typename T>
unsigned int UVector<T>::capacity() const
{
	return Capacity;
}

/////////////////////////////////////////////////////////////////////////////
// Function :	clear
// Notes : cleans up any dynamic memory and resets size and capacity to 0
/////////////////////////////////////////////////////////////////////////////
template<typename T>
void UVector<T>::clear()
{
	delete[] arr;
	arr = nullptr;
	Size = Capacity = 0;
}

/////////////////////////////////////////////////////////////////////////////
// Function : push_back
// Parameters : item - the item to be appended to the next open spot
// Notes : this function will append the item to the next open spot. if 
//		no room exists, the array's capacity will be doubled and then 
//		the item will be added
/////////////////////////////////////////////////////////////////////////////
template<typename T>
void UVector<T>::push_back(T & item)
{
	if (Capacity == 0) {
		Capacity = 1;
		arr = new T[Capacity];
	}
	else if (Size == Capacity) {
		T *temp = arr;
		Capacity *= 2;
		arr = new T[Capacity];
		for (decltype(Size) i = 0; i < Size; i++)
			arr[i] = temp[i];
		delete[] temp;
	}

	arr[Size++] = item;
}

/////////////////////////////////////////////////////////////////////////////
// Function : push_back
// Parameters : item - the item to be appended to the next open spot
// Notes : this function will append the item to the next open spot. if 
//		no room exists, the array's capacity will be doubled and then 
//		the item will be added
/////////////////////////////////////////////////////////////////////////////
template<typename T>
void UVector<T>::push_back(const T & item)
{
	if (Capacity == 0) {
		Capacity = 1;
		arr = new T[Capacity];
	}
	else if (Size == Capacity) {
		T *temp = arr;
		Capacity *= 2;
		arr = new T[Capacity];
		for (decltype(Size) i = 0; i < Size; i++)
			arr[i] = temp[i];
		delete[] temp;
	}

	arr[Size++] = item;
}

/////////////////////////////////////////////////////////////////////////////
// Function : resize
// Parameters : newCap - the new capacity
// Notes : 	- default parameter - resize more space in the array, based on 
//		the expansion rate (100%, 1 minimum).
//		- non-default parameter, expand to the specified capacity
//		- if newCap is LESS than the current capacity, do nothing. 
//		This function should NOT make the array smaller.
/////////////////////////////////////////////////////////////////////////////
template<typename T>
void UVector<T>::resize(const unsigned int & newCap)
{
	if (newCap > Capacity || newCap == 0) {
		if (newCap == 0) {
			if (Capacity == 0)
				Capacity = 1;
			else
				Capacity *= 2;
		}
		else
			Capacity = newCap;

		T *temp = arr;

		arr = new T[Capacity];
		Size = Capacity;
		for (decltype(Size) i = 0; i < Size; i++)
			arr[i] = temp[i];
		delete[] temp;
	}
}

/////////////////////////////////////////////////////////////////////////////
// Function : reserve
// Parameters : newCap - the new capacity
// Notes : 	- default parameter - reserve more space in the array, based on 
//		the expansion rate (100%, 1 minimum).
//		- non-default parameter, expand to the specified capacity
//		- if newCap is LESS than the current capacity, do nothing. 
//		This function should NOT make the array smaller.
/////////////////////////////////////////////////////////////////////////////
template<typename T>
void UVector<T>::reserve(const unsigned int & newCap)
{
	if (newCap > Capacity || newCap == 0) {
		if (newCap == 0) {
			if (Capacity == 0)
				Capacity = 1;
			else
				Capacity *= 2;
		}
		else
			Capacity = newCap;

		T *temp = arr;

		arr = new T[Capacity];
		for (decltype(Size) i = 0; i < Size; i++)
			arr[i] = temp[i];
		delete[] temp;
	}
}

/////////////////////////////////////////////////////////////////////////////
// Function :	insert
// Parameters : val - the value to insert
//		   index - the index to insert at
// Notes : if the array is full, this function should expand the array at 
//		the default expansion rate (double the capacity, 1 minimum)
/////////////////////////////////////////////////////////////////////////////
template<typename T>
void UVector<T>::insert(const T val, const unsigned int index)
{
	if (Capacity == 0 && index == 0) {
		Capacity = 1;
		arr = new T[Capacity];
		arr[Size++] = val;
	}
	else if (index < Size) {
		T *temp;
		if (Size == Capacity) {
			Capacity *= 2;
			temp = arr;
		}
		else {
			temp = new T[Size];
			for (decltype(Size) i = 0; i < Size; i++)
				temp[i] = arr[i];
			delete[] arr;
		}
		arr = new T[Capacity];

		for (decltype(Size) i = 0; i < index; i++) {
			arr[i] = temp[i];
		}
		arr[index] = val;
		Size++;
		for (decltype(Size) i = index + 1; i < Size; i++) {
			arr[i] = temp[i - 1];
		}
		delete[] temp;
	}
}

/////////////////////////////////////////////////////////////////////////////
// Function :	insert
// Parameters : val - the items to insert
//		   n - the number of items to insert
//		   index - the index to insert at
// Notes : if the array is full, this function should expand the array at 
//		the default expansion rate (double the capacity, 1 minimum) 
//		before inserting
/////////////////////////////////////////////////////////////////////////////
template<typename T>
void UVector<T>::insert(const T * val, const unsigned int n, const unsigned int index)
{
	for (unsigned int i = 0; i < n; i++)
		insert(val[i], index + i);
}

/////////////////////////////////////////////////////////////////////////////
// Function :	erase
// Parameters : index - the index to remove from
// Notes : this function removes one item from the specified index
/////////////////////////////////////////////////////////////////////////////
template<typename T>
void UVector<T>::erase(const unsigned int index)
{
	T *temp = nullptr;
	if (Capacity != 0)
		temp = new T[Capacity];

	for (decltype(Size) i = 0; i < index; i++)
		temp[i] = arr[i];
	if (Size > 0) {
		for (decltype(Size) i = index; i < Size - 1; i++)
			temp[i] = arr[i + 1];
	}


	if (Size > 0)
		Size--;

	delete[] arr;
	arr = temp;
}

/////////////////////////////////////////////////////////////////////////////
// Function :	erase
// Parameters : index - the index to remove from
//		   n - the number of items to remove
// Notes : this function removes multiple items from the specified index
/////////////////////////////////////////////////////////////////////////////
template<typename T>
void UVector<T>::erase(const unsigned int index, const unsigned int n)
{
	for (unsigned int i = 0; i < n; i++)
		remove(index);
}

