#include "priority_queue.h"
#include <unordered_set>
#include <iomanip>

// Size of the maze
#define ROW 10
#define COL 10

// Fast insertion and lookup due to being unordered. Used to keep track of visited location to prevent duplication in possible neighbors
std::unordered_set<Location> visited_nodes;

// Check if a location is valid based on certain conditions
bool is_valid(int row, int col) {
	// Check if row and column values are within the specified maze range
	if (!(row >= 0 && row < ROW && col >= 0 && col < COL)) {
		return false;
	}

	// Create a Location object for the current row and column and check in the set if location has been visited
	Location location(col, row);
	if (!(visited_nodes.find(location) == visited_nodes.end())) {
		return false;
	}

	// If the location satisfies all conditions, it is considered a valid node
	return true;
}

// Check if a location in the grid is unblocked (has value of 0) if (1) it means it is a wall in the maze
bool is_unblocked(int grid[ROW][COL], int row, int col) {
	return (grid[row][col] == 0);
}

// Check if the current node is the end node.
bool is_end(std::unique_ptr<Node>& node, std::unique_ptr<Node>& end) {
	return (node->location == end->location);
}

// Calculate the heirstic (H score) using the Euclidean distance between the source and the current node
double calc_h(std::unique_ptr<Node>& source, std::unique_ptr<Node>& node) {
	return static_cast<double>(std::sqrt(
		(source->location.col - node->location.col) * (source->location.col - node->location.col) +
		(source->location.row - node->location.row) * (source->location.row - node->location.row)
	));
}

// Calculate the cost (G score) to move from the current node to a neighbor node using the Euclidean distance
double calc_g(std::unique_ptr<Node>& current, std::unique_ptr<Node>& neighbor) {
	int d_x = neighbor->location.col - current->location.col;
	int d_y = neighbor->location.row - current->location.row;
	double distance = std::sqrt(d_x * d_x + d_y * d_y);

	return current->g_score + distance;
}

// Trace the path from the end node to the start node and print it on a maze grid
void trace_path(int maze[ROW][COL], std::unique_ptr<Node>& end) {
	Node* current = end.get();
	std::vector<Node*> final_path;

	// Traverse the path from the end node to the start node for the optimal path
	while (current != nullptr) {
		final_path.push_back(current);
		current = current->parent;
	}

	// Print a table of the nodes in the optimal path and their contents
	const char seperator = ' ';
	const int numWidth = 20;
	int iterations = 1;

	std::cout << "----------------------------------- Table of nodes ----------------------------------" << std::endl;

	std::cout << std::left << "\033[1;32m" << "Node:";
	std::cout << std::right << std::setfill(seperator) << std::setw(numWidth + 1) << "Location:";
	std::cout << std::right << std::setfill(seperator) << std::setw(numWidth) << "G Score:";
	std::cout << std::right << std::setfill(seperator) << std::setw(numWidth) << "H Score:";
	std::cout << std::right << std::setfill(seperator) << std::setw(numWidth) << "F Score:";
	std::cout << "\033[0m" << std::endl;
	for (auto it = final_path.rbegin(); it != final_path.rend(); ++it)
	{
		std::cout << std::left << std::setfill(seperator) << std::setw(numWidth) << iterations++;

		std::cout << std::right << std::setfill(seperator) << "(" << (*it)->location.col << ", " << (*it)->location.row << ")";

		std::cout << std::right << std::setfill(seperator) << std::setw(numWidth) << (*it)->g_score;

		std::cout << std::right << std::setfill(seperator) << std::setw(numWidth) << (*it)->h_score;

		std::cout << std::right << std::setfill(seperator) << std::setw(numWidth) << (*it)->f_score();

		std::cout << std::endl;
	}
	std::cout << "---------------------------------------------------------------------------------------" << std::endl << std::endl;

	// Print the maze grid with the optimal path marked by "O", obstacled marked by "|" and empty cells with "."
	std::cout << "--------------------------------- Visual representation -------------------------------" << std::endl;
	for (int i = 0; i < ROW; i++) {
		for (int j = 0; j < COL; j++) {
			bool skip = false;
			for (auto& node : final_path) {
				if (node->location == Location(j, i)) {
					// Print the path node as green "O"
					std::cout << "\033[1;32m" << "    O    " << "\033[0m";
					skip = true;
					break;
				}
			}
			if (!skip) {
				if (maze[i][j] == 1) {
					// Print obstacles as red "|"
					std::cout << "\033[1;31m" << "    |    " << "\033[0m";
				}
				else {
					// Print empty cell as white "."
					std::cout << "    .    ";
				}
			}
		}
		std::cout << std::endl << std::endl;
	}
	std::cout << "----------------------------------------------------------------------------------------" << std::endl << std::endl;
}

int a_star_search(int maze[ROW][COL], std::unique_ptr<Node> &start, std::unique_ptr<Node>& end) {
	// Create open list containing only the starting node
	priority_queue open_list;
	open_list.enqueue(start);
	// Create an empty closed list
	std::vector<std::unique_ptr<Node>> closed_list;

	while (!open_list.empty()) {
		// Find the node with the least f in the open list
		std::unique_ptr<Node> current_node = open_list.top();

		// Dequeue from the open list
		open_list.dequeue();


		// Generate all viable neighbours and set their parents to current node
		std::vector<std::unique_ptr<Node>> neighbours;

		for (int d_row = -1; d_row <= 1; d_row++) {
			for (int d_col = -1; d_col <= 1; d_col++) {
				if (d_row == 0 && d_col == 0) {
					continue;
				}
				int neighbor_row = current_node->location.row + d_row;
				int neighbor_col = current_node->location.col + d_col;
				if (is_valid(neighbor_row, neighbor_col) && is_unblocked(maze, neighbor_row, neighbor_col)) {
					std::unique_ptr<Node> neighbor_node(new Node(Location(neighbor_col, neighbor_row), nullptr, 0, 0));
					visited_nodes.insert(Location(neighbor_col, neighbor_row));
					neighbor_node->parent = current_node.get();

					neighbours.push_back(std::move(neighbor_node));
				}
			}
		}

		// For each neighbour
		for (auto& neighbour : neighbours) {
			// If neighbour is the goal, stop search
			if (is_end(neighbour, end)) {
				end->parent = neighbour.get();

				// Trace path from end to start
				trace_path(maze, neighbour);

				return 1;
			}
			// Else, compute both g and h for neighbour
			else {
				neighbour->g_score = calc_g(current_node, neighbour);
				neighbour->h_score = calc_h(neighbour, end);
			}

			// Check if neighbour should be skipped
			bool skip_neighbour = false;

			// If a node with the same position as neighbour exists in the open list with lower f, skip neighbour
			for (int i = 0; i < open_list.size(); i++) {
				if (open_list.arr[i]->location == neighbour->location) {
					if (open_list.arr[i]->f_score() < neighbour->f_score()) {
						skip_neighbour = true;
						break;
					}
				}
			}
			// If a node with the same position as neighbour exists in the closed list with lower f, skip neighbour
			if (!skip_neighbour) {
				for (auto& closed_node : closed_list) {
					if (closed_node->location == neighbour->location) {
						if (closed_node->f_score() < neighbour->f_score()) {
							skip_neighbour = true;
							break;
						}
					}
				}
			}
			// Otherwiese, add the neighbour to the open list
			if (!skip_neighbour) {
				open_list.enqueue(neighbour);
			}
		}
		// Push current node to the closed list
		closed_list.push_back(std::move(current_node));
	}

	return 0;
}

int main() {
	// Enable memory leak detection.
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// Define the maze as a 2D array.
	int maze[ROW][COL] =
	{
		{0, 0, 0, 1, 0, 0, 1, 0, 0, 0},
		{0, 0, 0, 1, 0, 0, 1, 0, 1, 0},
		{0, 0, 0, 1, 0, 0, 1, 0, 1, 0},
		{0, 0, 0, 1, 0, 1, 1, 0, 1, 0},
		{0, 0, 0, 1, 0, 1, 0, 0, 1, 0},
		{0, 0, 0, 1, 0, 1, 0, 0, 1, 0},
		{0, 0, 0, 1, 0, 0, 0, 0, 1, 0},
		{0, 0, 0, 1, 0, 0, 1, 0, 1, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 1, 0}
	};

	// Create unique pointers for the starting and end node.
	std::unique_ptr<Node> start(new Node(Location(0, 0), nullptr, 0, 0));
	std::unique_ptr<Node> end(new Node(Location(9, 9), nullptr, 0, 0));

	// Perform A* search to find the optimal path from the start to the end based on the maze
	if (a_star_search(maze, start, end)) {
		std::cout << "Could find a optimal path to the destination!" << std::endl << std::endl;
	}
	else {
		std::cout << "Couldn't find a path to the destination!" << std::endl;
	}

	return 0;
}
