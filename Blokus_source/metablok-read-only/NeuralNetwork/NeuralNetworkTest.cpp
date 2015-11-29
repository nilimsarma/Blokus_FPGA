
#include <iostream>

#include "NetworkTrainer.h"

int main() {
    NetworkTrainer trainer;
    VerifyResult result;
    
    std::cout << "Starting new network with default values" << std::endl;
    std::cout << "Training with file test.sav" << std::endl;
    trainer.train("test.sav");
    std::cout << "Verifying results with file test.sav" << std::endl;
    result = trainer.verify("test.sav");
    std::cout << "Output neurons: " 
        << result.networkOutput1 << " " << result.networkOutput2 << std::endl;
    std::cout << "Desired score: " << result.desiredOutput << std::endl;
    std::cout << "Saving result to testnetwork.txt" << std::endl;
    trainer.save("testnetwork.txt");
    
    std::cout << "Creating new network with values from testnetwork.txt" << std::endl;
    try {
        NetworkTrainer trainer2("testnetwork.txt");
        std::cout << "Verifying results with file test.sav" << std::endl;
        result = trainer2.verify("test.sav");
        std::cout << "Output neurons: " 
            << result.networkOutput1 << " " << result.networkOutput2 << std::endl;
        std::cout << "Desired score: " << result.desiredOutput << std::endl;
    } catch(...) {
        std::cerr << "Error occurred while loading testnetwork.txt" << std::endl;
    }

    return 0;
}
