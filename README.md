# kuin-ctags
Kuin-ctags for universal-ctags ( https://github.com/universal-ctags/ctags )

# 使い方
- `kuin.c` を `parsers` フォルダに配置。
- `win32\ctags_vs2013.vcxproj` に `    <ClCompile Include="..\parsers\kuin.c" />`を追加。
- `main\parsers.h` に `	KuinParser, \` を追加。
- `win32\ctags_vs2013.sln` でコンパイル。
