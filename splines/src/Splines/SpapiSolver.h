#ifndef _SPAPI_SOLVER_H
#define _SPAPI_SOLVER_H

#include <iostream>
#include <vector>
#include <algorithm>
// #include <lapacke.h>
// #include <eigen3/Eigen/Sparse>
#include "Spline.h"
#include "splines_common.h"

// using namespace Eigen;

class SpapiSolver
{
private:
	std::vector<double> knots;
	std::vector<double> x;
	std::vector<double> y;
	std::vector<int> degree;
	// SparseMatrix<double> M;
	std::vector<Base_Spline> base;
	int m = 0;
	int k = 0;
	std::vector<double> res;
	bool test = true;

public:
	SpapiSolver() = default;
	~SpapiSolver() = default;

	SpapiSolver(std::vector<double> _knots, std::vector<double> _x, std::vector<double> _y)
		: knots(_knots), x(_x), y(_y), m(x.size()), k(knots.size() - x.size())
	{
		if (k < 1)
		{
			// std::cerr << "Too few knots !" << std::endl;
			test = false;
		}
		std::vector<double>::iterator ite = unique(_x.begin(), _x.end());
		_x.erase(ite, _x.end());
		/*
		for (int i = 1; i < (int)(_x.size() - 1); i++)
		{
			int t1 = std::count(knots.begin(), knots.end(), _x[i]);
			int t2 = std::count(x.begin(), x.end(), _x[i]);
			if ((t1 + t2 > k) || (t2 >= k))
			{
				// std::cerr << "Too many derivatives !" << std::endl;
				test = false;
			}
		}
		if ((std::count(x.begin(), x.end(), _x[0]) >= k) || (std::count(x.begin(), x.end(), _x[_x.size() - 1]) >= k))
		{
			test = false;
		}
		*/
		degree.push_back(0);
		for (int i = 1; i < m; i++)
		{
			if (x[i] == x[i - 1])
			{
				degree.push_back(degree[i - 1] + 1);
			}
			else
			{
				degree.push_back(0);
			}
		}
	}

	SpapiSolver(int _k, std::vector<double> _x, std::vector<double> _y)
		: x(_x), y(_y), m(x.size()), k(_k)
	{
		if (k > static_cast<int>(x.size()))
		{
			k = static_cast<int>(x.size());
		}
		knots = baltam::splines::aptknt(x, k);
		degree.push_back(0);
		for (int i = 1; i < m; i++)
		{
			if (x[i] == x[i - 1])
			{
				degree.push_back(degree[i - 1] + 1);
			}
			else
			{
				degree.push_back(0);
			}
		}
	}

	bool solve()
	{
		if (test == false)
		{
			return false;
		}
		for (int i = 0; i < m; i++)
		{
			std::vector<double> tmp(knots.begin() + i, knots.begin() + k + i + 1);
			base.push_back(Base_Spline(tmp));
		}
		// lapacke
		vector<double> A;
		// for (int i=0;i < m;i++)
		//   if (degree[i]!=0) throw std::invalid_argument("degree错误");
		for (int j = 0; j < m; j++)
		{
			for (int i = 0; i < m; i++)
			{
				if ((x[i] >= base[j].knot(-1)) && (x[i] < base[j].knot(k - 1)))
				{
					A.push_back(base[j].d(degree[i], x[i]));
				}
				else
				{
					A.push_back(0);
				}
			}
		}
		//	throw std::invalid_argument("矩阵赋值成功");
		vector<double> rhs = y;
		vector<int> ipiv(m);
		int info = LAPACKE_dgesv(LAPACK_COL_MAJOR, m, 1, &A[0], m, &ipiv[0], &rhs[0], m);
		// int info = 0;
		if (info != 0)
		{
			throw std::invalid_argument("求解方程失败");
		}
		res = rhs;
		/*
		SparseMatrix<double> M = SparseMatrix<double>(m, m);
		int N = k - 1;
		std::vector<Triplet<double>> tripletlist;
		for (int j = 0; j < m; j++)
		{
			for (int i = 0; i < m; i++)
			{
				if ((x[i] >= base[j].knot(-1)) && (x[i] <= base[j].knot(N)))
				{
					tripletlist.push_back(Triplet<double>(i, j, base[j].d(degree[i], x[i])));
				}
			}
		}
		M.setFromTriplets(tripletlist.begin(), tripletlist.end());
		M.makeCompressed();
		SparseQR<SparseMatrix<double>, NaturalOrdering<int>> solver;
		solver.compute(M);
		int rank = solver.rank();
		if (rank != m)
		{
			return false;
		}
		VectorXd rhs(m);
		for (int i = 0; i < m; i++)
		{
			rhs[i] = y[i];
		}
		VectorXd result = solver.solve(rhs);
		for (int i = 0; i < m; i++)
		{
			res.push_back(result[i]);
		}*/
		return true;
	}

	std::vector<double> get_res()
	{
		return res;
	}

	std::vector<double> get_knots()
	{
		return knots;
	}
};

#endif
