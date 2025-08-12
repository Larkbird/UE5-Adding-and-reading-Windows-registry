// Fill out your copyright notice in the Description page of Project Settings.


#include "RegistryManager.h"

#if PLATFORM_WINDOWS
#include "Windows/AllowWindowsPlatformTypes.h"
#include <Windows.h>
#include "Windows/HideWindowsPlatformTypes.h"
#elif PLATFORM_MAC
#include <sys/sysctl.h>
#endif

bool URegistryManager::WriteStringToRegistry(const FString& KeyPath, const FString& ValueName, const FString& Data)
{
	HKEY hKey;
    
	// 打开或创建注册表键 (使用当前用户根键)
	if (!OpenRegistryKey(HKEY_CURRENT_USER, KeyPath, KEY_WRITE, hKey))
	{
		return false;
	}

	// 准备写入数据
	const TCHAR* ValueData = *Data;
	DWORD DataSize = (Data.Len() + 1) * sizeof(TCHAR); // +1 包含空终止符

	// 写入字符串值
	LONG Result = RegSetValueEx(
		hKey,               // 键句柄
		*ValueName,         // 值名称
		0,                  // 保留
		REG_SZ,             // 字符串类型
		(const BYTE*)ValueData, // 数据指针
		DataSize            // 数据大小
	);

	// 关闭键句柄
	RegCloseKey(hKey);

	// 检查结果
	if (Result != ERROR_SUCCESS)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to write registry value %s. Error: %d"), *ValueName, Result);
		return false;
	}

	UE_LOG(LogTemp, Log, TEXT("Successfully wrote to registry: %s\\%s"), *KeyPath, *ValueName);
	return true;
}

bool URegistryManager::ReadStringFromRegistry(const FString& KeyPath, const FString& ValueName, FString& OutData)
{
	HKEY hKey;
    
	// 打开注册表键 (只读)
	if (!OpenRegistryKey(HKEY_CURRENT_USER, KeyPath, KEY_READ, hKey))
	{
		return false;
	}

	// 获取值大小
	DWORD DataSize = 0;
	LONG Result = RegQueryValueEx(
		hKey,           // 键句柄
		*ValueName,      // 值名称
		nullptr,         // 保留
		nullptr,         // 类型 (可空)
		nullptr,         // 数据缓冲区 (可空)
		&DataSize       // 接收数据大小
	);

	if (Result != ERROR_SUCCESS || DataSize == 0)
	{
		RegCloseKey(hKey);
		UE_LOG(LogTemp, Warning, TEXT("Registry value %s not found or empty"), *ValueName);
		return false;
	}

	// 分配缓冲区
	TArray<BYTE> Buffer;
	Buffer.SetNumUninitialized(DataSize);

	// 实际读取值
	Result = RegQueryValueEx(
		hKey,
		*ValueName,
		nullptr,
		nullptr,
		Buffer.GetData(),
		&DataSize
	);

	// 关闭键句柄
	RegCloseKey(hKey);

	if (Result != ERROR_SUCCESS)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to read registry value %s. Error: %d"), *ValueName, Result);
		return false;
	}

	// 转换为FString (假设数据是REG_SZ类型)
	OutData = FString((TCHAR*)Buffer.GetData());
	return true;
}

bool URegistryManager::WriteDwordToRegistry(const FString& KeyPath, const FString& ValueName, uint32 Data)
{
	HKEY hKey;
    
	// 打开或创建注册表键
	if (!OpenRegistryKey(HKEY_CURRENT_USER, KeyPath, KEY_WRITE, hKey))
	{
		return false;
	}

	// 写入DWORD值
	LONG Result = RegSetValueEx(
		hKey,
		*ValueName,
		0,
		REG_DWORD,
		(const BYTE*)&Data,
		sizeof(Data)
	);

	// 关闭键句柄
	RegCloseKey(hKey);

	if (Result != ERROR_SUCCESS)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to write DWORD value %s. Error: %d"), *ValueName, Result);
		return false;
	}

	return true;
}

bool URegistryManager::ReadDwordFromRegistry(const FString& KeyPath, const FString& ValueName, uint32& OutData)
{
	HKEY hKey;
    
	// 打开注册表键
	if (!OpenRegistryKey(HKEY_CURRENT_USER, KeyPath, KEY_READ, hKey))
	{
		return false;
	}

	// 准备读取
	DWORD Data = 0;
	DWORD DataSize = sizeof(Data);
	DWORD Type = REG_DWORD;

	// 读取DWORD值
	LONG Result = RegQueryValueEx(
		hKey,
		*ValueName,
		nullptr,
		&Type,
		(LPBYTE)&Data,
		&DataSize
	);

	// 关闭键句柄
	RegCloseKey(hKey);

	if (Result != ERROR_SUCCESS || Type != REG_DWORD)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to read DWORD value %s. Error: %d"), *ValueName, Result);
		return false;
	}

	OutData = Data;
	return true;
}

bool URegistryManager::OpenRegistryKey(HKEY RootKey, const FString& SubKey, REGSAM Access,
	HKEY& OutKey)
{
	// 将FString转换为Windows API需要的TCHAR指针
	const TCHAR* SubKeyPtr = *SubKey;
    
	// 尝试打开注册表键
	LONG Result = RegOpenKeyEx(
		RootKey,        // 预定义的根键 (如HKEY_CURRENT_USER)
		SubKeyPtr,      // 子键路径
		0,              // 保留参数，必须为0
		Access,         // 访问权限 (如KEY_READ, KEY_WRITE)
		&OutKey         // 接收打开的键句柄
	);

	// 如果打开失败，尝试创建键
	if (Result != ERROR_SUCCESS)
	{
		Result = RegCreateKeyEx(
			RootKey,
			SubKeyPtr,
			0,          // 保留
			nullptr,     // 类名 (可空)
			REG_OPTION_NON_VOLATILE, // 永久存储
			Access,     // 访问权限
			nullptr,     // 安全属性 (默认)
			&OutKey,    // 接收新创建的键句柄
			nullptr      // 是否已存在 (可空)
		);
	}

	// 检查最终结果
	if (Result != ERROR_SUCCESS)
	{
		// 记录错误日志
		UE_LOG(LogTemp, Error, TEXT("Failed to open/create registry key: %s. Error: %d"), *SubKey, Result);
		return false;
	}

	return true;
}