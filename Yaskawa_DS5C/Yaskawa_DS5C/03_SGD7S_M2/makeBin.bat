fromelf --bincombined --output=../../00_Products/Bin/03_SGD7S_M2.bin ../../00_Products/Bin/03_SGD7S_M2.axf 
fromelf --strip=debug,symbols,comment,notes --elf --output=../../00_Products/Bin/03_SGD7S_M2_div.axf ../../00_Products/Bin/03_SGD7S_M2.axf
fromelf --text -a --expandarrays --output=../../00_Products/Bin/03_SGD7S_M2.map ../../00_Products/Bin/03_SGD7S_M2.axf
rem 工場出荷用暗号化ファイル生成
rem MakeElfZip  ワークパス  入力ファイル(.axf)  出力ファイル名(拡張子なし)  パスワード
..\..\..\..\MakeElfZip.exe  ..\..\00_Products\Bin\  03_SGD7S_M2_div.axf  03_SGD7S_M2_i Initial.
rem 暗号化ファイル生成
..\..\..\..\MakeElfZip.exe  ..\..\00_Products\Bin\  03_SGD7S_M2_div.axf  03_SGD7S_M2   M32Jr8A2
rem 一時ファイル削除
del ..\..\00_Products\Bin\03_SGD7S_M2_div.axf
rem ダウンロード用ファイル生成(全種類)
rem MakeDlFile  生成ファイル指定  ワークパス  ファイル名(拡張子なし)  パスワード
..\..\..\..\MakeDlFile.exe  3  ..\..\00_Products\Bin\  03_SGD7S_M2  Initial.
