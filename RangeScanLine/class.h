//#pragma once

#include<vector>
#include<iostream>
using namespace std;

class vertex
{
public:
	float x;
	float y;
	float z;

	vertex() { this->x = 0; this->y = 0; this->z = 0; };
	vertex(float x, float y, float z) { this->x = x; this->y = y; this->z = z; };
	/*~vertex() {};*/

	//color vertex2color() { return color(this->x,this->y,this->z); };

	const vertex operator * (const vertex& v) { return vertex(this->x * v.x, this->y * v.y, this->z * v.z); };
	const vertex operator * (float s) { return vertex(this->x * s, this->y * s, this->z * s); };
	const vertex operator / (const vertex& v) { return vertex(this->x / v.x, this->y / v.y, this->z / v.z); };
	const vertex operator / (float s) { return vertex(this->x / s, this->y / s, this->z / s); };
	const vertex operator + (const vertex& v) { return vertex(this->x + v.x, this->y + v.y, this->z + v.z); };
	const vertex operator + (float s) { return vertex(this->x + s, this->y + s, this->z + s); };
	const vertex operator - (const vertex& v) { return vertex(this->x - v.x, this->y - v.y, this->z - v.z); };
	const vertex operator - (float s) { return vertex(this->x - s, this->y - s, this->z - s); };
	//const vertex operator = (const vertex& v) { return vertex(v.x,v.y, v.z); };
	const bool operator == (const vertex& v) { return (this->x == v.x && this->y == v.y && this->z == v.z); };
	const bool operator != (const vertex& v) { return !(this->x == v.x && this->y == v.y && this->z == v.z); };

	float dot(vertex v2) { return (this->x * v2.x + this->y * v2.y + this->z * v2.z); };
	vertex cross(vertex v2)
	{
		vertex c;
		c.x = this->y * v2.z - this->z * v2.y;
		c.y = this->z * v2.x - this->x * v2.z;
		c.z = this->x * v2.y - this->y * v2.x;
		return c;
	};
	void normalize() {
		float mod = (float)sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
		this->x /= mod;
		this->y /= mod;
		this->z /= mod;
	};
};

struct face
{
	vector<int> idx_v;
};

class color
{
public:

	float r;
	float g;
	float b;
	color() { this->r = 0; this->g = 0; this->b = 0; };
	color(float r, float g, float b) { this->r = r; this->g = g; this->b = b; };
	//~color() {};
	color operator * (color v) { return color(this->r * v.r, this->g * v.g, this->b * v.b); };
	color operator * (float s) { return color(this->r * s, this->g * s, this->b * s); };
	color operator / (color v) { return color(this->r / v.r, this->g / v.g, this->b / v.b); };
	color operator / (float s) { return color(this->r / s, this->g / s, this->b / s); };
	color operator + (color v) { return color(this->r + v.r, this->g + v.g, this->b + v.b); };
	color operator - (color v) { return color(this->r - v.r, this->g - v.g, this->b - v.b); };
	//color operator = (color v) { return color(v.r, v.g, v.b); };
	bool operator == (color v) { return (this->r == v.r && this->g == v.g && this->b == v.b); };
	bool operator != (color v) { return !(this->r == v.r && this->g == v.g && this->b == v.b); };

	float dot(color v2) { return (this->r * v2.r + this->g * v2.g + this->b * v2.b); };
	color cross(color v2)
	{
		color c;
		c.r = this->g * v2.b - this->b * v2.g;
		c.g = this->b * v2.r - this->r * v2.b;
		c.b = this->r * v2.g - this->b * v2.r;
		return c;
	};
	void normalize() {
		float mod = (float)sqrt(this->r * this->r + this->g * this->g + this->b * this->b);
		this->r /= mod;
		this->g /= mod;
		this->b /= mod;
		//return vertex(this->x / mod, this->y / mod, this->z / mod);
	};
};

