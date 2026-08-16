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

*/

#include <stdint.h>
#include <stddef.h>


typedef enum {
    UNIT_METER,
    UNIT_METER_SQ,
    UNIT_METER_CU,
    UNIT_KILOGRAM,
    UNIT_LITER,
    UNIT_TON, /* metric */
    UNIT_UNITS
} unit_id_t;

typedef enum {
    LABOUR_WORKERS_WEEK,
    LABOUR_WORK_HOURS
} labour_unit_t;


typedef struct {
    uint32_t    id;
    char       *name_long;
    char       *name_short;
} unit_t;

extern const unit_t units[];

typedef struct {
    uint32_t        id;
    char           *name;
    const unit_t   *unit;
} product_t;

typedef struct {
    product_t  *product;
    uint32_t    quant_gross;
    uint32_t    quant_net; /* opt */
} quantity_t;

typedef struct {
    uint32_t    id;
    char       *name;
    /* input */
    uint32_t    labour_direct;
    quantity_t *inputs;
    size_t      inputs_size, inputs_capacity;
    /* output */
    quantity_t  output;

    uint32_t    labour_total;
} industry_io_t;

typedef struct {
    product_t      *products;
    size_t          products_size, products_capacity;
    industry_io_t  *industries;
    size_t          industries_size, industries_capacity;

    uint32_t        workweek_hours;
} econ_t;


econ_t *econ_new(uint32_t workweek_hours);

void econ_destroy(econ_t *econ);

product_t *econ_product_new(econ_t *econ, uint32_t id, const char *name,
    uint32_t unit_id);

industry_io_t *econ_industry_new(econ_t *econ, uint32_t id, const char *name, 
    uint32_t direct_labour, labour_unit_t labour_unit);

quantity_t *econ_industry_input_add(industry_io_t *ind, product_t *product,
    uint32_t quant);

void econ_industry_output_add(industry_io_t *ind, product_t *product,
    uint32_t quant_gross, uint32_t quant_net);

uint32_t econ_labour_total(econ_t *econ, labour_unit_t unit);

