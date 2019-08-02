/* RISC-V ELF specific backend routines.
   Copyright (C) 2011-2019 Free Software Foundation, Inc.

   Contributed by Andrew Waterman (andrew@sifive.com).
   Based on MIPS target.

   This file is part of BFD, the Binary File Descriptor library.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING3. If not,
   see <http://www.gnu.org/licenses/>.  */

#include "elf/common.h"
#include "elf/internal.h"

extern reloc_howto_type *
riscv_reloc_name_lookup (bfd *, const char *);

extern reloc_howto_type *
riscv_reloc_type_lookup (bfd *, bfd_reloc_code_real_type);

extern reloc_howto_type *
riscv_elf_rtype_to_howto (bfd *, unsigned int r_type);

#define RISCV_DONT_CARE_VERSION -1

/* The information of architecture attribute.  */
struct riscv_subset_t
{
  const char *name;
  int major_version;
  int minor_version;
  struct riscv_subset_t *next;
};

typedef struct riscv_subset_t riscv_subset_t;

typedef struct {
  riscv_subset_t *head;
  riscv_subset_t *tail;
} riscv_subset_list_t;

extern void
riscv_release_subset_list (riscv_subset_list_t *);

extern void
riscv_add_subset (riscv_subset_list_t *,
		  const char *,
		  int, int);

extern riscv_subset_t *
riscv_lookup_subset (const riscv_subset_list_t *,
		     const char *);

extern riscv_subset_t *
riscv_lookup_subset_version (const riscv_subset_list_t *,
			     const char *,
			     int, int);

typedef enum riscv_isa_ext_class
  {
   RV_ISA_CLASS_UNKNOWN,
   RV_ISA_CLASS_X,
   RV_ISA_CLASS_S,
   RV_ISA_CLASS_SX,
   RV_ISA_CLASS_Z
  } riscv_isa_ext_class_t;

/* Structure used for describing how to parse architecture
   directives other than the base "rvXXimafd...n" set.  */

typedef struct riscv_parse_config
{
  /* Class of the extension. */
  riscv_isa_ext_class_t class;

  /* Lower-case prefix string for error printing
     and internal parser usage, e.g. "sx", "z".  */
  const char *prefix_lower;

  /* Upper-case prefix string for error printing,
     e.g. "SX". Used as "Error: SX extension ..."  */
  const char *prefix_upper;

  /* Predicate which is used for checking whether
     this is a "known" extension. For 'x' and 'sx',
     it always returns true (since they are by
     definition non-standard and cannot be known.  */
  int (*ext_valid_p) (const char *);
} riscv_parse_config_t;

typedef struct riscv_isa_ver {
  /* Is this version a release version?
     If TRUE, ignore the `date' field.
     If FALSE, ignore the `major' and `minor' fields.  */
  bfd_boolean release_p;

  /* If this is a release version, the provide
     the major and minor release version, and refer to it.
     For instance, `$ as -misa-version=2.2'.  */
  unsigned int major;
  unsigned int minor;

  /* If this isn't a release version, then provide
     the ISA draft date instead, and refer to it.
     For instance, `$ as -misa-version=20190604'.  */
  unsigned int date;
} riscv_isa_ver_t;

void
riscv_init_isa_ver_null (riscv_isa_ver_t *isa_ver);

#define RISCV_ISA_VER_NULL_P(X)			\
  (!(X)->major && !(X)->minor)

typedef struct {
  riscv_subset_list_t *subset_list;
  riscv_isa_ver_t *isa_ver;
  void (*error_handler) (const char *,
			 ...) ATTRIBUTE_PRINTF_1;
  unsigned *xlen;
} riscv_parse_subset_t;

typedef struct {
  riscv_isa_ver_t *isa_ver;
  void (*error_handler) (const char *,
			 ...) ATTRIBUTE_PRINTF_1;
} riscv_parse_isa_ver_t;

extern bfd_boolean
riscv_parse_subset (riscv_parse_subset_t *,
		    const char *);

bfd_boolean
riscv_parse_isa_ver (riscv_parse_isa_ver_t *rpiv,
		     const char *misa_ver);

extern const char *
riscv_supported_std_ext (void);

extern void
riscv_release_subset_list (riscv_subset_list_t *);

extern char *
riscv_arch_str (unsigned, const riscv_subset_list_t *);

#define RISCV_STD_Z_EXT_COUNT 4
extern const char * const riscv_std_z_ext_strtab[RISCV_STD_Z_EXT_COUNT];

#define RISCV_STD_S_EXT_COUNT 0
extern const char * const riscv_std_s_ext_strtab[RISCV_STD_S_EXT_COUNT];

int
riscv_std_z_ext_index (const char *ext);

typedef struct riscv_isa_spec {
  /* Version information for this isa spec entry.  */
  const riscv_isa_ver_t ver;

  /* Pointer to a parsing order table, so we parse
     flags on the command line in a manner that
     corresponds to the ISA spec at that particular
     point in time / release version.  */
  const riscv_parse_config_t *parse_order;

  /* sizeof the above table.  */
  int parse_count;
} riscv_isa_spec_t;
