// Minimal unit tests for BlockModel (no external framework)
#include "block_model.h"
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>

using ShinoEditor::BlockModel;

static int g_failures = 0;

#define ASSERT_TRUE(cond) do { \
  if(!(cond)) { \
    std::cerr << __FILE__ << ":" << __LINE__ << " ASSERT_TRUE failed: " #cond "\n"; \
    ++g_failures; \
  } \
} while(0)

#define ASSERT_EQ(a,b) do { \
  auto _va = (a); auto _vb = (b); \
  if(!((_va) == (_vb))) { \
    std::cerr << __FILE__ << ":" << __LINE__ << " ASSERT_EQ failed: " << _va << " != " << _vb << "\n"; \
    ++g_failures; \
  } \
} while(0)

static void test_visible_identity() {
  std::vector<std::string> lines = {
    "# H1", "line1", "line2"
  };
  BlockModel bm(lines);
  auto vis = bm.GetVisibleLines();
  auto map = bm.GetVisibleLineIndices();
  ASSERT_EQ((int)vis.size(), 3);
  ASSERT_EQ((int)map.size(), 3);
  ASSERT_EQ(map[0], 0);
  ASSERT_EQ(map[1], 1);
  ASSERT_EQ(map[2], 2);
  ASSERT_EQ(vis[1], "line1");
}

static void test_fold_paragraph_mapping() {
  std::vector<std::string> lines = {
    "# H1",
    "p1", "p2",
    "> q1", "> q2"
  };
  BlockModel bm(lines);
  // Fold paragraph starting at real line 1
  bm.ToggleFold(1);
  auto vis = bm.GetVisibleLines();
  auto map = bm.GetVisibleLineIndices();
  ASSERT_EQ((int)vis.size(), 4); // header, folded paragraph, q1, q2
  ASSERT_EQ(map[0], 0);
  ASSERT_EQ(map[1], 1); // folded paragraph header represents start line
  ASSERT_EQ(map[2], 3);
  ASSERT_EQ(map[3], 4);
}

static void test_move_block_rotate() {
  std::vector<std::string> lines = {
    "# H1",     // header
    "a", "b",   // paragraph1
    "```", "c", "```", // code fence
    "tail"      // paragraph2
  };
  BlockModel bm(lines);
  // Move first paragraph (real line 1) down below code fence
  bool ok = bm.MoveBlockDown(1);
  ASSERT_TRUE(ok);
  std::cerr << "After MoveDown:\n";
  for (size_t i = 0; i < lines.size(); ++i) {
    std::cerr << i << ":" << lines[i] << "\n";
  }
  // Expected order: header, code fence, paragraph1, tail
  ASSERT_EQ(lines[0], std::string("# H1"));
  ASSERT_EQ(lines[1], std::string("```"));
  ASSERT_EQ(lines[2], std::string("c"));
  ASSERT_EQ(lines[3], std::string("```"));
  ASSERT_EQ(lines[4], std::string("a"));
  ASSERT_EQ(lines[5], std::string("b"));
  ASSERT_EQ(lines[6], std::string("tail"));

  // Move that paragraph back up above code fence
  ok = bm.MoveBlockUp(5); // any line inside that block works
  ASSERT_TRUE(ok);
  std::cerr << "After MoveUp:\n";
  for (size_t i = 0; i < lines.size(); ++i) {
    std::cerr << i << ":" << lines[i] << "\n";
  }
  ASSERT_EQ(lines[0], std::string("# H1"));
  ASSERT_EQ(lines[1], std::string("a"));
  ASSERT_EQ(lines[2], std::string("b"));
  ASSERT_EQ(lines[3], std::string("tail"));
  ASSERT_EQ(lines[4], std::string("```"));
  ASSERT_EQ(lines[5], std::string("c"));
  ASSERT_EQ(lines[6], std::string("```"));
}

int main() {
  test_visible_identity();
  test_fold_paragraph_mapping();
  test_move_block_rotate();
  if (g_failures == 0) {
    std::cout << "All tests passed\n";
    return EXIT_SUCCESS;
  }
  std::cerr << g_failures << " test(s) failed\n";
  return EXIT_FAILURE;
}
