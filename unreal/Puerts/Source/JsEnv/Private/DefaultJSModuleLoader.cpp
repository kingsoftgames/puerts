/*
 * Tencent is pleased to support the open source community by making Puerts available.
 * Copyright (C) 2020 THL A29 Limited, a Tencent company.  All rights reserved.
 * Puerts is licensed under the BSD 3-Clause License, except for the third-party components listed in the file 'LICENSE' which may
 * be subject to their corresponding license terms. This file is subject to the terms and conditions defined in file 'LICENSE',
 * which is part of this source code package.
 */

#include "JSModuleLoader.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "Algo/Reverse.h"
#if (ENGINE_MAJOR_VERSION >= 5)
#include "HAL/PlatformFileManager.h"
#else
#include "HAL/PlatformFilemanager.h"
#endif

namespace puerts
{
static FString PathNormalize(const FString& PathIn)
{
    TArray<FString> PathFrags;
    PathIn.ParseIntoArray(PathFrags, TEXT("/"));
    Algo::Reverse(PathFrags);
    TArray<FString> NewPathFrags;
    bool FromRoot = PathIn.StartsWith(TEXT("/"));
    while (PathFrags.Num() > 0)
    {
        FString E = PathFrags.Pop();
        if (E != TEXT("") && E != TEXT("."))
        {
            if (E == TEXT("..") && NewPathFrags.Num() > 0 && NewPathFrags.Last() != TEXT(".."))
            {
                NewPathFrags.Pop();
            }
            else
            {
                NewPathFrags.Push(E);
            }
        }
    }
    if (FromRoot)
    {
        return TEXT("/") + FString::Join(NewPathFrags, TEXT("/"));
    }
    else
    {
        return FString::Join(NewPathFrags, TEXT("/"));
    }
}

bool DefaultJSModuleLoader::CheckExists(const FString& PathIn, FString& Path, FString& AbsolutePath)
{
    IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
    FString NormalizedPath = PathNormalize(PathIn);
    if (PlatformFile.FileExists(*NormalizedPath))
    {
        // --> modified by kg begin
        // chenyoumin: ConvertToAbsolutePathForExternalAppForRead 转换后路径在移动为 Pak: ... 非标准路径
        // 非移动端 NormalizedPath 为全路径，移动端 NormalizedPath 为相对路径
        // AbsolutePath = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*NormalizedPath);
        AbsolutePath = NormalizedPath;
        // --< end
        Path = NormalizedPath;
        return true;
    }

    return false;
}

bool DefaultJSModuleLoader::SearchModuleInDir(
    const FString& Dir, const FString& RequiredModule, FString& Path, FString& AbsolutePath)
{
    FString Extension = FPaths::GetExtension(RequiredModule);
    bool IsJs = Extension == TEXT("js") || Extension == TEXT("mjs") || Extension == TEXT("cjs") || Extension == TEXT("json");
    if (Extension == TEXT("") || !IsJs)
    {
        return SearchModuleWithExtInDir(Dir, RequiredModule + ".js", Path, AbsolutePath) ||
               SearchModuleWithExtInDir(Dir, RequiredModule + ".mjs", Path, AbsolutePath) ||
               SearchModuleWithExtInDir(Dir, RequiredModule + ".cjs", Path, AbsolutePath) ||
               // --> modified by kg begin
               // songfuhao: 调整 index.js 与 package.json 的加载顺序，保证第三方 module 的正常调用
               /**
               SearchModuleWithExtInDir(Dir, RequiredModule / "package.json", Path, AbsolutePath) ||
               SearchModuleWithExtInDir(Dir, RequiredModule / "index.js", Path, AbsolutePath);
               */
               // --< end
               SearchModuleWithExtInDir(Dir, RequiredModule / "index.js", Path, AbsolutePath) ||
               SearchModuleWithExtInDir(Dir, RequiredModule / "package.json", Path, AbsolutePath);
    }
    else
    {
        return SearchModuleWithExtInDir(Dir, RequiredModule, Path, AbsolutePath);
    }
}

bool DefaultJSModuleLoader::SearchModuleWithExtInDir(
    const FString& Dir, const FString& RequiredModule, FString& Path, FString& AbsolutePath)
{
    return CheckExists(Dir / RequiredModule, Path, AbsolutePath) ||
           (!Dir.EndsWith(TEXT("node_modules")) && CheckExists(Dir / TEXT("node_modules") / RequiredModule, Path, AbsolutePath));
}

bool DefaultJSModuleLoader::Search(const FString& RequiredDir, const FString& RequiredModule, FString& Path, FString& AbsolutePath)
{
    if (SearchModuleInDir(RequiredDir, RequiredModule, Path, AbsolutePath))
    {
        return true;
    }
    // --> modified by ksg begin
    // tiansen:此处去掉contains("/")条件判断，该条件判断会导致部分插件查找不到（比如：@protobufjs/aspromise）
    // if (RequiredDir != TEXT("") && !RequiredModule.GetCharArray().Contains('/') && !RequiredModule.EndsWith(TEXT(".js")) &&
    //     !RequiredModule.EndsWith(TEXT(".mjs")))
    if (RequiredDir != TEXT("") && !RequiredModule.EndsWith(TEXT(".js")) && !RequiredModule.EndsWith(TEXT(".mjs")))
    // --< end
    {
        // 调用require的文件所在的目录往上找
        TArray<FString> pathFrags;
        RequiredDir.ParseIntoArray(pathFrags, TEXT("/"));
        pathFrags.Pop();    // has try in "if (SearchModuleInDir(RequiredDir, RequiredModule, Path, AbsolutePath))"
        while (pathFrags.Num() > 0)
        {
            if (!pathFrags.Last().Equals(TEXT("node_modules")))
            {
                if (SearchModuleInDir(FString::Join(pathFrags, TEXT("/")), RequiredModule, Path, AbsolutePath))
                {
                    return true;
                }
            }
            pathFrags.Pop();
        }
    }

    return SearchModuleInDir(FPaths::ProjectContentDir() / ScriptRoot, RequiredModule, Path, AbsolutePath) ||
           (ScriptRoot != TEXT("JavaScript") &&
               SearchModuleInDir(FPaths::ProjectContentDir() / TEXT("JavaScript"), RequiredModule, Path, AbsolutePath));
}

bool DefaultJSModuleLoader::Load(const FString& Path, TArray<uint8>& Content)
{
    // return (FPaths::FileExists(FullPath) && FFileHelper::LoadFileToString(Content, *FullPath));
    IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
    IFileHandle* FileHandle = PlatformFile.OpenRead(*Path);
    if (FileHandle)
    {
        int len = FileHandle->Size();
        Content.Reset(len + 2);
        Content.AddUninitialized(len);
        const bool Success = FileHandle->Read(Content.GetData(), len);
        delete FileHandle;

        return Success;
    }
    return false;
}

FString& DefaultJSModuleLoader::GetScriptRoot()
{
    return ScriptRoot;
}

}    // namespace puerts