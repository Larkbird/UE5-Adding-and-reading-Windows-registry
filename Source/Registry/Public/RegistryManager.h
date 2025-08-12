// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RegistryManager.generated.h"

//Window 注册表前置声明
#if PLATFORM_WINDOWS
#include "Windows/AllowWindowsPlatformTypes.h"
#include <minwindef.h>

struct HKEY__;
typedef  struct HKEY__ *HKEY;
#define REGSAM DWORD
#include "Windows/HideWindowsPlatformTypes.h"
#else
//非Windows凭他的伪定义
typedef void* HKEY;
#define REGSAM uint32
#endif

UCLASS()
class REGISTRY_API URegistryManager : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	/**
    	 * 将字符串写入Windows注册表
    	 * 
    	 * @param KeyPath 注册表键路径 (如: "Software\\YourGame\\Settings")
    	 * @param ValueName 值名称 (如: "PlayerName")
    	 * @param Data 要写入的字符串数据
    	 * @return 是否写入成功
    	 */
    	UFUNCTION(BlueprintCallable, Category = "Trial|Registry")
    	static bool WriteStringToRegistry(const FString& KeyPath, const FString& ValueName, const FString& Data);
    
    	/**
    	 * 从注册表读取字符串
    	 * 
    	 * @param KeyPath 注册表键路径
    	 * @param ValueName 值名称
    	 * @param OutData 输出读取到的数据
    	 * @return 是否读取成功
    	 */
    	UFUNCTION(BlueprintCallable, Category = "Trial|Registry")
    	static bool ReadStringFromRegistry(const FString& KeyPath, const FString& ValueName, FString& OutData);
    
    	/**
    	 * 将32位整数写入注册表
    	 * 
    	 * @param KeyPath 注册表键路径
    	 * @param ValueName 值名称
    	 * @param Data 要写入的整数值
    	 * @return 是否写入成功
    	 */
    	static bool WriteDwordToRegistry(const FString& KeyPath, const FString& ValueName, uint32 Data);
    
    	/**
    	 * 从注册表读取32位整数
    	 * 
    	 * @param KeyPath 注册表键路径
    	 * @param ValueName 值名称
    	 * @param OutData 输出读取到的整数值
    	 * @return 是否读取成功
    	 */
    	static bool ReadDwordFromRegistry(const FString& KeyPath, const FString& ValueName, uint32& OutData);
    
    private:
    	/**
    	 * 打开注册表键
    	 * 
    	 * @param RootKey 根键 (如HKEY_CURRENT_USER)
    	 * @param SubKey 子键路径
    	 * @param Access 访问权限
    	 * @param OutKey 输出打开的键句柄
    	 * @return 是否成功打开
    	 */
    	static bool OpenRegistryKey(HKEY RootKey, const FString& SubKey, REGSAM Access, HKEY& OutKey);
    	
};
