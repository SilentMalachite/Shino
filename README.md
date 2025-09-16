# ShinoEditor

ターミナルで動作する C++20 製のMarkdownエディタです。FTXUI を用いた軽量UI、nano風のキーバインド、ブロック折りたたみ、DOCX入出力（pandoc）に対応しています。日本語入力・表示（UTF-8）もサポートします。

## 特長

- nano 風のショートカットで直感的に操作
- 見出し/コード/引用のブロック折りたたみと移動
- プレビュー表示切替（Ctrl+P）
- pandoc による DOCX インポート/エクスポート（Ctrl+I/Ctrl+E）
- 日本語の入力・表示に対応（UTF-8）

## キーバインド（抜粋）

| キー | 機能 |
|------|------|
| Ctrl+O | 保存 |
| Ctrl+X | 終了 |
| Ctrl+W | 検索（未実装） |
| Ctrl+G | ヘルプ切替 |
| Ctrl+J | ブロック折りたたみ/展開 |
| PageUp/PageDown | ブロックの上下移動 |
| Ctrl+P | プレビュー切替 |
| Ctrl+I | DOCX インポート（pandoc） |
| Ctrl+E | DOCX エクスポート（pandoc） |
| ↑/↓ | カーソル上下 |
| Enter | 編集保存 / 新規行挿入 |
| Backspace/Delete | 編集中: 1文字削除（UTF-8対応）/ 非編集中: 行削除 |

## 依存関係

- C++20 対応コンパイラ（GCC 10+ / Clang 11+ / MSVC 2019+）
- CMake 3.16+
- FTXUI（見つからなければ自動取得）
- md4c（任意、HTML出力に使用）
- pandoc（任意、DOCX入出力に使用）

### 依存パッケージのインストール例

Ubuntu/Debian
```bash
sudo apt update
sudo apt install -y build-essential cmake libmd4c-dev pandoc
```

macOS (Homebrew)
```bash
brew install cmake md4c pandoc
```

## ビルド

```bash
mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . -j
```

## 使い方

```bash
# 新規起動
./ShinoEditor

# 既存ファイルを開く
./ShinoEditor document.md
```

## プロジェクト構成

```
src/
├── main.cpp              # エントリポイント
├── app.h/cpp             # アプリケーション本体
├── block_model.h/cpp     # ブロック検出/移動/折り畳み
├── markdown_renderer.*   # Markdownレンダリング
├── pandoc_io.*           # DOCX入出力（pandoc）
└── tui_bindings.*        # キーバインド/ヘルプ
```

## 開発/テスト

- ビルド: 上述のコマンドを参照
- テスト: CTest を使用
```bash
cmake -S . -B build -DSHINO_BUILD_TESTS=ON
cmake --build build -j
ctest --test-dir build --output-on-failure
```

## CI

- GitHub Actions により、Linux/macOS/Windows でビルドとテストを実行します。
- 生成物（バイナリ）はアーティファクトとして保存されます。

## リリース

- タグ（例: `v1.1.0`）を push すると、各OSでビルドし GitHub Release に成果物を添付します。

## 変更履歴

- `CHANGELOG.md` を参照。

## ライセンス

- MIT License（`LICENSE` を参照）。
