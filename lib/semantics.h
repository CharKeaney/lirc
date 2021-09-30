#pragma once

#include <string.h>

#define MAX_NUMBER_OF_SYMBOL_TABLE_ENTRIES 33 
//3343 

typedef enum symbol_entry_type {
	ENTRY_BLOCK,
	ENTRY_STRUCT,
	ENTRY_VAR,
} symbol_entry_type;

static char *symbol_entry_type_repr[] = {
	[ENTRY_BLOCK] = "block",
	[ENTRY_STRUCT] = "struct",
	[ENTRY_VAR] = "var"
};

typedef struct symbol_entry {
	/* Key Data	*/
	char* name;
	struct symbol_entry *next_entry;
	/* Satellite Data */
	symbol_entry_type entry_type;
	int block_number; /* index of this block in the list of blocks. */
	bool var_is_undeclared; /* is this LLVM IR variable temporary?	*/
	bool var_is_primitive; /* is this LLVM IR variable temporary?	*/
	ast_node* attrs;
} symbol_entry;

typedef struct symbol_table {
	symbol_entry* entries[MAX_NUMBER_OF_SYMBOL_TABLE_ENTRIES];
} symbol_table;

static void print_symbol_table(symbol_table* symtab) {
	printf("Name%*sEntry Type%*sBlock Number%*sTemporary?%*sPrimitive?\n", 
		12, " ", 6, " ", 8, " ", 8, " ");
	printf("----%*s----------%*s------------%*s----------%*s----------\n", 
		12, " ", 6, " ", 8, " ", 8, " ");

	symbol_entry** en = symtab->entries;
	int cap = symtab->entries + MAX_NUMBER_OF_SYMBOL_TABLE_ENTRIES;
	for (; en < cap; en++) {

		symbol_entry *e = *en;
		for (; e ; e = e->next_entry) {
			char buffer[16];
			_itoa_s(e->block_number, buffer, 16, 10);

			switch (e->entry_type) {

				case ENTRY_BLOCK:
					printf("%-16s%-16s%-20s%-18s%-10s\n",
						e->name,
						symbol_entry_type_repr[e->entry_type],
						buffer,
						"",
						"");
					break;

				case ENTRY_VAR:
					printf("%-16s%-16s%-20s%-18s%-10s\n",
						e->name,
						symbol_entry_type_repr[e->entry_type],
						"",
						e->var_is_undeclared ? "true" : "false",
						e->var_is_primitive ? "true" : "false");
					break;

				case ENTRY_STRUCT:
					printf("%-16s%-16s%-20s%-18s%-10s\n",
						e->name,
						"",
						"",
						"",
						"");
					break;
			}
		}
	}
}

static symbol_table* construct_symbol_table() {

	symbol_table* symtab = malloc(sizeof(symbol_table));
	for (int i = 0; i < MAX_NUMBER_OF_SYMBOL_TABLE_ENTRIES; i++) {
		symtab->entries[i] = NULL;
	}
	return symtab;
}

static int hash(char* name) {
	unsigned long sum = 0;
	for (char* c = name; *c != NULL; c++) {
		sum = (sum << 8) + *c;
	}
	int hash = (sum / 3167) % MAX_NUMBER_OF_SYMBOL_TABLE_ENTRIES;
	return hash;
}

static symbol_entry* symbol_table_lookup(symbol_table* table, char* name) {
	printf("symbol_table_lookup: performing lookup for %s.\n", name);
	int bucket_num = hash(name);
	symbol_entry* entry = table->entries[bucket_num];
	while (entry) {
		if (strcmp(name, entry->name) == 0) {
			printf("symbol_table_lookup: hit on lookup for %s.\n", name);
			return entry;
		}
		entry = entry->next_entry;
	}
	printf("symbol_table_lookup: miss on lookup for %s.\n", name);
	return NULL;
}

static void insert_symbol(symbol_table* table, symbol_entry* entry) {
	if (symbol_table_lookup(table, entry->name) != NULL) { return; }
	
	int bucket_num = hash(entry->name);
	
	symbol_entry *found_entry = table->entries[bucket_num];

	if (found_entry == NULL) {
		table->entries[bucket_num] = entry;
		return;
	}

	while (found_entry->next_entry != NULL) {
		found_entry = found_entry->next_entry;
	}
	found_entry->next_entry = entry;
}

static symbol_entry* create_entry(
	char* name,
	symbol_entry_type entry_type,
	int block_number,
	bool var_is_undeclared,
	bool var_is_primitive,
	ast_node* attrs) {

	symbol_entry* entry = (symbol_entry*) malloc(sizeof(symbol_entry));
	entry->name = name;
	entry->entry_type = entry_type;
	entry->next_entry = NULL;
	entry->block_number = block_number;
	entry->var_is_undeclared = var_is_undeclared;
	entry->var_is_primitive = var_is_primitive;
	entry->attrs = attrs;
	return entry;
}

static symbol_entry* create_entry_for_block(char* name, int block_number) {
	return create_entry(name, ENTRY_BLOCK,block_number, NULL, NULL, NULL);
}


static symbol_entry* create_entry_for_var(char* name, bool var_is_undeclared, bool var_is_primitive) {
	return create_entry(name, ENTRY_VAR, NULL, var_is_undeclared, var_is_primitive, NULL);
}

static symbol_entry* create_entry_for_struct(char* name, ast_node* args) {
	return create_entry(name, ENTRY_STRUCT, NULL, NULL, NULL, args);
}

