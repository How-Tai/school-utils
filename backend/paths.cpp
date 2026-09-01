#include "paths.h"

#include "M2/Science/planetWeightCalculator.h"

void initServer(crow::SimpleApp& app) {
    registerSciPlanetWeightCalculator(app);
    registerFrontend(app);
}
