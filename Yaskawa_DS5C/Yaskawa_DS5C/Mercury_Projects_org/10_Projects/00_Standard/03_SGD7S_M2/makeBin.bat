fromelf --bincombined --output=../../00_Products/Bin/03_SGD7S_M2.bin ../../00_Products/Bin/03_SGD7S_M2.axf 
fromelf --strip=debug,symbols,comment,notes --elf --output=../../00_Products/Bin/03_SGD7S_M2_div.axf ../../00_Products/Bin/03_SGD7S_M2.axf
fromelf --text -a --expandarrays --output=../../00_Products/Bin/03_SGD7S_M2.map ../../00_Products/Bin/03_SGD7S_M2.axf
rem �H��o�חp�Í����t�@�C������
rem MakeElfZip  ���[�N�p�X  ���̓t�@�C��(.axf)  �o�̓t�@�C����(�g���q�Ȃ�)  �p�X���[�h
..\..\..\..\MakeElfZip.exe  ..\..\00_Products\Bin\  03_SGD7S_M2_div.axf  03_SGD7S_M2_i Initial.
rem �Í����t�@�C������
..\..\..\..\MakeElfZip.exe  ..\..\00_Products\Bin\  03_SGD7S_M2_div.axf  03_SGD7S_M2   M32Jr8A2
rem �ꎞ�t�@�C���폜
del ..\..\00_Products\Bin\03_SGD7S_M2_div.axf
rem �_�E�����[�h�p�t�@�C������(�S���)
rem MakeDlFile  �����t�@�C���w��  ���[�N�p�X  �t�@�C����(�g���q�Ȃ�)  �p�X���[�h
..\..\..\..\MakeDlFile.exe  3  ..\..\00_Products\Bin\  03_SGD7S_M2  Initial.
