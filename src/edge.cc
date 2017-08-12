/**
 * This file is part of toplogy.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <fstream>
#include <mpi.h>
#include <map>
#include <sys/time.h>
#include <omp.h>

#define EIGEN_USE_MKL_ALL

#include "Eigen/Eigenvalues"
#include "Eigen/Dense"

#include "parser/progargparser.hh"
#include "model/modelfactory.h"
#include "parser/config/config.h"
#include "quan/quantityhandler.h"
#include "quan/rgrid.h"
#include "logger.h"

using namespace std;
using namespace ks;
using namespace ks_conf;

int main(int argc, char **argv) {
   MPI_Init(&argc, &argv);
   int rank;
   int size;
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   MPI_Comm_size(MPI_COMM_WORLD, &size);
   ProgArgParser progargs;
   progargs.arg<ProgArgTypes::String>("--goal").abbreviation("-g").mandatory(
           true).description("The goal of the execution");
   if (!progargs(argc, argv)) {
      cerr << "Could not parse program arguments." << endl;
      return 1;
   }
   const vector<string> args = progargs.arg<ProgArgTypes::String>("").get();
   if (args.empty()) {
      cerr << "No config file specified" << endl;
      return 1;
   }
   Config config;
   try {
      if (!config.parse(args[0]))
         throw ConfigException("Error while parsing config file");
      QuantityHandler::get()->setConfig(&config);
   } catch (ConfigException &e) {
      cerr << e.getWhat() << endl;
      cerr.flush();
      return 1;
   } catch (std::exception &e) {
      cerr << e.what() << endl;
      cerr.flush();
      return 1;
   }
   stringstream ss;
   const string goal = progargs.arg<ProgArgTypes::String>("--goal")[0];
   ss << goal << "_" << QuantityHandler::get()->getValue<RealGrid *>("rgrid")->getSystemDimension();
   Model *model = ModelFactory::get()->create(ss.str());
   if (model == nullptr) {
      cerr << "Model " << goal << " does not exist." << endl;
      return 1;
   }
   //cout << "Running model " << model->getName() << "..." << endl;
   LOG_INFO << "Running model " << model->getName() << "...";
   ostream *output = &cout;
   const bool file_output = args.size() > 1;
   if (file_output) {
      output = new ofstream;
      ((ofstream *) output)->open(args[1].c_str(), ios::out);
   }
#ifdef _OPENMP
   const double wt1 = omp_get_wtime();
#endif
   const clock_t t1 = clock();
   (*model)(*output);
#ifdef _OPENMP
   const double wt2 = omp_get_wtime();
#endif
   const clock_t t2 = clock();
   timeval tv1, tv2;
   struct timezone tz;
   gettimeofday(&tv2, &tz);
   cout << "CPU time (clock) = " << (t2 - t1) / 1000000.0 << " sec" << endl;
#ifdef _OPENMP
   cout << "wall clock time (omp_get_wtime) = " << wt2 - wt1 << " sec" << endl;
#endif
   cout << "wall clock time (gettimeofday)  = "
        << (tv2.tv_sec - tv1.tv_sec) + (tv2.tv_usec - tv1.tv_usec) * 1e-6
        << " sec" << endl;
   if (file_output) {
      ((ofstream *) output)->close();
      delete output;
   }
   delete model;
   MPI_Finalize();
   return 0;
}
