@echo off

:: *************************** !! IMPORTANT !! ***************************
::
:: Required vars to set: EnginePath, ProjectPureName, VersionSelector
::
:: *************************** !! IMPORTANT !! ***************************

rem Engine params
set EnginePath=f:\Builds\UE_5.0_InstalledBuild
set VersionSelector=c:\Program Files (x86)\Epic Games\Launcher\Engine\Binaries\Win64\UnrealVersionSelector.exe
rem Editor path
set EditorPath=%EnginePath%\Engine\Binaries\Win64\UnrealEditor.exe
set RunUATPath=%EnginePath%\Engine\Build\BatchFiles\RunUAT.bat

rem Project params
set ProjectRoot=%~dp0..
:: Pure name of your project. Example: name of you project file is TPS.uproject, then ProjectPureName=TPS
set ProjectPureName=Blaster
set ProjectName=%ProjectPureName%.uproject
set ProjectPath=%ProjectRoot%\%ProjectName%
set SourceCodePath=%ProjectRoot%\Source
set PluginsPath=%ProjectRoot%\Plugins

rem Build params
set Platform=Win64
:: Shipping, DebugGame
set Configuration=Development
set ArchivePath=%ProjectRoot%\Build

rem Other params
set DirsToRemove=Intermediate DerivedDataCache Saved Binaries .vs Build Documentation
set FilesToRemove=*.sln

rem Target generation params
set COPYRIGHT_LINE=// Blaster Multiplayer Game. All rights reserved.
set EXTRA_MODULE_NAMES="%ProjectPureName%"
set TargetTemplateFilePath=%ProjectRoot%\devops_ue\targets\GameModule.Target.cs.template

rem Run
set ServerExePath=%ProjectRoot%\Build\WindowsServer\%ProjectPureName%Server.exe
set ClientExePath=%ProjectRoot%\Build\WindowsClient\%ProjectPureName%Client.exe
set GameExePath=%ProjectRoot%\Build\Windows\%ProjectPureName%.exe

rem Tests
set TestNames=
set TestOutputLogPath=%ProjectRoot%\Build\Tests\Tests.log
set ReportOutputPath=%ProjectRoot%\Build\Tests
set ExludedPathForTestReport=%SourceCodePath%\%ProjectPureName%\Tests
set UEAutomationContentPath=%EnginePath%\Engine\Content\Automation
set OpenCPPCoveragePath=c:\Program Files\OpenCppCoverage\OpenCppCoverage.exe

rem Misc
set RarPath=C:\Program Files\WinRAR\Rar.exe
