#include "BVHValidation.h"
#include "BVHCameraBenchmark.h"

int main(int argc, char* argv[])
{
    // Step 1: Validate BVH correctness.
    runBVHTest();

    if (argc < 2) {
        std::cerr << "\nPlease provide an OBJ file path.\n";
        return 1;
    }

    try {

        // Step 2: Run the three-camera benchmark.
        runBVHCameraBenchmark(argv[1]);

    }
    catch (const std::exception& exception) {

        std::cerr << "\nError: "
                  << exception.what()
                  << '\n';

        return 1;
    }

    return 0;
}