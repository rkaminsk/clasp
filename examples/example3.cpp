//
// Copyright (c) 2014, Benjamin Kaufmann
//
// This file is part of Clasp. See http://www.cs.uni-potsdam.de/clasp/
//
// Clasp is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// Clasp is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Clasp; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//
// Add the libclasp directory to the list of
// include directoies of your build system.
#include <clasp/clasp_facade.h>
#include <clasp/solver.h>
#include "example.h"
#if CLASP_HAS_THREADS
// This example uses the ClaspFacade to compute
// the stable models of the program
//    a :- not b.
//    b :- not a.
//
// It is similar to example2() but uses the facade's asynchronous
// interface in order to get the models one by one.
void example3() {
	// See example2()
	Clasp::ClaspConfig config;
	config.solve.numModels = 0;

	// The "interface" to the clasp library.
	Clasp::ClaspFacade libclasp;

	// See example2()
	Clasp::Asp::LogicProgram& asp = libclasp.startAsp(config);
	addSimpleProgram(asp);

	// We are done with problem setup.
	// Prepare the problem for solving.
	libclasp.prepare();

	// Start the asynchronous solving process.
	Clasp::ClaspFacade::AsyncResult it = libclasp.startSolveAsync();
	// Get models one by one until iterator is exhausted.
	while (!it.end()) {
		printModel(libclasp.ctx.output, it.model());
		// Advance iterator to next model.
		it.next();
	}
	std::cout << "No more models!" << std::endl;
}
#endif
