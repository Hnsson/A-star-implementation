#include "priority_queue.h"
#include <iostream>

// Move a node up the min-heap to maintain the heap property
void priority_queue::heapify_up(int i)
{
	int parent = (i - 1) / 2;

	// Compare the f_score of the current node iwth its parent and swap if necessary
	while (i > 0 && this->arr[i]->f_score() < this->arr[parent]->f_score()) {
		std::swap(this->arr[i], this->arr[parent]);
		i = parent;
		parent = (i - 1) / 2;
	}
}

// Move a node down the min-heap to maintain the heap property
void priority_queue::heapify_down(int i)
{
	int left = 2 * i + 1;
	int right = 2 * i + 2;
	int smallest = i;

	// Find the smallest child node based on their f_score
	if (left < this->arr.size() && this->arr[left]->f_score() < this->arr[smallest]->f_score()) {
		smallest = left;
	}
	if (right < this->arr.size() && this->arr[right]->f_score() < this->arr[smallest]->f_score()) {
		smallest = right;
	}

	// If the smallest child is not the current node, swap the and continue the heapify process
	if (smallest != i) {
		std::swap(this->arr[i], this->arr[smallest]);
		this->heapify_down(smallest);
	}
}

// Destructor for the priority queue
priority_queue::~priority_queue() {}

// Enqueues a node to the priority queue
void priority_queue::enqueue(std::unique_ptr<Node>& node)
{
	this->arr.push_back(std::move(node)); // Move the node into the vecotr
	this->heapify_up(this->arr.size() - 1); // Move the node up if necessary
}

// Dequeue the highest priority node from the priority queue
void priority_queue::dequeue()
{
	if (this->arr.size() == 0) {
		std::cout << "Priority Queue is empty!" << std::endl;
		return;
	}

	this->arr[0] = std::move(this->arr.back()); // Move the last node to the front
	this->arr.pop_back(); // Remove the last node (shrink the queue)
	this->heapify_down(0); // Move the the front node down if necessary
}

// Returns the highest priority node from the queue
std::unique_ptr<Node> priority_queue::top()
{
	if (this->arr.size() == 0) {
		std::cout << "Priority Queue is empty!" << std::endl;
		return nullptr;
	}
	return std::move(this->arr.front()); // Move the first nod out of the vector and return it
}

// Get the number of nodes in the priority queue
size_t priority_queue::size()
{
	return this->arr.size();
}

// Check if the priority queue is empty
bool priority_queue::empty()
{
	return this->arr.size() == 0;
}

// Check if a node exists in the priority queue
bool priority_queue::exists(std::unique_ptr<Node>& node)
{
	return std::find(this->arr.begin(), this->arr.end(), node) != this->arr.end();
}
