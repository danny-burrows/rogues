#include "utils.h"

// Given three collinear points p, q, r, the function checks if
// point q lies on line segment 'pr'
static bool onSegment(Point p, Point q, Point r)
{
	if (q.x <= MAX(p.x, r.x) && q.x >= MIN(p.x, r.x) &&
		q.y <= MAX(p.y, r.y) && q.y >= MIN(p.y, r.y))
	return true;

	return false;
}

// To find orientation of ordered triplet (p, q, r).
// The function returns following values
// 0 --> p, q and r are collinear
// 1 --> Clockwise
// 2 --> Counterclockwise
static int orientation(Point p, Point q, Point r)
{
	// See https://www.geeksforgeeks.org/orientation-3-ordered-points/
	// for details of below formula.
	int val = (q.y - p.y) * (r.x - q.x) -
			(q.x - p.x) * (r.y - q.y);

	if (val == 0) return 0; // collinear

	return (val > 0)? 1: 2; // clock or counterclock wise
}

// Returns true if line segment 'p1q1' and 'p2q2' intersect.
bool line_segments_intersect(Point p1, Point q1, Point p2, Point q2)
{
	// Find the four orientations needed for general and
	// special cases
	int o1 = orientation(p1, q1, p2);
	int o2 = orientation(p1, q1, q2);
	int o3 = orientation(p2, q2, p1);
	int o4 = orientation(p2, q2, q1);

	// General case
	if (o1 != o2 && o3 != o4)
		return true;

	// Special Cases
	// p1, q1 and p2 are collinear and p2 lies on segment p1q1
	if (o1 == 0 && onSegment(p1, p2, q1)) return true;

	// p1, q1 and q2 are collinear and q2 lies on segment p1q1
	if (o2 == 0 && onSegment(p1, q2, q1)) return true;

	// p2, q2 and p1 are collinear and p1 lies on segment p2q2
	if (o3 == 0 && onSegment(p2, p1, q2)) return true;

	// p2, q2 and q1 are collinear and q1 lies on segment p2q2
	if (o4 == 0 && onSegment(p2, q1, q2)) return true;

	return false; // Doesn't fall in any of the above cases
}

// Returns true if point 'p' is on line segment 's1s2'.
bool point_is_on_line_segment(Point p, Point s1, Point s2)
{
    int dxc = p.x - s1.x;
    int dyc = p.y - s1.y;

    int dxl = s2.x - s1.x;
    int dyl = s2.y - s1.y;

    int cross = dxc * dyl - dyc * dxl;

    double threshold = 2.0; 
    if (abs((double)cross) > threshold) 
        return false;

    // For an exact match to the line
    // if (cross != 0)
    //     return false;

    if (abs(dxl) >= abs(dyl))
    return dxl > 0 ? 
        s1.x <= p.x && p.x <= s2.x :
        s2.x <= p.x && p.x <= s1.x;
    else
    return dyl > 0 ? 
        s1.y <= p.y && p.y <= s2.y :
        s2.y <= p.y && p.y <= s1.y;
}
