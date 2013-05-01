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

class Terrain {

    private:
        vector<point4> points;
        vector<color4> colors;

        vector<vector <float>  > terrain;
        int side_size;
        float roughness_constant;
        float range;
        int num_points;
        
        void generateHeightMap();
        float avgSquareHeight(int i, int j, int size);
        float avgDiamondHeight(int i, int j, int stride);
        void storePointsLines();
        void storePointsTriangles();
        void addTriangle();

    public:
        Terrain(int terrain_order = 8, float roughness_constant = 0.7f, 
                    float range = 1.0f, float init_height = 0.0f);

        point4 *getPoints();
        color4 *getColors();
        int getNumPoints();
        void dumpHeightMap();
};

#endif
