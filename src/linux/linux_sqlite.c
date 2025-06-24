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

	int rc = sqlite3_open(MEMORYCELLS_DB_FILE, & db);
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

	int rc = sqlite3_open(MEMORYCELLS_DB_FILE, & db);
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

#if defined(NVRAM_TYPE) && (NVRAM_TYPE == NVRAM_TYPE_LINUX)

#define BITFIELD_SIZE NVRAM_END

static uint8_t bitfield[BITFIELD_SIZE];
static sqlite3 *db = NULL;
volatile int need_sync = 0;

int init_database() {
    int rc = sqlite3_open(NVRAM_DB_FILE, &db);
    if (rc) {
        fprintf(stderr, "Error opening database: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    const char *create_table_sql =
        "CREATE TABLE IF NOT EXISTS BitField ("
        "id INTEGER PRIMARY KEY,"
        "data BLOB"
        ");";

    char *err_msg = NULL;
    rc = sqlite3_exec(db, create_table_sql, NULL, NULL, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error creating table: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        db = NULL;
        return rc;
    }

    // Check if a record exists in the table
    sqlite3_stmt *stmt;
    const char *select_sql = "SELECT data FROM BitField WHERE id = 1;";
    rc = sqlite3_prepare_v2(db, select_sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error preparing query: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        db = NULL;
        return rc;
    }

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        // If the record exists, load the data
        const void *blob = sqlite3_column_blob(stmt, 0);
        int blob_size = sqlite3_column_bytes(stmt, 0);
        memcpy(bitfield, blob, blob_size);
    } else {
        // If the record does not exist, create a new one with a bitfield filled with 0xFF
        memset(bitfield, 0xFF, BITFIELD_SIZE); // Fill the bitfield with 0xFF

        const char *insert_sql = "INSERT INTO BitField (id, data) VALUES (1, ?);";
        rc = sqlite3_prepare_v2(db, insert_sql, -1, &stmt, NULL);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "Error preparing query: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            db = NULL;
            return rc;
        }

        sqlite3_bind_blob(stmt, 1, bitfield, BITFIELD_SIZE, SQLITE_STATIC);
        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            fprintf(stderr, "Error executing query: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            db = NULL;
            return rc;
        }
    }

    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

void nvram_sync(void) {
    if (db == NULL) {
        return;
    }

    if (! need_sync)
    	return;

    sqlite3_stmt *stmt;
    const char *update_sql = "UPDATE BitField SET data = ? WHERE id = 1;";
    int rc = sqlite3_prepare_v2(db, update_sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error preparing query: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_blob(stmt, 1, bitfield, BITFIELD_SIZE, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Error executing query: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return;
    }

    sqlite3_finalize(stmt);

    need_sync = 0;
}

void nvram_close(void) {
    if (db != NULL) {
        sqlite3_close(db);
        db = NULL;
    }
}

uint8_t restore_i8(uint16_t addr) {
    return bitfield[addr];
}

uint_fast16_t restore_i16(uint16_t addr) {
    uint16_t value = *(uint16_t *)(bitfield + addr);
    return value; // Little endian
}

uint_fast32_t restore_i24(uint16_t addr) {
    uint32_t value = *(uint32_t *)(bitfield + addr);
    return value & 0x00FFFFFF; // Mask for 24 bits
}

uint_fast32_t restore_i32(uint16_t addr) {
    uint32_t value = *(uint32_t *)(bitfield + addr);
    return value; // Little endian
}

void save_i8(uint16_t addr, uint8_t v) {
    bitfield[addr] = v;
    need_sync = 1;
}

void save_i16(uint16_t addr, uint_fast16_t v) {
    *(uint16_t *)(bitfield + addr) = v; // Little endian
    need_sync = 1;
}

void save_i24(uint16_t addr, uint_fast32_t v) {
    *(uint32_t *)(bitfield + addr) = v & 0x00FFFFFF; // Mask for 24 bits
    need_sync = 1;
}

void save_i32(uint16_t addr, uint_fast32_t v) {
    *(uint32_t *)(bitfield + addr) = v; // Little endian
    need_sync = 1;
}

void nvram_initialize(void)
{
	init_database();
}

#endif /* defined(NVRAM_TYPE) && (NVRAM_TYPE == NVRAM_TYPE_LINUX) */

#endif /* LINUX_SUBSYSTEM */
