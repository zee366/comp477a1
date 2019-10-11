#pragma once
#include <cassert>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <GL/glut.h>
#include "cugl.h"
#include <windows.h>

using namespace std;
using namespace cugl;

// Return a point between p and q with positoin t, 0 <= t <= 1.
// t = 0 gives p; t = 1 gives q.
Point interpolate(const Point& p, const Point& q, double t)
{
	return p + t * (q - p);
}

// A group consists of four points.  The corresponding de Casteljau curve
// passes through p0 and p3.  The tangent at p0 passes through p1 and the
// tangent at p3 passes through p2.
class Group
{
public:
	Group(const Point& p0, const Point& p1, const Point& p2, const Point& p3)
		: p0(p0), p1(p1), p2(p2), p3(p3)
	{
		len = length();
	}

	// Return the point on the curve at time t, 0 <= t <= 1.
	// Uses de Cateljau's interpolation algorithm to find the point.
	Point getPos(double t) const
	{
		Point q0 = interpolate(p0, p1, t);
		Point q1 = interpolate(p1, p2, t);
		Point q2 = interpolate(p2, p3, t);
		Point r0 = interpolate(q0, q1, t);
		Point r1 = interpolate(q1, q2, t);
		return interpolate(r0, r1, t);
	}

	// Return a unit vector giving the direction of the curve at time t, 0 <= t <= 1.
	// Uses two nearby points to obtain the direction vector, then normalizes it.
	Vector getDir(double t) const
	{
		const double DELTA = 0.01;
		Point s, f;
		if (t < 0.5)
		{
			s = getPos(t);
			f = getPos(t + DELTA);
		}
		else
		{
			s = getPos(t - DELTA);
			f = getPos(t);
		}
		return (f - s).unit();
	}

	// Return the arc length of this segment.
	double getLen() const
	{
		return len;
	}

	// Return the normalized length of this segment.
	double getNormLen() const
	{
		return normlen;
	}

	// Set the normalized length of this segment.
	void setNormalizedLength(double len)
	{
		normlen = len;
	}

	// Display the group for debugging purposes.
	friend ostream& operator<<(ostream& os, const Group& g)
	{
		os << g.p0 << g.p1 << g.p2 << g.p3 << "  length = " << g.len << " (" << g.normlen << ")\n";
		return os;
	}

private:

	// Compute an approximation to the length of the segment
	// by adding the lengths of many small parts, each assumed straight.
	double length() const
	{
		const int NP = 100;
		Point p = p0;
		double d = 0;
		for (int i = 1; i <= NP; ++i)
		{
			Point q = getPos(double(i) / double(NP));
			d += dist(q, p);
			p = q;
		}
		return d;
	}

	Point p0, p1, p2, p3;  // The four points of the segment.
	double len;  // The actual length of the segment.
	double normlen;  // The length of the segment, normalized so that the entire curve has length 1.
};


// A Curve consists of several segments.
// Segments join with C^2 continuity.
class Curve
{
public:
	Curve() : ok(true) {};

	void setPoints(const vector<Point>& nsp, const vector<Point>& nep)
	{
		sp = nsp;
		ep = nep;
		np = sp.size();
		if (ep.size() != np)
		{
			cerr << "Start and end point arrays are not the same size\n";
			ok = false;
			return;
		}

		// Find midpoints
		for (size_t n = 0; n < np; ++n)
			mp.push_back(interpolate(sp[n], ep[n], 0.5));

		// Form groups
		for (size_t n = 0; n <= np - 2; ++n)
			gps.push_back(Group(mp[n], ep[n], sp[n + 1], mp[n + 1]));

		// Find total length of curve.
		totlen = 0;
		for (vector<Group>::const_iterator it = gps.begin(); it != gps.end(); ++it)
			totlen += it->getLen();

		// Find start points of each segment.
		dist.push_back(0);
		double cumdist = 0;
		for (vector<Group>::iterator it = gps.begin(); it != gps.end(); ++it)
		{
			double normlen = it->getLen() / totlen;
			cumdist += normlen;
			dist.push_back(cumdist);
			it->setNormalizedLength(normlen);
		}
	}

	// Return the point corresponding to time t on the curve.
	// Use binary search to find which segment it belongs to.
	Point getPos(double t)
	{
		if (ok)
		{
			size_t L = 0;
			size_t R = dist.size() - 1;
			while (R - L > 1)
			{
				size_t M = (L + R) / 2;
				double d = dist[M];
				if (t > d)
					L = M;
				else
					R = M;
			}
			assert(R = L + 1);
			double r = (t - dist[L]) / (dist[R] - dist[L]);
			return gps[L].getPos(r);
		}
		else return Point();
	}

	// Return the direction at time t on the curve.
	// Use binary search to find which segment it belongs to.
	Vector getDir(double t)
	{
		if (ok)
		{
			size_t L = 0;
			size_t R = dist.size() - 1;
			while (R - L > 1)
			{
				size_t M = (L + R) / 2;
				double d = dist[M];
				if (t > d)
					L = M;
				else
					R = M;
			}
			assert(R = L + 1);
			double r = (t - dist[L]) / (dist[R] - dist[L]);
			return gps[L].getDir(r);
		}
		else return Vector();
	}

	// Draw the tangents used to construct the curve.
	void showTangents()
	{
		if (ok)
		{
			for (size_t n = 0; n < np; ++n)
			{
				sp[n].draw();
				ep[n].draw();
			}
		}
	}

	// Display Curve for debugging purposes.
	friend ostream& operator<<(ostream& os, const Curve& c)
	{
		if (c.ok)
		{
			os << "Length: " << c.totlen << endl;
			os << "Distances: ";
			for (vector<double>::const_iterator it = c.dist.begin(); it != c.dist.end(); ++it)
				os << ' ' << *it;
			os << endl;
			os << "Points: \n";
			for (size_t n = 0; n < c.np; ++n)
				os << setw(3) << c.sp[n] << setw(3) << c.mp[n] << setw(3) << c.ep[n] << endl;
			os << "Groups:\n";
			for (vector<Group>::const_iterator it = c.gps.begin(); it != c.gps.end(); ++it)
				os << *it;
		}
		return os;
	}

private:
	bool ok;  // Valid data has been supplied and used.
	size_t np;  // Number of tangent pairs.
	double totlen;  // Total length of the curve.
	vector<Point> sp; // Start points (given)
	vector<Point> ep; // End points (given)
	vector<Point> mp; // Midpoints (computed)
	vector<Group> gps; // Groups of 4 linked points
	vector<double> dist; // Normalize distance to start of group
};