#pragma once

#define WIN32_LEAN_AND_MEAN
/*
���� �ð��� ������ �� �ֵ��� Visual C++�� ���� ������� �ʴ� API�� �Ϻθ� �����Ͽ�
Win32 ��� ������ ũ�⸦ ���̱� ���� WIN32_LEAN_AND_MEAN ���� ��ũ�θ� �����Ѵ�.

MFC�� �ƴ� ���� ���α׷��� ��쿡�� WIN32_LEAN_AND_MEAN ��ũ�θ� �����Ͽ� ���� �ð��� ������ �� �ִ�.

#include <Windows.h> �� �����ϱ� ���� WIN32_LEAN_AND_MEAN �� �����Ѵ�.

������Ʈ ������ '�̸� �����ϵ� ���' �ɼ��� �����ϸ� stdafx.h ���Ͽ� �ڵ����� WIN32_LEAN_AND_MEAN ��ũ�ΰ� ���� �ȴ�.

MSDN ��ũ

��ó: https://kksuny.tistory.com/55 [������ ���� ���α׷���]
*/

#include <Windows.h> // FOr HRESULT
#include <exception>

inline void ThrowIfFailed(HRESULT hr)
{
	if (FAILED(hr))
	{
		throw std::exception();
	}
}