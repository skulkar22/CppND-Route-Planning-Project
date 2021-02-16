#include "route_planner.h"
#include <algorithm>

RoutePlanner::RoutePlanner(RouteModel &model, float start_x, float start_y, float end_x, float end_y): m_Model(model) {
    // Convert inputs to percentage:
    start_x *= 0.01;
    start_y *= 0.01;
    end_x *= 0.01;
    end_y *= 0.01;

    // TODO 2: Use the m_Model.FindClosestNode method to find the closest nodes to the starting and ending coordinates.
    // Store the nodes you find in the RoutePlanner's start_node and end_node attributes.
    start_node = &m_Model.FindClosestNode(start_x, start_y);
    end_node = &m_Model.FindClosestNode(end_x, end_y);

    //std::cout << "Start node x: " << start_node->x << std::endl;
    //std::cout << "Start node y: " << start_node->y << std::endl;
    //std::cout << "End node x: " << end_node->x << std::endl;
    //std::cout << "End node y: " << end_node->y << std::endl;

}

// TODO 3: Implement the CalculateHValue method.
// Tips:
// - You can use the distance to the end_node for the h value.
// - Node objects have a distance method to determine the distance to another node.

float RoutePlanner::CalculateHValue(RouteModel::Node const *node) {
    return node->distance(*end_node);
}

// TODO 4: Complete the AddNeighbors method to expand the current node by adding all unvisited neighbors to the open list.
// Tips:
// - Use the FindNeighbors() method of the current_node to populate current_node.neighbors vector with all the neighbors.
// - For each node in current_node.neighbors, set the parent, the h_value, the g_value.
// - Use CalculateHValue below to implement the h-Value calculation.
// - For each node in current_node.neighbors, add the neighbor to open_list and set the node's visited attribute to true.

void RoutePlanner::AddNeighbors(RouteModel::Node *current_node) {
    // Get neighbors of current_node
    current_node->FindNeighbors();

    for(RouteModel::Node *neighbor:current_node->neighbors){

        // Skip node if already visited
        if(neighbor->visited)
            continue;

        // Set neighbor's parent to current_node
        neighbor->parent = current_node;

        // Compute g-value
        neighbor->g_value = current_node->g_value + current_node->distance(*neighbor);;

        // Compute h-value
        neighbor->h_value = CalculateHValue(neighbor);

        // Add neighbor to open_list
        open_list.push_back(neighbor);

        // Set neighbor visited attribute
        neighbor->visited = true;
    }

}

// TODO 5: Complete the NextNode method to sort the open list and return the next node.
// Tips:
// - Sort the open_list according to the sum of the h value and g value.
// - Create a pointer to the node in the list with the lowest sum.
// - Remove that node from the open_list.
// - Return the pointer.

bool Compare(const RouteModel::Node *a, const RouteModel::Node *b) {
    float f1 = a->g_value + a->h_value; // f1 = g1 + h1
    float f2 = b->g_value + b->h_value; // f2 = g2 + h2
    return f1 > f2;
}

RouteModel::Node *RoutePlanner::NextNode() {
    // Sort open_list using total cost function (f = g + h)
    sort(open_list.begin(), open_list.end(), Compare);

    // Node with the least cost
    RouteModel::Node *lowest_node = open_list.back();

    // Remove lowest_node from open_list
    open_list.pop_back();

    // Return the node with lowest cost function
    return lowest_node;
}

// TODO 6: Complete the ConstructFinalPath method to return the final path found from your A* search.
// Tips:
// - This method should take the current (final) node as an argument and iteratively follow the
//   chain of parents of nodes until the starting node is found.
// - For each node in the chain, add the distance from the node to its parent to the distance variable.
// - The returned vector should be in the correct order: the start node should be the first element
//   of the vector, the end node should be the last element.

std::vector<RouteModel::Node> RoutePlanner::ConstructFinalPath(RouteModel::Node *current_node) {
    // Create path_found vector
    distance = 0.0f;
    std::vector<RouteModel::Node> path_found;

    while(current_node != start_node){
        // Add the node to path_found vector
        path_found.push_back(*current_node);

        // Add the distance from node to its parent
        distance += current_node->distance(*current_node->parent);

        // Update current node to its parent
        current_node = current_node->parent;
    }

    path_found.push_back(*current_node); // Add start_node to path_found
    std::reverse(path_found.begin(), path_found.end()); // Reverse node sequence of path
    distance *= m_Model.MetricScale(); // Multiply the distance by the scale of the map to get meters.
    return path_found;
}


// TODO 7: Write the A* Search algorithm here.
// Tips:
// - Use the AddNeighbors method to add all of the neighbors of the current node to the open_list.
// - Use the NextNode() method to sort the open_list and return the next node.
// - When the search has reached the end_node, use the ConstructFinalPath method to return the final path that was found.
// - Store the final path in the m_Model.path attribute before the method exits. This path will then be displayed on the map tile.

void RoutePlanner::AStarSearch() {
    RouteModel::Node *current_node = nullptr;

    // Set visited attribute for start node
    start_node->visited = true;

    // Search loop
    current_node = start_node;

    while(current_node != end_node){
        AddNeighbors(current_node);
        current_node = NextNode();
    }

    // Construct the final path
    m_Model.path = ConstructFinalPath(current_node);
}