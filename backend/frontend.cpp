#include <crow.h>

namespace {
	crow::response staticFile(const std::string& path) {
		crow::response res;
		res.set_static_file_info(path);
		return res;
	}
}

void registerFrontend(crow::SimpleApp& app) {
	CROW_ROUTE(app, "/")([] { return staticFile("frontend/index.html"); });
	CROW_ROUTE(app, "/style.css")([] { return staticFile("frontend/style.css"); });
	CROW_ROUTE(app, "/script.js")([] { return staticFile("frontend/script.js"); });

	CROW_ROUTE(app, "/account")([] { return staticFile("frontend/account/index.html"); });
	CROW_ROUTE(app, "/account/style.css")([] { return staticFile("frontend/account/style.css"); });
	CROW_ROUTE(app, "/account/script.js")([] { return staticFile("frontend/account/script.js"); });

	CROW_ROUTE(app, "/feedback")([] { return staticFile("frontend/feedback/index.html"); });
	CROW_ROUTE(app, "/feedback/style.css")([] { return staticFile("frontend/feedback/style.css"); });
	CROW_ROUTE(app, "/feedback/script.js")([] { return staticFile("frontend/feedback/script.js"); });

	CROW_ROUTE(app, "/admin")([] { return staticFile("frontend/admin/index.html"); });
	CROW_ROUTE(app, "/admin/style.css")([] { return staticFile("frontend/admin/style.css"); });
	CROW_ROUTE(app, "/admin/script.js")([] { return staticFile("frontend/admin/script.js"); });

	CROW_ROUTE(app, "/M2/Math/quadratic-solver")([] { return staticFile("frontend/M2/Math/quadratic-solver/index.html"); });
	CROW_ROUTE(app, "/M2/Math/quadratic-solver/script.js")([] { return staticFile("frontend/M2/Math/quadratic-solver/script.js"); });
	CROW_ROUTE(app, "/M2/Math/quadratic-solver/style.css")([] { return staticFile("frontend/M2/Math/quadratic-solver/style.css"); });

	CROW_ROUTE(app, "/M2/Science/planet-weight")([] { return staticFile("frontend/M2/Science/planet-weight/index.html"); });
	CROW_ROUTE(app, "/M2/Science/planet-weight/script.js")([] { return staticFile("frontend/M2/Science/planet-weight/script.js"); });
	CROW_ROUTE(app, "/M2/Science/planet-weight/style.css")([] { return staticFile("frontend/M2/Science/planet-weight/style.css"); });

	CROW_ROUTE(app, "/M2/Science/sig-fig-counting")([] { return staticFile("frontend/M2/Science/sig-fig-counting/index.html"); });
	CROW_ROUTE(app, "/M2/Science/sig-fig-counting/script.js")([] { return staticFile("frontend/M2/Science/sig-fig-counting/script.js"); });
	CROW_ROUTE(app, "/M2/Science/sig-fig-counting/style.css")([] { return staticFile("frontend/M2/Science/sig-fig-counting/style.css"); });

	CROW_ROUTE(app, "/dschool/attendance")([] { return staticFile("frontend/dschool/attendance/index.html"); });
	CROW_ROUTE(app, "/dschool/attendance/script.js")([] { return staticFile("frontend/dschool/attendance/script.js"); });
	CROW_ROUTE(app, "/dschool/attendance/style.css")([] { return staticFile("frontend/dschool/attendance/style.css"); });
}
