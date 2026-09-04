#include "paths.h"

#include "frontend.h"
#include "account/account.h"
#include "community/community.h"
#include "M2/Math/quadratic-solver/quadraticSolver.h"
#include "M2/Science/planet-weight/planetWeightCalculator.h"
#include "M2/Science/sig-fig-counting/countingSigFig.h"
#include "dschool/attendance/attendanceMarker.h"

void initServer(crow::SimpleApp& app) {
	registerFrontend(app);
	registerAccount(app);
	registerCommunity(app);
	registerMathQuadraticSolver(app);
	registerSciPlanetWeightCalculator(app);
	registerSciSigFigCounting(app);
	registerDschoolAttendanceMarker(app);
}
