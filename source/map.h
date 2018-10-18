#pragma once
#include <string>
#include <vector>
#include <map>

using namespace std;

struct location
{
	long long id = 0;
	long double latitude = 0.0;
	long double longtitude = 0.0;
};


struct road
{
	long long start_id = 0;
	long long end_id = 0;
	string street_name = "";
	long double speed_limit = 0.0;
};


class Map
{
public:
	Map();
	void add_location(location loc);
	void add_road(road r);
	const location get_location(long long id);
	const vector<road> get_road(long long id);
private:
	map<long long, location> locations;
	map<long long, vector<road> > roads;
};

Map::Map() {}

void Map::add_location(location l)
{
	locations.insert(make_pair(l.id, l));
	return;
}

void Map::add_road(road r)
{
	roads[r.start_id].push_back(r);
	roads[r.end_id].push_back(r);
}

const location Map::get_location(long long id)
{
	return locations[id];
}

const vector<road> Map::get_road(long long id)
{
	return roads[id];
}