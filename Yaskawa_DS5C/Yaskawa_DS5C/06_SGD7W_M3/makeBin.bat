fromelf --bincombined --output=../../00_Products/Bin/06_SGD7W_M3.bin ../../00_Products/Bin/06_SGD7W_M3.axf 
fromelf --strip=debug,symbols,comment,notes --elf --output=../../00_Products/Bin/06_SGD7W_M3_div.axf ../../00_Products/Bin/06_SGD7W_M3.axf
fromelf --text -a --expandarrays --output=../../00_Products/Bin/06_SGD7W_M3.map ../../00_Products/Bin/06_SGD7W_M3.axf
rem �H��o�חp�Í����t�@�C������
rem MakeElfZip  ���[�N�p�X  ���̓t�@�C��(.axf)  �o�̓t�@�C����(�g���q�Ȃ�)  �p�X���[�h
..\..\..\..\MakeElfZip.exe  ..\..\00_Products\Bin\  06_SGD7W_M3_div.axf  06_SGD7W_M3_i Initial.
rem �Í����t�@�C������
..\..\..\..\MakeElfZip.exe  ..\..\00_Products\Bin\  06_SGD7W_M3_div.axf  06_SGD7W_M3   MWPTY33W
rem �ꎞ�t�@�C���폜
del ..\..\00_Products\Bin\06_SGD7W_M3_div.axf
rem �_�E�����[�h�p�t�@�C������(�S���)
rem MakeDlFile  �����t�@�C���w��  ���[�N�p�X  �t�@�C����(�g���q�Ȃ�)  �p�X���[�h
..\..\..\..\MakeDlFile.exe  3  ..\..\00_Products\Bin\  06_SGD7W_M3  Initial.
