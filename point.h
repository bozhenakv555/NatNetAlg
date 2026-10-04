#pragma once
struct Point {
	double x, y;
	int cluster_num;
	bool isNewObservation;

	Point(double p_x, double p_y, int cluster, bool isNew = false)
		: x(p_x), y(p_y), cluster_num(cluster), isNewObservation(isNew) {
	}
};