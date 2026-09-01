#include "quadraticSolver.h"

constexpr double EPS = 1e-9;

bool isZero(double x) {
    return std::abs(x) < EPS;
}

bool isInteger(double x) {
    return std::abs(x - std::round(x)) < EPS;
}

std::string formatNumber(double x) {
    if (isZero(x)) x = 0;

    if (isInteger(x)) return std::to_string(static_cast<long long>(std::llround(x)));

    std::ostringstream out;
    out << std::fixed << std::setprecision(6) << x;

    std::string s = out.str();
    while (!s.empty() && s.back() == '0') s.pop_back();
    if (!s.empty() && s.back() == '.') s.pop_back();

    return s;
}

std::string latexXTerm(double coeff) {
    if (isZero(coeff - 1)) return "x";
    if (isZero(coeff + 1)) return "-x";
    return formatNumber(coeff) + "x";
}

std::string latexIntXTerm(long long coeff) {
    if (coeff == 0) return "0";
    if (coeff == 1) return "x";
    if (coeff == -1) return "-x";
    return std::to_string(coeff) + "x";
}

std::string latexX2Term(long long coeff) {
    if (coeff == 0) return "0";
    if (coeff == 1) return "x^2";
    if (coeff == -1) return "-x^2";
    return std::to_string(coeff) + "x^2";
}

std::string latexPoly(double a, double b, double c) {
    std::string res;

    if (isZero(a - 1)) res += "x^2";
    else if (isZero(a + 1)) res += "-x^2";
    else res += formatNumber(a) + "x^2";

    if (!isZero(b)) {
        if (b > 0) res += " + ";
        else res += " - ";

        double absB = std::abs(b);
        if (!isZero(absB - 1)) res += formatNumber(absB);
        res += "x";
    }

    if (!isZero(c)) {
        if (c > 0) res += " + " + formatNumber(c);
        else res += " - " + formatNumber(-c);
    }

    return res;
}

std::string latexEquation(double a, double b, double c) {
    return latexPoly(a, b, c) + " = 0";
}

std::string latexExpressionWithBX(double b) {
    std::string res = "x^2";

    if (!isZero(b)) {
        if (b > 0) res += " + ";
        else res += " - ";

        double absB = std::abs(b);
        if (!isZero(absB - 1)) res += formatNumber(absB);
        res += "x";
    }

    return res;
}

std::string latexInsideSquare(double halfB) {
    if (halfB > 0) return "x + " + formatNumber(halfB);
    if (halfB < 0) return "x - " + formatNumber(-halfB);
    return "x";
}

std::string latexLinearFactor(long long xCoeff, long long constant) {
    std::string res = "\\left(";

    if (xCoeff == 1) res += "x";
    else if (xCoeff == -1) res += "-x";
    else res += std::to_string(xCoeff) + "x";

    if (constant > 0) res += " + " + std::to_string(constant);
    else if (constant < 0) res += " - " + std::to_string(-constant);

    res += "\\right)";
    return res;
}

void addStep(crow::json::wvalue& result, size_t index, const std::string& title, const std::string& latex) {
    result["steps"][index]["title"] = title;
    result["steps"][index]["latex"] = latex;
}

crow::response jsonResponse(int code, crow::json::wvalue& body) {
    crow::response res;
    res.code = code;
    res.set_header("Content-Type", "application/json");
    res.body = body.dump();
    return res;
}

crow::response errorResponse(int code, const std::string& message) {
    crow::json::wvalue body;
    body["message"] = message;
    return jsonResponse(code, body);
}

std::pair<double, double> solveQuadratic(double a, double b, double c) {
    double discriminant = b * b - 4 * a * c;
    double root = std::sqrt(discriminant);

    double root1 = (-b - root) / (2 * a);
    double root2 = (-b + root) / (2 * a);

    return {root1, root2};
}

struct Factorization {
    long long p = 0;
    long long q = 0;
    long long r = 0;
    long long s = 0;
    bool found = false;
};

std::vector<long long> divisors(long long x) {
    std::vector<long long> result;

    if (x == 0) {
        result.push_back(0);
        return result;
    }

    long long n = std::llabs(x);

    for (long long i = 1; i * i <= n; ++i) {
        if (n % i != 0) continue;

        result.push_back(i);
        result.push_back(-i);

        if (i * i != n) {
            result.push_back(n / i);
            result.push_back(-(n / i));
        }
    }

    return result;
}

Factorization factorByCross(long long a, long long b, long long c) {
    if (c == 0) return {1, 0, a, b, true};

    std::vector<long long> aFactors = divisors(a);
    std::vector<long long> cFactors = divisors(c);

    for (long long p : aFactors) {
        if (p == 0 || a % p != 0) continue;

        long long r = a / p;

        for (long long q : cFactors) {
            if (q == 0 || c % q != 0) continue;

            long long s = c / q;

            if (p * s + q * r == b) return {p, q, r, s, true};
        }
    }

    return {};
}

void fillCrossVisual(crow::json::wvalue& result, long long p, long long q, long long r, long long s) {
    long long topCross = q * r;
    long long bottomCross = p * s;

    result["crossVisual"]["enabled"] = true;

    result["crossVisual"]["topLeft"] = latexIntXTerm(p);
    result["crossVisual"]["topRight"] = std::to_string(q);
    result["crossVisual"]["topCross"] = latexIntXTerm(topCross);

    result["crossVisual"]["bottomLeft"] = latexIntXTerm(r);
    result["crossVisual"]["bottomRight"] = std::to_string(s);
    result["crossVisual"]["bottomCross"] = latexIntXTerm(bottomCross);

    result["crossVisual"]["productA"] = latexX2Term(p * r);
    result["crossVisual"]["productC"] = std::to_string(q * s);
    result["crossVisual"]["middle"] = latexIntXTerm(topCross + bottomCross);
}

void registerMathQuadraticSolver(crow::SimpleApp& app) {
    CROW_ROUTE(app, "/M2/Math/quadratic-solver")
    .methods(crow::HTTPMethod::POST)
    ([](const crow::request& req) {
        try {
            auto payload = crow::json::load(req.body);
            if (!payload) return errorResponse(400, "Invalid JSON");

            std::string method = payload["method"].s();

            double a = payload["a"].d();
            double b = payload["b"].d();
            double c = payload["c"].d();

            if (isZero(a)) {
                return errorResponse(400, "a cannot be 0 because this is not a quadratic equation");
            }

            double discriminant = b * b - 4 * a * c;

            crow::json::wvalue result;
            result["method"] = method;
            result["discriminant"] = discriminant;
            result["steps"] = crow::json::wvalue::list();
            result["crossVisual"]["enabled"] = false;

            if (method == "1") result["methodName"] = "Quadratic Formula";
            else if (method == "2") result["methodName"] = "Completing the Square";
            else if (method == "3") result["methodName"] = "Cross Method";
            else return errorResponse(400, "Unknown method");

            if (discriminant < -EPS) {
                result["hasRealRoots"] = false;

                addStep(result, 0, "Start", latexEquation(a, b, c));
                addStep(result, 1, "Calculate the discriminant", "b^2 - 4ac = " + formatNumber(discriminant));
                addStep(result, 2, "Conclusion", "\\text{Since } b^2 - 4ac < 0,\\text{ there are no real roots.}");

                return jsonResponse(200, result);
            }

            if (isZero(discriminant)) discriminant = 0;

            result["discriminant"] = discriminant;
            result["hasRealRoots"] = true;

            // -------------------------------------------------
            // Method 1: Quadratic Formula
            // -------------------------------------------------

            if (method == "1") {
                auto roots = solveQuadratic(a, b, c);

                double squareRoot = std::sqrt(discriminant);
                double denominator = 2 * a;
                double numeratorPlus = -b + squareRoot;
                double numeratorMinus = -b - squareRoot;

                result["root1"] = roots.first;
                result["root2"] = roots.second;

                size_t step = 0;

                addStep(result, step++, "Start", latexEquation(a, b, c));

                addStep(result, step++, "Identify the coefficients",
                    "a = " + formatNumber(a) + ",\\quad b = " + formatNumber(b) + ",\\quad c = " + formatNumber(c));

                addStep(result, step++, "Use the quadratic formula",
                    "x = \\frac{-b \\pm \\sqrt{b^2 - 4ac}}{2a}");

                addStep(result, step++, "Calculate the discriminant",
                    "b^2 - 4ac = " + formatNumber(discriminant));

                addStep(result, step++, "Substitute",
                    "x = \\frac{" + formatNumber(-b) + " \\pm \\sqrt{" + formatNumber(discriminant) + "}}{" + formatNumber(denominator) + "}");

                addStep(result, step++, "Evaluate the square root",
                    "x = \\frac{" + formatNumber(-b) + " \\pm " + formatNumber(squareRoot) + "}{" + formatNumber(denominator) + "}");

                if (isZero(discriminant)) {
                    addStep(result, step++, "Simplify the numerator",
                        "x = \\frac{" + formatNumber(-b) + "}{" + formatNumber(denominator) + "}");

                    addStep(result, step++, "Solve",
                        "x = " + formatNumber(roots.first));
                }
                else {
                    addStep(result, step++, "Split the ±",
                        "x = \\frac{" + formatNumber(-b) + " + " + formatNumber(squareRoot) + "}{" + formatNumber(denominator) + "}"
                        " \\quad \\text{or} \\quad "
                        "x = \\frac{" + formatNumber(-b) + " - " + formatNumber(squareRoot) + "}{" + formatNumber(denominator) + "}");

                    addStep(result, step++, "Simplify the numerators",
                        "x = \\frac{" + formatNumber(numeratorPlus) + "}{" + formatNumber(denominator) + "}"
                        " \\quad \\text{or} \\quad "
                        "x = \\frac{" + formatNumber(numeratorMinus) + "}{" + formatNumber(denominator) + "}");

                    addStep(result, step++, "Solve",
                        "x = " + formatNumber(roots.second) +
                        " \\quad \\text{or} \\quad x = " + formatNumber(roots.first));
                }

                return jsonResponse(200, result);
            }

            // -------------------------------------------------
            // Method 2: Completing the Square
            // -------------------------------------------------

            if (method == "2") {
                double normalizedB = b / a;
                double normalizedC = c / a;
                double halfB = normalizedB / 2;
                double square = halfB * halfB;
                double rhs = -normalizedC + square;

                if (rhs < 0 && isZero(rhs)) rhs = 0;

                double rootPart = std::sqrt(rhs);

                double root1 = -halfB - rootPart;
                double root2 = -halfB + rootPart;

                result["root1"] = root1;
                result["root2"] = root2;

                size_t step = 0;

                addStep(result, step++, "Start", latexEquation(a, b, c));

                if (!isZero(a - 1)) {
                    addStep(result, step++, "Divide everything by a",
                        latexEquation(1, normalizedB, normalizedC));
                }

                addStep(result, step++, "Move the constant",
                    latexExpressionWithBX(normalizedB) + " = " + formatNumber(-normalizedC));

                addStep(result, step++, "Take half of the x-coefficient",
                    "\\frac{" + formatNumber(normalizedB) + "}{2} = " + formatNumber(halfB));

                addStep(result, step++, "Square that value",
                    "\\left(" + formatNumber(halfB) + "\\right)^2 = " + formatNumber(square));

                addStep(result, step++, "Add it to both sides",
                    latexExpressionWithBX(normalizedB) + " + " + formatNumber(square) + " = " + formatNumber(rhs));

                std::string inside = latexInsideSquare(halfB);

                addStep(result, step++, "Factor the perfect square",
                    "\\left(" + inside + "\\right)^2 = \\left(" + formatNumber(rootPart) + "\\right)^2");

                addStep(result, step++, "Take the square root of both sides",
                    inside + " = \\pm " + formatNumber(rootPart));

                if (isZero(discriminant)) {
                    addStep(result, step++, "Move the constant",
                        "x = " + formatNumber(-halfB));

                    addStep(result, step++, "Solve",
                        "x = " + formatNumber(root1));
                }
                else {
                    addStep(result, step++, "Split the ±",
                        "x = " + formatNumber(-halfB) + " + " + formatNumber(rootPart) +
                        " \\quad \\text{or} \\quad "
                        "x = " + formatNumber(-halfB) + " - " + formatNumber(rootPart));

                    addStep(result, step++, "Solve",
                        "x = " + formatNumber(root2) +
                        " \\quad \\text{or} \\quad x = " + formatNumber(root1));
                }

                return jsonResponse(200, result);
            }

            // -------------------------------------------------
            // Method 3: Cross Method
            // -------------------------------------------------

            if (method == "3") {
                addStep(result, 0, "Start", latexEquation(a, b, c));

                if (!isInteger(a) || !isInteger(b) || !isInteger(c)) {
                    result["success"] = false;
                    result["message"] = "Cross method requires integer coefficients.";
                    return jsonResponse(422, result);
                }

                long long A = std::llround(a);
                long long B = std::llround(b);
                long long C = std::llround(c);

                Factorization factor = factorByCross(A, B, C);

                if (!factor.found) {
                    result["success"] = false;
                    result["message"] = "This quadratic cannot be factored cleanly using the cross method.";

                    addStep(result, 1, "Try integer factors",
                        "\\text{No integer factorization gives the middle term } " + std::to_string(B) + "x.");

                    return jsonResponse(422, result);
                }

                result["success"] = true;

                fillCrossVisual(result, factor.p, factor.q, factor.r, factor.s);

                long long topCross = factor.q * factor.r;
                long long bottomCross = factor.p * factor.s;

                std::string factor1 = latexLinearFactor(factor.p, factor.q);
                std::string factor2 = latexLinearFactor(factor.r, factor.s);

                addStep(result, 1, "Choose factors of the first and last terms",
                    latexX2Term(A) + " = "
                    + latexIntXTerm(factor.p) + "\\left(" + latexIntXTerm(factor.r) + "\\right)"
                    + ",\\quad "
                    + std::to_string(C) + " = "
                    + std::to_string(factor.q) + "\\left(" + std::to_string(factor.s) + "\\right)");

                addStep(result, 2, "Multiply diagonally",
                    latexIntXTerm(factor.r) + "\\left(" + std::to_string(factor.q) + "\\right) = "
                    + latexIntXTerm(topCross)
                    + ",\\quad "
                    + latexIntXTerm(factor.p) + "\\left(" + std::to_string(factor.s) + "\\right) = "
                    + latexIntXTerm(bottomCross));

                addStep(result, 3, "Add the cross products",
                    latexIntXTerm(topCross) + " + " + latexIntXTerm(bottomCross)
                    + " = " + latexIntXTerm(B));

                addStep(result, 4, "Factor the quadratic",
                    factor1 + factor2 + " = 0");

                addStep(result, 5, "Set each factor equal to zero",
                    factor1 + " = 0 \\quad \\text{or} \\quad " + factor2 + " = 0");

                double root1 = -static_cast<double>(factor.q) / factor.p;
                double root2 = -static_cast<double>(factor.s) / factor.r;

                result["root1"] = root1;
                result["root2"] = root2;

                if (isZero(root1 - root2)) {
                    addStep(result, 6, "Solve",
                        "x = " + formatNumber(root1));
                }
                else {
                    addStep(result, 6, "Solve",
                        "x = " + formatNumber(root1)
                        + " \\quad \\text{or} \\quad x = " + formatNumber(root2));
                }

                return jsonResponse(200, result);
            }

            return errorResponse(400, "Unknown method");
        }
        catch (const std::exception&) {
            return errorResponse(400, "Invalid input");
        }
    });
}