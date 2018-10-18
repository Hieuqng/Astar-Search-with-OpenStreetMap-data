#ifndef _PQUEUE_H
#define _PQUEUE_H

#include <map>
#include <unordered_map>
#include <set>
#include <string>
#include <memory>
#include <functional>

using namespace std;

template <class T>
struct ptr_equal_to
{
	bool operator()(const shared_ptr<T> & p1, const shared_ptr<T> & p2) const
	{
		return *p1 == *p2;
	}
};

template <class T>
struct ptr_hash
{
	size_t operator()(const shared_ptr<T> & p) const
	{
		hash<T> hash_fn;
		return hash_fn(*p);
	}
};

template <class T, class N>
class pqueue
{
public:

	pqueue();

	bool empty();
	unsigned long size();

	void enqueue(const T& item, N pri);
	T dequeue();
	N top_priority();
	N get_priority(const T& item);
	void change_priority(const T& item, N newpri);
	bool contains(const T& item);

	void printDebug();

private:

	//set<T*> items;
	multimap<N, shared_ptr<T>> priToItems;
	unordered_map<shared_ptr<T>, N, ptr_hash<T>, ptr_equal_to<T>> itemsToPri;
};

#include "pqueue.cpp"

#endif
