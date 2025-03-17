// This file is part of PIQP.
//
// Copyright (c) 2023 EPFL
//
// This source code is licensed under the BSD 2-Clause License found in the
// LICENSE file in the root directory of this source tree.

#include <cstdlib>

#include "piqp.h"

#include "gtest/gtest.h"

/*
 * first QP:
 * min 3 x1^2 + 2 x2^2 - x1 - 4 x2
 * s.t. -1 <= x <= 1, x1 = 2 x2
 *
 * second QP:
 * min 4 x1^2 + 2 x2^2 - x1 - 4 x2
 * s.t. -1 <= x <= 2, x1 = 3 x2
*/
TEST(CInterfaceTest, SimpleDenseQPWithUpdate)
{
    piqp_int n = 2;
    piqp_int p = 1;
    piqp_int m = 2;

    piqp_float P[4] = {6, 0, 0, 4};
    piqp_float c[2] = {-1, -4};

    piqp_float A[2] = {1, -2};
    piqp_float b[1] = {0};

    piqp_float G[4] = {1, 0, -1, 0};
    piqp_float h[2] = {1, 1};

    piqp_float x_lb[2] = {-PIQP_INF, -1};
    piqp_float x_ub[2] = {PIQP_INF, 1};

    piqp_workspace* work;
    piqp_settings* settings = (piqp_settings*) malloc(sizeof(piqp_settings));
    piqp_data_dense* data = (piqp_data_dense*) malloc(sizeof(piqp_data_dense));

    piqp_set_default_settings_dense(settings);
    settings->verbose = 1;

    data->n = n;
    data->p = p;
    data->m = m;
    data->P = P;
    data->c = c;
    data->A = A;
    data->b = b;
    data->G = G;
    data->h = h;
    data->x_lb = x_lb;
    data->x_ub = x_ub;

    piqp_setup_dense(&work, data, settings);
    piqp_status status = piqp_solve(work);

    ASSERT_EQ(status, PIQP_SOLVED);
    ASSERT_NEAR(work->result->x[0], 0.4285714, 1e-6);
    ASSERT_NEAR(work->result->x[1], 0.2142857, 1e-6);
    ASSERT_NEAR(work->result->y[0], -1.5714286, 1e-6);
    ASSERT_NEAR(work->result->z[0], 0, 1e-6);
    ASSERT_NEAR(work->result->z[1], 0, 1e-6);
    ASSERT_NEAR(work->result->z_lb[0], 0, 1e-6);
    ASSERT_NEAR(work->result->z_lb[1], 0, 1e-6);
    ASSERT_NEAR(work->result->z_ub[0], 0, 1e-6);
    ASSERT_NEAR(work->result->z_ub[1], 0, 1e-6);

    P[0] = 8;
    A[1] = -3;
    h[0] = 2;
    x_ub[1] = 2;

    piqp_update_dense(work, data->P, NULL, data->A, NULL, NULL, data->h, NULL, data->x_ub);
    status = piqp_solve(work);

    ASSERT_EQ(status, PIQP_SOLVED);
    ASSERT_NEAR(work->result->x[0], 0.2763157, 1e-6);
    ASSERT_NEAR(work->result->x[1], 0.0921056, 1e-6);
    ASSERT_NEAR(work->result->y[0], -1.2105263, 1e-6);
    ASSERT_NEAR(work->result->z[0], 0, 1e-6);
    ASSERT_NEAR(work->result->z[1], 0, 1e-6);
    ASSERT_NEAR(work->result->z_lb[0], 0, 1e-6);
    ASSERT_NEAR(work->result->z_lb[1], 0, 1e-6);
    ASSERT_NEAR(work->result->z_ub[0], 0, 1e-6);
    ASSERT_NEAR(work->result->z_ub[1], 0, 1e-6);

    piqp_cleanup(work);
    if (settings) free(settings);
    if (data) free(data);
}

/*
 * first QP:
 * min 3 x1^2 + 2 x2^2 - x1 - 4 x2
 * s.t. -1 <= x <= 1, x1 = 2 x2
 *
 * second QP:
 * min 4 x1^2 + 2 x2^2 - x1 - 4 x2
 * s.t. -1 <= x <= 2, x1 = 3 x2
*/
TEST(CInterfaceTest, SimpleSparseQPWithUpdate)
{
    piqp_int n = 2;
    piqp_int p = 1;
    piqp_int m = 2;

    piqp_float P_x[2] = {6, 4};
    piqp_int P_nnz = 2;
    piqp_int P_p[3] = {0, 1, 2};
    piqp_int P_i[2] = {0, 1};
    piqp_float c[2] = {-1, -4};

    piqp_float A_x[2] = {1, -2};
    piqp_int A_nnz = 2;
    piqp_int A_p[3] = {0, 1, 2};
    piqp_int A_i[2] = {0, 0};
    piqp_float b[1] = {0};

    piqp_float G_x[2] = {1, -1};
    piqp_int G_nnz = 2;
    piqp_int G_p[3] = {0, 2, 2};
    piqp_int G_i[2] = {0, 1};
    piqp_float h[2] = {1, 1};

    piqp_float x_lb[2] = {-PIQP_INF, -1};
    piqp_float x_ub[2] = {PIQP_INF, 1};

    piqp_workspace* work;
    piqp_settings* settings = (piqp_settings*) malloc(sizeof(piqp_settings));
    piqp_data_sparse* data = (piqp_data_sparse*) malloc(sizeof(piqp_data_sparse));

    piqp_set_default_settings_sparse(settings);
    settings->verbose = 1;

    data->n = n;
    data->p = p;
    data->m = m;
    data->P = piqp_csc_matrix(data->n, data->n, P_nnz, P_p, P_i, P_x);
    data->c = c;
    data->A = piqp_csc_matrix(data->p, data->n, A_nnz, A_p, A_i, A_x);
    data->b = b;
    data->G = piqp_csc_matrix(data->m, data->n, G_nnz, G_p, G_i, G_x);
    data->h = h;
    data->x_lb = x_lb;
    data->x_ub = x_ub;

    piqp_setup_sparse(&work, data, settings);
    piqp_status status = piqp_solve(work);

    ASSERT_EQ(status, PIQP_SOLVED);
    ASSERT_NEAR(work->result->x[0], 0.4285714, 1e-6);
    ASSERT_NEAR(work->result->x[1], 0.2142857, 1e-6);
    ASSERT_NEAR(work->result->y[0], -1.5714286, 1e-6);
    ASSERT_NEAR(work->result->z[0], 0, 1e-6);
    ASSERT_NEAR(work->result->z[1], 0, 1e-6);
    ASSERT_NEAR(work->result->z_lb[0], 0, 1e-6);
    ASSERT_NEAR(work->result->z_lb[1], 0, 1e-6);
    ASSERT_NEAR(work->result->z_ub[0], 0, 1e-6);
    ASSERT_NEAR(work->result->z_ub[1], 0, 1e-6);

    P_x[0] = 8;
    A_x[1] = -3;
    h[0] = 2;
    x_ub[1] = 2;

    piqp_update_sparse(work, data->P, NULL, data->A, NULL, NULL, data->h, NULL, data->x_ub);
    status = piqp_solve(work);

    ASSERT_EQ(status, PIQP_SOLVED);
    ASSERT_NEAR(work->result->x[0], 0.2763157, 1e-6);
    ASSERT_NEAR(work->result->x[1], 0.0921056, 1e-6);
    ASSERT_NEAR(work->result->y[0], -1.2105263, 1e-6);
    ASSERT_NEAR(work->result->z[0], 0, 1e-6);
    ASSERT_NEAR(work->result->z[1], 0, 1e-6);
    ASSERT_NEAR(work->result->z_lb[0], 0, 1e-6);
    ASSERT_NEAR(work->result->z_lb[1], 0, 1e-6);
    ASSERT_NEAR(work->result->z_ub[0], 0, 1e-6);
    ASSERT_NEAR(work->result->z_ub[1], 0, 1e-6);

    piqp_cleanup(work);
    if (settings) free(settings);
    if (data)
    {
        if (data->P) free(data->P);
        if (data->A) free(data->A);
        if (data->G) free(data->G);
        free(data);
    }
}
