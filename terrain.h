// terrain.h
// Robert Gruener
// 4/28/13

#ifndef _TERRAIN_H
#define _TERRAIN_H

#include <iostream>
#include <list>
#include <vector>
#include <ctime>
#include <cstdlib>
#include "Angel.h"

using namespace std;

typedef Angel::vec4 color4;
typedef Angel::vec4 point4;

enum { TRIANGLES = 0, LINES = 1};

class Terrain {

    private:
        vector<point4> points;
        vector<color4> colors;
        vector<vec3> normals;

        vector<vector <float>  > terrain;
        int side_size;
        float roughness_constant;
        float range;
        int num_points;
        int method;
        
        void generateHeightMap();
        float avgSquareHeight(int i, int j, int size);
        float avgDiamondHeight(int i, int j, int stride);
        void storePointsLines();
        void storePointsTriangles();
        void addTriangle(point4 p1, point4 p2, point4 p3, color4 high, color4 mid, color4 low, color4 brown);

    public:
        Terrain(int method = TRIANGLES, int terrain_order = 8, float roughness_constant = 0.7f, 
                    float range = 1.0f, float init_height = 0.0f);

        point4 *getPoints();
        color4 *getColors();
        vec3 *getNormals();
        int getNumPoints();
        void dumpHeightMap();
};

#endif
