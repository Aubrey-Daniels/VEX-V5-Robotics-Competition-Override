#include "main.h"
#include "subsystems.h"

#define RED_BALL_SIG_ID 1


// Define a vision signature for the red ball
pros::vision_signature_s_t RED_BALL_SIG = pros::Vision::signature_from_utility(
    RED_BALL_SIG_ID, 
    8929, 11005, 9967,   // U parameter values (uMin, uMax, uMean)
    -1059, -153, -606,   // V parameter values (vMin, vMax, vMean)
    3.000,               // Range coefficient
    0                    // Type
);


void visionSensorObjectDetection();