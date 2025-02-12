@echo off

setlocal enabledelayedexpansion

REM 设置文件夹路径
set SYNTAX_FILE=syntax\syntax1.txt
set INPUT_DIR=input
set OUTPUT_DIR=output
set EXPECTED_DIR=expected_output

REM 确保输出文件夹存在（清空旧结果）
if exist %OUTPUT_DIR% (
    echo 清空输出目录...
    del /q "%OUTPUT_DIR%\*"
) else (
    mkdir %OUTPUT_DIR%
)

echo =========================
echo 开始自动测试...
echo =========================

REM 遍历 input 文件夹中的所有文件
for %%F in (%INPUT_DIR%\*) do (
    set INPUT_FILE=%%F
    set FILE_NAME=%%~nF
    set OUTPUT_FILE=%OUTPUT_DIR%\!FILE_NAME!.txt
    set EXPECTED_FILE=%EXPECTED_DIR%\!FILE_NAME!.txt

    echo 测试文件: !INPUT_FILE!
    
    REM 执行 dsl.exe
    dsl.exe %SYNTAX_FILE% !INPUT_FILE! !OUTPUT_FILE!
    
    REM 检查程序是否成功输出文件
    if not exist !OUTPUT_FILE! (
        echo [错误] 无法生成输出文件: !OUTPUT_FILE!
        goto :continue
    )

    REM 比较输出结果与预期输出
    fc /b "!OUTPUT_FILE!" "!EXPECTED_FILE!" >nul
    if !errorlevel! equ 0 (
        echo [成功] 测试通过: !FILE_NAME!
    ) else (
        echo [失败] 测试未通过: !FILE_NAME!
    )

    :continue
    echo -------------------------
)

echo =========================
echo 测试完成！
echo =========================

pause
endlocal
