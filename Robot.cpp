#include <iostream>
#include <cstdio>
#include <cmath>
#include "global.h"
#include "WorldState.h"
#include "Robot.h"
#include "Sensor.h"
#include "Marker.h"

Robot::Robot(float a, float b, float c){

    //ctor
    x = a;
    y = b;
    theta = c;
    generate_noise_matrix(q_matrix_low);
    generate_q_matrix();
}

void Robot::generate_q_matrix(){

    array <array <float, dim>, 3> temp2{{}};
    for (int i = 0; i < 3; i++){

        for (int j = 0; j < dim; j++){

            for (int k = 0; k < 3; k++){

                temp2[i][j] += q_matrix_low[i][k] * f[k][j];

            }
        }
    }

    /*for (int i = 0; i < 3; i++){

        for (int j = 0; j < dim; j++){
                cout << temp2[i][j] << " ";
        }
        cout << endl;
    }*/

    for (int i = 0; i < dim; i++){

        for (int j = 0; j < dim; j++){

            for (int k = 0; k < 3; k++){

                q_matrix[i][j] += f_transpose[i][k] * temp2[k][j];

            }
        }
    }

}

array <float, 3> Robot::get_velocities(){

    // Comment out the next three lines when velocities from actual hardware is available.
    v_tx = 1.0;
    v_ty = 0.0;
    omega_t = 0.0;

    ret_matrix = {v_tx, v_ty, omega_t};
    return ret_matrix;

}

array <float, 3> Robot::get_pose(){

    ret_matrix = {x, y, theta};
    return ret_matrix;

}

void Robot::motion_model(float dt, WorldState& segway_world){

        // Change the angular velocity equation depending on units
        x = x + (((cos(theta * PI / 180) * v_tx) - (sin(theta * PI / 180) * v_ty)) * dt);
        y = y + (((sin(theta * PI / 180) * v_tx) + (cos(theta * PI / 180) * v_ty)) * dt);
        theta = theta + (omega_t * 180 / PI) * dt;

        segway_world.predict_wstate(x, y, theta);
        jacobian_motion(dt, segway_world);
}

void Robot::jacobian_motion(float dt, WorldState& segway_world){

    jacobian_matrix_low[0] = {1.0, 0.0, (((-sin(theta * PI / 180) * v_tx) - (cos(theta * PI / 180) * v_ty)) * dt)};
    jacobian_matrix_low[1] = {0.0, 1.0, (((cos(theta * PI / 180) * v_tx) - (sin(theta * PI / 180) * v_ty)) * dt)};
    jacobian_matrix_low[2] = {0.0, 0.0, 1.0};
    // Reached till Eq. 5.

    // The construction of jacobian high. Make necessary changes if jacobian_matrix_low is eliminated.

    jacobian[0][2] = jacobian_matrix_low[0][2];
    jacobian[1][2] = jacobian_matrix_low[1][2];

    for (int i = 0; i < dim; i++){

        jacobian[i][i] = 1;

    }

    transpose(jacobian, jacobian_transposed);
    //cout << "Jacobian" << endl;
    //print_matrix1(jacobian);

    //cout << "Transposed Jacobian" << endl;
    //print_matrix1(jacobian_transposed);

    //cout << "Q matrix" << endl;
    //print_matrix2(q_matrix);


    segway_world.predict_cmatrix(jacobian, jacobian_transposed, q_matrix);

}
