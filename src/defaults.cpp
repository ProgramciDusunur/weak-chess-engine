#include "defaults.hpp"
// === Parameter Definitions ===
SearchParam tt_size("Hash", 64, 1, 16384, 1);
SearchParam threads("Threads", 1, 1, 1, 1);
SearchParam reverse_futility_margin("ReverseFutilityMargin", 60, 30, 100, 10);
SearchParam reverse_futility_depth("ReverseFutilityDepth", 8, 4, 10, 1);
SearchParam null_move_depth("NullMoveDepth", 2, 1, 5, 1);
SearchParam null_move_reduction("NullMoveReduction", 4, 2, 6, 1);
SearchParam late_move_reduction_depth("LateMoveReductionDepth", 3, 1, 6, 1);
SearchParam late_move_reduction_base("LateMoveReductionBase", 75, 30, 120, 15);
SearchParam late_move_reduction_multiplier("LateMoveReductionMultiplier", 40, 10, 70, 7);
SearchParam aspiration_window_depth("AspirationWindowDepth", 4, 2, 8, 1);
SearchParam aspiration_window_delta("AspirationWindowDelta", 10, 5, 30, 2);
SearchParam aspiration_widening_factor("AspirationWideningFactor", 30, 1, 200, 20);
SearchParam internal_iterative_reduction_depth("InternalIterativeReductionDepth", 7, 2, 10, 1);
SearchParam see_noisy_margin("SeeNoisyMargin", -95, -120, -30, 6);
SearchParam see_quiet_margin("SeeQuietMargin", -60, -120, -30, 6);
SearchParam history_bonus_base("HistoryBonusBase", 150, -384, 768, 64);
SearchParam history_bonus_mul_linear("HistoryBonusMulLinear", 200, 64, 384, 32);
SearchParam history_bonus_mul_quad("HistoryBonusMulQuad", 500, 1, 1536, 64);
SearchParam history_malus_base("HistoryMalusBase", 50, -384, 768, 64);
SearchParam history_malus_mul_linear("HistoryMalusMulLinear", 280, 64, 384, 32);
SearchParam history_malus_mul_quad("HistoryMalusMulQuad", 300, 1, 1536, 64);
SearchParam razoring_max_depth("RazoringMaxDepth", 3, 1, 5, 1);
SearchParam razoring_base("RazoringBase", 500, -384, 768, 64);
SearchParam razoring_linear_mul("RazoringLinearMul", 0, -64, 384, 32);
SearchParam razoring_quad_mul("RazoringQuadMul", 300, 1, 1536, 64);
SearchParam tempo("Tempo", 5, 0, 20, 3);
