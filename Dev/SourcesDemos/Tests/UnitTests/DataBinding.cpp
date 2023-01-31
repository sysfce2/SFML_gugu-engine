////////////////////////////////////////////////////////////////
// Header

#include "DataBinding.h"

////////////////////////////////////////////////////////////////
// Includes

#include "Gugu/Resources/ManagerResources.h"
#include "Gugu/System/SystemUtility.h"

////////////////////////////////////////////////////////////////
// File Implementation

namespace tests {

////////////////////////////////////////////////////////////////
namespace EWeaponType
{
    void Register()
    {
        gugu::DataEnumInfos* enumInfos = new gugu::DataEnumInfos;
        enumInfos->values.reserve(5);
        enumInfos->values.push_back("Unknown");
        enumInfos->values.push_back("Sword");
        enumInfos->values.push_back("Mace");
        enumInfos->values.push_back("Axe");
        enumInfos->values.push_back("Crossbow");

        gugu::GetResources()->RegisterDataEnumInfos("weaponType", enumInfos);
    }

    const gugu::DataEnumInfos* GetDataEnumInfos()
    {
        return gugu::GetResources()->GetDataEnumInfos("weaponType");
    }

    void GetEnumValues(std::vector<EWeaponType::Type>& enumValues)
    {
        enumValues.reserve(5);
        enumValues.push_back(EWeaponType::Unknown);
        enumValues.push_back(EWeaponType::Sword);
        enumValues.push_back(EWeaponType::Mace);
        enumValues.push_back(EWeaponType::Axe);
        enumValues.push_back(EWeaponType::Crossbow);
    }

    size_t GetSize()
    {
        const gugu::DataEnumInfos* enumInfos = gugu::GetResources()->GetDataEnumInfos("weaponType");
        if (enumInfos)
            return enumInfos->values.size();
        return 0;
    }
}

////////////////////////////////////////////////////////////////
DS_Entity::DS_Entity()
{
    name = "Entity Name";
    health = 100;
}

DS_Entity::~DS_Entity()
{
}

void DS_Entity::ParseMembers(gugu::DataParseContext& context)
{
    //gugu::DatasheetObject::ParseMembers(context);

    gugu::binding::ReadString(context, "name", name);
    gugu::binding::ReadInt(context, "health", health);
}

////////////////////////////////////////////////////////////////
DS_Character::DS_Character()
{
    stamina = 100;
    speed = 100;
    weapon = EWeaponType::Sword;
}

DS_Character::~DS_Character()
{
}

void DS_Character::ParseMembers(gugu::DataParseContext& context)
{
    DS_Entity::ParseMembers(context);

    gugu::binding::ReadInt(context, "stamina", stamina);
    gugu::binding::ReadFloat(context, "speed", speed);
    gugu::binding::ReadEnum(context, "weapon", "weaponType", weapon);
}

////////////////////////////////////////////////////////////////
DS_Faction::DS_Faction()
{
    name = "";
    leader = nullptr;
}

DS_Faction::~DS_Faction()
{
    leader = nullptr;
}

void DS_Faction::ParseMembers(gugu::DataParseContext& context)
{
    //gugu::DatasheetObject::ParseMembers(context);

    gugu::binding::ReadString(context, "name", name);
    gugu::binding::ReadDatasheetReference(context, "leader", leader);
}

////////////////////////////////////////////////////////////////
DS_Item::DS_Item()
{
    name = "";
}

DS_Item::~DS_Item()
{
}

void DS_Item::ParseMembers(gugu::DataParseContext& context)
{
    //gugu::DatasheetObject::ParseMembers(context);

    gugu::binding::ReadString(context, "name", name);
}

////////////////////////////////////////////////////////////////
DS_Condition::DS_Condition()
{
    intendedResult = true;
}

DS_Condition::~DS_Condition()
{
}

void DS_Condition::ParseMembers(gugu::DataParseContext& context)
{
    //gugu::DatasheetObject::ParseMembers(context);

    gugu::binding::ReadBool(context, "intendedResult", intendedResult);
}

////////////////////////////////////////////////////////////////
DS_ConditionAnd::DS_ConditionAnd()
{
}

DS_ConditionAnd::~DS_ConditionAnd()
{
    ClearStdVector(conditions);
}

void DS_ConditionAnd::ParseMembers(gugu::DataParseContext& context)
{
    DS_Condition::ParseMembers(context);

    gugu::binding::ReadDatasheetInstanceArray(context, "conditions", "condition", conditions);
}

////////////////////////////////////////////////////////////////
DS_ConditionPlayerLevel::DS_ConditionPlayerLevel()
{
    minLevel = -1;
    maxLevel = -1;
}

DS_ConditionPlayerLevel::~DS_ConditionPlayerLevel()
{
}

void DS_ConditionPlayerLevel::ParseMembers(gugu::DataParseContext& context)
{
    DS_Condition::ParseMembers(context);

    gugu::binding::ReadInt(context, "minLevel", minLevel);
    gugu::binding::ReadInt(context, "maxLevel", maxLevel);
}

////////////////////////////////////////////////////////////////
DS_GameSave::DS_GameSave()
{
    score = -1;
    name = "DEFAULT";
    singleWeapon = EWeaponType::Unknown;
    emptyCharacter = nullptr;
    singleCharacter = nullptr;
    emptyItem = nullptr;
    singleItem = nullptr;
}

DS_GameSave::~DS_GameSave()
{
    emptyCharacter = nullptr;
    singleCharacter = nullptr;
    multipleCharacters.clear();
    SafeDelete(emptyItem);
    SafeDelete(singleItem);
    ClearStdVector(multipleItems);
}

void DS_GameSave::ParseMembers(gugu::DataParseContext& context)
{
    //gugu::DatasheetObject::ParseMembers(context);

    gugu::binding::ReadInt(context, "score", score);
    gugu::binding::ReadString(context, "name", name);
    gugu::binding::ReadIntArray(context, "multipleScores", multipleScores);
    gugu::binding::ReadStringArray(context, "multipleNames", multipleNames);
    gugu::binding::ReadEnum(context, "singleWeapon", "weaponType", singleWeapon);
    gugu::binding::ReadEnumArray(context, "multipleWeapons", "weaponType", multipleWeapons);
    gugu::binding::ReadDatasheetReference(context, "emptyCharacter", emptyCharacter);
    gugu::binding::ReadDatasheetReference(context, "singleCharacter", singleCharacter);
    gugu::binding::ReadDatasheetReferenceArray(context, "multipleCharacters", multipleCharacters);
    gugu::binding::ReadDatasaveInstance(context, "emptyItem", "itemSave", emptyItem);
    gugu::binding::ReadDatasaveInstance(context, "singleItem", "itemSave", singleItem);
    gugu::binding::ReadDatasaveInstanceArray(context, "multipleItems", "itemSave", multipleItems);
}

void DS_GameSave::SerializeMembers(gugu::DataSaveContext& context) const
{
    //gugu::DatasheetObject::SerializeMembers(context);

    gugu::binding::WriteInt(context, "score", score);
    gugu::binding::WriteString(context, "name", name);
    gugu::binding::WriteIntArray(context, "multipleScores", multipleScores);
    gugu::binding::WriteStringArray(context, "multipleNames", multipleNames);
    gugu::binding::WriteEnum(context, "singleWeapon", "weaponType", singleWeapon);
    gugu::binding::WriteEnumArray(context, "multipleWeapons", "weaponType", multipleWeapons);
    gugu::binding::WriteDatasheetReference(context, "emptyCharacter", emptyCharacter);
    gugu::binding::WriteDatasheetReference(context, "singleCharacter", singleCharacter);
    gugu::binding::WriteDatasheetReferenceArray(context, "multipleCharacters", multipleCharacters);
    gugu::binding::WriteDatasaveInstance(context, "emptyItem", "itemSave", emptyItem);
    gugu::binding::WriteDatasaveInstance(context, "singleItem", "itemSave", singleItem);
    gugu::binding::WriteDatasaveInstanceArray(context, "multipleItems", "itemSave", multipleItems);
}

const std::string& DS_GameSave::GetDataInstanceType() const
{
    static const std::string dataInstanceType = "gameSave";
    return dataInstanceType;
}

////////////////////////////////////////////////////////////////
DS_ItemSave::DS_ItemSave()
{
    item = nullptr;
    quantity = 0;
}

DS_ItemSave::~DS_ItemSave()
{
    item = nullptr;
}

void DS_ItemSave::ParseMembers(gugu::DataParseContext& context)
{
    //gugu::DatasheetObject::ParseMembers(context);

    gugu::binding::ReadDatasheetReference(context, "item", item);
    gugu::binding::ReadInt(context, "quantity", quantity);
}

void DS_ItemSave::SerializeMembers(gugu::DataSaveContext& context) const
{
    //gugu::DatasheetObject::SerializeMembers(context);

    gugu::binding::WriteDatasheetReference(context, "item", item);
    gugu::binding::WriteInt(context, "quantity", quantity);
}

const std::string& DS_ItemSave::GetDataInstanceType() const
{
    static const std::string dataInstanceType = "itemSave";
    return dataInstanceType;
}

////////////////////////////////////////////////////////////////
void DataBinding_Register()
{
    EWeaponType::Register();

    gugu::GetResources()->RegisterDataObjectFactory(DataBinding_InstanciateDataObject);
}

////////////////////////////////////////////////////////////////
gugu::DataObject* DataBinding_InstanciateDataObject(std::string_view classType)
{
    if (classType == "character")
    {
        return new DS_Character;
    }
    if (classType == "faction")
    {
        return new DS_Faction;
    }
    if (classType == "item")
    {
        return new DS_Item;
    }
    if (classType == "conditionAnd")
    {
        return new DS_ConditionAnd;
    }
    if (classType == "conditionPlayerLevel")
    {
        return new DS_ConditionPlayerLevel;
    }
    if (classType == "gameSave")
    {
        return new DS_GameSave;
    }
    if (classType == "itemSave")
    {
        return new DS_ItemSave;
    }
    return nullptr;
}

} // namespace tests
