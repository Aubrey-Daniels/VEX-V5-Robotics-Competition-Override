#include "main.h"
#include "visionSensor.h"


void visionSensorObjectDetection() {

int numberOfObjectsDetected = visionSensor.get_object_count();

std::cout << "Number of objects detected: " << numberOfObjectsDetected << std::endl;

 }


 void visionSensorColorDetection() {

    // This uploads signature parameters to Slot 1 on sensor
    visionSensor.set_signature(RED_BALL_SIG_ID, &RED_BALL_SIG);

    // Define a variable that uses vision sensor API for specific color signature (Red Object In This Case) 
    pros::vision_object_s_t redBall = visionSensor.get_by_sig(0, RED_BALL_SIG_ID);


    // Check if the red ball is detected
    if (redBall.signature == RED_BALL_SIG_ID) {             // Also possibly try != VISION_OBJECT_ERR_SIG
        std::cout << "Red ball detected!" << std::endl;
        
        std::cout << "X: " << redBall.x_middle_coord << 
        ", Y: " << redBall.y_middle_coord << std::endl;
    
    } else {
        std::cout << "Red ball not detected." << std::endl;
    }
 }


    

