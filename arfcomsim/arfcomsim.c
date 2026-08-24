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

    arfcomsim.c: Entry point

*/

#include <econ.h>

#include <stdio.h>

int main()
{
    printf("arfcomsim socialist economic simulator\n"
        "Copyright (C) 2026  Angel Ruiz Fernandez <arf20@arf20.com>\n"
        "This is free software, and you are welcome to redistribute it\n"
        "under the conditions of the GNU General Publishing License version 3;\n\n");

    econ_t *econ = econ_new(35);

    product_t *prod_bread, *prod_oil;
    prod_bread = econ_product_new(econ, 0, "bread", UNIT_UNITS);
    prod_oil = econ_product_new(econ, 0, "oil", UNIT_METER_CU);

    industry_io_t *ind_bread, *ind_oil;
    ind_bread = econ_industry_new(econ, 0, "bread", 2000, LABOUR_WORKERS_WEEK);
    econ_industry_input_add(ind_bread, prod_oil, 2000);
    econ_industry_output_add(ind_bread, prod_bread, 40000, 40000);

    ind_oil = econ_industry_new(econ, 0, "oil", 1000, LABOUR_WORKERS_WEEK);
    econ_industry_input_add(ind_oil, prod_oil, 500);
    econ_industry_output_add(ind_oil, prod_oil, 3000, 500);

    printf("economy:\n  workweek: %.2f hours\n  total labour: %.2f person-weeks\n"
        "  products[%ld]:\n",
        econ->workweek_hours, econ_labour_total(econ, LABOUR_WORKERS_WEEK),
        econ->products_size);
    for (int i = 0; i < econ->products_size; i++)
        printf("    %d: %s in %s\n", econ->products[i].id,
            econ->products[i].name, econ->products[i].unit->name_short);
    printf("  industries[%ld]:\n", econ->industries_size);
    for (int i = 0; i < econ->industries_size; i++) {
        printf("    %d: %s employs %.2f people-week\n", econ->industries[i].id,
            econ->industries[i].name, econ->industries[i].labour_direct);
        for (int j = 0; j < econ->industries[i].inputs_size; j++)
            printf("      input %s quantity %.2f %s\n",
                econ->industries[i].inputs[j].product->name,
                econ->industries[i].inputs[j].quant_gross,
                econ->industries[i].inputs[j].product->unit->name_short);
        printf("      output %s quantity %.2f gross %.2f net %s\n",
            econ->industries[i].output.product->name,
            econ->industries[i].output.quant_gross,
            econ->industries[i].output.quant_net,
            econ->industries[i].output.product->unit->name_short);
    }

    int iter = econ_labour_content_solve2(econ, 3);

    printf("\nconverged in %d iterations\n", iter);
    for (int i = 0; i < econ->industries_size; i++)
        printf("  %s: %.2f hours/%s\n", econ->industries[i].output.product->name,
            /*econ->workweek_hours* */ econ->industries[i].labour_content,
            econ->industries[i].output.product->unit->name_short);

    econ_destroy(econ);
}

