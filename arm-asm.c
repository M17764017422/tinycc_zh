/*************************************************************/
/*
 *  ARM dummy assembler for TCC
 *
 */

#�綨�� TARGET_DEFS_ONLY

#���� CONFIG_TCC_ASM
#���� NB_ASM_REGS 16

ST_FUNC �� g(�� c);
ST_FUNC �� gen_le16(�� c);
ST_FUNC �� gen_le32(�� c);

/*************************************************************/
#��
/*************************************************************/

#���� "tcc.h"

��̬ �� asm_error(��)
{
    tcc_error("ARM asm not implemented.");
}

/* XXX: make it faster ? */
ST_FUNC �� g(�� c)
{
    �� ind1;
    �� (nocode_wanted)
        ����;
    ind1 = ind + 1;
    �� (ind1 > cur_text_section->data_allocated)
        section_realloc(cur_text_section, ind1);
    cur_text_section->data[ind] = c;
    ind = ind1;
}

ST_FUNC �� gen_le16 (�� i)
{
    g(i);
    g(i>>8);
}

ST_FUNC �� gen_le32 (�� i)
{
    gen_le16(i);
    gen_le16(i>>16);
}

ST_FUNC �� gen_expr32(ExprValue *pe)
{
    gen_le32(pe->v);
}

ST_FUNC �� asm_opcode(TCCState *s1, �� opcode)
{
    asm_error();
}

ST_FUNC �� subst_asm_operand(CString *add_str, SValue *sv, �� modifier)
{
    asm_error();
}

/* generate prolog and epilog code for asm statement */
ST_FUNC �� asm_gen_code(ASMOperand *operands, �� nb_operands,
                         �� nb_outputs, �� is_output,
                         uint8_t *clobber_regs,
                         �� out_reg)
{
}

ST_FUNC �� asm_compute_constraints(ASMOperand *operands,
                                    �� nb_operands, �� nb_outputs,
                                    ���� uint8_t *clobber_regs,
                                    �� *pout_reg)
{
}

ST_FUNC �� asm_clobber(uint8_t *clobber_regs, ���� �� *str)
{
    asm_error();
}

ST_FUNC �� asm_parse_regvar (�� t)
{
    asm_error();
    ���� -1;
}

/*************************************************************/
#���� /* ndef TARGET_DEFS_ONLY */
