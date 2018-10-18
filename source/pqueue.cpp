#ifndef _PQUEUE_CPP
#define _PQUEUE_CPP

#include <iostream>
#include "pqueue.h"

using namespace std;



template <class T, class N>
pqueue<T, N>::pqueue() {
}

template <class T, class N>
bool pqueue<T, N>::empty()
{
	return itemsToPri.empty();
}

template <class T, class N>
unsigned long pqueue<T, N>::size()
{
	return itemsToPri.size();
}

template <class T, class N>
void pqueue<T, N>::enqueue(const T& item, N pri)
{
	shared_ptr<T> newitem = make_shared<T>(item);
	if (itemsToPri.find(newitem) != itemsToPri.end())
	{
		cerr << "Error in pqueue::enqueue: priority queue already contains item." << endl;
		exit(1);
	}

	itemsToPri.insert(pair<shared_ptr<T>, N>(newitem, pri));
	priToItems.insert(pair<N, shared_ptr<T>>(pri, newitem));
}

template <class T, class N>
T pqueue<T, N>::dequeue()
{
	if (empty())
	{
		cerr << "Error in pqueue::dequeue: priority queue is empty." << endl;
		exit(1);
	}

	// find item with smallest priority
	auto it = priToItems.begin();
	N lowestpri = it->first;
	shared_ptr<T> itemToReturn = it->second;

	// erase from 2 places
	priToItems.erase(it);
	auto it2 = itemsToPri.find(itemToReturn);
	itemsToPri.erase(it2);

	return *itemToReturn;
}

template <class T, class N>
N pqueue<T, N>::top_priority()
{
	if (empty())
	{
		cerr << "Error in pqueue::top_priority: priority queue is empty." << endl;
		exit(1);
	}

	// find item with smallest priority
	auto it = priToItems.begin();
	N lowestpri = it->first;
	return lowestpri;
}

template <class T, class N>
bool pqueue<T, N>::contains(const T& item)
{
	shared_ptr<T> itemPtr = make_shared<T>(item);
	return (itemsToPri.find(itemPtr) != itemsToPri.end());
}

template <class T, class N>
N pqueue<T, N>::get_priority(const T &item)
{
	if (empty()) {
		cerr << "Error in pqueue::get_priority: priority queue is empty." << endl;
		exit(1);
	}

	// change items to pri
	shared_ptr<T> itemPtr = make_shared<T>(item);
	if (itemsToPri.find(itemPtr) == itemsToPri.end()) {
		cerr << "Error in pqueue::get_priority: priority queue does not contain item." << endl;
		exit(1);
	}

	return itemsToPri[itemPtr];
}

template <class T, class N>
void pqueue<T, N>::change_priority(const T &item, N newpri)
{
	if (empty())
	{
		cerr << "Error in pqueue::change_priority: priority queue is empty." << endl;
		exit(1);
	}

	// change items to pri
	shared_ptr<T> itemPtr = make_shared<T>(item);
	auto it2 = itemsToPri.find(itemPtr);
	if (it2 == itemsToPri.end())
	{
		cerr << "Error in pqueue::enqueue: priority queue does not contain item." << endl;
		exit(1);
	}

	N oldPri = itemsToPri[itemPtr];
	itemsToPri.erase(it2);
	itemsToPri[itemPtr] = newpri;

	// change Pri to items
	auto it = priToItems.lower_bound(oldPri);
	while (it != priToItems.end() && !(((*((*it).second)) == item)))
	{
		//cout << "iterator is now " << (*((*it).second)) << endl;
		it++;
	}

	if (it == priToItems.end())
	{
		cerr << "Error in pqueue::change_priority: no match for item." << endl;
		exit(1);
	}

	itemPtr = it->second;
	priToItems.erase(it);
	priToItems.insert(make_pair(newpri, itemPtr));
};

template <class T, class N>
void pqueue<T, N>::printDebug()
{
	cout << "these should match: " << priToItems.size() << " " << itemsToPri.size() << endl;
	cout << "status of items to pri:" << endl;

	for (auto item : itemsToPri)
		cout << *item.first << " " << item.second << endl;

	cout << "status of pri to items:" << endl;

	for (auto item : priToItems)
		cout << item.first << " " << *item.second << endl;
};

#endif
