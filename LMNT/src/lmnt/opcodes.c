#include "lmnt/opcodes.h"

#include "lmnt/interpreter.h"
#include "lmnt/ops_fncall.h"
#include "lmnt/ops_math.h"
#include "lmnt/ops_misc.h"
#include "lmnt/ops_trig.h"
#include "lmnt/ops_util.h"

LMNT_ATTR_FAST lmnt_op_fn lmnt_op_functions[LMNT_OP_END] = {
    0, // noop
    lmnt_op_assignss,
    lmnt_op_assignvv,
    lmnt_op_assignsv,
    lmnt_op_assigniis,
    lmnt_op_assignibs,
    lmnt_op_assigniiv,
    lmnt_op_assignibv,
    lmnt_op_addss,
    lmnt_op_addvv,
    lmnt_op_subss,
    lmnt_op_subvv,
    lmnt_op_mulss,
    lmnt_op_mulvv,
    lmnt_op_divss,
    lmnt_op_divvv,
    lmnt_op_modss,
    lmnt_op_modvv,
    lmnt_op_sin,
    lmnt_op_cos,
    lmnt_op_tan,
    lmnt_op_asin,
    lmnt_op_acos,
    lmnt_op_atan,
    lmnt_op_sincos,
    lmnt_op_powss,
    lmnt_op_powvv,
    lmnt_op_powvs,
    lmnt_op_sqrts,
    lmnt_op_sqrtv,
    lmnt_op_abss,
    lmnt_op_absv,
    lmnt_op_sumv,
    lmnt_op_minss,
    lmnt_op_minvv,
    lmnt_op_maxss,
    lmnt_op_maxvv,
    lmnt_op_minvs,
    lmnt_op_maxvs,
    lmnt_op_floors,
    lmnt_op_floorv,
    lmnt_op_rounds,
    lmnt_op_roundv,
    lmnt_op_ceils,
    lmnt_op_ceilv,
    lmnt_op_indexdis,
    lmnt_op_indexsss,
    lmnt_op_indexdss,
    lmnt_op_indexdsd,
    lmnt_op_extcall,
};

const lmnt_op_info lmnt_opcode_info[LMNT_OP_END] = {
    { "NOOP",      LMNT_OPERAND_UNUSED,  LMNT_OPERAND_UNUSED,  LMNT_OPERAND_UNUSED  },
    { "ASSIGNSS",  LMNT_OPERAND_STACK1,  LMNT_OPERAND_UNUSED,  LMNT_OPERAND_STACK1  },
    { "ASSIGNVV",  LMNT_OPERAND_STACK4,  LMNT_OPERAND_UNUSED,  LMNT_OPERAND_STACK4  },
    { "ASSIGNSV",  LMNT_OPERAND_STACK1,  LMNT_OPERAND_UNUSED,  LMNT_OPERAND_STACK4  },
    { "ASSIGNIIS", LMNT_OPERAND_IMM,     LMNT_OPERAND_IMM,     LMNT_OPERAND_STACK1  },
    { "ASSIGNIBS", LMNT_OPERAND_IMM,     LMNT_OPERAND_IMM,     LMNT_OPERAND_STACK1  },
    { "ASSIGNIIV", LMNT_OPERAND_IMM,     LMNT_OPERAND_IMM,     LMNT_OPERAND_STACK4  },
    { "ASSIGNIBV", LMNT_OPERAND_IMM,     LMNT_OPERAND_IMM,     LMNT_OPERAND_STACK4  },
    { "ADDSS",     LMNT_OPERAND_STACK1,  LMNT_OPERAND_STACK1,  LMNT_OPERAND_STACK1  },
    { "ADDVV",     LMNT_OPERAND_STACK4,  LMNT_OPERAND_STACK4,  LMNT_OPERAND_STACK4  },
    { "SUBSS",     LMNT_OPERAND_STACK1,  LMNT_OPERAND_STACK1,  LMNT_OPERAND_STACK1  },
    { "SUBVV",     LMNT_OPERAND_STACK4,  LMNT_OPERAND_STACK4,  LMNT_OPERAND_STACK4  },
    { "MULSS",     LMNT_OPERAND_STACK1,  LMNT_OPERAND_STACK1,  LMNT_OPERAND_STACK1  },
    { "MULVV",     LMNT_OPERAND_STACK4,  LMNT_OPERAND_STACK4,  LMNT_OPERAND_STACK4  },
    { "DIVSS",     LMNT_OPERAND_STACK1,  LMNT_OPERAND_STACK1,  LMNT_OPERAND_STACK1  },
    { "DIVVV",     LMNT_OPERAND_STACK4,  LMNT_OPERAND_STACK4,  LMNT_OPERAND_STACK4  },
    { "MODSS",     LMNT_OPERAND_STACK1,  LMNT_OPERAND_STACK1,  LMNT_OPERAND_STACK1  },
    { "MODVV",     LMNT_OPERAND_STACK4,  LMNT_OPERAND_STACK4,  LMNT_OPERAND_STACK4  },
    { "SIN",       LMNT_OPERAND_STACK1,  LMNT_OPERAND_UNUSED,  LMNT_OPERAND_STACK1  },
    { "COS",       LMNT_OPERAND_STACK1,  LMNT_OPERAND_UNUSED,  LMNT_OPERAND_STACK1  },
    { "TAN",       LMNT_OPERAND_STACK1,  LMNT_OPERAND_UNUSED,  LMNT_OPERAND_STACK1  },
    { "ASIN",      LMNT_OPERAND_STACK1,  LMNT_OPERAND_UNUSED,  LMNT_OPERAND_STACK1  },
    { "ACOS",      LMNT_OPERAND_STACK1,  LMNT_OPERAND_UNUSED,  LMNT_OPERAND_STACK1  },
    { "ATAN",      LMNT_OPERAND_STACK1,  LMNT_OPERAND_UNUSED,  LMNT_OPERAND_STACK1  },
    { "SINCOS",    LMNT_OPERAND_STACK1,  LMNT_OPERAND_STACK1,  LMNT_OPERAND_STACK1  },
    { "POWSS",     LMNT_OPERAND_STACK1,  LMNT_OPERAND_STACK1,  LMNT_OPERAND_STACK1  },
    { "POWVV",     LMNT_OPERAND_STACK4,  LMNT_OPERAND_STACK4,  LMNT_OPERAND_STACK4  },
    { "POWVS",     LMNT_OPERAND_STACK4,  LMNT_OPERAND_STACK1,  LMNT_OPERAND_STACK4  },
    { "SQRTS",     LMNT_OPERAND_STACK1,  LMNT_OPERAND_UNUSED,  LMNT_OPERAND_STACK1  },
    { "SQRTV",     LMNT_OPERAND_STACK4,  LMNT_OPERAND_UNUSED,  LMNT_OPERAND_STACK4  },
    { "ABSS",      LMNT_OPERAND_STACK1,  LMNT_OPERAND_UNUSED,  LMNT_OPERAND_STACK1  },
    { "ABSV",      LMNT_OPERAND_STACK4,  LMNT_OPERAND_UNUSED,  LMNT_OPERAND_STACK4  },
    { "SUMV",      LMNT_OPERAND_STACK4,  LMNT_OPERAND_UNUSED,  LMNT_OPERAND_STACK1  },
    { "MINSS",     LMNT_OPERAND_STACK1,  LMNT_OPERAND_STACK1,  LMNT_OPERAND_STACK1  },
    { "MINVV",     LMNT_OPERAND_STACK4,  LMNT_OPERAND_STACK4,  LMNT_OPERAND_STACK4  },
    { "MAXSS",     LMNT_OPERAND_STACK1,  LMNT_OPERAND_STACK1,  LMNT_OPERAND_STACK1  },
    { "MAXVV",     LMNT_OPERAND_STACK4,  LMNT_OPERAND_STACK4,  LMNT_OPERAND_STACK4  },
    { "MINVS",     LMNT_OPERAND_STACK4,  LMNT_OPERAND_STACK1,  LMNT_OPERAND_STACK4  },
    { "MAXVS",     LMNT_OPERAND_STACK4,  LMNT_OPERAND_STACK1,  LMNT_OPERAND_STACK4  },
    { "FLOORS",    LMNT_OPERAND_STACK1,  LMNT_OPERAND_UNUSED,  LMNT_OPERAND_STACK1  },
    { "FLOORV",    LMNT_OPERAND_STACK4,  LMNT_OPERAND_UNUSED,  LMNT_OPERAND_STACK4  },
    { "ROUNDS",    LMNT_OPERAND_STACK1,  LMNT_OPERAND_UNUSED,  LMNT_OPERAND_STACK1  },
    { "ROUNDV",    LMNT_OPERAND_STACK4,  LMNT_OPERAND_UNUSED,  LMNT_OPERAND_STACK4  },
    { "CEILS",     LMNT_OPERAND_STACK1,  LMNT_OPERAND_UNUSED,  LMNT_OPERAND_STACK1  },
    { "CEILV",     LMNT_OPERAND_STACK4,  LMNT_OPERAND_UNUSED,  LMNT_OPERAND_STACK4  },
    { "INDEXDIS",  LMNT_OPERAND_DYNAMIC, LMNT_OPERAND_IMM,     LMNT_OPERAND_STACK1  },
    { "INDEXSSS",  LMNT_OPERAND_STACKN,  LMNT_OPERAND_STACK1,  LMNT_OPERAND_STACK1  },
    { "INDEXDSS",  LMNT_OPERAND_DYNAMIC, LMNT_OPERAND_STACK1,  LMNT_OPERAND_STACK1  },
    { "INDEXDSD",  LMNT_OPERAND_DYNAMIC, LMNT_OPERAND_STACK1,  LMNT_OPERAND_STACK1  },
    { "EXTCALL",   LMNT_OPERAND_DEFPTR,  LMNT_OPERAND_DEFPTR,  LMNT_OPERAND_STACKN  },
};

lmnt_op_fn lmnt_interrupt_functions[LMNT_OP_END] = {
    lmnt_op_interrupt,
    lmnt_op_interrupt,
    lmnt_op_interrupt,
    lmnt_op_interrupt,
    lmnt_op_interrupt,
    lmnt_op_interrupt,
    lmnt_op_interrupt,
    lmnt_op_interrupt,
    lmnt_op_interrupt,
    lmnt_op_interrupt,
    lmnt_op_interrupt,
    lmnt_op_interrupt,
    lmnt_op_interrupt,
    lmnt_op_interrupt,
    lmnt_op_interrupt,
    lmnt_op_interrupt,
    lmnt_op_interrupt,
    lmnt_op_interrupt,
    lmnt_op_interrupt,
    lmnt_op_interrupt,
    lmnt_op_interrupt,
    lmnt_op_interrupt,
    lmnt_op_interrupt,
    lmnt_op_interrupt,
    lmnt_op_interrupt,
    lmnt_op_interrupt,
    lmnt_op_interrupt,
    lmnt_op_interrupt,
    lmnt_op_interrupt,
    lmnt_op_interrupt,
    lmnt_op_interrupt,
    lmnt_op_interrupt,
    lmnt_op_interrupt,
    lmnt_op_interrupt,
    lmnt_op_interrupt,
    lmnt_op_interrupt,
    lmnt_op_interrupt,
    lmnt_op_interrupt,
    lmnt_op_interrupt,
    lmnt_op_interrupt,
    lmnt_op_interrupt,
    lmnt_op_interrupt,
    lmnt_op_interrupt,
    lmnt_op_interrupt,
    lmnt_op_interrupt,
    lmnt_op_interrupt,
    lmnt_op_interrupt,
    lmnt_op_interrupt,
    lmnt_op_interrupt,
    lmnt_op_interrupt,
};
