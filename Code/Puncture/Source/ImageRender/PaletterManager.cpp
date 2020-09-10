#include "StdAfx.h"
#include "PaletterManager.h"

using namespace  ImageRender;

PaletterManager::PaletterManager(void)
{
	m_nOnePaletterSize = sizeof(ColorPalette)+256 * sizeof(ARGB);

	//����ͼ��
	m_GrayPaletteBuffer.resize(m_nOnePaletterSize);
	ColorPalette *Palette = (ColorPalette*)&m_GrayPaletteBuffer[0]; 
	for(int i=0; i<256; i++)
	{
		Palette->Entries[i] =Color::MakeARGB(255,i,i,i);
	}
	Palette->Count = 256;
	Palette->Flags = 0x00000002;

	//overlay image palette
	m_OverlayPaletteBuffer.resize(m_nOnePaletterSize);
	Palette = (ColorPalette*)&m_OverlayPaletteBuffer[0]; 

	//������ɫ�壬0����Ϊ͸����ɫ����������Ϊ��͸����ɫ��Ŀǰ��֪�����������ɫѡ��
	for(int i=0; i<1; i++)
	{
		Palette->Entries[i] =Color::MakeARGB(0,0,0,0);
	}
	Palette->Entries[1] = Color::MakeARGB(64, 0, 255, 0);	//maskΪ1��ǰ���٣���ɫ
	Palette->Entries[2] = Color::MakeARGB(64, 255, 0, 0);	//maskΪ2�������ɫ
	Palette->Entries[3] = Color::MakeARGB(64, 0, 0, 255);	//maskΪ3��ֱ������ɫ
	for(int i=4; i<256; i++)
	{
		//Palette->Entries[i] = Color::MakeARGB(32, 255, 0, 255);
		Palette->Entries[i] = Color::MakeARGB(64, 255, 255, 255);	//mask����4����mask�غϲ��֣���Ϊ��ɫ
	}
	Palette->Count = 256;
	Palette->Flags = PaletteFlagsHasAlpha;

	InitTotalPaletteBuffer();
}

PaletterManager::~PaletterManager(void)
{

}

ColorPalette* PaletterManager::GetGrayColorPalette()
{
	return (ColorPalette*)&m_GrayPaletteBuffer[0]; 
}

ColorPalette* PaletterManager::GetOverlayGrayColorPalette()
{
	return (ColorPalette*)&m_OverlayPaletteBuffer[0]; 
}

ColorPalette* PaletterManager::GetNormalColorPalette(ColorTableName nColorName)
{
	return (ColorPalette*)&m_TotalPaletteBuffer[nColorName*m_nOnePaletterSize]; 
}

void PaletterManager::InitTotalPaletteBuffer()
{
	m_TotalPaletteBuffer.resize(m_nOnePaletterSize*COLOR_SCALE_COUNT);
	for(int nIndex=0; nIndex<COLOR_SCALE_COUNT; nIndex++)
	{
		ColorPalette *Palette = (ColorPalette*)&m_TotalPaletteBuffer[nIndex*m_nOnePaletterSize]; 
		for(int i=0; i<256; i++)
		{
			Palette->Entries[i] =Color::MakeARGB(255,gColorsTable[nIndex][i][0], gColorsTable[nIndex][i][1], gColorsTable[nIndex][i][2]);
		}
		Palette->Count = 256;
		Palette->Flags = 0x00000002;
	}
}
