#pragma once

#define WIN32_LEAN_AND_MEAN
/*
빌드 시간을 단축할 수 있도록 Visual C++는 자주 사용하지 않는 API의 일부를 제외하여
Win32 헤더 파일의 크기를 줄이기 위해 WIN32_LEAN_AND_MEAN 같은 매크로를 제공한다.

MFC가 아닌 응용 프로그램의 경우에는 WIN32_LEAN_AND_MEAN 매크로를 정의하여 빌드 시간을 단축할 수 있다.

#include <Windows.h> 를 선언하기 전에 WIN32_LEAN_AND_MEAN 을 정의한다.

프로젝트 생성시 '미리 컴파일된 헤더' 옵션을 선택하면 stdafx.h 파일에 자동으로 WIN32_LEAN_AND_MEAN 매크로가 정의 된다.

MSDN 링크

출처: https://kksuny.tistory.com/55 [생각이 없는 프로그래밍]
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