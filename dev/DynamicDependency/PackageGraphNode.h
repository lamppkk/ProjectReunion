﻿// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.

#pragma once

#include <appmodel.h>

#include "MsixDynamicDependency.h"

#include <wil/resource.h>
#include "wil_msixdynamicdependency.h"

#include "PackageInfo.h"

namespace MddCore
{
class PackageGraphNode
{
public:
    PackageGraphNode() = default;

    ~PackageGraphNode()
    {
        Reset();
    }

    PackageGraphNode(const PackageGraphNode& other) = delete;

    PackageGraphNode(PackageGraphNode&& other) :
        m_packageInfoReference(std::move(other.m_packageInfoReference)),
        m_packageInfo(std::move(other.m_packageInfo)),
        m_rank(std::move(other.m_rank)),
        m_pathList(std::move(other.m_pathList)),
        m_context(std::move(other.m_context)),
        m_addDllDirectoryCookies(std::move(other.m_addDllDirectoryCookies))
    {
    }

    PackageGraphNode(
        PCWSTR packageFullName,
        INT32 rank);

    PackageGraphNode& operator=(PackageGraphNode& other) = delete;

    PackageGraphNode& operator=(PackageGraphNode&& other)
    {
        if (this != &other)
        {
            Reset();
            m_packageInfoReference = std::move(other.m_packageInfoReference);
            m_packageInfo = std::move(other.m_packageInfo);
            m_rank = std::move(other.m_rank);
            m_pathList = std::move(other.m_pathList);
            m_context = std::move(other.m_context);
            m_addDllDirectoryCookies = std::move(other.m_addDllDirectoryCookies);
        }
        return *this;
    }

    void Reset()
    {
        m_addDllDirectoryCookies.clear();
        m_context.reset();
        m_pathList.clear();
        m_rank = MDD_PACKAGE_DEPENDENCY_RANK_DEFAULT;
        m_packageInfo.Reset();
        m_packageInfoReference.reset();
    }

    bool IsDynamic() const
    {
        return !IsStatic();
    }

    bool IsStatic() const
    {
        return !m_packageInfoReference.get();
    }

    UINT32 CountMatchingPackages(
        const UINT32 flags,
        const PackagePathType packagePathType) const;

    UINT32 GetMatchingPackages(
        const UINT32 flags,
        const PackagePathType packagePathType,
        wil::unique_cotaskmem_ptr<BYTE[]>& buffer) const;

    const MddCore::PackageInfo& PackageInfo() const
    {
        return m_packageInfo;
    }

    int32_t Rank() const
    {
        return m_rank;
    }

    const std::wstring& PathList() const
    {
        return m_pathList;
    }

    MDD_PACKAGEDEPENDENCY_CONTEXT Context()
    {
        return m_context.get();
    }

    void GenerateContext();

    void AddDllDirectories();

    void RemoveDllDirectories();

private:
    void BuildPathList();

private:
    static volatile MDD_PACKAGEDEPENDENCY_CONTEXT s_lastContext;

private:
    mutable wil::unique_package_info_reference m_packageInfoReference;
    MddCore::PackageInfo m_packageInfo;
    INT32 m_rank{ MDD_PACKAGE_DEPENDENCY_RANK_DEFAULT };
    std::wstring m_pathList;
    wil::unique_package_dependency_context m_context;
    std::vector<wil::unique_dll_directory_cookie> m_addDllDirectoryCookies;
};
}