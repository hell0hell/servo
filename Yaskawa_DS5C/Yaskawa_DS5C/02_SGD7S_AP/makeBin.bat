fromelf --bincombined --output=../../00_Products/Bin/02_SGD7S_AP.bin ../../00_Products/Bin/02_SGD7S_AP.axf 
fromelf --strip=debug,symbols,comment,notes --elf --output=../../00_Products/Bin/02_SGD7S_AP_div.axf ../../00_Products/Bin/02_SGD7S_AP.axf
fromelf --text -a --expandarrays --output=../../00_Products/Bin/02_SGD7S_AP.map ../../00_Products/Bin/02_SGD7S_AP.axf
rem �H��o�חp�Í����t�@�C������
rem MakeElfZip  ���[�N�p�X  ���̓t�@�C��(.axf)  �o�̓t�@�C����(�g���q�Ȃ�)  �p�X���[�h
..\..\..\..\MakeElfZip.exe  ..\..\00_Products\Bin\  02_SGD7S_AP_div.axf  02_SGD7S_AP_i Initial.
rem �Í����t�@�C������
..\..\..\..\MakeElfZip.exe  ..\..\00_Products\Bin\  02_SGD7S_AP_div.axf  02_SGD7S_AP   AZlGWoBP
rem �ꎞ�t�@�C���폜
del ..\..\00_Products\Bin\02_SGD7S_AP_div.axf
rem �_�E�����[�h�p�t�@�C������(�S���)
rem MakeDlFile  �����t�@�C���w��  ���[�N�p�X  �t�@�C����(�g���q�Ȃ�)  �p�X���[�h
..\..\..\..\MakeDlFile.exe  3  ..\..\00_Products\Bin\  02_SGD7S_AP  Initial.
