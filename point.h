#pragma once
struct Point {
	double x1, x2;
	int cluster_num;
	bool isNewObservation;

	Point(double p_x1, double p_x2, int cluster, bool isNew = false)
		: x1(p_x1), x2(p_x2), cluster_num(cluster), isNewObservation(isNew) {
	}
};