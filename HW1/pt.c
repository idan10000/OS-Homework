#include <stdio.h>
#include "os.h"

// Goes over all the table and checks if any page isn't empty.
// Returns 1 if all pages are empty, 0 otherwise.
int checkIfEmptyTable(uint32_t *table) {
    int deleteSubTableFlag, i;
    deleteSubTableFlag = 1;
    for (i = 0; i < 128; i++) {
        if (table[i] != 0) {
            deleteSubTableFlag = 0;
            break;
        }
    }
    return deleteSubTableFlag;
}

void page_table_update(uint32_t pt, uint32_t vpn, uint32_t ppn) {
    int deleteFlag, deleteSubTableFlag;
    uint32_t *tableRoot, *secTableRoot, entry1, entry2;
    deleteFlag = ppn == NO_MAPPING;
    // first layer
    tableRoot = phys_to_virt(pt << 12);
    entry1 = vpn >> 10; // get 10 left most bits 10-19

    /* Check if sub page-table is valid,
     * if invalid and deleting end method,
     * if inserting create a new sub page-table using alloc_page_frame*/
    if ((tableRoot[entry1] & 1) == 0) {
        if (deleteFlag)
            return;
        else
            tableRoot[entry1] = (alloc_page_frame() << 12) + 1;
    }

    secTableRoot = phys_to_virt(tableRoot[entry1] - 1); // remove bit valid and get address

    // second layer
    entry2 = (vpn & 0x3FF); // get bits 0-9
    if (deleteFlag) {
        secTableRoot[entry2] = 0;
        // if deleting and no deeper table, check if the table is empty and delete it if so
        deleteSubTableFlag = checkIfEmptyTable(secTableRoot);
        if (deleteSubTableFlag)
            tableRoot[entry1] = 0;
        return;
    }

    secTableRoot[entry2] = (ppn << 12) + 1;
}

uint32_t page_table_query(uint32_t pt, uint32_t vpn) {
    uint32_t *tableRoot, *secTableRoot, entry1, entry2;

    // first layer
    tableRoot = phys_to_virt(pt << 12);
    entry1 = vpn >> 10; // get bits 10-19
    if ((tableRoot[entry1] & 1) == 0) // check if valid, otherwise make it 0
        return NO_MAPPING;

    // second layer
    secTableRoot = phys_to_virt(tableRoot[entry1] - 1); // remove bit valid and get address
    entry2 = (vpn & 0x3FF); // get bits 0-9
    if ((secTableRoot[entry2] & 1) == 0)
        return NO_MAPPING;

    return secTableRoot[entry2] >> 12;
}
