// terrain.cpp
// Robert Gruener
// 4/28/13

#include "terrain.h"
#include <iostream>

#define RAND_NUM(num_range) (-num_range + ((float)rand()/((float)RAND_MAX))*(2*num_range))
#define NORMALIZE(point,factor) (float)(point)/(float)(factor*1.3f)

Terrain::Terrain(int terrain_order /*= 8 */, float roughness_constant /* = 0.7f */, 
                    float range /* = 1.0f */, float init_height /* = 0f */){
    this->side_size = pow(2,terrain_order) + 1;
    this->roughness_constant = roughness_constant;
    this->range = range;
    this->points.resize(pow(this->side_size,2)*4);
    this->colors.resize(pow(this->side_size,2)*4);
    this->terrain.resize(this->side_size);
    for (int i=0; i < side_size; i++){
        terrain[i].resize(side_size,init_height);
    }
    srand(time(NULL));
    cout << "Before generate map" << endl;
    generateHeightMap();
    cout << "After generate map" << endl;
    storePoints();
    cout << "After points stored" << endl;
}


void Terrain::generateHeightMap(){
    int i,j;
    int stride = (this->side_size-1) / 2;
    int high_index_i, high_index_j, midpoint_index_i, midpoint_index_j, odd_line;
    float new_midpoint;
    float cur_range = this->range;
    while (stride > 0){

        // Perform the diamond step
        for (i=0; i < this->side_size-1; i+=stride*2){
            for (j=0; j < this->side_size-1; j+=stride*2){
                terrain[i+stride][j+stride] = avgSquareHeight(i,j,stride*2) + RAND_NUM(cur_range);
            }
        }

        // Perform the square step
        odd_line = 0;
        for (i=0; i < this->side_size-1; i+=stride){
            odd_line = (odd_line % 2);
            for (j=0; j < this->side_size-1; j+=stride){
                if ((odd_line) && !j){
                    j += stride;
                }
                terrain[i][j] = avgDiamondHeight(i,j,stride) + RAND_NUM(cur_range);
            }
        }
        cur_range *= powf(2.0,-this->roughness_constant);
        stride /= 2;
    } 
}

float Terrain::avgSquareHeight(int i, int j, int size){
    int high_index_i = i + size;
    int high_index_j = j + size;
    return (this->terrain[i][j] + this->terrain[i][high_index_j] + 
                this->terrain[high_index_i][j] + this->terrain[high_index_i][high_index_j]) / 4.0f;
    
}

float Terrain::avgDiamondHeight(int i, int j, int stride){
    if (i == 0){
        return (this->terrain[this->side_size-1-stride][j] + this->terrain[i+stride][j] + this->terrain[i][j-stride] + 
                    this->terrain[i][j+stride]) / 4.0f;
    } else if (i = this->side_size-1){
        return (this->terrain[i-stride][j] + this->terrain[stride][j] + this->terrain[i][j-stride] + 
                    this->terrain[i][j+stride]) / 4.0f;
    } else if (j == 0){
        return (this->terrain[i-stride][j] + this->terrain[i+stride][j] + this->terrain[i][this->side_size-1-stride] + 
                    this->terrain[i][j+stride]) / 4.0f;
    } else if (j = this->side_size-1){
        return (this->terrain[i-stride][j] + this->terrain[i+stride][j] + this->terrain[i][j-stride] + 
                    this->terrain[i][stride]) / 4.0f;
    } else {
        return (this->terrain[i-stride][j] + this->terrain[i+stride][j] + this->terrain[i][j-stride] + 
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
            points[k] = point4(NORMALIZE(i-center_factor,center_factor),NORMALIZE(j-center_factor,center_factor),terrain[i][j],1.0);
            colors[k++] = color4(1.0, 1.0, 1.0, 1.0); // White
            points[k] = point4(NORMALIZE(i-center_factor,center_factor),NORMALIZE(j+1-center_factor,center_factor),terrain[i][j+1],1.0);
            colors[k++] = color4(1.0, 1.0, 1.0, 1.0); // White
            // X Line
            if (i < this->side_size-1){
                points[k] = point4(NORMALIZE(i-center_factor,center_factor),NORMALIZE(j-center_factor,center_factor),terrain[i][j],1.0);
                colors[k++] = color4(1.0, 1.0, 1.0, 1.0); // White
                points[k] = point4(NORMALIZE(i+1-center_factor,center_factor),NORMALIZE(j-center_factor,center_factor),terrain[i+1][j],1.0);
                colors[k++] = color4(1.0, 1.0, 1.0, 1.0); // White
            }
        }
        // Final X Line
        if (i < this->side_size-1){
            points[k] = point4(NORMALIZE(i-center_factor,center_factor),NORMALIZE(j-center_factor,center_factor),terrain[i][j],1.0);
            colors[k++] = color4(1.0, 1.0, 1.0, 1.0); // White
            points[k] = point4(NORMALIZE(i+1-center_factor,center_factor),NORMALIZE(j-center_factor,center_factor),terrain[i+1][j],1.0);
            colors[k++] = color4(1.0, 1.0, 1.0, 1.0); // White
        }
    }
    this->num_points = k;
}

void Terrain::storePointsTriangles(){
    int i,j,k=0;
    float init_scale_factor = 0.5;
    int center_factor = (this->side_size-1)/2;
    for (i=0; i<this->side_size; i++){
        for (j=0; j<this->side_size-1; j++){
            // Y Line
            points[k] = point4(NORMALIZE(i-center_factor,center_factor),NORMALIZE(j-center_factor,center_factor),terrain[i][j],1.0);
            colors[k++] = color4(1.0, 1.0, 1.0, 1.0); // White
            points[k] = point4(NORMALIZE(i-center_factor,center_factor),NORMALIZE(j+1-center_factor,center_factor),terrain[i][j+1],1.0);
            colors[k++] = color4(1.0, 1.0, 1.0, 1.0); // White
            // X Line
            if (i < this->side_size-1){
                points[k] = point4(NORMALIZE(i-center_factor,center_factor),NORMALIZE(j-center_factor,center_factor),terrain[i][j],1.0);
                colors[k++] = color4(1.0, 1.0, 1.0, 1.0); // White
                points[k] = point4(NORMALIZE(i+1-center_factor,center_factor),NORMALIZE(j-center_factor,center_factor),terrain[i+1][j],1.0);
                colors[k++] = color4(1.0, 1.0, 1.0, 1.0); // White
            }
        }
        // Final X Line
        if (i < this->side_size-1){
            points[k] = point4(NORMALIZE(i-center_factor,center_factor),NORMALIZE(j-center_factor,center_factor),terrain[i][j],1.0);
            colors[k++] = color4(1.0, 1.0, 1.0, 1.0); // White
            points[k] = point4(NORMALIZE(i+1-center_factor,center_factor),NORMALIZE(j-center_factor,center_factor),terrain[i+1][j],1.0);
            colors[k++] = color4(1.0, 1.0, 1.0, 1.0); // White
        }
    }
    this->num_points = k;
}

void Terrain::dumpHeightMap(){
    int i,j,k=0;
    float init_scale_factor = 0.5;
    int center_factor = (this->side_size-1)/2;
    for (j=0; j<this->side_size; j++){
        for (i=0; i<this->side_size; i++){
            cout << i << ", " << j << ": " << terrain[i][j] << "\t\t";
        }
        cout << endl;
    }
}

point4 *Terrain::getPoints(){
    return &this->points[0];
}

color4 *Terrain::getColors(){
    return &this->colors[0];
}

int Terrain::getNumPoints(){
    return this->num_points;
}
