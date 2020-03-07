#pragma once

/**
 * @file memory.h
 * @brief Memory for Gamemu
 *
 * @author C. Hölzl, EPFL
 * @date 2019
 */

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief types for Game Boy data (8 bits) and addresses (16 bits)
 */
/* TODO WEEK 05:
 * Définir ici les types addr_t et data_t
 * (et supprimer ces quatre lignes de commentaire).
 */

/**
 * @brief Structure for memory,
 *        Holds a pointer to the memory, its size and if it is writable
 */
/* TODO WEEK 05:
 * Définir ici le type memory_t
 * (et supprimer ces quatre lignes de commentaire).
 */

/**
 * @brief Creates memory structure
 *
 * @param mem memory structure pointer to initialize
 * @param size size of the memory to create
 * @return error code
 */
int mem_create(memory_t* mem, size_t size);

/**
 * @brief Destroys memory structure
 *
 * @param mem memory structure pointer to destroy
 */
void mem_free(memory_t* mem);

#ifdef __cplusplus
}
#endif
