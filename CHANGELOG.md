# Changelog

このプロジェクトは [Keep a Changelog](https://keepachangelog.com/ja/1.1.0/) に準拠し、バージョンは [Semantic Versioning](https://semver.org/lang/ja/) に従います。

## [1.2.0] - 2025-09-17
### 追加
- PromptForFilename関数を実装（ファイル名入力ダイアログ）。
- ファイル名入力ダイアログのUIコンポーネントを追加（FTXUI使用）。
- ビルド設定の改善（CMAKE_BUILD_TYPEのデフォルト設定）。

### 変更
- README.mdのキーバインド表で検索機能の未実装を明記。
- CMAKE_BUILD_TYPEをデフォルトでReleaseに設定（パフォーマンス向上）。

### 修正
- FTXUIライブラリの重複リンク警告を修正（ftxui::componentのみリンク）。

## [1.1.0] - 2025-09-16
### 追加
- 日本語入力・表示に対応（UTF-8の安全なバックスペース処理、ワイド文字描画）。
- 折りたたみ直後にカーソルが不可視領域へ入らない自動補正を実装。
- `BlockModel` の可視行↔実行行マップAPIを追加。
- 最小限のユニットテスト（`tests/block_model_test.cpp`）。
- GitHub Actions CI（ビルド/テスト）とタグからのリリースワークフローを追加。

### 変更
- `BlockModel` がテキスト行バッファを参照保持するように変更（無駄なコピーを排除）。
- ブロック移動を `std::rotate` による安全な入替へ変更。
- 段落ブロックを導入し、移動時の整合性を向上。

### 修正
- pandoc 実行時の引数エスケープを追加（安全性向上）。

## [1.0.0] - 2025-01-01
- 初期リリース。

