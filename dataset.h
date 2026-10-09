#pragma once
#include <vector>
#include <string>
#include "point.h"

class Dataset {
private:
	std::vector<Point> points;
	int N_C; //celkovy pocet klastrov
public:
	Dataset() {};

	void generateData(int N_c);

	void loadDataFromFile(std::string filename);

	void clear();
	void addPoint(const Point& p);
	const std::vector<Point>& getPoints();
	int getNumClusters() const;
	void updatePointCoordinates(int index, double newX1, double newX2);

	void setNewcomerStatus(int index, bool status);

	~Dataset() {};
};