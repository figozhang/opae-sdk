// Copyright(c) 2018, Intel Corporation
//
// Redistribution  and  use  in source  and  binary  forms,  with  or  without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of  source code  must retain the  above copyright notice,
//   this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the name  of Intel Corporation  nor the names of its contributors
//   may be used to  endorse or promote  products derived  from this  software
//   without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,  BUT NOT LIMITED TO,  THE
// IMPLIED WARRANTIES OF  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED.  IN NO EVENT  SHALL THE COPYRIGHT OWNER  OR CONTRIBUTORS BE
// LIABLE  FOR  ANY  DIRECT,  INDIRECT,  INCIDENTAL,  SPECIAL,  EXEMPLARY,  OR
// CONSEQUENTIAL  DAMAGES  (INCLUDING,  BUT  NOT LIMITED  TO,  PROCUREMENT  OF
// SUBSTITUTE GOODS OR SERVICES;  LOSS OF USE,  DATA, OR PROFITS;  OR BUSINESS
// INTERRUPTION)  HOWEVER CAUSED  AND ON ANY THEORY  OF LIABILITY,  WHETHER IN
// CONTRACT,  STRICT LIABILITY,  OR TORT  (INCLUDING NEGLIGENCE  OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,  EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#include <getopt.h>
#include "fpgainfo.h"
#include "sysinfo.h"
#include "fmeinfo.h"
#include "safe_string/safe_string.h"
#include <opae/fpga.h>
#include <uuid/uuid.h>
#include <inttypes.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/types.h>

/*
 * Print help
 */
void fme_help(void)
{
	printf("\nPrint FME information\n"
	       "        fpgainfo fme [-h]\n"
	       "                -h,--help           Print this help\n"
	       "\n");
}

static int get_flash_ctrl_path(const char *start_path, char *out_path, int size)
{
	if (start_path == NULL || out_path == NULL)
		return -1;
	snprintf_s_s(out_path, size,
				 "%s/spi-altera.*.auto/spi_master/spi*/spi*.*/fpga_flash_ctrl",
				 start_path);
	if (glob_sysfs_path(out_path) != FPGA_OK) {
		return -2;
	}

	return 0;
}

static void print_fme_info(fpga_properties props)
{
	fpga_result result;
	char path[SYSFS_PATH_MAX];
	uint32_t page = 0;
	int offset = 0;
	const char *sysfspath = get_sysfs_path(props, FPGA_DEVICE, NULL);

	fpgainfo_print_common("//****** FME ******//", props);
	if (!sysfspath) {
		fprintf(stderr, "WARNING: sysfs path not found\n");
		return;
	}
	if (0 != get_flash_ctrl_path(sysfspath, path, SYSFS_PATH_MAX)) {
		fprintf(stderr, "WARNING: fpga_flash_ctrl not found\n");
		return;
	}

	offset = strlen(path);
	snprintf_s_s(path+offset, sizeof(path)-offset, "/%s",
				 "fpga_image_load");
	result = fpgainfo_sysfs_read_u32(path, &page);
	if (FPGA_OK != result) {
		fpgainfo_print_err("getting boot page of FPGA", result);
		return;
	}
	printf("%-29s : %s\n", "Boot Page", page == 0 ? "factory" : "user");
}

fpga_result fme_filter(fpga_properties *filter, int argc, char *argv[])
{
	(void)argc;
	(void)argv;
	fpga_result res = FPGA_OK;
	res = fpgaPropertiesSetObjectType(*filter, FPGA_DEVICE);
	fpgainfo_print_err("setting type to FPGA_DEVICE", res);
	return res;
}

fpga_result fme_command(fpga_token *tokens, int num_tokens, int argc,
			char *argv[])
{
	(void)tokens;
	(void)num_tokens;
	(void)argc;
	(void)argv;

	fpga_result res = FPGA_OK;
	fpga_properties props;

	optind = 0;
	struct option longopts[] = {
		{"help", no_argument, NULL, 'h'},
		{0, 0, 0, 0},
	};

	int getopt_ret;
	int option_index;

	while (-1
	       != (getopt_ret = getopt_long(argc, argv, ":h", longopts,
					    &option_index))) {
		const char *tmp_optarg = optarg;

		if ((optarg) && ('=' == *tmp_optarg)) {
			++tmp_optarg;
		}

		switch (getopt_ret) {
		case 'h': /* help */
			fme_help();
			return res;

		case ':': /* missing option argument */
			fprintf(stderr, "Missing option argument\n");
			fme_help();
			return FPGA_INVALID_PARAM;

		case '?':
		default: /* invalid option */
			fprintf(stderr, "Invalid cmdline options\n");
			fme_help();
			return FPGA_INVALID_PARAM;
		}
	}

	int i = 0;
	for (i = 0; i < num_tokens; ++i) {
		res = fpgaGetProperties(tokens[i], &props);
		ON_FPGAINFO_ERR_GOTO(res, out_destroy,
				     "reading properties from token");
		print_fme_info(props);
		fpgaDestroyProperties(&props);
	}

	return res;

out_destroy:
	fpgaDestroyProperties(&props);
	return res;
}
