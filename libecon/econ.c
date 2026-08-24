/*

    arfcomsim: Socliast economic simulator
    Copyright (C) 2026 arf20 (Ángel Ruiz Fernandez)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

    econ.c: Economy implementation

*/

#include "econ.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define INIT_VEC_SIZE   1024

const unit_t units[] = {
    { UNIT_METER,       "meter",        "m" },
    { UNIT_METER_SQ,    "square meter", "m²" },
    { UNIT_METER_CU,    "cubic meter",  "m³" },
    { UNIT_KILOGRAM,    "kilogram",     "kg" },
    { UNIT_LITER,       "liter",        "l" },
    { UNIT_TON,         "metric ton",   "tn" }, /* metric */
    { UNIT_UNITS,       "units",        "uds" }
};


econ_t *econ_new(uint32_t workweek_hours)
{
    econ_t *e = malloc(sizeof(econ_t));

    e->products_size = 0;
    e->products_capacity = INIT_VEC_SIZE;
    e->products = malloc(e->products_capacity * sizeof(product_t));
    e->industries_size = 0;
    e->industries_capacity = INIT_VEC_SIZE;
    e->industries = malloc(e->industries_capacity * sizeof(industry_io_t));

    e->workweek_hours = workweek_hours;

    return e;
}

void econ_destroy(econ_t *econ)
{
    for (int i = 0; i < econ->products_size; i++)
        free(econ->products[i].name);
    for (int i = 0; i < econ->industries_size; i++)
        free(econ->industries[i].name);
    free(econ->products);
    free(econ->industries);
    free(econ);
}

product_t *econ_product_new(econ_t *econ, uint32_t id, const char *name,
    unit_id_t unit_id)
{
    if (econ->products_size + 1 > econ->products_capacity) {
        econ->products_capacity *= 2;
        econ->products = realloc(econ->products,
            econ->products_capacity * sizeof(product_t));
    }

    econ->products[econ->products_size].id = id ? id : econ->products_size;
    econ->products[econ->products_size].name = strdup(name);
    econ->products[econ->products_size].unit = &units[unit_id];

    return &econ->products[econ->products_size++];
}

industry_io_t *econ_industry_new(econ_t *econ, uint32_t id, const char *name, 
    uint32_t direct_labour, labour_unit_t labour_unit)
{
    if (econ->industries_size + 1 > econ->industries_capacity) {
        econ->industries_capacity *= 2;
        econ->industries = realloc(econ->industries,
            econ->industries_capacity * sizeof(product_t));
    }

    econ->industries[econ->industries_size].id =
        id ? id : econ->industries_size;
    econ->industries[econ->industries_size].name = strdup(name);

    switch (labour_unit) {
        case LABOUR_WORKERS_WEEK: break;
        case LABOUR_WORK_HOURS: direct_labour /= econ->workweek_hours;
    }
    econ->industries[econ->industries_size].labour_direct = direct_labour;

    return &econ->industries[econ->industries_size++];
}

quantity_t *econ_industry_input_add(industry_io_t *ind, product_t *product,
    uint32_t quant)
{
    if (ind->inputs_size + 1 > ind->inputs_capacity) {
        ind->inputs_capacity *= 2;
        ind->inputs = realloc(ind->inputs,
            ind->inputs_capacity * sizeof(product_t));
    }

    ind->inputs[ind->inputs_size].product = product;
    ind->inputs[ind->inputs_size].quant_gross = quant;

    return &ind->inputs[ind->inputs_size++];
}

void econ_industry_output_add(industry_io_t *ind, product_t *product,
    uint32_t quant_gross, uint32_t quant_net)
{
    ind->output.product = product;
    ind->output.quant_gross = quant_gross;
    ind->output.quant_net = quant_net;
}

float econ_labour_total(econ_t *econ, labour_unit_t unit)
{
    float l = 0;
    for (int i = 0; i < econ->industries_size; i++)
        l += econ->industries[i].labour_direct;
    
    switch (unit) {
        case LABOUR_WORKERS_WEEK: return l;
        case LABOUR_WORK_HOURS: return l * econ->workweek_hours;
    }
}

/**
 * Matrix Gauss-Seidel method
 */
int econ_labour_content_solve(econ_t *econ, uint32_t precision) {
    size_t N = econ->industries_size;
    float e = powf(10.0f, -(float)precision);

    /* alloc */
    float (*M)[N][N] = malloc(sizeof(*M));
    float (*x)[N] = malloc(sizeof(*x));
    float (*b)[N] = malloc(sizeof(*x));
    memset(M, 0, sizeof(*M));
    memset(M, 0, sizeof(*x));
    memset(M, 0, sizeof(*b));

    /* init */
    for (int i = 0; i < N; i++) {
        (*x)[econ->industries[i].output.product->id] = econ->industries[i].labour_direct;

        for(int j = 0; j < econ->industries[i].inputs_size; j++) {
            (*M)[econ->industries[i].output.product->id]
                [econ->industries[i].inputs[j].product->id] =
                    econ->industries[i].inputs[j].quant_gross;
        }

        (*M)[econ->industries[i].output.product->id]
            [econ->industries[i].output.product->id] -=
                econ->industries[i].output.quant_gross;

        (*b)[econ->industries[i].output.product->id] =
            -econ->industries[i].labour_direct;
    }

    /* solve */
    int converged = 0, iter = 0;

    while (!converged) {
        converged = 1;

        for(int i = 0; i < N; ++i) {
            float s = 0.0f;

            for(int j = 0; j < N; ++j) {
                if(j != i) {
                    s += (*M)[i][j] * (*x)[j];
                }
            }

            float xp = (1.0f / (*M)[i][i]) * ((*b)[i] - s);
            converged &= fabsf((*x)[i] - xp) < e;
            (*x)[i] = xp;
        }

        iter++;
    }

    for (int i = 0; i < N; i++)
        econ->industries[i].labour_content = (*x)[i];

    return iter;
}

/* List Gauss-Seidel method */
int econ_labour_content_solve2(econ_t *econ, uint32_t precision) {
    float e = powf(10.0f, -(float)precision);

    /* init */
    for (int i = 0; i < econ->industries_size; ++i)
        econ->industries[i].labour_content = econ->industries[i].labour_direct;

    /* solve */
    int converged = 0, iter = 0;

    while (!converged) {
        converged = 1;

        for (int i = 0; i < econ->industries_size; i++) {
            float s = 0.0f;

            float self_input = 0.0f;

            for (int j = 0; j < econ->industries[i].inputs_size; j++) {
                if (econ->industries[i].output.product->id !=
                    econ->industries[i].inputs[j].product->id)
                {
                    s += econ->industries[i].inputs[j].quant_gross *
                        econ->industries[econ->industries[i].inputs[j].product->id].labour_content;
                } else {
                    self_input = econ->industries[i].inputs[j].quant_gross;
                }
            }

            self_input -= econ->industries[i].output.quant_gross;

            float xp = (1.0f / self_input) *
                (-econ->industries[i].labour_direct - s);
            converged &= fabsf(econ->industries[i].labour_content - xp) < e;
            econ->industries[i].labour_content = xp;
        }

        iter++;
    }

    return iter;
}

