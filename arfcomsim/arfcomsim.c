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
    printf("arfcomsim socialist economic simulator");

    econ_t *econ = econ_new(35);

    econ_product_new(econ, 0, "bread", UNIT_UNITS);
    econ_product_new(econ, 0, "oil", UNIT_METER_CU);

    econ_industry_new(econ, 0, "bread", 2000, LABOUR_WORKERS_WEEK);
    econ_industry_new(econ, 0, "oil", 1000, LABOUR_WORKERS_WEEK);

    printf("economy\n  workweek: %d hours\n  products[%ld]:\n", econ->workweek_hours, econ->products_size);
    for (int i = 0; i < econ->products_size; i++)
        printf("    %d: %s in %s\n", econ->products[i].id,
            econ->products[i].name, econ->products[i].unit->name_short);
    printf("  industries[%ld]:\n", econ->industries_size);
    for (int i = 0; i < econ->industries_size; i++)
        printf("    %d: %s emplys %d people-week\n", econ->industries[i].id,
            econ->industries[i].name, econ->industries[i].labour_direct);

    econ_destroy(econ);
}

