#include <clasp/clasp_facade.h>
#include <clasp/solver.h>

#include <iostream>
#include <fstream>

int main() {
	while (true) {
		Clasp::ClaspConfig config;
		config.solve.numModels = 0;
		config.solve.setSolvers(64);
		Clasp::ClaspFacade libclasp;

		std::ifstream input("queens.aspif");
		libclasp.start(config, input);

		size_t num_models[] = { 1, 0, 0, 2, 10, 4, 40, 92, 352, 724 };
		for (size_t i = 0; libclasp.read(); ++i) {
			libclasp.prepare();
			libclasp.solve();
			size_t n = libclasp.summary().numEnum;
			if (num_models[i] != n) {
				std::cerr
					<< "error expected " << num_models[i]
					<< " models in call " << (i + 1)
					<< " but got " << n << std::endl;
			}
			assert(num_models[i] == n);
		}
	}
}
