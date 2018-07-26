#pragma once

#include <stdio.h>
#include <switch.h>
#include "debug.h"

#define PROPAGATE_RESULT_STDOUT(rc, desc) if (R_FAILED(rc)) { fprintf(stdout, "%s%u: %s.  Error code: 0x%08x\n", __func__, __LINE__, desc, rc); return rc; }
#define PROPAGATE_RESULT(rc, desc) if (R_FAILED(rc)) { fprintf(nxlinkout, "%s%u: %s.  Error code: 0x%08x\n", __func__, __LINE__, desc, rc); return rc; }