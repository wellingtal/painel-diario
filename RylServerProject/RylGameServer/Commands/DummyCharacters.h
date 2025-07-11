#ifndef _GAMA_GAME_DUMMY_CHARACTERS_H_
#define _GAMA_GAME_DUMMY_CHARACTERS_H_

#include <list>
#include <map>
#include <vector>

#include <Item/ItemConstants.h>

/*
 ĳ���� ��ε�ĳ���� ��� �ۼ� - 
    ���� ������ �Ȱ��� ������ ���� ������ ������Ʈ��  �̸��� �ٲپ �۾��� �����Ѵ�. 
 
 ���� ������ �о� �����ϴ� ����� ���´�. default�� off�̴�.
 ������ �ൿ - �ʰ����Ա�. �չٲٱ�. �̵�. ���ݸ��.
 
 ���� ������ ������ ���� �׸��� ���´�.
 - ��ε�ĳ���ÿ� �ʿ��� �⺻ ĳ���� ���� (�̸�, ����, Ŭ���� ��)
 - ���� �� Ŭ������ �´� ���� ���� ��. (�� ���� ������ ���� itemUID. �޸�, ��ĭ ����.)
 - ĳ���ͺ� �̵� ���, �ʰ����Ա� ��, �չٲٱ� ��, �̵� �� ���ݸ�� ��.
 - ĳ���͸� �� ���� 100�� 800���� ���� ���� ����� Ʈ���� �� ���� ���¸� ����.
 
1. ���� �� ���� ������Ʈ ����, ���� ���� ���� ������ �׿� �´� ������ ������ �����ϰ� �о�´�.
2. ��¥ ĳ���͸� �����ؼ� �����͸� �����ϰ� ���� ���� �ִ´�.
3. ��¥ ĳ���Ͱ� 1�ʿ� 4���� ��¥ �̵� ��Ŷ�� �������� �Ѵ�.
4. ��¥ ĳ���Ͱ� ��θ� ���� �̵��� �� �� �ְ� �Ѵ�. �̵�ó�� �� ��¥ �̵� ��Ŷ ������..
5. ��¥ ĳ���Ͱ� �ֱ������� ���� �������� �� �ְ� �Ѵ�.
6. ��¥ ĳ���Ͱ� �ֱ������� ���� �ٲ� �� �ְ� �Ѵ�.
7. ��¥ ĳ���Ͱ� �ֱ������� ���� ����� ǥ���� �� �ְ� �Ѵ�.
8. ��¥ ĳ���Ͱ� �ֱ������� �ֺ��� �ִ� ĳ���͸� ����� ��ų(!) �� �� �� �ְ� �Ѵ�.

���� ĳ���� ���� ����

// ĳ���� ����
 
CID/�̸�/����/Ŭ����/�Ӹ�Į/��/����/�ʰ����Ա� ��/���ݸ�� ��/��ų��� ��

// ���� ����
�ΰ� : ���� ���� ���� ���� �尩 �Ź�
��ĭ : ���� ��ų�� ���� ���� ������Ʈ�� �ŵ�

// �̵� ���

*/

// forward decl.
class CModifyDummyCharacter;

class CDummyCharacterList
{
public:

    struct EquipmentData
    {
        unsigned short  m_usEquipments[Item::EquipmentPos::MAX_EQUPMENT_POS];
    };

    static CDummyCharacterList& GetInstance();

    bool Initialize(unsigned long dwLoadCharNum);
    bool Destroy();

private:

    CDummyCharacterList();
    ~CDummyCharacterList();

    typedef std::vector<EquipmentData>                  EquipmentVector;
    typedef std::map<unsigned long, EquipmentVector>    EquipmentClassSet;

    typedef std::list<CModifyDummyCharacter*>           DummyCharList;

    static bool LoadEquipments(const char* szFileName, EquipmentVector& equipmentVector);
    static bool LoadDummyChars(const char* szFileName, 
        EquipmentClassSet& equipmentSet, DummyCharList& dummyCharList, unsigned long dwLoadCharNum);

    DummyCharList       m_DummyCharacterList;
    EquipmentClassSet   m_EquipmentClassSet;
};


#endif