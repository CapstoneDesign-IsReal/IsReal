// Fill out your copyright notice in the Description page of Project Settings.


#include "DA_EnemyPartInfo.h"


const FPartInfo* UDA_EnemyPartInfo::GetPartInfoByBoneName(FName BoneName)
{
	return BoneMap.Find(BoneName);	//Return nullptr if there is no such bone.
}