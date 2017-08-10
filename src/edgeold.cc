#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <complex>
#include <stdlib.h>
//#define EIGEN_USE_MKL_ALL
#include "Eigen/Eigenvalues"
#include "Eigen/Dense"

using namespace Eigen;
using namespace std;

void add_term_edge_Yobc(MatrixXcd *H, int nat, int cyllength, Vector3d *Kp,
                        int i, int j, int ty, Vector3d *Rij, double ret, double imt) {
   const complex<double> I(0, 1);
   j += nat * ty;
   if ((j >= 0) && (j < nat * cyllength)) {
      (*H)(i, j) += ((complex<double>) ret + I * (complex<double>) imt)
                    * exp(I * ((*Kp).dot(*Rij)));
   }
   return;
}

void add_term_edge_Ypbc(MatrixXcd *H, int nat, int cyllength, Vector3d *Kp,
                        int i, int j, int ty, Vector3d *Rij, double ret, double imt) {
   const complex<double> I(0, 1);
   j += nat * ty;
   if (j < 0) {
      j += nat * cyllength;
   }
   if (j >= nat * cyllength) {
      j -= nat * cyllength;
   }
   (*H)(i, j) += ((complex<double>) ret + I * (complex<double>) imt)
                 * exp(I * ((*Kp).dot(*Rij)));
   return;
}

void plot_term_svg(double xi, double yi, double xj, double yj) {
   cerr << "<line x1=\"" << xi << "\" y1=\"" << yi << "\" x2=\"" << xi + xj
        << "\" y2=\"" << yi + yj
        << "\" style=\"stroke:rgb(0,0,0);stroke-width:0.02\" />" << endl;
   cerr << "<circle cx=\"" << xi << "\" cy=\"" << yi
        << "\" r=\"0.1\" stroke=\"black\" stroke-width=\".02\" fill=\"white\" />"
        << endl;
   return;
}

void plot_term_Yobc(int nat, int cyllength, int xtranslation, int i, int j,
                    int ty, MatrixXd *Atoms, Vector3d *Rij, Matrix3d *Cell) {
   j += nat * ty;
   if ((j >= 0) && (j < nat * cyllength)) {
      Vector3d T(xtranslation, (int) i / nat, 0);
      plot_term_svg((*Atoms)((int) i % nat, 0) + ((*Cell).col(0).dot(T)),
                    (*Atoms)((int) i % nat, 1) + ((*Cell).col(1).dot(T)), (*Rij)[0],
                    (*Rij)[1]);
   }
   return;
}

void plot_term_Ypbc(int nat, int cyllength, int xtranslation, int i, int j,
                    int ty, MatrixXd *Atoms, Vector3d *Rij, Matrix3d *Cell) {
   j += nat * ty;
   if (j < 0) {
      j += nat * cyllength;
   }
   if (j >= nat * cyllength) {
      j -= nat * cyllength;
   }
   Vector3d T(xtranslation, (int) i / nat, 0);
   plot_term_svg((*Atoms)((int) i % nat, 0) + ((*Cell).col(0).dot(T)),
                 (*Atoms)((int) i % nat, 1) + ((*Cell).col(1).dot(T)), (*Rij)[0],
                 (*Rij)[1]);
   return;
}

int main(int argc, char *argv[]) {

   if (argc < 6) {
      cerr << "Usage: " << argv[0]
           << " --goal <goal> config-file hr-file siw-file kpoints-file"
           << endl << endl
           << " goal should be \'bs\', \'dos\', \'Aw\', \'Awk\', "
           << " \'Awkverbose\', \'Awkedge[T/B][U/D]\', \'Z2\' or \'chern\'"
           << endl;
      return 1;
   }
   string goal;
   if ((string(argv[1]) == "--goal") || (string(argv[1]) == "-g")) {
      goal = argv[2];
   } else {
      cerr << "Error: goal should be specified." << endl;
      return 1;
   }
   cout << "# EDGE (O. Janson, version 16.11.2016)" << endl;

   const double pi = atan(1) * 4;
   const double one = 1;
   const double two = 2;
   const double delta = 10e-14;

   const char *conffile = argv[3];
   const char *hrfile = argv[4];
   const char *swfile = argv[5];
   const char *kpfile = argv[6];

//  print format precision
   IOFormat printFmt(FullPrecision, DontAlignCols, " ", " ", "", "", "", "");

//  read configuration
   string line;
   int nat, cyllength, wdens;
   char bc;
   double wmin, wmax, encutoff, distcutoff, imagdelta, bzcartesianfactor;
   fstream config(conffile);
   if (config.is_open()) {
      getline(config, line);
      nat = atoi(line.substr(0, 14).c_str());
      getline(config, line);
      cyllength = atoi(line.substr(0, 14).c_str());
      getline(config, line);
      bc = line[6];
      getline(config, line);
      wmin = atof(line.substr(0, 14).c_str());
      getline(config, line);
      wmax = atof(line.substr(0, 14).c_str());
      getline(config, line);
      wdens = atoi(line.substr(0, 14).c_str());
      getline(config, line);
      encutoff = atof(line.substr(0, 14).c_str());
      getline(config, line);
      distcutoff = atof(line.substr(0, 14).c_str());
      getline(config, line);
      imagdelta = atof(line.substr(0, 14).c_str());
      getline(config, line);
      bzcartesianfactor = atof(line.substr(0, 14).c_str());
   } else {
      cerr << "Error: Config file could not be opened." << endl;
      return 1;
   }
   config.close();
   const int dim = nat * cyllength;
   const double wstep = (wmax - wmin) / ((double) wdens - one);
   complex<double> idelta(0, imagdelta);
   MatrixXcd H = MatrixXcd::Zero(dim, dim);

// read the hamiltonian
   vector<string> vecString;
   vector<int> Ii;
   vector<int> Jj;
   vector<int> Tb;
   vector<float> TijR;
   vector<float> TijI;
   vector<vector<double> > RijTV;
   Matrix3d Cell = Matrix3d::Zero();
   MatrixXd Atoms = MatrixXd::Zero(dim, 3);
   fstream hrin(hrfile);
   if (hrin.is_open()) {
      for (int i = 0; i < 3; i++) {
         getline(hrin, line);
         Cell(i, 0) = atof(line.substr(25, 11).c_str());
         Cell(i, 1) = atof(line.substr(36, 11).c_str());
         Cell(i, 2) = atof(line.substr(47, 11).c_str());
      }
      for (int i = 0; i < nat; i++) {
         getline(hrin, line);
         Atoms(i, 0) = atof(line.substr(30, 10).c_str());
         Atoms(i, 1) = atof(line.substr(41, 10).c_str());
         Atoms(i, 2) = atof(line.substr(52, 10).c_str());
      }
      while (getline(hrin, line)) {
         if (atof(line.substr(28, 9).c_str())
             * atof(line.substr(28, 9).c_str())
             + atof(line.substr(40, 9).c_str())
               * atof(line.substr(40, 9).c_str())
             >= encutoff * encutoff || encutoff < 0.) {
            Vector3d R = Vector3d::Zero();
            Vector3d RT = Vector3d::Zero();
            R += Cell.row(0) * atoi(line.substr(0, 5).c_str());
            R += Cell.row(1) * atoi(line.substr(5, 5).c_str());
            R += Cell.row(2) * atoi(line.substr(10, 5).c_str());
            R += Atoms.row(atoi(line.substr(20, 5).c_str()) - 1);
            R -= Atoms.row(atoi(line.substr(15, 5).c_str()) - 1);
            RT += Cell.row(0) * atoi(line.substr(0, 5).c_str());
            RT += Cell.row(1) * atoi(line.substr(5, 5).c_str());
            RT += Cell.row(2) * atoi(line.substr(10, 5).c_str());
            if (R.norm() < distcutoff || distcutoff < 0.) {
               vector<double> NewvecRT(RT.data(), RT.data() + 3);
               RijTV.push_back(NewvecRT);
               Tb.push_back(atoi(line.substr(5, 5).c_str()));
               Ii.push_back(atoi(line.substr(15, 5).c_str()) - 1);
               Jj.push_back(atoi(line.substr(20, 5).c_str()) - 1);
               TijR.push_back(atof(line.substr(28, 9).c_str()));
               TijI.push_back(atof(line.substr(40, 9).c_str()));
            }
         }
      }
   } else {
      cerr << "Error: The _hr.dat file could not be opened." << endl;
      return 1;
   }
   hrin.close();

//  define the Brillouin zone (!!!  WORKAROUND  !!!)
   if (bzcartesianfactor > delta) {
      for (int i = 0; i < 3; i++) {
         double v = 0;
         for (int j = 0; j < 3; j++) {
            v += Cell(i, j) * Cell(i, j);
            Cell(i, j) = 0;
         }
         Cell(i, i) = bzcartesianfactor * sqrt(v);
      }
   }
   Matrix3d Bzone = (two * pi * Cell.inverse()).transpose();

//  read the k-points
   vector<vector<double> > Kvec;
   vector<double> Nx; // indices of the neighboring k-points along x
   vector<double> Ny; // indices of the neighboring k-points along y

   fstream kpin(kpfile);
   if (kpin.is_open()) {
      while (getline(kpin, line)) {
         int kx, ky, kz, kdiv;
         kx = atoi(line.substr(10, 5).c_str());
         ky = atoi(line.substr(15, 5).c_str());
         kz = atoi(line.substr(20, 5).c_str());
         kdiv = atoi(line.substr(25, 5).c_str());
         Vector3d Krel;
         Krel << (double) kx / kdiv, (double) ky / kdiv, (double) kz / kdiv;
         Krel = Krel.transpose() * Bzone;
         if ((goal == "chern") || (goal == "Z2")) {
            if (line.length() < 40) {
               cerr << "Error: The neighbors in the k-points are missing."
                    << endl;
               return 1;
            }
            Nx.push_back(atoi(line.substr(30, 6).c_str()));
            Ny.push_back(atoi(line.substr(36, 6).c_str()));
         }
         vector<double> Kpoint(Krel.data(), Krel.data() + 3);
         Kvec.push_back(Kpoint);
      }
   } else {
      cerr << "Error: File containing the k-points could not be opened."
           << endl;
      return 1;
   }
   const int kdens = Kvec.size();
   MatrixXd K = MatrixXd::Zero(kdens, 3);
   kpin.close();

//  read the self-energy
   vector<vector<double> > SwR;
   vector<vector<double> > SwI;
   vector<double> Newcol(nat, 0);

   fstream swin(swfile);
   if (swin.is_open()) {
      while (getline(swin, line)) {
         char *sArr = new char[line.length() + 1];
         strcpy(sArr, line.c_str());
         char *pos = strtok(sArr, " ");
         SwR.push_back(Newcol);
         SwI.push_back(Newcol);
         for (int i = 0; i < nat; i++) {
            pos = strtok(NULL, " ");
            SwR.back().at(i) = strtod(pos, NULL);
            pos = strtok(NULL, " ");
            SwI.back().at(i) = strtod(pos, NULL);
         }
         delete[] sArr;
      }
   } else {
      cerr << "Error: File containging the self-energy could not be opened."
           << endl;
      return 1;
   }
   swin.close();

   const complex<double> I(0, 1);
   complex<double> iw;
//      <<< DOS
   vector<double> ek;
   if (goal == "dos")
      ek.reserve(dim * kdens);
//      DOS >>>
//      <<< k-integrated spectral function (DOS)
   vector<double> wk(wdens);
   if (goal == "Aw")
      fill(wk.begin(), wk.end(), 0);
//      k-integrated spectral function (DOS) >>>

//  <<< Chern/Z2
   vector<complex<double>> Eigvecs;

//  a multi-orbital form of the sigma_y Pauli matrix for the time reversal 
//  symmetry operator
   MatrixXcd NSy = MatrixXcd::Zero(dim, dim);
   for (int i = 0; i < dim / 2; ++i) {
      NSy(i, i + dim / 2) = -I;
      NSy(i + dim / 2, i) = I;
   }

   if ((goal == "chern") || (goal == "Z2"))
      Eigvecs.reserve(dim * dim * kdens);
//  Chern/Z2 >>>

   for (int ki = 0; ki != kdens; ++ki) {
      Vector3d Kp(Kvec[ki].data());
      H = MatrixXcd::Zero(dim, dim);
      for (int x = 0; x != cyllength; ++x) {
         for (unsigned int i = 0; i != TijR.size(); ++i) {
            Vector3d R(RijTV[i].data());
            if (bc == 'o') {
               add_term_edge_Yobc(&H, nat, cyllength, &Kp, x * nat + Ii[i],
                                  x * nat + Jj[i], Tb[i], &R, TijR[i], TijI[i]);
            } else if (bc == 'p' && cyllength != 1) {
               add_term_edge_Ypbc(&H, nat, cyllength, &Kp, Ii[i],
                                  x * nat + Jj[i], Tb[i], &R, TijR[i], TijI[i]);
            } else {
               H(Ii[i], Jj[i]) += ((complex<double>) TijR[i]
                                   + I * (complex<double>) TijI[i])
                                  * exp(-I * (Kp.dot(R)));
            }
         }
      }

      // !    !   !  ! !!! !  !   !    !
      // TEST: artificial Zeeman splitting!!!
//	double zeeman = 10.0;
//        for (int i = 0; i != dim; ++i) {
////		int zsign = -1 + 2 * (i / 6);
//		int zsign = -1 + 2 * (i / 2);
//		H(i, i) += (complex<double>) zeeman * (complex<double>) zsign;
//	}
      // END TEST
      // !    !   !  ! !!! !  !   !    !

//      <<< band structure
      if (goal == "bs") {
//	    cout << H << endl;
         SelfAdjointEigenSolver<MatrixXcd> es(H, EigenvaluesOnly);
         cout << ki << " " << es.eigenvalues().transpose().format(printFmt)
              << endl;
      }
//      band stucture >>

//      <<< Chern/Z2 eigenvectors
      if ((goal == "chern") || (goal == "Z2")) {
         SelfAdjointEigenSolver<MatrixXcd> es(H);
//	    cout << ki << " " << es.eigenvalues().transpose().format(printFmt) << endl;
         for (int i = 0; i != dim; ++i) {
            for (int j = 0; j != dim; ++j) {
               Eigvecs.push_back(es.eigenvectors()(j, i));
            }
         }
         /*
          //          ---DEBUG>>>
          cout << endl << endl;
          cout << es.eigenvalues().transpose() << endl;
          cout << endl;
          for (int i = 0; i != dim; ++i) {
          cout << es.eigenvectors().col(i).transpose() << endl;
          }
          cout << endl;
          for (int i = 0; i != dim; ++i) {
          cout << (I * NSy * (es.eigenvectors().col(i).conjugate())).transpose()
          << endl;
          }
          //            <<<DEBUG---
          */

      }
//      Chern/Z2 eigenvectors >>

//      <<< DOS
      if (goal == "dos") {
         SelfAdjointEigenSolver<MatrixXcd> es(H, EigenvaluesOnly);
         for (int i = 0; i != dim; ++i) {
            ek.push_back(es.eigenvalues()(i));
         }
      }
//      DOS >>>

//      <<< spectral function
      if ((goal == "Aw") || (goal == "Awk") || (goal == "Awkedge")
          || (goal == "Awkverbose")) {

         MatrixXcd C = MatrixXcd::Zero(dim, dim);

         for (int x = 0; x != wdens; ++x) {
            iw = (complex<double>) wmin + wstep * x + idelta;
            MatrixXcd Sw = MatrixXcd::Zero(dim, dim);
            for (int i = 0; i != cyllength; ++i) {
               for (int j = 0; j != nat; ++j) {
                  Sw(nat * i + j, nat * i + j) = complex<double>(
                          SwR[x][j], SwI[x][j]);
               }
            }
//          cout << Sw << endl;
            C = (iw * MatrixXcd::Identity(dim, dim) - H - Sw).inverse();

//      >========================================================>
            if ((goal == "Awk") || (goal == "Awkverbose")) {
               cout << ki << " " << wmin + wstep * x << " "
                    << -one * imag(C.trace()) / pi / (kdens + one)
                       / dim;
               if (goal == "Awkverbose") {
                  cout << " "
                       << -one
                          * C.block(0, 0, nat, nat).diagonal().imag()
                          / pi / (kdens + one) / dim;
                  cout << " "
                       << -one
                          * C.block(dim - nat - 1, dim - nat - 1,
                                    nat, nat).diagonal().imag() / pi
                          / (kdens + one) / dim;
               }
               cout << endl;
            } else if (goal == "Awkedge") {
               complex<double> edgetrace(0, 0);
               if (goal == "Awkedge") {
                  for (int i = 0; i != nat; ++i) {
                     edgetrace += C(i, i) + C(dim - i - 1, dim - i - 1);
                  }
               }
               cout << ki << " " << wmin + wstep * x << " "
                    << -one * imag(edgetrace) / pi / (kdens + one) / dim
                    << endl;
            } else {
//      <========================================================<
//          <<< k-integrated spectral function (DOS)
               wk[x] +=
                       (-one * imag(C.trace()) / pi / (kdens + one) / dim);
            }
//          k-integrated spectral function (DOS) >>>
         }
//      >========================================================>
         if ((goal == "Awk") || (goal == "Awkverbose") || (goal == "Awkedge")
             || (goal == "AwkedgeT") || (goal == "AwkedgeB")
             || (goal == "AwkedgeTU") || (goal == "AwkedgeTD")
             || (goal == "AwkedgeBU") || (goal == "AwkedgeBD"))
            cout << endl;
      }
//      <========================================================<
//      spectral function >>>
   }

//  <<< DOS
   if (goal == "dos") {
      cout << "# Density of states:" << endl;
      for (int x = 0; x != wdens; ++x) {
         int ecount =
                 count_if(ek.begin(), ek.end(),
                          [wmin, x, wstep](double j) {
                             return (j < wmin + (x + 1) * wstep) && (wmin + x * wstep <= j);
                          });
         cout << wmin + x * wstep << " "
              << (double) wdens * ecount / (kdens + 1) / dim
                 / (wmax - wmin) << endl;
      }
   }
//  DOS >>>

//  <<< k-integrated spectral function (DOS)
   if (goal == "Aw") {
      cout << "# k-intergrated spectral function:" << endl;
      for (int x = 0; x != wdens; ++x) {
         cout << wmin + x * wstep << " " << wk[x] << endl;
      }
   }
//  k-integrated spectral function (DOS) >>>

//  <<< Chern number calculations
   if (goal == "chern") {

      vector<complex<double>> U1;
      U1.reserve(kdens * dim);
      vector<complex<double>> U2;
      U2.reserve(kdens * dim);
      for (int band = 0; band < dim; ++band) {
         complex<double> product_of_eigestates(0, 0);
         for (int k = 0; k < kdens; ++k) {
            RowVectorXcd Uk0 = VectorXcd::Map(
                    &Eigvecs[k * dim * dim + band * dim], dim).adjoint();
            VectorXcd Ukx = VectorXcd::Map(
                    &Eigvecs[Nx[k] * dim * dim + band * dim], dim);
            product_of_eigestates = (Uk0 * Ukx)(0, 0);
            U1.push_back(
                    product_of_eigestates / abs(product_of_eigestates));
            VectorXcd Uky = VectorXcd::Map(
                    &Eigvecs[Ny[k] * dim * dim + band * dim], dim);
            product_of_eigestates = (Uk0 * Uky)(0, 0);
            U2.push_back(
                    product_of_eigestates / abs(product_of_eigestates));
         }
      }
      for (int band = 0; band < dim; ++band) {
         complex<double> chern(0, 0);
         for (int k = 0; k < kdens; ++k) {
            chern += log(
                    U1[band * kdens + k] * U2[band * kdens + Nx[k]]
                    / U1[band * kdens + Ny[k]]
                    / U2[band * kdens + k]);
         }
         cout << "Band " << (band + 1) << ": "
              << real(chern / (two * pi * I)) << " (numerical result "
              << chern / (two * pi * I) << ")" << endl;
      }

   }
//  Chern number calculations >>>

//  <<< Z2 calculations 
   if (goal == "Z2") {

      cout << "Z2 invariants are calculated using the method" << endl;
      cout << "of Fukui and Hatsugai, JPSJ 76, 053702 (2007)." << endl;
      int cylkdens = kdens / 4;
      int cylkdensedge = (int) sqrt(two * cylkdens + one) - 1;
      vector<int> Z2v;
      Z2v.reserve(dim * 2);

      for (int cylinder = 0; cylinder < 4; ++cylinder) {

         long int zeroshift = cylinder * cylkdens * dim * dim;

//          constrain the eigenvectors on the edges of the half-BZ
//          |u(k)_2n> = Th |u(-k)_2n>a
         for (int k = (1 + cylkdensedge / 2); k < cylkdensedge; ++k) {
            for (int band = 0; band < dim; ++band) {
               long int indexshift = zeroshift + band * dim;
               VectorXcd Ubr = VectorXcd::Map(
                       &Eigvecs[indexshift + k * dim * dim], dim);
               VectorXcd Utl = VectorXcd::Map(
                       &Eigvecs[indexshift + (cylkdens - k) * dim * dim],
                       dim);
               VectorXcd Ubl = I * NSy * (Ubr.conjugate());
               VectorXcd Utr = I * NSy * (Utl.conjugate());
               for (int i = 0; i < dim; ++i) {
                  indexshift = zeroshift + band * dim + i;
                  Eigvecs[indexshift + (cylkdensedge - k) * dim * dim] =
                          Ubl(i);
                  Eigvecs[indexshift
                          + (cylkdens - cylkdensedge + k) * dim * dim] =
                          Utr(i);
               }
            }
         }

//          constrain the eigenvectors at TRIM  |u_2n+1> = Th |u_2n>
         for (int band = 0; band < dim / 2; ++band) {

            long int indexshift = zeroshift + 2 * band * dim;

            VectorXcd Umm = VectorXcd::Map(&Eigvecs[indexshift], dim);
            VectorXcd U0m = VectorXcd::Map(
                    &Eigvecs[indexshift + cylkdensedge * dim * dim / 2],
                    dim);
            VectorXcd Um0 = VectorXcd::Map(
                    &Eigvecs[indexshift
                             + (cylkdens - cylkdensedge) * dim * dim], dim);
            VectorXcd U00 = VectorXcd::Map(
                    &Eigvecs[indexshift
                             + (cylkdens - cylkdensedge / 2) * dim * dim],
                    dim);

            VectorXcd Ummd = I * NSy * (Umm.conjugate());
            VectorXcd U0md = I * NSy * (U0m.conjugate());
            VectorXcd Um0d = I * NSy * (Um0.conjugate());
            VectorXcd U00d = I * NSy * (U00.conjugate());

            for (int i = 0; i < dim; ++i) {
               indexshift = zeroshift + (2 * band + 1) * dim + i;
               Eigvecs[indexshift] = Ummd(i);
               Eigvecs[indexshift + cylkdensedge * dim * dim / 2] = U0md(
                       i);
               Eigvecs[indexshift + (cylkdens - cylkdensedge) * dim * dim] =
                       Um0d(i);
               Eigvecs[indexshift
                       + (cylkdens - cylkdensedge / 2) * dim * dim] = U00d(
                       i);
            }
         }

//          calculate U1 and U2 link variables
         vector<complex<double>> U1;
         U1.reserve(cylkdens * dim / 2);
         vector<complex<double>> U2;
         U2.reserve(cylkdens * dim / 2);

         for (int band = 0; band < dim / 2; ++band) {
            for (int k = 0; k < cylkdens; ++k) {
               long int indexshift = zeroshift + 2 * band * dim;
               MatrixXcd Vxk = MatrixXcd::Map(
                       &Eigvecs[indexshift + Nx[k] * dim * dim], dim, 2);
               MatrixXcd Vyk = MatrixXcd::Map(
                       &Eigvecs[indexshift + Ny[k] * dim * dim], dim, 2);
               MatrixXcd Vk = MatrixXcd::Map(
                       &Eigvecs[indexshift + k * dim * dim], dim, 2);

               complex<double> product_of_eigestates(0, 0);
               Matrix2cd Temp_prod = Matrix2cd::Zero();
               Temp_prod = (Vk.adjoint()) * Vxk;
               product_of_eigestates = Temp_prod(0, 0) * Temp_prod(1, 1)
                                       - Temp_prod(1, 0) * Temp_prod(0, 1);
               U1.push_back(
                       product_of_eigestates / abs(product_of_eigestates));
               Temp_prod = (Vk.adjoint()) * Vyk;
               product_of_eigestates = Temp_prod(0, 0) * Temp_prod(1, 1)
                                       - Temp_prod(1, 0) * Temp_prod(0, 1);
               U2.push_back(
                       product_of_eigestates / abs(product_of_eigestates));
            }
         }

//          calculate the invariants 
         cout << "Z2 invariants (cylinder " << (cylinder + 1) << "):"
              << endl;
         complex<double> z2(0, 0);
         for (int band = 0; band < dim / 2; ++band) {
            for (int k = 0; k < cylkdensedge; ++k) {
               z2 += log(U1[band * cylkdens + k])
                     - log(U1[(band + 1) * cylkdens + k - cylkdensedge]);
            }
            for (int k = 0; k < (cylkdens - cylkdensedge); ++k) {
               z2 -= log(
                       U1[band * cylkdens + k]
                       * U2[band * cylkdens + Nx[k]]
                       / U1[band * cylkdens + Ny[k]]
                       / U2[band * cylkdens + k]);
            }
            cout << "up to the band " << (band + 1) << ": "
                 << round(abs(remainder(real(z2 / (two * pi * I)), 2)))
                 << " (numerical result " << real(z2 / (two * pi * I))
                 << ")" << endl;
            Z2v.push_back(
                    round(abs(remainder(real(z2 / (two * pi * I)), 2))));
         }
      }

//      print a summary: nu_0 (x_pi; y_pi; z_pi) for every band 
      MatrixXi Z2m = MatrixXi::Map(&Z2v[0], dim / 2, 4);
      cout << endl << "Summary:" << endl;
      for (int band = 0; band < dim / 2; ++band) {
         cout << "up to the band " << (band + 1) << ": "
              << round(abs(remainder(Z2m(band, 0) + Z2m(band, 3), 2)))
              << "(" << Z2m(band, 1) << ";" << Z2m(band, 2) << ";"
              << Z2m(band, 3) << ")" << endl;
      }
   }
//  Z2 calculations >>>

   return 0;
}
