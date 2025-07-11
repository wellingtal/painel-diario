#ifndef __MINERAL_VEIN_MANAGER_H__
#define __MINERAL_VEIN_MANAGER_H__

#pragma once

#define g_MinveralVeinMgr CMineralVeinMgr::GetInstance()

#include <list>
#include <map>

// ���� ���� ����ü
struct MineralInfo
{
	unsigned short	m_dwMineralID;	// ���� Item ID
	unsigned char	m_cMin;			// ���� ���� Min
	unsigned char	m_cMax;			// ���� ���� Max
};
typedef std::list<MineralInfo>	MineralInfoList;


// ���� ���� ����ü
struct MineralVeinInfo
{
	enum Length
	{
		MAX_VEIN_NAME_LEN	= 32
	};

	char				m_szVeinName[ MAX_VEIN_NAME_LEN ];	// ���� �̸�
	unsigned long		m_dwVeinColor;						// ������ ���� ID
	unsigned long		m_dwMaxFertility;					// ������ �ִ� ����
	unsigned long		m_dwNowFertility;					// ������ ���� ����	(DB ���� ������ ��)
	MineralInfoList		m_lstMineralInfo;					// ���� ����Ʈ
};
typedef std::map<unsigned long, MineralVeinInfo*>	MineralVeinMap;


// TGA ���� ���
#pragma pack (1)
struct sTargaHeader {
	unsigned char	IDLength, ColorMapType, ImageType ;
	unsigned short	ColorMapOrigin, ColorMapSize ;
	unsigned char	ColorMapEntrySize ;
	unsigned short	XOrigin, YOrigin, Width, Height ;
	unsigned char	PixelSize ;
	unsigned char	ImageDescriptor ;
};
#pragma pack (8)

// ���� ���� Ŭ����
class CMineralVeinMgr
{
public:
	enum Const
	{
		MAX_TYPE_STRING_LEN	= 32,
		MAX_HEXA_COLOR_LEN	= 10,

		COLOR_IMAGE_SIZE	= 512
	};

	static CMineralVeinMgr&		GetInstance();

	bool	Initialize();
	void	Destroy();

	bool	LoadMineralVeinsFromFiles(const char* szTextFile, const char* szImageFile = 0);
	bool	SaveMineralVeinsToText(const char* szScriptFile);
	bool	SaveMineralVeinsToBinary(const char* szBinaryFile, const char* szTrashFile = 0);
	bool	LoadMineralVeinsFromBinary(const char* szBinaryFile);

	MineralVeinInfo*	GetMineralVein(unsigned long dwVeinColor) const;
	MineralVeinInfo*	GetMineralVein(float fX, float fZ) const;
	unsigned long		GetVeinColor(float fX, float fZ) const;
	const char*			GetVeinName(float fX, float fZ) const;
	int					GetVeinNum() const;

	// ���� ���μ��� �Լ�
	void	InitializeMiningCampNum();						// ���´� ä���� ���� 0 ���� ����
    void	CalculateMiningCampNum(float fX, float fZ);		// ä���� ��ġ�� �ش��ϴ� ������ ��� ���� �������ִ� �Լ�
	int		GetMiningCampNum(unsigned long dwVeinColor);	// ���� ������ ä������ ä���� ���� ����
	void	CalculateFertility(unsigned long dwProcessType);							// ���� ������Ʈ
	void	SerializeOutFertility(unsigned long* lpData_Out, unsigned short& wSize);


	// DB ���� ���� ���� ���°��� �����ϴ� �Լ�
	bool	SetNowFertility(unsigned short wNum, unsigned long* szDataFromDB);
	bool	SetNowFertility(unsigned long dwVeinColor, unsigned long dwFertility);

private:

	typedef std::map<unsigned long, int>	VeinToCampNumMap;	// < VeinColor, MiningCampNum >

	CMineralVeinMgr();
	~CMineralVeinMgr();

	MineralVeinMap		m_mapMineralVein;
	VeinToCampNumMap	m_mapCampNum;
	unsigned long		m_dwColorTable[ COLOR_IMAGE_SIZE ][ COLOR_IMAGE_SIZE ];
};

#endif	// __MINERAL_VEIN_MANAGER_H__