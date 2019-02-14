
echo off

cd /d %~dp0
del X10_KeyB_App_With_Bin.bin
fsutil file createnew X10_KeyB_App_With_Bin.bin 8192
cat App_Bin\X10_KeyBoard_app.bin >>X10_KeyB_App_With_Bin.bin
hbin boot_Bin\Boot_X10KeyBoard.bin X10_KeyB_App_With_Bin.bin


copy ..\Objects\X10_KeyBoard_app.hex App_Bin

echo on