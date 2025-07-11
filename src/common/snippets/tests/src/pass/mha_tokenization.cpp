// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include <gtest/gtest.h>
#include <pass/mha_tokenization.hpp>
#include <subgraph_mha.hpp>
#include "snippets/pass/tokenization.hpp"
#include "snippets/pass/mha_tokenization.hpp"
#include "snippets/pass/common_optimizations.hpp"
#include "snippets/pass/extract_reshapes_from_mha.hpp"

namespace ov {
namespace test {
namespace snippets {

void TokenizeMHASnippetsTests::run() {
    ASSERT_TRUE(model);
    manager.register_pass<ov::snippets::pass::ExtractReshapesFromMHA>();
    manager.register_pass<ov::snippets::pass::EnumerateNodes>();
    manager.register_pass<ov::snippets::pass::TokenizeMHASnippets>(config);
    manager.register_pass<ov::snippets::pass::CommonOptimizations>(config);
    disable_rt_info_check();
}

TEST_F(TokenizeMHASnippetsTests, smoke_Snippets_MHA_2D_Static) {
    const auto &f = MHA2DFunction(std::vector<PartialShape>{{12, 64}, {64, 12}, {12, 12}, {12, 64}},
                                  std::vector<ov::element::Type>({ov::element::f32, ov::element::f32, ov::element::f32, ov::element::f32}));
    execute_and_validate_function(*this, f);
}

TEST_F(TokenizeMHASnippetsTests, smoke_Snippets_MHA_2D_Dynamic) {
    const auto &f = MHA2DFunction(std::vector<PartialShape>{{-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}},
                                  std::vector<ov::element::Type>({ov::element::f32, ov::element::f32, ov::element::f32, ov::element::f32}));
    execute_and_validate_function(*this, f);
}

TEST_F(TokenizeMHASnippetsTests, smoke_Snippets_MHA_2D_Partially_Dynamic) {
    const auto &f = MHA2DFunction(std::vector<PartialShape>{{12, 64}, {64, 12}, {-1, 12}, {12, 64}},
                                  std::vector<ov::element::Type>({ov::element::f32, ov::element::f32, ov::element::f32, ov::element::f32}));
    execute_and_validate_function(*this, f);
}

TEST_F(TokenizeMHASnippetsTests, smoke_Snippets_MHA_4D) {
    const auto &f = MHAFunction(std::vector<PartialShape>{{1, 128, 12, 64}, {1, 128, 12, 64}, {1, 12, 128, 128}, {1, 128, 12, 64}},
                                std::vector<ov::element::Type>({ov::element::f32, ov::element::f32, ov::element::f32, ov::element::f32}));
    execute_and_validate_function(*this, f);
}

TEST_F(TokenizeMHASnippetsTests, smoke_Snippets_MHA_4D_Dynamic) {
    const auto &f = MHAFunction(std::vector<PartialShape>{{-1, -1, -1, -1}, {-1, -1, -1, -1}, {-1, -1, -1, -1}, {-1, -1, -1, -1}},
                                std::vector<ov::element::Type>({ov::element::f32, ov::element::f32, ov::element::f32, ov::element::f32}), true, false);
    execute_and_validate_function(*this, f);
}

TEST_F(TokenizeMHASnippetsTests, smoke_Snippets_MHA_4D_Partially_Dynamic) {
    const auto &f = MHAFunction(std::vector<PartialShape>{{1, 128, 12, 64}, {1, 128, 12, 64}, {1, 1, -1, 1}, {1, 128, 12, 64}},
                                std::vector<ov::element::Type>({ov::element::f32, ov::element::f32, ov::element::f32, ov::element::f32}), true, false);
    execute_and_validate_function(*this, f);
}

TEST_F(TokenizeMHASnippetsTests, smoke_Snippets_MHA_4D_Partially_Dynamic_Disabled_Dynamic) {
    const auto &f = MHAFunction(std::vector<PartialShape>{{1, 128, 12, 64}, {1, 128, 12, 64}, {1, 1, -1, 1}, {1, 128, 12, 64}},
                                std::vector<ov::element::Type>({ov::element::f32, ov::element::f32, ov::element::f32, ov::element::f32}), true, false);
    model = f.getOriginal();
    config = ov::snippets::pass::SnippetsTokenization::Config{1, std::numeric_limits<size_t>::max(), true, true, false, { 3, 4 }};
    run();
}

TEST_F(TokenizeMHASnippetsTests, smoke_Snippets_MHA_4D_Dynamic_M) {
    const auto &f = MHAFunction(std::vector<PartialShape>{{1, -1, 12, 64}, {1, 128, 12, 64}, {1, 12, -1, 128}, {1, 128, 12, 64}},
                                std::vector<ov::element::Type>({ov::element::f32, ov::element::f32, ov::element::f32, ov::element::f32}), true, false);
    execute_and_validate_function(*this, f);
}

TEST_F(TokenizeMHASnippetsTests, smoke_Snippets_MHA_4D_Dynamic_K) {
    const auto &f = MHAFunction(std::vector<PartialShape>{{1, 128, 12, -1}, {1, 128, 12, -1}, {1, 12, 128, 128}, {1, 128, 12, 64}},
                                std::vector<ov::element::Type>({ov::element::f32, ov::element::f32, ov::element::f32, ov::element::f32}), true, false);
    execute_and_validate_function(*this, f);
}

TEST_F(TokenizeMHASnippetsTests, smoke_Snippets_MHA_3D) {
    const auto &f = MHAFunction(std::vector<PartialShape>{{128, 12, 64}, {128, 12, 64}, {12, 128, 128}, {128, 12, 64}},
                                std::vector<ov::element::Type>({ov::element::f32, ov::element::f32, ov::element::f32, ov::element::f32}));
    execute_and_validate_function(*this, f);
}

TEST_F(TokenizeMHASnippetsTests, smoke_Snippets_MHA_3D_Dynamic) {
    const auto &f = MHAFunction(std::vector<PartialShape>{{-1, -1, -1}, {-1, -1, -1}, {-1, -1, -1}, {-1, -1, -1}},
                                std::vector<ov::element::Type>({ov::element::f32, ov::element::f32, ov::element::f32, ov::element::f32}), true, false);
    execute_and_validate_function(*this, f);
}

TEST_F(TokenizeMHASnippetsTests, smoke_Snippets_MHA_with_MatMul0_Transpose) {
    const auto &f = MHAMatMul0TransposeFunction(std::vector<PartialShape>{{1, 128, 12, 64}, {1, 128, 12, 64}, {1, 12, 128, 128}, {1, 128, 12, 64}},
                                                std::vector<ov::element::Type>({ov::element::f32, ov::element::f32, ov::element::f32, ov::element::f32}));
    execute_and_validate_function(*this, f);
}

TEST_F(TokenizeMHASnippetsTests, smoke_Snippets_MHA_with_MatMul0_Transpose_Dynamic) {
    const auto &f = MHAMatMul0TransposeFunction(std::vector<PartialShape>{{-1, -1, -1, -1}, {-1, -1, -1, -1}, {-1, -1, -1, -1}, {-1, -1, -1, -1}},
                                                std::vector<ov::element::Type>({ov::element::f32, ov::element::f32, ov::element::f32, ov::element::f32}),
                                                false);
    execute_and_validate_function(*this, f);
}

TEST_F(TokenizeMHASnippetsTests, smoke_Snippets_MHA_with_int_Matmuls) {
    const auto &f = MHAINT8MatMulTypeRelaxedFunction(std::vector<PartialShape>{{1, 128, 12, 64}, {1, 128, 12, 64}, {1, 12, 128, 128}, {1, 128, 12, 64}});
    execute_and_validate_function(*this, f);
}

TEST_F(TokenizeMHASnippetsTests, smoke_Snippets_MHA_Transpose_extraction) {
    const auto& f = MHATransposedInputFunction(std::vector<PartialShape>{{1, 128, 12, 64}, {1, 128, 12, 64}, {1, 128, 12, 64}}, true);
    execute_and_validate_function(*this, f);
}

TEST_F(TokenizeMHASnippetsTests, smoke_Snippets_MHA_Dynamic_Transpose_extraction) {
    const auto& f = MHATransposedInputFunction(std::vector<PartialShape>{{-1, -1, -1, -1}, {-1, -1, -1, -1}, {-1, -1, -1, -1}}, true);
    execute_and_validate_function(*this, f);
}

TEST_F(TokenizeMHASnippetsTests, smoke_Snippets_MHA_Transpose_extraction_and_unsupported_existing_transpose) {
    const auto& f = MHATransposedInputFunction(std::vector<PartialShape>{{1, 128, 12, 64}, {1, 12, 64, 128}, {1, 128, 12, 64}}, true,
                                               std::vector<int64_t>{0, 3, 1, 2});
    execute_and_validate_function(*this, f);
}

TEST_F(TokenizeMHASnippetsTests, smoke_Snippets_MHA_Dynamic_Transpose_extraction_and_unsupported_existing_transpose) {
    const auto& f = MHATransposedInputFunction(std::vector<PartialShape>{{-1, -1, -1, -1}, {-1, -1, -1, -1}, {-1, -1, -1, -1}}, true,
                                               std::vector<int64_t>{0, 3, 1, 2});
    execute_and_validate_function(*this, f);
}

TEST_F(TokenizeMHASnippetsTests, smoke_Snippets_MHA_Transpose_fusion) {
    const auto& f = MHATransposedInputFunction(std::vector<PartialShape>{{1, 128, 12, 64}, {1, 64, 128, 12}, {1, 128, 12, 64}}, false,
                                               std::vector<int64_t>{0, 2, 1, 3});
    execute_and_validate_function(*this, f);
}

TEST_F(TokenizeMHASnippetsTests, smoke_Snippets_MHA_Dynamic_Transpose_fusion) {
    const auto& f = MHATransposedInputFunction(std::vector<PartialShape>{{-1, -1, -1, -1}, {-1, -1, -1, -1}, {-1, -1, -1, -1}}, false,
                                               std::vector<int64_t>{0, 2, 1, 3});
    execute_and_validate_function(*this, f);
}

TEST_F(TokenizeMHASnippetsTests, smoke_Snippets_MHA3D_SplitM) {
    const auto& f = MHASplitMFunction(std::vector<PartialShape>{{128, 12, 64}, {128, 12, 64}, {12, 128, 128}, {128, 12, 64}},
                                      std::vector<ov::element::Type>({ov::element::f32, ov::element::f32, ov::element::f32, ov::element::f32}),
                                      std::vector<Shape>{{2, 64, 12, 64}, {12, 1, 64, 128}, {12, 2, 64, 128}, {1, 128, 12, 64}, {128, 12, 64}},
                                      false);
    config.set_concurrency(24);
    execute_and_validate_function(*this, f);
}

TEST_F(TokenizeMHASnippetsTests, smoke_Snippets_MHA3D_SplitM_withMul) {
    const auto& f = MHASplitMFunction(std::vector<PartialShape>{{128, 12, 64}, {128, 12, 64}, {12, 128, 128}, {128, 12, 64}},
                                      std::vector<ov::element::Type>({ov::element::f32, ov::element::f32, ov::element::f32, ov::element::f32}),
                                      std::vector<Shape>{{4, 32, 12, 64}, {12, 1, 64, 128}, {12, 4, 32, 128}, {1, 128, 12, 64}, {128, 12, 64}},
                                      true);
    config.set_concurrency(16);
    execute_and_validate_function(*this, f);
}

TEST_F(TokenizeMHASnippetsTests, smoke_Snippets_MHA4D_SplitM) {
    const auto& f = MHASplitMFunction(std::vector<PartialShape>{{1, 384, 16, 64}, {1, 384, 16, 64}, {1, 1, 1, 384}, {1, 384, 16, 64}},
                                      std::vector<ov::element::Type>({ov::element::f32, ov::element::f32, ov::element::f32, ov::element::f32}),
                                      std::vector<Shape>{{1, 12, 32, 16, 64}, {1, 16, 1, 64, 384}, {1, 1, 1, 1, 384}, {1, 1, 384, 16, 64}, {1, 384, 16, 64}},
                                      false);
    config.set_concurrency(60);
    execute_and_validate_function(*this, f);
}

TEST_F(TokenizeMHASnippetsTests, smoke_Snippets_MHA4D_SplitM_withMul) {
    const auto& f = MHASplitMFunction(std::vector<PartialShape>{{1, 384, 16, 64}, {1, 384, 16, 64}, {1, 1, 1, 384}, {1, 384, 16, 64}},
                                      std::vector<ov::element::Type>({ov::element::f32, ov::element::f32, ov::element::f32, ov::element::f32}),
                                      std::vector<Shape>{{1, 12, 32, 16, 64}, {1, 16, 1, 64, 384}, {1, 1, 1, 1, 384}, {1, 1, 384, 16, 64}, {1, 384, 16, 64}},
                                      true);
    config.set_concurrency(60);
    execute_and_validate_function(*this, f);
}

TEST_F(TokenizeMHASnippetsTests, smoke_Snippets_MHAWOTranspose_SplitM) {
    const auto& f = MHAWOTransposeSplitMFunction(std::vector<PartialShape>{{10, 9216, 128}, {10, 128, 9216}, {10, 9216, 128}},
                                                 std::vector<ov::element::Type>({ov::element::f32, ov::element::f32, ov::element::f32}),
                                                 std::vector<Shape>{{10, 18, 512, 128}, {10, 1, 128, 9216}, {10, 1, 9216, 128}, {10, 9216, 128}});
    config.set_concurrency(18);
    execute_and_validate_function(*this, f);
}

TEST_F(TokenizeMHASnippetsTests, smoke_Snippets_MHA_SplitM_AlmostAllThreads) {
    const auto& f = MHAWOTransposeSplitMFunction(std::vector<PartialShape>{{5, 30, 32}, {5, 32, 30}, {5, 30, 32}},
                                                 std::vector<ov::element::Type>({ov::element::f32, ov::element::f32, ov::element::f32}),
                                                 std::vector<Shape>{{5, 10, 3, 32}, {5, 1, 32, 30}, {5, 1, 30, 32}, {5, 30, 32}});
    config.set_concurrency(32);
    execute_and_validate_function(*this, f);
}

TEST_F(TokenizeMHASnippetsTests, smoke_Snippets_MHA_4D_SplitM_DynamicParameter) {
    const auto &f = MHAFunction(std::vector<PartialShape>{{1, 128, 16, 64}, {1, 128, 16, 64}, {1, 16, 128, -1}, {1, 128, 16, 64}},
                                std::vector<ov::element::Type>({ov::element::f32, ov::element::f32, ov::element::f32, ov::element::f32}), false, false);
    config.set_concurrency(32);
    execute_and_validate_function(*this, f);
}

TEST_F(TokenizeMHASnippetsTests, smoke_Snippets_MHASelect_SplitM) {
    const auto& f = MHASelectSplitMFunction(std::vector<PartialShape>{{8, 512, 18}, {8, 18, 64}, {1, 512, 64}, {1, 1, 64}, {8, 64, 512}},
                                            std::vector<Shape>{{8, 2, 256, 18}, {8, 1, 18, 64}, {1, 2, 256, 64}, {1, 1, 1, 64},
                                                               {8, 1, 64, 512}, {8, 512, 512}});
    config.set_concurrency(16);
    execute_and_validate_function(*this, f);
}

TEST_F(TokenizeMHASnippetsTests, smoke_Snippets_MHASelect_SplitM_ScalarParams) {
    const auto& f = MHASelectSplitMFunction(std::vector<PartialShape>{{8, 512, 18}, {8, 18, 64}, {1}, {64}, {8, 64, 512}},
                                            std::vector<Shape>{{8, 2, 256, 18}, {8, 1, 18, 64}, {}, {},
                                                               {8, 1, 64, 512}, {8, 512, 512}});
    config.set_concurrency(16);
    execute_and_validate_function(*this, f);
}

TEST_F(TokenizeMHASnippetsTests, smoke_Snippets_MHA_Reshape_extraction) {
    const auto& f = MHAWithExtractedReshapeFunction(std::vector<PartialShape>{{400, 196, 80},
                                                                              {400, 80, 196},
                                                                              {400, 14, 14, 14},
                                                                              {400, 14, 14, 1, 14},
                                                                              {400, 196, 80}}, true);
    execute_and_validate_function(*this, f);
}

TEST_F(TokenizeMHASnippetsTests, smoke_Snippets_MHA_Rank_Change_Reshape_extraction) {
    const auto& f = MHARankUpgradeToReductionFunction(std::vector<PartialShape>{{100, 16, 100, 32},
                                                                                {100, 16, 32, 100},
                                                                                {1, 16, 100, 100},
                                                                                {1, 100, 1, 100, 100},
                                                                                {100, 16, 100, 32}});
    execute_and_validate_function(*this, f);
}

}  // namespace snippets
}  // namespace test
}  // namespace ov
