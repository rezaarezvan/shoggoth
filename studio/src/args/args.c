/**** args.c ****
 *
 *  Copyright (c) 2023 ShogAI - https://shog.ai
 *
 * Part of the Shoggoth project, under the MIT License.
 * See LICENSE file for license information.
 * SPDX-License-Identifier: MIT
 *
 ****/

#include <netlibc/assert.h>
#include <netlibc/error.h>
#include <netlibc/log.h>
#include <netlibc/mem.h>

#include "../utils/utils.h"
#include "args.h"

#include <stdlib.h>

/****U
 * argc and argv are directly passed here.
 * the arguments are parsed and returned as a struct
 *
 ****/
result_t args_parse(const int argc, char **argv) {
  // assert preconditions
  ASSERT_TRUE(argc > 0, NULL);
  ASSERT_TRUE(argv != NULL, NULL);
  ASSERT_TRUE(argv[0] != NULL, NULL);

  args_t *args = ncalloc(1, sizeof(args_t));

  for (int i = 1; i < argc; i++) {
    char *current_arg = argv[i];

    if (current_arg[0] == '-') {
      if (strcmp(current_arg, "--help") == 0 ||
          strcmp(current_arg, "-h") == 0) {
        args->help = true;
      } else if (strcmp(current_arg, "--version") == 0 ||
                 strcmp(current_arg, "-v") == 0) {
        args->version = true;
      } else if (strcmp(current_arg, "-c") == 0) {
        args->set_config = true;
        if (i + 1 < argc) {
          args->config_path = argv[++i];
        } else {
          return ERR("No file was passed to -c flag");
        }
      } else if (strcmp(current_arg, "-r") == 0) {
        args->set_runtime_path = true;
        if (i + 1 < argc) {
          args->runtime_path = argv[++i];

          char *runtime_path = strdup(args->runtime_path);

          if (realpath(runtime_path, args->runtime_path) == NULL) {
            perror("realpath"); // Print an error message if realpath fails
            PANIC("realpath failed");
          }

          nfree(runtime_path);

        } else {
          return ERR("No path was passed to -r flag");
        }
      } else {
        args->has_invalid_flag = true;
        args->invalid_flag = current_arg;
      }
    } else {
      args->has_command = true;
      args->command = current_arg;

      if (i + 1 < argc && argv[i + 1][0] != '-') {
        args->has_command_arg = true;
        args->command_arg = argv[i + 1];

        if (i + 2 < argc) {
          args->has_subcommand_arg = true;
          args->subcommand_arg = argv[i + 2];

          i++;
        }

        i++;
      }
    }
  }

  if (argc == 1) {
    args->no_args = true;
  }

  return OK(args);
}
