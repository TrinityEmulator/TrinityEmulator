
#
# Bit Count
# ---------
#
/opt/img/bin/mips-img-linux-gnu-gcc       bit-count/test_msa_nloc_b.c          \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_nloc_b_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc       bit-count/test_msa_nloc_h.c          \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_nloc_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc       bit-count/test_msa_nloc_w.c          \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_nloc_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc       bit-count/test_msa_nloc_d.c          \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_nloc_d_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc       bit-count/test_msa_nlzc_b.c          \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_nlzc_b_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc       bit-count/test_msa_nlzc_h.c          \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_nlzc_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc       bit-count/test_msa_nlzc_w.c          \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_nlzc_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc       bit-count/test_msa_nlzc_d.c          \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_nlzc_d_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc       bit-count/test_msa_pcnt_b.c          \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_pcnt_b_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc       bit-count/test_msa_pcnt_h.c          \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_pcnt_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc       bit-count/test_msa_pcnt_w.c          \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_pcnt_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc       bit-count/test_msa_pcnt_d.c          \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_pcnt_d_64r6eb

#
# Bit move
# --------
#
/opt/img/bin/mips-img-linux-gnu-gcc         bit-move/test_msa_binsl_b.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o   /tmp/test_msa_binsl_b_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc         bit-move/test_msa_binsl_h.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o   /tmp/test_msa_binsl_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc         bit-move/test_msa_binsl_w.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o   /tmp/test_msa_binsl_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc         bit-move/test_msa_binsl_d.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o   /tmp/test_msa_binsl_d_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc         bit-move/test_msa_binsr_b.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o   /tmp/test_msa_binsr_b_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc         bit-move/test_msa_binsr_h.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o   /tmp/test_msa_binsr_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc         bit-move/test_msa_binsr_w.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o   /tmp/test_msa_binsr_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc         bit-move/test_msa_binsr_d.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o   /tmp/test_msa_binsr_d_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc         bit-move/test_msa_bmnz_v.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o   /tmp/test_msa_bmnz_v_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc         bit-move/test_msa_bmz_v.c          \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o   /tmp/test_msa_bmz_v_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc         bit-move/test_msa_bsel_v.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o   /tmp/test_msa_bsel_v_64r6eb

#
# Bit Set
# -------
#
/opt/img/bin/mips-img-linux-gnu-gcc         bit-set/test_msa_bclr_b.c          \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_bclr_b_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc         bit-set/test_msa_bclr_h.c          \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_bclr_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc         bit-set/test_msa_bclr_w.c          \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_bclr_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc         bit-set/test_msa_bclr_d.c          \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_bclr_d_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc         bit-set/test_msa_bneg_b.c          \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_bneg_b_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc         bit-set/test_msa_bneg_h.c          \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_bneg_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc         bit-set/test_msa_bneg_w.c          \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_bneg_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc         bit-set/test_msa_bneg_d.c          \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_bneg_d_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc         bit-set/test_msa_bset_b.c          \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_bset_b_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc         bit-set/test_msa_bset_h.c          \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_bset_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc         bit-set/test_msa_bset_w.c          \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_bset_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc         bit-set/test_msa_bset_d.c          \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_bset_d_64r6eb

#
# Fixed Multiply
# --------------
#
/opt/img/bin/mips-img-linux-gnu-gcc  fixed-multiply/test_msa_madd_q_h.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_madd_q_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc  fixed-multiply/test_msa_madd_q_w.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_madd_q_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc  fixed-multiply/test_msa_maddr_q_h.c       \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_maddr_q_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc  fixed-multiply/test_msa_maddr_q_w.c       \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_maddr_q_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc  fixed-multiply/test_msa_msub_q_h.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_msub_q_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc  fixed-multiply/test_msa_msub_q_w.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_msub_q_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc  fixed-multiply/test_msa_msubr_q_h.c       \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_msubr_q_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc  fixed-multiply/test_msa_msubr_q_w.c       \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_msubr_q_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc  fixed-multiply/test_msa_mul_q_h.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_mul_q_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc  fixed-multiply/test_msa_mul_q_w.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_mul_q_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc  fixed-multiply/test_msa_mulr_q_h.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_mulr_q_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc  fixed-multiply/test_msa_mulr_q_w.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_mulr_q_w_64r6eb

#
# Float Max Min
# -------------
#
/opt/img/bin/mips-img-linux-gnu-gcc   float-max-min/test_msa_fmax_a_w.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_fmax_a_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc   float-max-min/test_msa_fmax_a_d.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_fmax_a_d_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc   float-max-min/test_msa_fmax_w.c          \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_fmax_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc   float-max-min/test_msa_fmax_d.c          \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_fmax_d_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc   float-max-min/test_msa_fmin_a_w.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_fmin_a_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc   float-max-min/test_msa_fmin_a_d.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_fmin_a_d_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc   float-max-min/test_msa_fmin_w.c          \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_fmin_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc   float-max-min/test_msa_fmin_d.c          \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_fmin_d_64r6eb

#
# Int Add
# -------
#
/opt/img/bin/mips-img-linux-gnu-gcc         int-add/test_msa_add_a_b.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_add_a_b_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc         int-add/test_msa_add_a_h.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_add_a_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc         int-add/test_msa_add_a_w.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_add_a_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc         int-add/test_msa_add_a_d.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_add_a_d_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc         int-add/test_msa_adds_a_b.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_adds_a_b_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc         int-add/test_msa_adds_a_h.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_adds_a_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc         int-add/test_msa_adds_a_w.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_adds_a_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc         int-add/test_msa_adds_a_d.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_adds_a_d_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc         int-add/test_msa_adds_s_b.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_adds_s_b_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc         int-add/test_msa_adds_s_h.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_adds_s_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc         int-add/test_msa_adds_s_w.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_adds_s_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc         int-add/test_msa_adds_s_d.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_adds_s_d_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc         int-add/test_msa_adds_u_b.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_adds_u_b_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc         int-add/test_msa_adds_u_h.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_adds_u_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc         int-add/test_msa_adds_u_w.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_adds_u_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc         int-add/test_msa_adds_u_d.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_adds_u_d_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc         int-add/test_msa_addv_b.c          \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_addv_b_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc         int-add/test_msa_addv_h.c          \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_addv_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc         int-add/test_msa_addv_w.c          \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_addv_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc         int-add/test_msa_addv_d.c          \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_addv_d_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc         int-add/test_msa_hadd_s_h.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_hadd_s_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc         int-add/test_msa_hadd_s_w.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_hadd_s_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc         int-add/test_msa_hadd_s_d.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_hadd_s_d_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc         int-add/test_msa_hadd_u_h.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_hadd_u_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc         int-add/test_msa_hadd_u_w.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_hadd_u_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc         int-add/test_msa_hadd_u_d.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_hadd_u_d_64r6eb

#
# Int Average
# -----------
#
/opt/img/bin/mips-img-linux-gnu-gcc     int-average/test_msa_ave_s_b.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_ave_s_b_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc     int-average/test_msa_ave_s_h.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_ave_s_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc     int-average/test_msa_ave_s_w.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_ave_s_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc     int-average/test_msa_ave_s_d.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_ave_s_d_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc     int-average/test_msa_ave_u_b.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_ave_u_b_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc     int-average/test_msa_ave_u_h.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_ave_u_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc     int-average/test_msa_ave_u_w.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_ave_u_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc     int-average/test_msa_ave_u_d.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_ave_u_d_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc     int-average/test_msa_aver_s_b.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_aver_s_b_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc     int-average/test_msa_aver_s_h.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_aver_s_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc     int-average/test_msa_aver_s_w.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_aver_s_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc     int-average/test_msa_aver_s_d.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_aver_s_d_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc     int-average/test_msa_aver_u_b.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_aver_u_b_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc     int-average/test_msa_aver_u_h.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_aver_u_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc     int-average/test_msa_aver_u_w.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_aver_u_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc     int-average/test_msa_aver_u_d.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_aver_u_d_64r6eb

#
# Int Compare
# -----------
#
/opt/img/bin/mips-img-linux-gnu-gcc     int-compare/test_msa_ceq_b.c           \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_ceq_b_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc     int-compare/test_msa_ceq_h.c           \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_ceq_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc     int-compare/test_msa_ceq_w.c           \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_ceq_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc     int-compare/test_msa_ceq_d.c           \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_ceq_d_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc     int-compare/test_msa_cle_s_b.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_cle_s_b_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc     int-compare/test_msa_cle_s_h.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_cle_s_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc     int-compare/test_msa_cle_s_w.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_cle_s_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc     int-compare/test_msa_cle_s_d.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_cle_s_d_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc     int-compare/test_msa_cle_u_b.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_cle_u_b_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc     int-compare/test_msa_cle_u_h.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_cle_u_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc     int-compare/test_msa_cle_u_w.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_cle_u_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc     int-compare/test_msa_cle_u_d.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_cle_u_d_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc     int-compare/test_msa_clt_s_b.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_clt_s_b_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc     int-compare/test_msa_clt_s_h.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_clt_s_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc     int-compare/test_msa_clt_s_w.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_clt_s_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc     int-compare/test_msa_clt_s_d.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_clt_s_d_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc     int-compare/test_msa_clt_u_b.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_clt_u_b_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc     int-compare/test_msa_clt_u_h.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_clt_u_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc     int-compare/test_msa_clt_u_w.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_clt_u_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc     int-compare/test_msa_clt_u_d.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_clt_u_d_64r6eb

#
# Int Divide
# ----------
#
/opt/img/bin/mips-img-linux-gnu-gcc      int-divide/test_msa_div_s_b.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_div_s_b_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc      int-divide/test_msa_div_s_h.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_div_s_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc      int-divide/test_msa_div_s_w.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_div_s_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc      int-divide/test_msa_div_s_d.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_div_s_d_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc      int-divide/test_msa_div_u_b.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_div_u_b_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc      int-divide/test_msa_div_u_h.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_div_u_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc      int-divide/test_msa_div_u_w.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_div_u_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc      int-divide/test_msa_div_u_d.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_div_u_d_64r6eb

#
# Int Dot Product
# ---------------
#
/opt/img/bin/mips-img-linux-gnu-gcc int-dot-product/test_msa_dotp_s_h.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_dotp_s_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc int-dot-product/test_msa_dotp_s_w.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_dotp_s_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc int-dot-product/test_msa_dotp_s_d.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_dotp_s_d_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc int-dot-product/test_msa_dotp_u_h.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_dotp_u_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc int-dot-product/test_msa_dotp_u_w.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_dotp_u_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc int-dot-product/test_msa_dotp_u_d.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_dotp_u_d_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc int-dot-product/test_msa_dpadd_s_h.c       \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_dpadd_s_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc int-dot-product/test_msa_dpadd_s_w.c       \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_dpadd_s_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc int-dot-product/test_msa_dpadd_s_d.c       \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_dpadd_s_d_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc int-dot-product/test_msa_dpadd_u_h.c       \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_dpadd_u_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc int-dot-product/test_msa_dpadd_u_w.c       \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_dpadd_u_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc int-dot-product/test_msa_dpadd_u_d.c       \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_dpadd_u_d_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc int-dot-product/test_msa_dpsub_s_h.c       \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_dpsub_s_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc int-dot-product/test_msa_dpsub_s_w.c       \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_dpsub_s_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc int-dot-product/test_msa_dpsub_s_d.c       \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_dpsub_s_d_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc int-dot-product/test_msa_dpsub_u_h.c       \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_dpsub_u_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc int-dot-product/test_msa_dpsub_u_w.c       \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_dpsub_u_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc int-dot-product/test_msa_dpsub_u_d.c       \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_dpsub_u_d_64r6eb

#
# Int Max Min
# -----------
#
/opt/img/bin/mips-img-linux-gnu-gcc     int-max-min/test_msa_max_a_b.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_max_a_b_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc     int-max-min/test_msa_max_a_h.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_max_a_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc     int-max-min/test_msa_max_a_w.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_max_a_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc     int-max-min/test_msa_max_a_d.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_max_a_d_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc     int-max-min/test_msa_max_s_b.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_max_s_b_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc     int-max-min/test_msa_max_s_h.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_max_s_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc     int-max-min/test_msa_max_s_w.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_max_s_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc     int-max-min/test_msa_max_s_d.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_max_s_d_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc     int-max-min/test_msa_max_u_b.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_max_u_b_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc     int-max-min/test_msa_max_u_h.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_max_u_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc     int-max-min/test_msa_max_u_w.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_max_u_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc     int-max-min/test_msa_max_u_d.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_max_u_d_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc     int-max-min/test_msa_min_a_b.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_min_a_b_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc     int-max-min/test_msa_min_a_h.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_min_a_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc     int-max-min/test_msa_min_a_w.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_min_a_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc     int-max-min/test_msa_min_a_d.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_min_a_d_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc     int-max-min/test_msa_min_s_b.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_min_s_b_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc     int-max-min/test_msa_min_s_h.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_min_s_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc     int-max-min/test_msa_min_s_w.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_min_s_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc     int-max-min/test_msa_min_s_d.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_min_s_d_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc     int-max-min/test_msa_min_u_b.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_min_u_b_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc     int-max-min/test_msa_min_u_h.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_min_u_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc     int-max-min/test_msa_min_u_w.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_min_u_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc     int-max-min/test_msa_min_u_d.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_min_u_d_64r6eb

#
# Int Modulo
# ----------
#
/opt/img/bin/mips-img-linux-gnu-gcc      int-modulo/test_msa_mod_s_b.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_mod_s_b_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc      int-modulo/test_msa_mod_s_h.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_mod_s_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc      int-modulo/test_msa_mod_s_w.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_mod_s_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc      int-modulo/test_msa_mod_s_d.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_mod_s_d_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc      int-modulo/test_msa_mod_u_b.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_mod_u_b_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc      int-modulo/test_msa_mod_u_h.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_mod_u_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc      int-modulo/test_msa_mod_u_w.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_mod_u_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc      int-modulo/test_msa_mod_u_d.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_mod_u_d_64r6eb

#
# Int Multiply
# ------------
#
/opt/img/bin/mips-img-linux-gnu-gcc    int-multiply/test_msa_maddv_b.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_maddv_b_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc    int-multiply/test_msa_maddv_h.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_maddv_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc    int-multiply/test_msa_maddv_w.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_maddv_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc    int-multiply/test_msa_maddv_d.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_maddv_d_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc    int-multiply/test_msa_msubv_b.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_msubv_b_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc    int-multiply/test_msa_msubv_h.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_msubv_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc    int-multiply/test_msa_msubv_w.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_msubv_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc    int-multiply/test_msa_msubv_d.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_msubv_d_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc    int-multiply/test_msa_mulv_b.c          \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_mulv_b_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc    int-multiply/test_msa_mulv_h.c          \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_mulv_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc    int-multiply/test_msa_mulv_w.c          \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_mulv_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc    int-multiply/test_msa_mulv_d.c          \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_mulv_d_64r6eb

#
# Int Subtract
# ------------
#
/opt/img/bin/mips-img-linux-gnu-gcc    int-subtract/test_msa_asub_s_b.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_asub_s_b_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc    int-subtract/test_msa_asub_s_h.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_asub_s_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc    int-subtract/test_msa_asub_s_w.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_asub_s_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc    int-subtract/test_msa_asub_s_d.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_asub_s_d_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc    int-subtract/test_msa_asub_u_b.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_asub_u_b_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc    int-subtract/test_msa_asub_u_h.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_asub_u_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc    int-subtract/test_msa_asub_u_w.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_asub_u_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc    int-subtract/test_msa_asub_u_d.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_asub_u_d_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc    int-subtract/test_msa_hsub_s_h.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_hsub_s_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc    int-subtract/test_msa_hsub_s_w.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_hsub_s_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc    int-subtract/test_msa_hsub_s_d.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_hsub_s_d_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc    int-subtract/test_msa_hsub_u_h.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_hsub_u_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc    int-subtract/test_msa_hsub_u_w.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_hsub_u_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc    int-subtract/test_msa_hsub_u_d.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_hsub_u_d_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc    int-subtract/test_msa_subs_s_b.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_subs_s_b_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc    int-subtract/test_msa_subs_s_h.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_subs_s_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc    int-subtract/test_msa_subs_s_w.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_subs_s_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc    int-subtract/test_msa_subs_s_d.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_subs_s_d_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc    int-subtract/test_msa_subs_u_b.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_subs_u_b_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc    int-subtract/test_msa_subs_u_h.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_subs_u_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc    int-subtract/test_msa_subs_u_w.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_subs_u_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc    int-subtract/test_msa_subs_u_d.c        \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_subs_u_d_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc    int-subtract/test_msa_subsus_u_b.c      \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_subsus_u_b_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc    int-subtract/test_msa_subsus_u_h.c      \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_subsus_u_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc    int-subtract/test_msa_subsus_u_w.c      \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_subsus_u_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc    int-subtract/test_msa_subsus_u_d.c      \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_subsus_u_d_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc    int-subtract/test_msa_subsuu_s_b.c      \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_subsuu_s_b_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc    int-subtract/test_msa_subsuu_s_h.c      \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_subsuu_s_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc    int-subtract/test_msa_subsuu_s_w.c      \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_subsuu_s_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc    int-subtract/test_msa_subsuu_s_d.c      \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_subsuu_s_d_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc    int-subtract/test_msa_subv_b.c          \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_subv_b_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc    int-subtract/test_msa_subv_h.c          \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_subv_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc    int-subtract/test_msa_subv_w.c          \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_subv_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc    int-subtract/test_msa_subv_d.c          \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_subv_d_64r6eb

#
# Interleave
# ----------
#
/opt/img/bin/mips-img-linux-gnu-gcc      interleave/test_msa_ilvev_b.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_ilvev_b_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc      interleave/test_msa_ilvev_h.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_ilvev_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc      interleave/test_msa_ilvev_w.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_ilvev_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc      interleave/test_msa_ilvev_d.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_ilvev_d_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc      interleave/test_msa_ilvod_b.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_ilvod_b_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc      interleave/test_msa_ilvod_h.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_ilvod_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc      interleave/test_msa_ilvod_w.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_ilvod_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc      interleave/test_msa_ilvod_d.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_ilvod_d_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc      interleave/test_msa_ilvl_b.c          \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_ilvl_b_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc      interleave/test_msa_ilvl_h.c          \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_ilvl_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc      interleave/test_msa_ilvl_w.c          \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_ilvl_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc      interleave/test_msa_ilvl_d.c          \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_ilvl_d_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc      interleave/test_msa_ilvr_b.c          \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_ilvr_b_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc      interleave/test_msa_ilvr_h.c          \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_ilvr_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc      interleave/test_msa_ilvr_w.c          \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_ilvr_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc      interleave/test_msa_ilvr_d.c          \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_ilvr_d_64r6eb

#
# Logic
# -----
#
/opt/img/bin/mips-img-linux-gnu-gcc           logic/test_msa_and_v.c           \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_and_v_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc           logic/test_msa_nor_v.c           \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_nor_v_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc           logic/test_msa_or_v.c            \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_or_v_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc           logic/test_msa_xor_v.c           \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_xor_v_64r6eb

#
# Move
# ----
#
/opt/img/bin/mips-img-linux-gnu-gcc            move/test_msa_move_v.c          \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_move_v_64r6eb

#
# Pack
# ----
#
/opt/img/bin/mips-img-linux-gnu-gcc            pack/test_msa_pckev_b.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_pckev_b_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc            pack/test_msa_pckev_h.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_pckev_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc            pack/test_msa_pckev_w.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_pckev_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc            pack/test_msa_pckev_d.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_pckev_d_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc            pack/test_msa_pckod_b.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_pckod_b_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc            pack/test_msa_pckod_h.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_pckod_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc            pack/test_msa_pckod_w.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_pckod_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc            pack/test_msa_pckod_d.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_pckod_d_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc            pack/test_msa_vshf_b.c          \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_vshf_b_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc            pack/test_msa_vshf_h.c          \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_vshf_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc            pack/test_msa_vshf_w.c          \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_vshf_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc            pack/test_msa_vshf_d.c          \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o  /tmp/test_msa_vshf_d_64r6eb

#
# Shift
# -----
#
/opt/img/bin/mips-img-linux-gnu-gcc            shift/test_msa_sll_b.c          \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o   /tmp/test_msa_sll_b_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc            shift/test_msa_sll_h.c          \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o   /tmp/test_msa_sll_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc            shift/test_msa_sll_w.c          \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o   /tmp/test_msa_sll_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc            shift/test_msa_sll_d.c          \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o   /tmp/test_msa_sll_d_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc            shift/test_msa_sra_b.c          \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o   /tmp/test_msa_sra_b_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc            shift/test_msa_sra_h.c          \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o   /tmp/test_msa_sra_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc            shift/test_msa_sra_w.c          \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o   /tmp/test_msa_sra_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc            shift/test_msa_sra_d.c          \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o   /tmp/test_msa_sra_d_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc            shift/test_msa_srar_b.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o   /tmp/test_msa_srar_b_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc            shift/test_msa_srar_h.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o   /tmp/test_msa_srar_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc            shift/test_msa_srar_w.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o   /tmp/test_msa_srar_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc            shift/test_msa_srar_d.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o   /tmp/test_msa_srar_d_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc            shift/test_msa_srl_b.c          \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o   /tmp/test_msa_srl_b_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc            shift/test_msa_srl_h.c          \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o   /tmp/test_msa_srl_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc            shift/test_msa_srl_w.c          \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o   /tmp/test_msa_srl_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc            shift/test_msa_srl_d.c          \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o   /tmp/test_msa_srl_d_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc            shift/test_msa_srlr_b.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o   /tmp/test_msa_srlr_b_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc            shift/test_msa_srlr_h.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o   /tmp/test_msa_srlr_h_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc            shift/test_msa_srlr_w.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o   /tmp/test_msa_srlr_w_64r6eb
/opt/img/bin/mips-img-linux-gnu-gcc            shift/test_msa_srlr_d.c         \
-EB -static -mabi=64 -march=mips64r6 -mmsa -o   /tmp/test_msa_srlr_d_64r6eb
