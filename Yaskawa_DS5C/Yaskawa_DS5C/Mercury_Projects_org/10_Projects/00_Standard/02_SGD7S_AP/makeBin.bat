fromelf --bincombined --output=../../00_Products/Bin/02_SGD7S_AP.bin ../../00_Products/Bin/02_SGD7S_AP.axf 
fromelf --strip=debug,symbols,comment,notes --elf --output=../../00_Products/Bin/02_SGD7S_AP_div.axf ../../00_Products/Bin/02_SGD7S_AP.axf
fromelf --text -a --expandarrays --output=../../00_Products/Bin/02_SGD7S_AP.map ../../00_Products/Bin/02_SGD7S_AP.axf
rem 工場出荷用暗号化ファイル生成
rem MakeElfZip  ワークパス  入力ファイル(.axf)  出力ファイル名(拡張子なし)  パスワード
..\..\..\..\MakeElfZip.exe  ..\..\00_Products\Bin\  02_SGD7S_AP_div.axf  02_SGD7S_AP_i Initial.
rem 暗号化ファイル生成
..\..\..\..\MakeElfZip.exe  ..\..\00_Products\Bin\  02_SGD7S_AP_div.axf  02_SGD7S_AP   AZlGWoBP
rem 一時ファイル削除
del ..\..\00_Products\Bin\02_SGD7S_AP_div.axf
rem ダウンロード用ファイル生成(全種類)
rem MakeDlFile  生成ファイル指定  ワークパス  ファイル名(拡張子なし)  パスワード
..\..\..\..\MakeDlFile.exe  3  ..\..\00_Products\Bin\  02_SGD7S_AP  Initial.
