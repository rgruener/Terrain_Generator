// terrain.cpp
// Robert Gruener
// 4/28/13

#include "terrain.h"
#include <iostream>

#define RAND_NUM(num_range) (-num_range + ((float)rand()/((float)RAND_MAX))*(2*num_range))
#define NORMALIZE(point,factor) ((float)(point)/(float)(factor*1.3f))

Terrain::Terrain(int method /*=TRIANGLES */, int terrain_order /*= 8 */, float roughness_constant /* = 0.7f */, 
                    float range /* = 1.0f */, float init_height /* = 0f */){
    this->side_size = pow(2,terrain_order) + 1;
    this->roughness_constant = roughness_constant;
    this->range = range;
    this->method = method;
    this->points.resize(pow(this->side_size,2)*4);
    this->colors.resize(pow(this->side_size,2)*4);
    this->normals.resize(pow(this->side_size,2)*4);
    this->terrain.resize(this->side_size);
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
    cout << "New range: " << cur_range << endl;
    float rand_f;
    while (stride > 0){

        // Perform the diamond step
        for (i=0; i < this->side_size-1; i+=stride*2){
            for (j=0; j < this->side_size-1; j+=stride*2){
                rand_f = RAND_NUM(cur_range);
                //cout << "Compute Diamond Step, set " << i+stride << ", " << j+stride << ", " << rand_f << endl;
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
                //cout << "Compute Square Step, set " << i << ", " << j << ", " << rand_f << endl;
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
            points[k] = point4(NORMALIZE(i-center_factor,center_factor),
                                NORMALIZE(j-center_factor,center_factor),terrain[i][j],1.0);
            colors[k++] = color4(1.0, 1.0, 1.0, 1.0); // White
            points[k] = point4(NORMALIZE(i-center_factor,center_factor),
                                NORMALIZE(j+1-center_factor,center_factor),terrain[i][j+1],1.0);
            colors[k++] = color4(1.0, 1.0, 1.0, 1.0); // White
            // X Line
            if (i < this->side_size-1){
                points[k] = point4(NORMALIZE(i-center_factor,center_factor),
                                    NORMALIZE(j-center_factor,center_factor),terrain[i][j],1.0);
                colors[k++] = color4(1.0, 1.0, 1.0, 1.0); // White
                points[k] = point4(NORMALIZE(i+1-center_factor,center_factor),
                                    NORMALIZE(j-center_factor,center_factor),terrain[i+1][j],1.0);
                colors[k++] = color4(1.0, 1.0, 1.0, 1.0); // White
            }
        }
        // Final X Line
        if (i < this->side_size-1){
            points[k] = point4(NORMALIZE(i-center_factor,center_factor),
                                NORMALIZE(j-center_factor,center_factor),terrain[i][j],1.0);
            colors[k++] = color4(1.0, 1.0, 1.0, 1.0); // White
            points[k] = point4(NORMALIZE(i+1-center_factor,center_factor),
                                NORMALIZE(j-center_factor,center_factor),terrain[i+1][j],1.0);
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
            point4 p1 = point4(NORMALIZE(i-center_factor,center_factor),
                                NORMALIZE(j-center_factor,center_factor),
                                terrain[i][j],1.0);
            point4 p2 = point4(NORMALIZE(i-center_factor,center_factor),
                                NORMALIZE(j+1-center_factor,center_factor),
                                terrain[i][j+1],1.0);
            point4 p3 = point4(NORMALIZE(i+1-center_factor,center_factor),
                                NORMALIZE(j+1-center_factor,center_factor),
                                terrain[i+1][j+1],1.0);
            point4 p4 = point4(NORMALIZE(i+1-center_factor,center_factor),
                                NORMALIZE(j-center_factor,center_factor),
                                terrain[i+1][j],1.0);
            color4 c = color4(.5f, .35f, .05f, 1.0); // Brown

            addTriangle(p1, p2, p3, c);
            addTriangle(p1, p3, p4, c);
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

void Terrain::addTriangle(point4 p1, point4 p2, point4 p3, color4 color){
    vec4 u = p2 - p1;
    vec4 v = p3 - p1;
    vec3 normal = normalize( cross(u, v) );

    points[this->num_points] = p1;
    normals[this->num_points] = normal;
    colors[this->num_points++] = color;
    points[this->num_points] = p2;
    normals[this->num_points] = normal;
    colors[this->num_points++] = color;
    points[this->num_points] = p3;
    normals[this->num_points] = normal;
    colors[this->num_points++] = color;
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
