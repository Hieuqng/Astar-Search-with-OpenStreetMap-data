#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>
#include <map>
#include <iterator>
#include "map.h"
#include "pqueue.h"

#define _USE_MATH_DEFINES 

using namespace std;

//Element of frontier
struct node
{
	double f, g, h = 0.0;
	location loc;
	long long parent_id = 0;
};

//Structure of pqueue's keys. Helps compare keys in pqueue
struct keys
{
	keys(long long id, node node) { x = id; y = node;}
	long long x;
	node y;
};

//Operation of comparing 2 keys
bool operator==(const keys& lhs, const keys& rhs)
{
	return lhs.x == rhs.x;
}

// Inject a hash function into the std namespace.
// Note that the hash function should return size_t, which is an
// unsigned int.  This sample hash function combines two calls to the
// standard int hash function.  We use the bitwise XOR operator ^ to
// combine them because it helps some with collisions.  To include
// a hash on a third field, just use multiple hash calls and combine
// with bitwise XOR.
namespace std
{
	template<> struct hash<keys>
	{
		size_t operator()(const keys & s) const
		{
			size_t h1 = hash<long long>()(s.x);
			return h1;
		}
	};
}


//Holds values in pqueue. The first element is priority measure
struct values
{
	long long id;
	double f, g, h;
};


vector<string> split(const string &s, char delim);
double cost(double curr_lat, double curr_long, double next_lat, double next_long, double speed);
vector<long long> aStarSearch(Map* map, long long start_id, long long end_id, const bool DEBUG);
bool is_in(vector<node> v, long long id);
string get_road_name(Map* map, long long start_id, long long end_id);


int main() 
{
	const bool DEBUG = true;
	Map map;
	location l;
	road r;

	string filename, line;
	cout << "What filename do you want to read? ";
	cin >> filename;
	ifstream infile(filename);
	if (!infile.is_open())
	{
		cerr << "Couldn't open file " << filename << endl;
		exit(1);
	}

	while (getline(infile, line))
	{
		vector<string> words = split(line, '|');

		if (words[0] == "location")
		{
			l.id = stoll(words[1]);
			l.longtitude = stod(words[2]);
			l.latitude = stod(words[3]);
			map.add_location(l);
		}
		else if (words[0] == "road")
		{
			r.start_id = stoll(words[1]);
			r.end_id = stoll(words[2]);
			r.speed_limit = stod(words[3]);
			r.street_name = words[4];
			map.add_road(r);
		}

	}


	long long start_id;
	long long end_id;
	vector<long long> path;

	//Collect inputs from user
	cout << "Enter a start location ID or zero to quit: ";
	cin >> start_id;
	cout << "Enter a end location ID or zero to quit: ";
	cin >> end_id;

	while ((start_id != 0) && (end_id != 0))
	{
		cout << "Routing from " << start_id << " to " << end_id << endl;

		path = aStarSearch(&map, start_id, end_id, DEBUG);
		if (path.size() < 2)
		{
			cout << "No path found." << endl;
			return 0;
		}

		cout << "Path found is: " << endl;
		cout << start_id << " (starting location)" << endl;
		long long curr_id = start_id;
		for (int i = path.size() - 2; i >= 0; i--)
		{
			cout << path[i] << " " << get_road_name(&map, curr_id, path[i]) << endl;
			curr_id = path[i];
		}
			
		//New inputs
		cout << "Enter a start location ID or zero to quit: ";
		cin >> start_id;

		cout << "Enter a end location ID or zero to quit: ";
		cin >> end_id;

	}

	return 0;
}


vector<long long> aStarSearch(Map* map, long long start_id, long long end_id, const bool DEBUG)
{
	//Get the goal node (goal state)
	node goal;
	goal.loc = map->get_location(end_id);

	// Initialize start node (initial state)
	node start;
	start.loc = map->get_location(start_id);
	start.g = 0.0;
	start.h = cost(start.loc.latitude, start.loc.longtitude, goal.loc.latitude, goal.loc.longtitude, 65.0);
	start.f = start.g + start.h;

	//Initialize frontier
	pqueue<keys, double> frontier;
	frontier.enqueue(keys(start.loc.id, start), start.f);
	
	//Initialize explored set
	vector<node> explored;

	//Child-Parent map
	std::map<long long, values> child_parent;
	values parent_start = { 0, start.f, start.g, start.h };
	child_parent.insert(make_pair(start_id, parent_start));

	int total_nodes_expanded = 0;
	
	//Loop forever
	while (true)
	{
		//If frontier is empty, return false
		if (frontier.empty()) 
			break;

		//Dequeue top node
		node curr_node = frontier.dequeue().y;
		if (DEBUG && (curr_node.parent_id != 0))
			cout << endl << "Visiting " << curr_node.loc.id << ", g=" << curr_node.g << ", h=" << curr_node.h << ", f=" << curr_node.f << endl;
		total_nodes_expanded++;

		//Add node to explored
		explored.push_back(curr_node);

		//If node is goal, return solution
		if (curr_node.loc.id == end_id)
			break;
		
		
		//Loop through roads from node
		vector<road> current_roads = map->get_road(curr_node.loc.id);

		for (int i = 0; i < current_roads.size(); i++)
		{
			node child_node;
			road r = current_roads[i];
			
			//Get child_node
			if (r.start_id == curr_node.loc.id)
			{
				child_node.loc = map->get_location(r.end_id);
			}
			else
				child_node.loc = map->get_location(r.start_id);

			//Get lat, long info
			double curr_lat = curr_node.loc.latitude;
			double curr_long = curr_node.loc.longtitude;
			double next_lat = child_node.loc.latitude;
			double next_long = child_node.loc.longtitude;
	
			//Assign values to child_node
			child_node.g = curr_node.g + cost(curr_lat, curr_long, next_lat, next_long, r.speed_limit);
			child_node.h = cost(next_lat, next_long, goal.loc.latitude, goal.loc.longtitude, 65.0);
			child_node.f = child_node.g + child_node.h;
			child_node.parent_id = curr_node.loc.id;

			//Check if child node is in explored
			bool is_in_explored = is_in(explored, child_node.loc.id);

			//If child_node is not in explored or frontier. Else if child_node is in frontier, but child_node.f is better than frontier's
			if (!is_in_explored && !frontier.contains(keys(child_node.loc.id, child_node)))
			{
				//Enqueue child to frontier
				frontier.enqueue(keys(child_node.loc.id, child_node), child_node.f);

				//Update child_parent map
				values parent_info = { child_node.parent_id, child_node.f, child_node.g, child_node.h };
				child_parent.insert(make_pair(child_node.loc.id, parent_info));

				if (DEBUG) 
					cout << "   Adding " << child_node.loc.id << ", g=" << child_node.g << ", h=" << child_node.h << ", f=" << child_node.f << " to the frontier." << endl;
			}
			else if (frontier.contains(keys(child_node.loc.id, child_node)) && (child_node.f < frontier.get_priority(keys(child_node.loc.id, child_node))))
			{
				//Update priority of child_node
				frontier.change_priority(keys(child_node.loc.id, child_node), child_node.f);
				
				if (DEBUG)
				{
					cout << "   Updated priority of " << child_node.loc.id << endl;
					//Get back the old child
					values old_child_node = child_parent[child_node.loc.id];
					cout << "      old: " << old_child_node.id << ", g=" << old_child_node.g << ", h=" << old_child_node.h << ", f=" << old_child_node.f << endl;
					cout << "      new: " << child_node.loc.id << ", g=" << child_node.g << ", h=" << child_node.h << ", f=" << child_node.f << endl;
				}

				//Update the child_parent map
				values parent_info = { child_node.parent_id, child_node.f, child_node.g, child_node.h };
				child_parent[child_node.loc.id] = parent_info;
			}
		}
	} //end while loop

	
	//Print out total traveled time, # of node expanded 
	cout << endl << "Total travel time is: " << explored.back().g << " minutes." << endl;
	cout << "Number of nodes expanded: " << total_nodes_expanded - 1 << endl;

	//Return solution path
	vector<long long> solution_ids;
	solution_ids.push_back(end_id);
	long long curr_id = end_id;
	while (child_parent[curr_id].id != 0)
	{
		solution_ids.push_back(child_parent[curr_id].id);
		curr_id = child_parent[curr_id].id;
	}

	return solution_ids;
}


double cost(double curr_lat, double curr_long, double next_lat, double next_long, double speed)
{
	const double pi = acos(-1.0);
	
	//Convert latitude and longitude to spherical coordinates in radians.
	double degrees_to_radians = pi / 180.0;

	//phi = 90 - latitude
	double phi1 = (90.0 - curr_lat) * degrees_to_radians;
	double phi2 = (90.0 - next_lat) * degrees_to_radians;

	//theta = longitude
	double theta1 = curr_long * degrees_to_radians;
	double theta2 = next_long * degrees_to_radians;

	//Compute spherical distance from spherical coordinates.
	double cosine = (sin(phi1) * sin(phi2) * cos(theta1 - theta2) + cos(phi1) * cos(phi2));
	double distance = acos(cosine) * 3960.0;
	double time = distance / speed * 60.0;

	return time;
}


bool is_in(vector<node> v, long long id)
{
	for (int i = 0; i < v.size(); i++)
	{
		if (id == v[i].loc.id)
		{
			return true;
			break;
		}
	}

	return false;
}


string get_road_name(Map* map, long long start_id, long long end_id)
{
	vector<road> roads = map->get_road(start_id);
	string name = "";

	for (size_t i = 0; i < roads.size(); i++)
	{
		if ((roads[i].end_id == end_id) || (roads[i].start_id == end_id))
		{
			return roads[i].street_name;
		}
	}

	return name;
}


vector<string> split(const string &s, char delim)
{
	stringstream ss(s);
	string item;
	vector<string> elems;
	while (getline(ss, item, delim)) {
		elems.push_back(item);
	}
	
	return elems;
}