#pragma once

#include <vector>
#include <iostream>

// Define a struct to represent a location in the maze
struct Location {
	int col; // Column coordinate (x)
	int row; // Row coordinate (y)

	// Overload the quality operator to compare two location objects.
	bool operator==(const Location& other) const {
		return (col == other.col) && (row == other.row);
	}

	// Overload the hash operator for a Location object
	size_t operator()(const Location& locationToHash) const noexcept {
		size_t hash = locationToHash.col + 10 * locationToHash.row;
		return hash;
	};

	// Constructor for the location struct
	Location(int col = 0, int row = 0) : col(col), row(row) {}
};
// Define a specialization of the std::hash template for the location struct
namespace std {
	template<> struct hash<Location>
	{
		// Define the hash function for Location objects
		std::size_t operator()(const Location& location) const noexcept
		{
			return location(location); // Call the function call operator to calculate the hash value
		}
	};
}

// Define a struct to represent a cell as a node in the maze
struct Node {
	Location location; // The location associated iwth the node
	Node* parent; // Pointer to the parent node. Allows us to trace back optimal path
	double g_score; // The cost from the start node to this node
	double h_score; // The estimate cost from this node to the goal node
	double f_score() const { return(this->g_score + this->h_score); }; // Calculation of the total cost of the node (g + h)

	// Constructor for the node struct
	Node(Location location, Node* parent, double g_score, double h_score)
		: location(location), parent(parent), g_score(g_score), h_score(h_score) {}
};

// Defina a priority queue class for storing nodes in a specific order based on f score
class priority_queue {
private:
	void heapify_up(int i); // Move a node up the min-heap to maintain the heap property
	void heapify_down(int i); // Move a node down the heap to maintain the heap property
public:
	~priority_queue(); // Destructor for the priority queue

	std::vector<std::unique_ptr<Node>> arr; // Vector to store the nodes in a priority queue

	void enqueue(std::unique_ptr<Node>& node); // Enqueues a node to the priority queue
	void dequeue(); // Dequeue the highest priority node from the priority queue
	std::unique_ptr<Node> top(); // Returns the highest priority node from the queue
	size_t size(); // Get the number of nodes in the priority queue
	bool empty(); // Check if the priority queue is empty
	bool exists(std::unique_ptr<Node>& node); // Check if a node exists in the priority queue.
};
