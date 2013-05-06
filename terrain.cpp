// terrain.cpp
// Robert Gruener
// 4/28/13

#include "terrain.h"
#include <iostream>
#include <cmath>

#define RAND_NUM(num_range) (-num_range + ((float)rand()/((float)RAND_MAX))*(2*num_range))
#define NORMALIZE(point,factor) ((float)(point)/(float)(factor*1.3f))
#define POSITIVE(num) (num = num < 0.0 ? num : 0)

Terrain::Terrain(int method /*=TRIANGLES */, int terrain_order /*= 8 */, float roughness_constant /* = 0.7f */, 
                    float range /* = 1.0f */, float init_height /* = 0f */){
    float height_factor = terrain_order;
    this->side_size = pow(2,terrain_order) + 1;
    this->roughness_constant = roughness_constant;
    this->method = method;
    this->range = range * terrain_order * height_factor;
    this->points.resize(pow(this->side_size,2));
    this->colors.resize(pow(this->side_size,2));
    this->normals.resize(pow(this->side_size,2));
    this->terrain.resize(this->side_size);
    this->num_points = 0;
    for (int i=0; i < side_size; i++){
        terrain[i].resize(side_size,init_height);
    }
    srand(time(NULL));
    generateHeightMap();
    switch (this->method){
        case TRIANGLES:
            storePointsTriangles();
            break;
        case LINES:
            storePointsLines();
            break;
    }
}


void Terrain::generateHeightMap(){
    int i,j;
    int stride = (this->side_size-1) / 2;
    int high_index_i, high_index_j, midpoint_index_i, midpoint_index_j, odd_line;
    float new_midpoint;
    float ratio = powf(2.0,-this->roughness_constant);
    float cur_range = this->range;
    float rand_f, new_height;
    while (stride > 0){

        // Perform the diamond step
        for (i=0; i < this->side_size-1; i+=stride*2){
            for (j=0; j < this->side_size-1; j+=stride*2){
                rand_f = RAND_NUM(cur_range);
                terrain[i+stride][j+stride] = avgSquareHeight(i,j,stride*2) + 
                                                rand_f;
            }
        }

        // Perform the square step
        odd_line = 0;
        for (i=0; i < this->side_size-1; i+=stride){
            odd_line = (odd_line == 0);
            for (j=0; j < this->side_size-1; j+=stride){
                if ((odd_line) && !j){
                    j += stride;
                }
                rand_f = RAND_NUM(cur_range);
                terrain[i][j] = avgDiamondHeight(i,j,stride) + rand_f;

                // Enable wrapping by copying over edges
                if (i == 0){
                    terrain[this->side_size-1][j] = terrain[i][j];
                }
                if (j == 0){
                    terrain[i][this->side_size-1] = terrain[i][j];
                }
                j += stride;
            }
        }
        cur_range *= ratio;
        stride /= 2;
    } 
}

float Terrain::avgSquareHeight(int i, int j, int size){
    int high_index_i = i + size;
    int high_index_j = j + size;
    return (this->terrain[i][j] + this->terrain[i][high_index_j] + 
                this->terrain[high_index_i][j] +
                this->terrain[high_index_i][high_index_j]) / 4.0f;
    
}

float Terrain::avgDiamondHeight(int i, int j, int stride){
    if (i == 0){
        return (this->terrain[this->side_size-1-stride][j] + 
                    this->terrain[i+stride][j] + this->terrain[i][j-stride] + 
                    this->terrain[i][j+stride]) / 4.0f;
    } else if (i == this->side_size-1){
        return (this->terrain[i-stride][j] + this->terrain[stride][j] +
                    this->terrain[i][j-stride] + 
                    this->terrain[i][j+stride]) / 4.0f;
    } else if (j == 0){
        return (this->terrain[i-stride][j] + this->terrain[i+stride][j] + 
                    this->terrain[i][this->side_size-1-stride] + 
                    this->terrain[i][j+stride]) / 4.0f;
    } else if (j == this->side_size-1){
        return (this->terrain[i-stride][j] + this->terrain[i+stride][j] + 
                    this->terrain[i][j-stride] + 
                    this->terrain[i][stride]) / 4.0f;
    } else {
        return (this->terrain[i-stride][j] + this->terrain[i+stride][j] + 
                    this->terrain[i][j-stride] + 
                    this->terrain[i][j+stride]) / 4.0f;
    }

}

void Terrain::storePointsLines(){
    int i,j,k=0;
    float init_scale_factor = 0.5;
    int center_factor = (this->side_size-1)/2;
    for (i=0; i<this->side_size; i++){
        for (j=0; j<this->side_size-1; j++){
            // Y Line
            points[k] = point4(i-center_factor,j-center_factor,terrain[i][j],1.0);
            colors[k++] = color4(1.0, 1.0, 1.0, 1.0); // White
            points[k] = point4(i-center_factor,j+1-center_factor,terrain[i][j+1],1.0);
            colors[k++] = color4(1.0, 1.0, 1.0, 1.0); // White
            // X Line
            if (i < this->side_size-1){
                points[k] = point4(i-center_factor,j-center_factor,terrain[i][j],1.0);
                colors[k++] = color4(1.0, 1.0, 1.0, 1.0); // White
                points[k] = point4(i+1-center_factor,j-center_factor,terrain[i+1][j],1.0);
                colors[k++] = color4(1.0, 1.0, 1.0, 1.0); // White
            }
        }
        // Final X Line
        if (i < this->side_size-1){
            points[k] = point4((i-center_factor),j-center_factor,terrain[i][j],1.0);
            colors[k++] = color4(1.0, 1.0, 1.0, 1.0); // White
            points[k] = point4(i+1-center_factor,(j-center_factor),terrain[i+1][j],1.0);
            colors[k++] = color4(1.0, 1.0, 1.0, 1.0); // White
        }
    }
    this->num_points = k;
}

void Terrain::storePointsTriangles(){
    int i,j,k=0;
    float init_scale_factor = 0.5;
    int center_factor = (this->side_size-1)/2;
    for (i=0; i<this->side_size-1; i++){
        for (j=0; j<this->side_size-1; j++){
            point4 p1 = point4(i-center_factor,j-center_factor,terrain[i][j],1.0);
            point4 p2 = point4(i-center_factor,j+1-center_factor,terrain[i][j+1],1.0);
            point4 p3 = point4(i+1-center_factor,j+1-center_factor,terrain[i+1][j+1],1.0);
            point4 p4 = point4(i+1-center_factor,j-center_factor,terrain[i+1][j],1.0);
            color4 high = color4(.95f, .95f, .95f, 1.0); // White
            color4 mid = color4(.05f, .75f, .45f, 1.0); // Green
            color4 low = color4(.05f, .05f, .95f, 1.0); // Blue
            color4 brown = color4(0.5f, 0.35f, 0.05f, 1.0); // Brown

            addTriangle(p1, p2, p3, high, mid, low, brown);
            addTriangle(p1, p3, p4, high, mid, low, brown);
        }
    }
}

void Terrain::dumpHeightMap(){
    int i,j,k=0;
    float init_scale_factor = 0.5;
    int center_factor = (this->side_size-1)/2;
    for (j=0; j<this->side_size; j++){
        for (i=0; i<this->side_size; i++){
            cout << i << ", " << j << ": " << terrain[i][j] << "\t\t\t";
        }
        cout << endl;
    }
}

void Terrain::addTriangle(point4 p1, point4 p2, point4 p3, color4 high, color4 mid, color4 low, color4 brown){

    point4 p1_new = point4(p1.x, p1.y, POSITIVE(p1.z), p1.w);
    point4 p2_new = point4(p2.x, p2.y, POSITIVE(p2.z), p2.w);
    point4 p3_new = point4(p3.x, p3.y, POSITIVE(p3.z), p3.w);
    vec4 u = p2_new - p1_new;
    vec4 v = p3_new - p1_new;
    vec3 normal = -normalize( cross(u, v) );

    if (this->points.size() < this->num_points + 5){
        this->points.resize(2*this->points.size());
        this->normals.resize(2*this->points.size());
        this->colors.resize(2*this->points.size());
    }

    points[this->num_points] = p1_new;
    normals[this->num_points] = normal;
    if (p1.z == 0){
        colors[this->num_points++] = low;
    } else if (p1.z > -0.5*this->range){
        colors[this->num_points++] = mid;
    } else if (p1.z > -0.75*this->range){
        colors[this->num_points++] = brown;
    } else {
        colors[this->num_points++] = high;
    }
    points[this->num_points] = p2_new;
    normals[this->num_points] = normal;
    if (p2.z == 0){
        colors[this->num_points++] = low;
    } else if (p2.z > -0.5*this->range){
        colors[this->num_points++] = mid;
    } else if (p3.z > -0.75*this->range){
        colors[this->num_points++] = brown;
    } else {
        colors[this->num_points++] = high;
    }
    points[this->num_points] = p3_new;
    normals[this->num_points] = normal;
    if (p3.z == 0){
        colors[this->num_points++] = low;
    } else if (p3.z > -0.5*this->range){
        colors[this->num_points++] = mid;
    } else if (p3.z > -0.75*this->range){
        colors[this->num_points++] = brown;
    } else {
        colors[this->num_points++] = high;
    }
}

point4 *Terrain::getPoints(){
    return &this->points[0];
}

color4 *Terrain::getColors(){
    return &this->colors[0];
}

vec3 *Terrain::getNormals(){
    return &this->normals[0];
}

int Terrain::getNumPoints(){
    return this->num_points;
}

int Terrain::getSideSize(){
    return this->side_size;
}
