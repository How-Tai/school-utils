#include "countingSigFig.h"

void registerSciSigFigCounting(crow::SimpleApp& app) {
	CROW_ROUTE(app, "/M2/Science/sigFigCounter")
	.methods(crow::HTTPMethod::POST)
	([](const crow::request& req){
		auto payload = crow::json::load(req.body);

		if(!payload) {
			return crow::response(400, "Invalid data");
		}

		std::string data = payload["input"].s();

		if(data.empty()) {
			return crow::response(400, "Invalid data");
		}

		int ePos = -1;
		int eCnt = 0;

		for(int i = 0; i < (int)data.size(); ++i) {
			if(data[i] == 'e' || data[i] == 'E') {
				++eCnt;
				ePos = i;
			}
		}

		if(eCnt > 1) {
			return crow::response(400, "Invalid data");
		}

		std::string coefficient = data;
		std::string exponent;

		if(ePos != -1) {
			coefficient = data.substr(0, ePos);
			exponent = data.substr(ePos + 1);

			if(coefficient.empty() || exponent.empty()) {
				return crow::response(400, "Invalid data");
			}

			int start = 0;

			if(exponent[0] == '+' || exponent[0] == '-') {
				start = 1;
			}

			if(start == (int)exponent.size()) {
				return crow::response(400, "Invalid data");
			}

			for(int i = start; i < (int)exponent.size(); ++i) {
				if(!isdigit(static_cast<unsigned char>(exponent[i]))) {
					return crow::response(400, "Invalid data");
				}
			}
		}

		int coeffStart = 0;

		if(coefficient[0] == '+' || coefficient[0] == '-') {
			coeffStart = 1;
		}

		if(coeffStart == (int)coefficient.size()) {
			return crow::response(400, "Invalid data");
		}

		int dotCnt = 0;

		for(int i = coeffStart; i < (int)coefficient.size(); ++i) {
			if(coefficient[i] == '.') {
				++dotCnt;
			}
			else if(!isdigit(static_cast<unsigned char>(coefficient[i]))) {
				return crow::response(400, "Invalid data");
			}
		}

		if(dotCnt > 1) {
			return crow::response(400, "Invalid data");
		}

		if(coefficient.substr(coeffStart) == ".") {
			return crow::response(400, "Invalid data");
		}

		bool hasDecimal = coefficient.find('.', coeffStart) != std::string::npos;

		int firstNonZero = coeffStart;

		while(firstNonZero < (int)coefficient.size() &&
			  (coefficient[firstNonZero] == '0' || coefficient[firstNonZero] == '.')) {
			++firstNonZero;
		}

		int sigFig = 0;

		if(firstNonZero == (int)coefficient.size()) {
			if(hasDecimal) {
				int decimalPos = coefficient.find('.', coeffStart);

				for(int i = decimalPos + 1; i < (int)coefficient.size(); ++i) {
					if(coefficient[i] == '0') ++sigFig;
				}
			}
		}
		else {
			int lastSigFig = coefficient.size() - 1;

			if(!hasDecimal) {
				while(lastSigFig >= firstNonZero && coefficient[lastSigFig] == '0') {
					--lastSigFig;
				}
			}

			for(int i = firstNonZero; i <= lastSigFig; ++i) {
				if(isdigit(static_cast<unsigned char>(coefficient[i]))) {
					++sigFig;
				}
			}
		}

		crow::json::wvalue result;
		result["count"] = sigFig;

		return crow::response(result);
	});
}