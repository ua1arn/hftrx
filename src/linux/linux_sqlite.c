/*
 * By RA4ASN
 */

#include "hardware.h"	/* зависящие от процессора функции работы с портами */
#include "formats.h"	// for debug prints
#include "audio.h"

#if LINUX_SUBSYSTEM

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sqlite3.h>

void load_memory_cells(uint32_t * mem, uint8_t cnt)
{
	sqlite3 * db = NULL;
	sqlite3_stmt * stmt;

	int rc = sqlite3_open("/mnt/sd-mmcblk0p1/hftrx_data/memory_cells.db", & db);
	if (rc != SQLITE_OK)
	{
		printf("sqlite3_open: %d\n", rc);
		sqlite3_close(db);
		return;
	}

	rc = sqlite3_prepare_v2(db, "SELECT * FROM memory_cells", -1, & stmt, 0);
	if (rc != SQLITE_OK)
	{
		printf("sqlite3_prepare_v2: %d\n", rc);
		sqlite3_close(db);
		return;
	}

	while (sqlite3_step(stmt) != SQLITE_DONE)
	{
		int id = sqlite3_column_int(stmt, 0);
		int freq = sqlite3_column_int(stmt, 1);
		int s = sqlite3_column_int(stmt, 2);

		if(id < cnt)
			mem[id] = freq;
	}

	sqlite3_finalize(stmt);
	sqlite3_close(db);
}

void write_memory_cells(uint32_t * mem, uint8_t cnt)
{
	sqlite3 * db = NULL;
	sqlite3_stmt * stmt;

	int rc = sqlite3_open("/mnt/sd-mmcblk0p1/hftrx_data/memory_cells.db", & db);
	if (rc != SQLITE_OK)
	{
		printf("sqlite3_open: %d\n", rc);
		sqlite3_close(db);
		return;
	}

	for (int i = 0; i < cnt; i ++)
	{
		char d[60];
		memset(d, 0, sizeof(d));
		local_snprintf_P(d, ARRAY_SIZE(d), "UPDATE memory_cells SET freq = %d WHERE id = %d;", mem[i], i);

		rc = sqlite3_prepare_v2(db, d, -1, & stmt, 0);
		if (rc != SQLITE_OK)
		{
			printf("sqlite3_prepare_v2: %d\n", rc);
			sqlite3_close(db);
			return;
		}

		while (sqlite3_step(stmt) != SQLITE_DONE) {}
	}

	sqlite3_finalize(stmt);
	sqlite3_close(db);
}

#endif /* LINUX_SUBSYSTEM */
