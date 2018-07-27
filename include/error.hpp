#pragma once

#include <stdio.h>
#include <switch.h>
#include "debug.h"

#define PROPAGATE_RESULT_STDOUT(rc_out, desc) if (R_FAILED(rc_out)) { fprintf(stdout, "%s:%u: %s.  Error code: 0x%08x\n", __func__, __LINE__, desc, rc_out); return rc_out; }
#define PROPAGATE_RESULT(rc_out, desc) if (R_FAILED(rc_out)) { fprintf(nxlinkout, "%s:%u: %s.  Error code: 0x%08x\n", __func__, __LINE__, desc, rc_out); return rc_out; }