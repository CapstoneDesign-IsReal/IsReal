// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreEventSubsystem.h"

// broadcast the RewindDone event to all bound listeners // 클래스 간 중개소
void UCoreEventSubsystem::RewindDone()
{
	if (RewindDoneDelegate.IsBound()) 
	{
		RewindDoneDelegate.Broadcast();
	}
}