@rem ************************************************************
@rem *	DelObj.cmd : Delete Src\Zwrkobj\*.* files				*
@rem ************************************************************
@setlocal
@set TOPDIR=.\

@echo ********************************************************************************
@echo *	Delete VC Intellisense Files				*
@echo ********************************************************************************
@echo ** delete "ipch" Directry **
rmdir %TOPDIR%99_VC_Projects\ipch /s/q
@echo ** delete *.sdf **
del   %TOPDIR%99_VC_Projects\MercurySimulation.sdf /s /q
@echo ** インテリセンスファイル削除完了 **
@echo **
@echo **

@echo ********************************************************************************
@echo *	Delete object files							*
@echo ********************************************************************************
@echo ** delete "Debug" Directry **
for /r %%A in (Debug) do rmdir /s /q "%%A"
@echo ** delete "history" Directry **
for /r %%A in (.history) do rmdir /s /q "%%A"
@echo ** delete "indexes" Directry **
for /r %%A in (.indexes) do rmdir /s /q "%%A"
@echo ** オブジェクトファイル削除完了 **
@echo **
@echo **

@echo ********************************************************************************
@echo *	Delete plugin files							*
@echo ********************************************************************************
@echo ** delete *.log **
for /f %%A in ('dir /b /s *.log') do del "%%A"
@echo ** delete *.pdom **
for /f %%A in ('dir /b /s *.pdom') do del "%%A"
@echo ** delete modulesKeys **
for /f %%A in ('dir /b /s modulesKeys') do del "%%A"
@echo ** delete ".fb"  **
for /f %%A in ('dir /b /s *.fb') do del "%%A"
@echo ** pluginファイル削除完了 **
pause
