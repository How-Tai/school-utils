#include "paths.h"

#include "frontend.h"
#include "M2/Science/planet-weight/planetWeightCalculator.h"
#include "M2/Math/quadratic-solver/quadraticSolver.h"

void initServer(crow::SimpleApp& app) {
    registerSciPlanetWeightCalculator(app);
    registerFrontend(app);
    registerMathQuadraticSolver(app);
}
