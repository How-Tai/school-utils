#include "paths.h"

#include "frontend.h"

#include "M2/Math/quadratic-solver/quadraticSolver.h"

#include "M2/Science/planet-weight/planetWeightCalculator.h"
#include "M2/Science/sig-fig-counting/countingSigFig.h"

#include "dschool/attendance/attendanceMarker.h"


void initServer(crow::SimpleApp& app) {

    registerFrontend(app);

    
    registerMathQuadraticSolver(app);


    registerSciPlanetWeightCalculator(app);
    registerSciSigFigCounting(app);


    registerDschoolAttendanceMarker(app);
}
