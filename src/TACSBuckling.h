#ifndef TACS_BUCKLING_H
#define TACS_BUCKLING_H

/*
  Implementation of buckling and frequency analysis and sensitivity
  analysis of eigenvalues.

  Copyright (c) 2015 Graeme Kennedy. All rights reserved. 
  Not for commercial purposes.
*/

#include "TACSAssembler.h"
#include "GSEP.h"

/*
  Linearized buckling analysis code.

  The efficient solution of generalized eigenvalue problems requires a
  shift and invert operation that involves among other things, copying
  and axpy operations on matrices.  These operations are not supported
  with the TACSMat interface because it would be difficult to do this
  for matrices of different alternate types. This intermediate object
  maintains consistency between matrix types involved in the operation
  without exposing the underlying matrix type.  
*/
class TACSLinearBuckling : public TACSObject {
 public:
  TACSLinearBuckling( TACSAssembler * _tacs, 
		      int _loadCase, TacsScalar _sigma,
		      TACSMat *_gmat, TACSMat *_kmat, 
		      TACSMat *_aux_mat, TACSKsm * _solver,
		      int _max_lanczos_vecs, 
		      int _num_eigvals, double _eig_tol );
  ~TACSLinearBuckling();
  
  // Functions to set the shift value
  // --------------------------------
  TacsScalar getSigma();
  void setSigma( TacsScalar sigma );

  // Solve the eigenvalue problem
  // ----------------------------
  void solve( KSMPrint * ksm_print = NULL );
  void evalEigenDVSens( int n, TacsScalar fdvSens[], int numDVs );

  // Extract the eigenvalue or check the solution 
  // --------------------------------------------
  TacsScalar extractEigenvalue( int n, TacsScalar * error );
  TacsScalar extractEigenvector( int n, BVec * ans, TacsScalar * error );
  void checkEigenvector( int n );
  TacsScalar checkOrthogonality();
  void printOrthogonality();

 private:  
  // Data for the eigenvalue analysis
  int loadCase;
  TacsScalar sigma;
  
  EPBucklingShiftInvert *ep_op;
  SEP *sep;
  
  // The tacs object
  TACSAssembler *tacs;

  // Tolerances/required number of eigenvalues
  int max_lanczos_vecs;
  int num_eigvals;
  double eig_tol;

  // These are used by the eigenvalue solver and to solve the linear systems
  // for the path determination problem
  TACSPc *pc;
  TACSKsm *solver;
  TACSMat *aux_mat, *kmat, *gmat;

  // Vectors used in the analysis
  BVec * path; // The solution path
  BVec *res, *update, *eigvec;  
};

/*!  
  The following class performs frequency analysis and gradient
  evaluation of a TACS finite-element model.

  The code computes eigenvalues and eigenvectors of the generalized
  eigenproblem:

  K u = lambda M u

  The natural frequency of vibration are determined where lambda =
  omega^2.

  The code uses a Lanczos eigenproblem solver with full
  orthogonalization.  The full orthogonalization ensures that the
  Lanczos basis is linearly independent to the required precision. The
  derivatives of the eigenvalues are obtained using an efficient
  method for computing the derivative of the inner product of two
  vectors and the corresponding matrix.
*/
class TACSFrequencyAnalysis : public TACSObject {
 public:
  TACSFrequencyAnalysis( TACSAssembler * _tacs, int _loadCase, 
			 TacsScalar _sigma,
			 TACSMat * _mmat, TACSMat * _kmat,
			 TACSKsm * _solver, int max_lanczos, 
			 int num_eigvals, double _eig_tol );
  ~TACSFrequencyAnalysis();

  // Solve the generalized eigenvalue problem
  // ----------------------------------------
  TacsScalar getSigma();
  void setSigma( TacsScalar _sigma );
  void solve( KSMPrint * ksm_print = NULL );
  void evalEigenDVSens( int n, TacsScalar fdvSens[], int numDVs );

  // Extract and check the solution 
  // ------------------------------
  TacsScalar extractEigenvalue( int n, TacsScalar * error );
  TacsScalar extractEigenvector( int n, BVec * ans, TacsScalar * error );
  void checkEigenvector( int n );
  TacsScalar checkOrthogonality();

 private:
  int loadCase;
  TACSAssembler * tacs;
 
  // The matrices used in the analysis
  TACSMat *mmat; // The mass matrix
  TACSMat *kmat; // The stiffness matrix
  TACSKsm *solver; // Associated with kmat
  TACSPc *pc;      // The preconditioner

  TacsScalar sigma;
  EPGeneralizedShiftInvert *ep_op;
  SEP *sep;

  // Vectors required for eigen-sensitivity analysis
  BVec *eigvec, *res;
};

#endif 
